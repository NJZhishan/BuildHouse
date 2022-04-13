// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/SceneCapture2D.h"
#include "MiniMap.generated.h"

/**
 * 小地图
 */
UCLASS(Blueprintable)
class BUILDHOUSE_API AMiniMap : public ASceneCapture2D
{
	GENERATED_BODY()
	
public:
	void InitMap(float width, float height);  //初始化小地图
	UFUNCTION(BlueprintCallable)
	void ResizeMiniMap(float width, float height);  //缩放场景捕捉视口
	UFUNCTION(BlueprintCallable)
	void ZoomMiniMap(float delta);    //缩放小地图
	UFUNCTION(BlueprintCallable)
	void PickUp(float mouseX,float mouseY);         //点击小地图
};
