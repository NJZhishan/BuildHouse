// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModulBaseActor.h"
#include "FloorModulBaseActor.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class BUILDHOUSE_API AFloorModulBaseActor : public AModulBaseActor
{
	GENERATED_BODY()
public:
	UFUNCTION()
	void CalculateCenter();  //计算中心点
};
