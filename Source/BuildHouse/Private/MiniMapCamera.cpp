// Fill out your copyright notice in the Description page of Project Settings.


#include "MiniMapCamera.h"
#include "../Public/BuildHousePlayerController.h"
#include "../Public/BuildHouseGameInstance.h"
#include "../Public/BuildHouseCharacter.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/Components/SceneCaptureComponent2D.h"
#include "Runtime/Engine/Classes/Engine/TextureRenderTarget2D.h"
#include "Runtime/Engine/Classes/Camera/CameraActor.h"
#include "Runtime/Engine/Classes/Camera/CameraComponent.h"
#include "Runtime/Engine/Classes/Engine/PostProcessVolume.h"
#include "../BuildHouseGameModeBase.h"

AMiniMapCamera::AMiniMapCamera(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	MyCaptureComponent2D = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("MySceneCaptureComponent2D"));
	MyCaptureComponent2D->SetupAttachment(GetCameraComponent());
}

 //初始化小地图：小地图居中显示，并将小地图显示到width*height范围内
//width*height是为户型预留的尺寸
void AMiniMapCamera::InitMap(float width, float height, MiniMapShowType showType/* = MiniMapShowType::MM_ST_VIEW*/)
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
			USceneCaptureComponent2D* captureComponent = MyCaptureComponent2D;
			GetCameraComponent()->OrthoWidth = 1920.0f;
			GetCameraComponent()->AspectRatio = (float)width / (float)height;
			captureComponent->OrthoWidth = 1920.0f;
			ResizeMiniMap(width, height);
			//尽量放大
			int32 X = GetWorld()->GetGameViewport()->Viewport->GetSizeXY().X;
			int32 Y = GetWorld()->GetGameViewport()->Viewport->GetSizeXY().Y;
			float xScale = X / width;
			float yScale = Y / height;
			float scaleTemp = xScale < yScale ? xScale : yScale;
			float oldWidth = width, oldHeight = height;
			width *= scaleTemp; height *= scaleTemp;
			//计算户型的中心点
			float minX = 10000000.0f, minY = 10000000.0f, maxX = -10000000.0f, maxY = -10000000.0f;
			for (int floorIndex = 0; floorIndex < (int)gameInstance->m_pScene->m_FloorList.size(); floorIndex++)
			{
				int index = -1;
				if (gameInstance->m_pScene->m_floorVisibleIndexList.Find(floorIndex, index)/* || showType==MiniMapShowType::MM_ST_RoomSelect*/)
				{
					AModulBaseActor* floor = gameInstance->m_pScene->m_FloorList[floorIndex];
					FBox boundLocal = floor->CalculateComponentsBoundingBoxInLocalSpace(true);
					FVector boundMin = UKismetMathLibrary::TransformLocation(floor->GetActorTransform(), boundLocal.Min);
					FVector boundMax = UKismetMathLibrary::TransformLocation(floor->GetActorTransform(), boundLocal.Max); //转化为世界坐标
					minX = boundMin.X < minX ? boundMin.X : minX;
					minY = boundMin.Y < minY ? boundMin.Y : minY;
					maxX = boundMax.X > maxX ? boundMax.X : maxX;
					maxY = boundMax.Y > maxY ? boundMax.Y : maxY;
				}
			}
			//将相机放到户型的中心位置
			FVector mid = FVector((minX + maxX) * 0.5f, (minY + maxY) * 0.5f, gameInstance->m_pScene->MyHouse->m_height * 100.0f);
			SetActorLocation(mid);
			//设置相机
			AActor* oldViewTarget = playerController->GetViewTarget();  //保存当前视角
			playerController->SetViewTarget(this);  //设置为捕捉场景的视角
			GetWorld()->GetFirstPlayerController()->PlayerCameraManager->UpdateCamera(0);  //更新相机
			//将户型图放大
			float frameWidth = width;   //为户型预留的尺寸
			float frameHeight = height;
			FVector2D ltScreen, rbScreen;
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
				scale = scale1 >= scale2 ? scale1 : scale2;
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
				scale = scale1 > scale2 ? scale1 : scale2;
			}
			captureComponent->OrthoWidth *= scale; //这个值越大看到的范围越小
			GetCameraComponent()->OrthoWidth = captureComponent->OrthoWidth;
			captureComponent->CaptureScene();
			//计算完成后恢复视角
			playerController->SetViewTarget(oldViewTarget);  //恢复原视角
			GetWorld()->GetFirstPlayerController()->PlayerCameraManager->UpdateCamera(0);  //更新相机
			//设置为捕获baseColor，这样捕获结果就不受灯光影响
			captureComponent->CaptureSource = ESceneCaptureSource::SCS_BaseColor;
		}
	}
	return;
}

