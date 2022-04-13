// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Private/EntryWidget.h"
#include "BuildHouseGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class BUILDHOUSE_API ABuildHouseGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	ABuildHouseGameModeBase();
	virtual void BeginPlay() override;  //启动游戏时调用
public:
	void AddProdOperationWidget(bool show = true);  //添加模型操作widget
	void AddGizmo(bool show = true,FVector location=FVector::ZeroVector,FRotator rotator= FRotator(0.0f,0.0f,0.0f));      //显示坐标轴
private:
	void AddEntryWidget();   //添加入口widget
public:
	UEntryWidget* m_EntryWidget;  //入口widget
	UUserWidget* m_ProdOperationWidget;  //模型操作widget
	AActor* m_GizmoActor;       //坐标轴
};
