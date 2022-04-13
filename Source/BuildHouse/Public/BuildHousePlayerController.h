// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BuildHousePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BUILDHOUSE_API ABuildHousePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	static ABuildHousePlayerController* GetPlayerController();
	ABuildHousePlayerController();     //���캯��
	void LeftMouseBtnUp();             //�������ɿ�
	bool ReturnHitResult(FHitResult &hitResult);  //�������߼��Ľ��
private:
	void PickUpThings();  //���߼��

private:
	UPROPERTY()
	FHitResult m_HitResult;  //���߼��Ľ��
	UPROPERTY()
	bool m_HitSuc;  //���߼Ӳ��Ƿ�ɹ�
};