//修改小地图的显示类型
void AMiniMapCamera::ChangeMiniMapShowType(MiniMapShowType showType)
{
	m_MiniMapShowType = showType;
	switch (m_MiniMapShowType)
	{
		case MiniMapShowType::MM_ST_VIEW:
		{//视图状态不显示吊顶
			TArray<AActor*> objList;
			UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("吊顶")), objList);
			USceneCaptureComponent2D* captureComponent = MyCaptureComponent2D;
			captureComponent->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_RenderScenePrimitives;
			captureComponent->HiddenActors = objList;
		}
		break;
		case MiniMapShowType::MM_ST_RoomSelect:
		{//房间选择状态下只显示地面
			TArray<AActor*> objList;
			UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("地面")), objList);
			USceneCaptureComponent2D* captureComponent = MyCaptureComponent2D;
			captureComponent->PrimitiveRenderMode = ESceneCapturePrimitiveRenderMode::PRM_UseShowOnlyList;
			captureComponent->ShowOnlyActors = objList;
		}
		break;
	}
	ABuildHouseGameModeBase* gameMode = Cast<ABuildHouseGameModeBase>(GetWorld()->GetAuthGameMode());
	if (gameMode)
	{//房间选择状态显示所有的地面，无论地面是否可见
		float width, height;
		gameMode->m_EntryWidget->ReturnMiniMapSize(width, height);
		InitMap(width, height, m_MiniMapShowType);
	}
	return;
}

//小地图的缩放
void AMiniMapCamera::ResizeMiniMap(float width, float height)
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
			USceneCaptureComponent2D* captureComponent = MyCaptureComponent2D;
			captureComponent->TextureTarget->SizeX = width;
			captureComponent->TextureTarget->SizeY = height;
			GetCameraComponent()->AspectRatio = (float)width / (float)height;
			//尽量放大
			int32 X = GetWorld()->GetGameViewport()->Viewport->GetSizeXY().X;
			int32 Y = GetWorld()->GetGameViewport()->Viewport->GetSizeXY().Y;
			float xScale = X / width;
			float yScale = Y / height;
			float scaleTemp = xScale < yScale ? xScale : yScale;
			float oldWidth = width, oldHeight = height;
			width *= scaleTemp; height *= scaleTemp;
			//计算户型的中心点
			float minX = 10000000.0f, minY = 10000000.0f, maxX = -10000000.0f, maxY = -10000000.0f;
			for (int floorIndex = 0; floorIndex < (int)gameInstance->m_pScene->m_FloorList.size(); floorIndex++)
			{
				int index = -1;
				if (gameInstance->m_pScene->m_floorVisibleIndexList.Find(floorIndex, index) /*|| m_MiniMapShowType == MiniMapShowType::MM_ST_RoomSelect*/)
				{
					AModulBaseActor* floor = gameInstance->m_pScene->m_FloorList[floorIndex];
					FBox boundLocal = floor->CalculateComponentsBoundingBoxInLocalSpace(true);
					FVector boundMin = UKismetMathLibrary::TransformLocation(floor->GetActorTransform(), boundLocal.Min);
					FVector boundMax = UKismetMathLibrary::TransformLocation(floor->GetActorTransform(), boundLocal.Max); //转化为世界坐标
					minX = boundMin.X < minX ? boundMin.X : minX;
					minY = boundMin.Y < minY ? boundMin.Y : minY;
					maxX = boundMax.X > maxX ? boundMax.X : maxX;
					maxY = boundMax.Y > maxY ? boundMax.Y : maxY;
				}
			}
			//设置相机
			AActor* oldViewTarget = playerController->GetViewTarget();  //保存当前视角
			playerController->SetViewTarget(this);  //设置为捕捉场景的视角
			GetWorld()->GetFirstPlayerController()->PlayerCameraManager->UpdateCamera(0);  //更新相机
			//将户型图放大
			float frameWidth = width;   //为户型预留的尺寸
			float frameHeight = height;
			FVector2D ltScreen, rbScreen;
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
				scale = scale1 >= scale2 ? scale1 : scale2;
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
				scale = scale1 > scale2 ? scale1 : scale2;
			}
			captureComponent->OrthoWidth *= scale; //这个值越大看到的范围越小
			GetCameraComponent()->OrthoWidth = captureComponent->OrthoWidth;
			captureComponent->CaptureScene();
			//计算完成后恢复视角
			playerController->SetViewTarget(oldViewTarget);  //恢复原视角
			GetWorld()->GetFirstPlayerController()->PlayerCameraManager->UpdateCamera(0);  //更新相机
		}
	}
	return;
}

