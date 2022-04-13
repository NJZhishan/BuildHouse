// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildHousePlayerController.h"
#include "../Public/ModulBaseActor.h"
#include "../Public/BuildHouseGameInstance.h"
#include "../Public/BuildHouseCharacter.h"
#include "Runtime/Engine/Classes/Kismet/KismetSystemLibrary.h"
#include "Runtime/Engine/Classes/Components/LineBatchComponent.h"
#include ".././BuildHouseGameModeBase.h"
#include "Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h"

ABuildHousePlayerController::ABuildHousePlayerController()
{ 
	SetShowMouseCursor(true); //显示鼠标
	bEnableClickEvents = true; //设置为true，在actor上松开鼠标按键时可以响应ActorOnReleased
	bEnableMouseOverEvents = true; //设置为true，当鼠标移动到actor上时可以响应ActorBeginMouseOver
}

//鼠标左键松开
void ABuildHousePlayerController::LeftMouseBtnUp()
{
	PickUpThings();  //射线检测
}

//射线检测
void ABuildHousePlayerController::PickUpThings()
{
	if (GetWorld())
	{
		UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
		if (gameInstance)
		{
			AModulBaseActor* lastSel = gameInstance->ReturnSelectModulBaseActor();
			if (lastSel)
			{//上次选中的模型
				lastSel->SetOutline(false);//取消上次选中物体的描边
				lastSel->SetSelComponent(nullptr);  //重置选中的组件
				gameInstance->SetSelectModulBaseActor(nullptr); //重置选中的模型
			}
			AHouseGroupActor* lastSelGroup = gameInstance->ReturnSelectGroupActor();
			if (lastSelGroup)
			{//上次选中的群组
				lastSelGroup->SetOutline(false); //取消上次选中的群组描边
				gameInstance->SetSelectGroupActor(nullptr); //重置选中的群组
			}
			m_HitSuc = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, m_HitResult);  //射线监测
			if (m_HitSuc)
			{
				AModulBaseActor* currentSel = Cast<AModulBaseActor>(m_HitResult.Actor.Get());
				if (currentSel)
				{
					AHouseGroupActor* parentGroup = gameInstance->m_pScene->GetInWhichGroup(currentSel);
					if (parentGroup)
					{//模型在群组中
						gameInstance->SetSelectGroupActor(parentGroup);
						parentGroup->SetOutline(true);
					}
					currentSel->SetOutline(true);//将当前选中的物体描边
					UMaterialInstanceDynamic* materialInstance = Cast<UMaterialInstanceDynamic>(m_HitResult.Component.Get()->GetMaterial(0));
					GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, materialInstance->Parent->GetName());  //显示选中的材质
					currentSel->SetSelComponent(Cast<UProceduralMeshComponent>(m_HitResult.Component.Get()));  //设置选中的组件
					gameInstance->SetSelectModulBaseActor(currentSel); //保存选中的模型
				}
			}
		}
	}
}

//返回射线检测的结果
bool ABuildHousePlayerController::ReturnHitResult(FHitResult& hitResult)
{
	if (!m_HitSuc)
	{//鼠标没有选中任何模型，则在鼠标的位置放置模型
		FVector pos, direct;
		bool transSuc = DeprojectMousePositionToWorld(pos,direct);
		if (transSuc)
		{
			ABuildHouseCharacter* charactor = Cast<ABuildHouseCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
			if (charactor && charactor->GetTravelType() == TravelType::TT_2D_Ortho)
			{
				pos.Z = 0.0f;
			}
			hitResult.ImpactPoint = hitResult.Location = pos;
			hitResult.ImpactNormal = hitResult.Normal = FVector::UpVector;  //设置法向向上
		}
	}
	else
	{
		hitResult = m_HitResult;
	}
	return m_HitSuc;
}

ABuildHousePlayerController* ABuildHousePlayerController::GetPlayerController()
{
	APlayerController* playerController = nullptr;
	if (GWorld)
	{
		playerController = GWorld->GetFirstPlayerController();
	}
	if (!playerController)
	{
		if (GEngine)
		{
			FWorldContext* context = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
			playerController = GEngine->GetFirstLocalPlayerController(context->World());
		}
	}
	if (!playerController)
		return nullptr;
	ABuildHousePlayerController* buildHousePlayerController = Cast<ABuildHousePlayerController>(playerController);
	return buildHousePlayerController;
}

