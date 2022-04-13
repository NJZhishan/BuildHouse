// Copyright Epic Games, Inc. All Rights Reserved.


#include "BuildHouseGameModeBase.h"
#include "Public/BuildHousePlayerController.h"
#include "Public/BuildHouseCharacter.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

ABuildHouseGameModeBase:: ABuildHouseGameModeBase()
{
	PlayerControllerClass = ABuildHousePlayerController::StaticClass();  //ָ��������
	//DefaultPawnClass = ABuildHouseCharacter::StaticClass();   //ָ����ɫ
	static ConstructorHelpers::FClassFinder<AActor> MyCharacterClass(TEXT("Blueprint'/Game/Blueprint/BP_BuilHouseCharactor.BP_BuilHouseCharactor_C'"));
	if (MyCharacterClass.Class != NULL)
	{
		DefaultPawnClass = MyCharacterClass.Class;
	}
	m_EntryWidget = nullptr;
	m_ProdOperationWidget = nullptr;
	m_GizmoActor = nullptr;
}

void ABuildHouseGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	AddEntryWidget();  //������widget
	AddProdOperationWidget(false);  //���ģ�Ͳ���widget
	
}

//��������ʾEntryWidget
void ABuildHouseGameModeBase::AddEntryWidget()
{
	TSubclassOf<UEntryWidget>entryWidgetClass = LoadClass<UEntryWidget>(NULL, _T("WidgetBlueprint'/Game/UI/UI_EntryWidget.UI_EntryWidget_c'"));//����EntryWidget��
	if (entryWidgetClass)
	{
		UWorld* world = GetWorld();
		if (world)
		{
			m_EntryWidget = (UEntryWidget*)CreateWidget(world, entryWidgetClass);  //����EntryWidget
			m_EntryWidget->AddToViewport();   //��EntryWidget��ʾ����Ļ��
		}
	}
}

//���ģ�Ͳ���widget
void ABuildHouseGameModeBase::AddProdOperationWidget(bool show /*= true*/)
{
	if (m_ProdOperationWidget == nullptr)
	{
		TSubclassOf<UUserWidget>prodOperationWidgetClass = LoadClass<UUserWidget>(NULL, _T("WidgetBlueprint'/Game/UI/UI_ProdOperation.UI_ProdOperation_C'"));
		if (prodOperationWidgetClass)
		{
			UWorld* world = GetWorld();
			if (world)
			{
				m_ProdOperationWidget = (UEntryWidget*)CreateWidget(world, prodOperationWidgetClass); 
				m_ProdOperationWidget->AddToViewport();
			}
		}
	}
	if (m_ProdOperationWidget)
	{
		if (show == false)
		{//����ʾ
			m_ProdOperationWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
		else if(m_ProdOperationWidget->IsVisible() == false)
		{//��ʾ
			m_ProdOperationWidget->SetVisibility(ESlateVisibility::Visible);
			float mouseX = 0, mouseY =0;
			GetWorld()->GetFirstPlayerController()->GetMousePosition(mouseX,mouseY);
			m_ProdOperationWidget->SetPositionInViewport(FVector2D(mouseX+5,mouseY));  //��ʾ���������λ��
		}
	}
}

//��ʾ������
void ABuildHouseGameModeBase::AddGizmo(bool show/* = true*/,  FVector location /*= FVector::ZeroVector*/, FRotator rotator /*= FRotator(0.0f, 0.0f, 0.0f)*/)
{
	if (m_GizmoActor == nullptr)
	{
		FTransform trans;
		FActorSpawnParameters aa;
		TSubclassOf<AActor>modulbaseInstance = LoadClass<AActor>(NULL, TEXT("Blueprint'/Game/Model/Gizmo/B_Gizmo.B_Gizmo_C'"));
		m_GizmoActor = GetWorld()->SpawnActor(modulbaseInstance, &trans, aa);  //����ģ��
		m_GizmoActor->SetActorEnableCollision(false);
	}
	if (m_GizmoActor)
	{
		m_GizmoActor->SetActorLocation(location);
		m_GizmoActor->SetActorRotation(rotator);   //����λ�ú���ת
		//m_GizmoActor->SetActorHiddenInGame(!show); //��ʾ��������������
		m_GizmoActor->SetActorHiddenInGame(true); //��ʾ��������������
	}
	return;
}