//小地图缩放
void AMiniMapCamera::ZoomMiniMap(float delta)
{
	USceneCaptureComponent2D* captureComponent = MyCaptureComponent2D;
	if (captureComponent)
	{
		captureComponent->OrthoWidth += 5 * delta;
		GetCameraComponent()->OrthoWidth = captureComponent->OrthoWidth;
	}
	return;
}

//点击小地图
void AMiniMapCamera::MiniMapPickUp(float mouseX, float mouseY)
{
	ABuildHousePlayerController* playerController = Cast<ABuildHousePlayerController>(GetWorld()->GetFirstPlayerController());
	UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
	ABuildHouseCharacter* buildHouseCharacter = Cast<ABuildHouseCharacter>(playerController->GetCharacter());
	if (playerController && gameInstance && buildHouseCharacter)
	{
		AActor* oldViewTarget = playerController->GetViewTarget();  //保存当前视角
		playerController->SetViewTarget(this);  //设置为捕捉场景的视角
		playerController->PlayerCameraManager->UpdateCamera(0);  //更新相机
		//将小地图的位置转化到视口位置，并转化为世界坐标，然后做射线检测
		USceneCaptureComponent2D* captureComponent = MyCaptureComponent2D;
		int32 X = GetWorld()->GetGameViewport()->Viewport->GetSizeXY().X;
		int32 Y = GetWorld()->GetGameViewport()->Viewport->GetSizeXY().Y;
		float xScale = X / (float)captureComponent->TextureTarget->SizeX;
		float yScale = Y / (float)captureComponent->TextureTarget->SizeY;
		float scaleTemp = xScale < yScale ? xScale : yScale;
		float actViewWidth = (float)captureComponent->TextureTarget->SizeX * scaleTemp;
		float actViewHeight = (float)captureComponent->TextureTarget->SizeY * scaleTemp;
		float newMouseX = (X - actViewWidth) * 0.5 + mouseX / (float)captureComponent->TextureTarget->SizeX * actViewWidth;
		float newMouseY = (Y - actViewHeight) * 0.5 + mouseY / (float)captureComponent->TextureTarget->SizeY * actViewHeight;
		mouseX = newMouseX; mouseY = newMouseY;
		FVector posWorld, dirWorld;
		playerController->DeprojectScreenPositionToWorld(mouseX, mouseY, posWorld, dirWorld);
		if (m_MiniMapShowType == MiniMapShowType::MM_ST_VIEW)
		{//视图状态双击设置相机的位置
			buildHouseCharacter->SetActorLocation(FVector(posWorld.X, posWorld.Y,buildHouseCharacter->GetActorLocation().Z));
		}
		else if (m_MiniMapShowType == MiniMapShowType::MM_ST_RoomSelect)
		{//房间选择
			TArray<AActor*> ActorsToIgnore;  //忽略不检测的actor
			UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("普通模型")), ActorsToIgnore);
			TArray<AActor*> CeilingActorsToIgnore;  //忽略不检测的actor
			UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("吊顶")), CeilingActorsToIgnore);
			ActorsToIgnore.Insert(CeilingActorsToIgnore, ActorsToIgnore.Num());
			TArray<AActor*> WallActorsToIgnore;  //忽略不检测的actor
			UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("墙体")), WallActorsToIgnore);
			ActorsToIgnore.Insert(WallActorsToIgnore, ActorsToIgnore.Num());
			FHitResult OutHit;
			bool bTrace = UKismetSystemLibrary::LineTraceSingle(GetWorld(), posWorld, posWorld + dirWorld * 500000.0f, ETraceTypeQuery::TraceTypeQuery1, true, ActorsToIgnore, EDrawDebugTrace::Type::None, OutHit, true);
			if (bTrace)
			{
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, OutHit.Actor.Get()->GetName());
				}
				//修改后期处理材质，由描边材质修改为纯色材质，默认第一个材质是描边材质，第二个材质是纯色材质
				//TArray<AActor*> ppvList; 
				//UGameplayStatics::GetAllActorsOfClass(GetWorld(), APostProcessVolume::StaticClass(), ppvList);
				//for (auto& ppv : ppvList)
				//{
				//	APostProcessVolume* tempPPV = Cast<APostProcessVolume>(ppv);
				//	for (int i = 0; i < (int)tempPPV->Settings.WeightedBlendables.Array.Num(); i++)
				//	{
				//		if (i == 0)
				//		{
				//			tempPPV->Settings.WeightedBlendables.Array[i].Weight = 0.0f;
				//		}
				//		else
				//		{
				//			tempPPV->Settings.WeightedBlendables.Array[i].Weight = 1.0f;
				//		}
				//	}
				//}
				AModulBaseActor* floor = Cast<AModulBaseActor>(OutHit.Actor);
				if (floor->ActorHasTag(TEXT("地面")))
				{
					int visibleFloorIndex = -1;  //选中的房间
					for (int i = 0; i < (int)gameInstance->m_pScene->m_FloorList.size(); i++)
					{
						if (floor == gameInstance->m_pScene->m_FloorList[i])
						{
							visibleFloorIndex = i;
							break;
						}
					}
					if (visibleFloorIndex > -1)
					{
						gameInstance->m_pScene->m_floorVisibleIndexList.Empty();
						gameInstance->m_pScene->m_floorVisibleIndexList.AddUnique(visibleFloorIndex);  //设置可见的房间
						gameInstance->m_pScene->UpdateRoomVisiblity(gameInstance->m_pScene->m_floorVisibleIndexList);//更新可见性
						buildHouseCharacter->SetActorLocation(floor->GetActorLocation()); //设置相机的位置							
						buildHouseCharacter->SpringArmComponent->TargetArmLength = 0;
						buildHouseCharacter->SpringArmComponent->SetRelativeRotation(FRotator(0.0f, 270.0f, 0.0f));
						buildHouseCharacter->SpringArmComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 170.0f));  //适当抬高，假设人有1.7m，所以抬高170cm
						FVector cameraPos, cameraDir;
						buildHouseCharacter->ReturnCamera(cameraPos, cameraDir);//获得视线位置和方向
						gameInstance->m_pScene->ModifyVisibility(cameraPos, cameraDir);//根据视点位置调整可见性
						//初始化小地图
						ABuildHouseGameModeBase* gameMode = Cast<ABuildHouseGameModeBase>(GetWorld()->GetAuthGameMode());
						if (gameMode)
						{
							float width, height;
							gameMode->m_EntryWidget->ReturnMiniMapSize(width, height);
							InitMap(width, height, MiniMapShowType::MM_ST_RoomSelect);
						}
					}
				}
			}
		}
		//计算完成后恢复视角
		playerController->SetViewTarget(oldViewTarget);  //恢复原视角
		GetWorld()->GetFirstPlayerController()->PlayerCameraManager->UpdateCamera(0);  //更新相机
	}
}

