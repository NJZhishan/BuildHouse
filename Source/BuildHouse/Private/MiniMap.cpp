// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniMap.h"
#include "../Public/BuildHousePlayerController.h"
#include "../Public/BuildHouseGameInstance.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/Components/SceneCaptureComponent2D.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"

 //初始化小地图：小地图居中显示，并将小地图显示到width*height范围内
//width*height是为户型预留的尺寸
void AMiniMap::InitMap(float width, float height)
{
	if (width * height < 0.1)
		return;  //长度或者宽度约等于0时不需要重新计算
	if (GetWorld())
	{
		ABuildHousePlayerController* playerController = Cast<ABuildHousePlayerController>(GetWorld()->GetFirstPlayerController());
		UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
		if (playerController && gameInstance && gameInstance->m_pScene && gameInstance->m_pScene->m_CeilingList.size() > 0)
		{
			//恢复原始大小
			USceneCaptureComponent2D* captureComponent = GetCaptureComponent2D();
			captureComponent->OrthoWidth = 1920.0f;
			ResizeMiniMap(width, height);
			//尽量放大
			int32 X = GetWorld()->GetGameViewport()->Viewport->GetSizeXY().X;
			int32 Y = GetWorld()->GetGameViewport()->Viewport->GetSizeXY().Y;
			float xScale = X /width;
			float yScale = Y/height;
			float scaleTemp = xScale < yScale ? xScale : yScale;
			float oldWidth = width, oldHeight = height;
			width *= scaleTemp; height *= scaleTemp;
			//计算户型的中心点
			float minX = 10000000.0f, minY = 10000000.0f, maxX = -10000000.0f, maxY = -10000000.0f;
			for (auto& ceiling : gameInstance->m_pScene->m_CeilingList)
			{
				FBox boundLocal = ceiling->CalculateComponentsBoundingBoxInLocalSpace(true);
				FVector boundMin = UKismetMathLibrary::TransformLocation(ceiling->GetActorTransform(), boundLocal.Min);
				FVector boundMax = UKismetMathLibrary::TransformLocation(ceiling->GetActorTransform(), boundLocal.Max); //转化为世界坐标
				minX = boundMin.X < minX ? boundMin.X : minX;
				minY = boundMin.Y < minY ? boundMin.Y : minY;
				maxX = boundMax.X > maxX ? boundMax.X : maxX;
				maxY = boundMax.Y > maxY ? boundMax.Y : maxY;
			}
			//将相机放到户型的中心位置
			FVector mid = FVector((minX + maxX) * 0.5f, (minY + maxY) * 0.5f, gameInstance->m_pScene->MyHouse->m_height*100.0f);
			SetActorLocation(mid);
			//设置相机
			AActor* oldViewTarget = playerController->GetViewTarget();  //保存当前视角
			FMinimalViewInfo oldViewInfor = playerController->PlayerCameraManager->GetCameraCachePOV();
			playerController->SetViewTarget(this);  //设置为捕捉场景的视角
			FMinimalViewInfo viewInfor = playerController->PlayerCameraManager->GetCameraCachePOV();
			viewInfor.bConstrainAspectRatio = true;
			viewInfor.AspectRatio = width / height;
			playerController->PlayerCameraManager->SetCameraCachePOV(viewInfor);
			playerController->PlayerCameraManager->ViewTarget.POV.bConstrainAspectRatio = true;
			playerController->PlayerCameraManager->ViewTarget.POV.AspectRatio = width / height;
			playerController->PlayerCameraManager->ViewTarget.POV.OrthoWidth = width;
			playerController->PlayerCameraManager->bDefaultConstrainAspectRatio = true;
			GetWorld()->GetFirstPlayerController()->PlayerCameraManager->UpdateCamera(0);  //更新相机
			//将户型图放大
			float frameWidth = width;   //为户型预留的尺寸
			float frameHeight = height;
			FVector2D ltScreen, rbScreen;
			playerController->PlayerCameraManager->UpdateCamera(0);  //更新相机
			playerController->ProjectWorldLocationToScreen(FVector(minX, minY, 0), ltScreen);
			playerController->ProjectWorldLocationToScreen(FVector(maxX, maxY, 0), rbScreen);
			float houseWidth = rbScreen.X - ltScreen.X;   //当前户型的尺寸
			float houseHeight = rbScreen.Y - ltScreen.Y;
			//按照为户型预留的尺寸，将户型进行缩放
			float scale = 1;
			if (houseWidth <= frameWidth && houseHeight < frameHeight)
			{//整个户型都在可见范围内,则计算最大缩放多少，能让户型既在可见范围，又能尽可能的放大
				float scale1 = (float)houseWidth / frameWidth;
				float scale2 = (float)houseHeight / frameHeight;
				scale = scale1 <= scale2 ? scale1 : scale2;
			}
			else if (houseWidth <= frameWidth && houseHeight > frameHeight)
			{//高度方向上不在可见范围，宽度方向上在可见范围内，则计算缩小多少，能让户型全部在可见范围内
				float scale1 = (float)houseHeight / frameHeight;
				scale = scale1;
			}
			else if (houseWidth > frameWidth && houseHeight <= frameHeight)
			{//高度方向上不在可见范围，宽度方向上在可见范围内，则计算缩小多少，能让户型全部在可见范围内
				float scale1 = (float)houseWidth / frameWidth;
				scale = scale1;
			}
			else if (houseWidth > frameWidth && houseHeight > frameHeight)
			{//宽度和高度方向上都不在可见范围内，则计算最小缩小多少，能让户型既在可见范围，又能尽可能的少的缩小
				float scale1 = (float)houseWidth / frameWidth;
				float scale2 = (float)houseHeight / frameHeight;
				scale = scale1 < scale2 ? scale1 : scale2;
			}
			captureComponent->OrthoWidth *= scale; //这个值越大看到的范围越小
			playerController->PlayerCameraManager->ViewTarget.POV.OrthoWidth = captureComponent->OrthoWidth;
			captureComponent->CaptureScene();
			//计算完成后恢复视角
			playerController->SetViewTarget(oldViewTarget);  //恢复原视角
			playerController->PlayerCameraManager->SetCameraCachePOV(oldViewInfor);
			GetWorld()->GetFirstPlayerController()->PlayerCameraManager->UpdateCamera(0);  //更新相机
			//不显示吊顶
			TArray<AActor*> objList;  
			UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("吊顶")), objList);
			captureComponent->HiddenActors = objList;
			//设置为捕获baseColor，这样捕获结果就不受灯光影响
			captureComponent->CaptureSource = ESceneCaptureSource::SCS_BaseColor;
		}
	}
	return;
}

