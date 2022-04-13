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
	SetShowMouseCursor(true); //��ʾ���
	bEnableClickEvents = true; //����Ϊtrue����actor���ɿ���갴��ʱ������ӦActorOnReleased
	bEnableMouseOverEvents = true; //����Ϊtrue��������ƶ���actor��ʱ������ӦActorBeginMouseOver
}

//�������ɿ�
void ABuildHousePlayerController::LeftMouseBtnUp()
{
	PickUpThings();  //���߼��
}

//���߼��
void ABuildHousePlayerController::PickUpThings()
{
	if (GetWorld())
	{
		UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
		if (gameInstance)
		{
			AModulBaseActor* lastSel = gameInstance->ReturnSelectModulBaseActor();
			if (lastSel)
			{//�ϴ�ѡ�е�ģ��
				lastSel->SetOutline(false);//ȡ���ϴ�ѡ����������
				lastSel->SetSelComponent(nullptr);  //����ѡ�е����
				gameInstance->SetSelectModulBaseActor(nullptr); //����ѡ�е�ģ��
			}
			AHouseGroupActor* lastSelGroup = gameInstance->ReturnSelectGroupActor();
			if (lastSelGroup)
			{//�ϴ�ѡ�е�Ⱥ��
				lastSelGroup->SetOutline(false); //ȡ���ϴ�ѡ�е�Ⱥ�����
				gameInstance->SetSelectGroupActor(nullptr); //����ѡ�е�Ⱥ��
			}
			m_HitSuc = GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, m_HitResult);  //���߼��
			if (m_HitSuc)
			{
				AModulBaseActor* currentSel = Cast<AModulBaseActor>(m_HitResult.Actor.Get());
				if (currentSel)
				{
					AHouseGroupActor* parentGroup = gameInstance->m_pScene->GetInWhichGroup(currentSel);
					if (parentGroup)
					{//ģ����Ⱥ����
						gameInstance->SetSelectGroupActor(parentGroup);
						parentGroup->SetOutline(true);
					}
					currentSel->SetOutline(true);//����ǰѡ�е��������
					UMaterialInstanceDynamic* materialInstance = Cast<UMaterialInstanceDynamic>(m_HitResult.Component.Get()->GetMaterial(0));
					GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, materialInstance->Parent->GetName());  //��ʾѡ�еĲ���
					currentSel->SetSelComponent(Cast<UProceduralMeshComponent>(m_HitResult.Component.Get()));  //����ѡ�е����
					gameInstance->SetSelectModulBaseActor(currentSel); //����ѡ�е�ģ��
				}
			}
		}
	}
}

//�������߼��Ľ��
bool ABuildHousePlayerController::ReturnHitResult(FHitResult& hitResult)
{
	if (!m_HitSuc)
	{//���û��ѡ���κ�ģ�ͣ���������λ�÷���ģ��
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
			hitResult.ImpactNormal = hitResult.Normal = FVector::UpVector;  //���÷�������
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