//显示所有房间
void AMiniMapCamera::ShowAllRoom()
{
	ABuildHousePlayerController* playerController = Cast<ABuildHousePlayerController>(GetWorld()->GetFirstPlayerController());
	UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
	ABuildHouseCharacter* buildHouseCharacter = Cast<ABuildHouseCharacter>(playerController->GetCharacter());
	if (playerController && gameInstance && buildHouseCharacter && gameInstance->m_pScene->m_FloorList.size()>0)
	{
		gameInstance->m_pScene->m_floorVisibleIndexList.Empty();
		for (int i = 0; i < (int)gameInstance->m_pScene->m_FloorList.size(); i++)
		{//设置可见的房间
			gameInstance->m_pScene->m_floorVisibleIndexList.AddUnique(i);
		}
		gameInstance->m_pScene->UpdateRoomVisiblity(gameInstance->m_pScene->m_floorVisibleIndexList);//更新可见性
		buildHouseCharacter->SetActorLocation(gameInstance->m_pScene->m_FloorList[0]->GetActorLocation()); //设置相机的位置							
		buildHouseCharacter->SpringArmComponent->TargetArmLength = 0;
		buildHouseCharacter->SpringArmComponent->SetRelativeRotation(FRotator(0.0f, 270.0f, 0.0f));
		buildHouseCharacter->SpringArmComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 170.0f));  //适当抬高，假设人有1.7m，所以抬高170cm
		FVector cameraPos, cameraDir;
		buildHouseCharacter->ReturnCamera(cameraPos, cameraDir);//获得视线位置和方向
		gameInstance->m_pScene->ModifyVisibility(cameraPos, cameraDir);//根据视点位置调整可见性
		//初始化小地图
		ABuildHouseGameModeBase* gameMode = Cast<ABuildHouseGameModeBase>(GetWorld()->GetAuthGameMode());
		if (gameMode)
		{
			float width, height;
			gameMode->m_EntryWidget->ReturnMiniMapSize(width, height);
			InitMap(width, height, MiniMapShowType::MM_ST_RoomSelect);
		}
	}
	return;
}