//小地图的缩放
void AMiniMap::ResizeMiniMap(float width, float height)
{
	if (width * height < 0.1)
		return;  //长度或者宽度约等于0时不需要重新计算
	USceneCaptureComponent2D* captureComponent = GetCaptureComponent2D();
	if (captureComponent)
	{
		captureComponent->TextureTarget->ResizeTarget(width, height);
		captureComponent->CaptureScene();
	}
	return;
}

//小地图缩放
void AMiniMap::ZoomMiniMap(float delta)
{
	USceneCaptureComponent2D* captureComponent = GetCaptureComponent2D();
	if (captureComponent)
	{
		captureComponent->OrthoWidth += 5 * delta;
	}
	return;
}

//点击小地图
void AMiniMap::PickUp(float mouseX, float mouseY)
{
	ABuildHousePlayerController* playerController = Cast<ABuildHousePlayerController>(GetWorld()->GetFirstPlayerController());
	if (playerController)
	{
		USceneCaptureComponent2D* captureComponent = GetCaptureComponent2D();
		AActor* oldViewTarget = playerController->GetViewTarget();  //保存当前视角
		FMinimalViewInfo oldViewInfor = playerController->PlayerCameraManager->GetCameraCachePOV();
		playerController->SetViewTarget(this);  //设置为捕捉场景的视角
		FMinimalViewInfo viewInfor = playerController->PlayerCameraManager->GetCameraCachePOV();
		viewInfor.bConstrainAspectRatio = true;
		viewInfor.AspectRatio = (float)captureComponent->TextureTarget->SizeX / (float)captureComponent->TextureTarget->SizeY;
		playerController->PlayerCameraManager->SetCameraCachePOV(viewInfor);
		playerController->PlayerCameraManager->UpdateCamera(0);  //更新相机
		FVector posWorld, dirWorld;

		//尽量放大
		int32 X = GetWorld()->GetGameViewport()->Viewport->GetSizeXY().X;
		int32 Y = GetWorld()->GetGameViewport()->Viewport->GetSizeXY().Y;
		mouseX = mouseX/(float)captureComponent->TextureTarget->SizeX*X;
		mouseY = mouseY/(float)captureComponent->TextureTarget->SizeY*Y;
		playerController->DeprojectScreenPositionToWorld(mouseX,mouseY,posWorld,dirWorld);

		FVector posWorldLeft, dirWorldLeft, posWorldRight, dirWorldRight;
		playerController->DeprojectScreenPositionToWorld(0, 0, posWorldLeft, dirWorldLeft);
		playerController->DeprojectScreenPositionToWorld(X, Y, posWorldRight, dirWorldRight);
		posWorld.X = posWorldLeft.X+(posWorldRight.X - posWorldLeft.X) / X * mouseX;
		posWorld.Y = posWorldLeft.Y+(posWorldRight.Y - posWorldLeft.Y) / Y * mouseY;
		posWorld.Z = 500;
		dirWorld = FVector(0.0f, 0.0f, -1.0f);
		TArray<AActor*> ActorsToIgnore;  //忽略不检测的actor
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("普通模型")), ActorsToIgnore);
		FHitResult OutHit;
		bool bTrace = UKismetSystemLibrary::LineTraceSingle(GetWorld(), posWorld, posWorld+dirWorld*500000.0f, ETraceTypeQuery::TraceTypeQuery1, true, ActorsToIgnore, EDrawDebugTrace::Type::None, OutHit, true);
		if (bTrace)
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, OutHit.Actor.Get()->GetName());
			}
		}
		//计算完成后恢复视角
		playerController->SetViewTarget(oldViewTarget);  //恢复原视角
		viewInfor.bConstrainAspectRatio = false;
		playerController->PlayerCameraManager->SetCameraCachePOV(oldViewInfor);
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->UpdateCamera(0);  //更新相机
	}
}