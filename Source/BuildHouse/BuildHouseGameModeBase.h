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
	virtual void BeginPlay() override;  //������Ϸʱ����
public:
	void AddProdOperationWidget(bool show = true);  //���ģ�Ͳ���widget
	void AddGizmo(bool show = true,FVector location=FVector::ZeroVector,FRotator rotator= FRotator(0.0f,0.0f,0.0f));      //��ʾ������
private:
	void AddEntryWidget();   //������widget
public:
	UEntryWidget* m_EntryWidget;  //���widget
	UUserWidget* m_ProdOperationWidget;  //ģ�Ͳ���widget
	AActor* m_GizmoActor;       //������
};
