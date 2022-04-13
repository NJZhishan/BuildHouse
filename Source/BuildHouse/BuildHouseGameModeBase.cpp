// Copyright Epic Games, Inc. All Rights Reserved.


#include "BuildHouseGameModeBase.h"
#include "Public/BuildHousePlayerController.h"
#include "Public/BuildHouseCharacter.h"
#include "Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h"

ABuildHouseGameModeBase:: ABuildHouseGameModeBase()
{
	PlayerControllerClass = ABuildHousePlayerController::StaticClass();  //指定控制器
	//DefaultPawnClass = ABuildHouseCharacter::StaticClass();   //指定角色
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
	AddEntryWidget();  //添加入口widget
	AddProdOperationWidget(false);  //添加模型操作widget
	
}

//创建并显示EntryWidget
void ABuildHouseGameModeBase::AddEntryWidget()
{
	TSubclassOf<UEntryWidget>entryWidgetClass = LoadClass<UEntryWidget>(NULL, _T("WidgetBlueprint'/Game/UI/UI_EntryWidget.UI_EntryWidget_c'"));//加载EntryWidget类
	if (entryWidgetClass)
	{
		UWorld* world = GetWorld();
		if (world)
		{
			m_EntryWidget = (UEntryWidget*)CreateWidget(world, entryWidgetClass);  //创建EntryWidget
			m_EntryWidget->AddToViewport();   //将EntryWidget显示在屏幕上
		}
	}
}

//添加模型操作widget
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
		{//不显示
			m_ProdOperationWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
		else if(m_ProdOperationWidget->IsVisible() == false)
		{//显示
			m_ProdOperationWidget->SetVisibility(ESlateVisibility::Visible);
			float mouseX = 0, mouseY =0;
			GetWorld()->GetFirstPlayerController()->GetMousePosition(mouseX,mouseY);
			m_ProdOperationWidget->SetPositionInViewport(FVector2D(mouseX+5,mouseY));  //显示在鼠标点击的位置
		}
	}
}

//显示坐标轴
void ABuildHouseGameModeBase::AddGizmo(bool show/* = true*/,  FVector location /*= FVector::ZeroVector*/, FRotator rotator /*= FRotator(0.0f, 0.0f, 0.0f)*/)
{
	if (m_GizmoActor == nullptr)
	{
		FTransform trans;
		FActorSpawnParameters aa;
		TSubclassOf<AActor>modulbaseInstance = LoadClass<AActor>(NULL, TEXT("Blueprint'/Game/Model/Gizmo/B_Gizmo.B_Gizmo_C'"));
		m_GizmoActor = GetWorld()->SpawnActor(modulbaseInstance, &trans, aa);  //创建模型
		m_GizmoActor->SetActorEnableCollision(false);
	}
	if (m_GizmoActor)
	{
		m_GizmoActor->SetActorLocation(location);
		m_GizmoActor->SetActorRotation(rotator);   //设置位置和旋转
		//m_GizmoActor->SetActorHiddenInGame(!show); //显示或者隐藏坐标轴
		m_GizmoActor->SetActorHiddenInGame(true); //显示或者隐藏坐标轴
	}
	return;
}