// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Public/ModulBaseActor.h"
#include "../Public/HouseGroupActor.h"
#include "EntryWidget.generated.h"

/**
 * 
 */
UCLASS()
class UEntryWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "EntryWidget|ReadFbx")
	void ReadFbx();      //读取fbx文件
	UFUNCTION(BlueprintCallable, Category = "EntryWidget|ReadFbx")
	void SaveDragProdInfor(FString infor); //保存拖拽的模型信息
	UFUNCTION(BlueprintCallable, Category = "EntryWidget|ReadFbx")
	void DragOver(FString mousePos); //拖拽完成
	UFUNCTION(BlueprintCallable)
	void UpdateDownloadProgress(float prog);   //更新下载进度
	UFUNCTION(BlueprintCallable, Category = "EntryWidget|ReadFbx")
	void ChangViewType(FString viewType);      //二维三维切换
	UFUNCTION(BlueprintCallable, Category = "EntryWidget|ReadFbx")
	void SetWallAdsorb(bool adsorb);           //墙体吸附
	UFUNCTION(BlueprintCallable, Category = "EntryWidget|ReadFbx")
	void SetObjectAdsorb(bool adsorb);         //物体吸附
	UFUNCTION(BlueprintCallable, Category = "EntryWidget|ReadFbx")
	void UpdateProdInfor(AModulBaseActor* selModel, AHouseGroupActor* selGroup);   //更新产品信息
	UFUNCTION(BlueprintImplementableEvent)
	void ReturnMiniMapSize(float &width,float &height);     //获得小地图的尺寸
	UFUNCTION(BlueprintImplementableEvent)
	void ShowMiniMap(bool showMiniMap);     //设置小地图的显示
public:
	UPROPERTY(EditAnyWhere,BlueprintReadWrite)
	FText m_downloadProgress;  //下载进度
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	float m_fDownloadProgress;  //下载进度
};
