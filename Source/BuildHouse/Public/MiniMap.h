// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/SceneCapture2D.h"
#include "MiniMap.generated.h"

/**
 * С��ͼ
 */
UCLASS(Blueprintable)
class BUILDHOUSE_API AMiniMap : public ASceneCapture2D
{
	GENERATED_BODY()
	
public:
	void InitMap(float width, float height);  //��ʼ��С��ͼ
	UFUNCTION(BlueprintCallable)
	void ResizeMiniMap(float width, float height);  //���ų�����׽�ӿ�
	UFUNCTION(BlueprintCallable)
	void ZoomMiniMap(float delta);    //����С��ͼ
	UFUNCTION(BlueprintCallable)
	void PickUp(float mouseX,float mouseY);         //���С��ͼ
};
