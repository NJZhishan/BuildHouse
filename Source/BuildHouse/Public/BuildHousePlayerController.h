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
	ABuildHousePlayerController();     //构造函数
	void LeftMouseBtnUp();             //鼠标左键松开
	bool ReturnHitResult(FHitResult &hitResult);  //返回射线检测的结果
private:
	void PickUpThings();  //射线检测

private:
	UPROPERTY()
	FHitResult m_HitResult;  //射线检测的结果
	UPROPERTY()
	bool m_HitSuc;  //射线加测是否成功
};
