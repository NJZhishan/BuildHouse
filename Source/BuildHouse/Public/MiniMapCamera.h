// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraActor.h"
#include "Runtime/Engine/Classes/Components/SceneCaptureComponent2D.h"
#include "MiniMapCamera.generated.h"

/**
 * 小地图的显示类型
 */
UENUM(BlueprintType)
enum class MiniMapShowType :uint8
{
	MM_ST_VIEW,         //视图
	MM_ST_RoomSelect,   //房间选择
};

/**
 * 
 */
UCLASS(Blueprintable)
class BUILDHOUSE_API AMiniMapCamera : public ACameraActor
{
	GENERATED_BODY()
public:
	AMiniMapCamera(const FObjectInitializer& ObjectInitializer);
public:
	void InitMap(float width, float height, MiniMapShowType showType = MiniMapShowType::MM_ST_VIEW);  //初始化小地图
	UFUNCTION(BlueprintCallable)
	void ResizeMiniMap(float width, float height);  //缩放场景捕捉视口
	UFUNCTION(BlueprintCallable)
	void ZoomMiniMap(float delta);    //缩放小地图
	UFUNCTION(BlueprintCallable)
	void ChangeMiniMapShowType(MiniMapShowType showType);    //修改小地图的显示类型
	UFUNCTION(BlueprintCallable)
	void MiniMapPickUp(float mouseX,float mouseY);         //点击小地图
	UFUNCTION(BlueprintCallable)
	void ShowAllRoom();         //显示所有房间
public:
	UPROPERTY(Category = DecalActor, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class USceneCaptureComponent2D* MyCaptureComponent2D;
	MiniMapShowType m_MiniMapShowType;   //小地图显示类型

};
