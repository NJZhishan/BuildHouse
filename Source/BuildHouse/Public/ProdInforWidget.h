// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ModulBaseActor.h"
#include "HouseGroupActor.h"
#include "ProdInforWidget.generated.h"

/**
 * 更新的素材信息
 */
USTRUCT(BlueprintType)
struct BUILDHOUSE_API FProdInforStruct
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	bool bModel;      //是否是模型
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	FString prodName;  //产品名称
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	FString brand;     //品牌
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	float length;      //长度  y
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	float width;       //宽度  x
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	float height;      //高度  z
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	float lengthScale;      //长度缩放
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	float widthScale;       //宽度缩放
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	float heightScale;      //高度缩放
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	bool equalScale;       //等比例缩放
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	float pitch;            //旋转y
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	float yaw;       //旋转z
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	float roll;      //旋转x
};
/**
 * 
 */
UCLASS(Blueprintable)
class BUILDHOUSE_API UProdInforWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void UpdateProdInfor(AModulBaseActor* selModel, AHouseGroupActor* selGroup);//更新产品信息
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateHouseUI();     //更新房间信息UI
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateUI(FProdInforStruct prodInfor);     //更新产品信息的UI
	UFUNCTION(BlueprintCallable)
	void ScaleModel(float lenthScale,float widthScale,float heightScale);//缩放模型
	UFUNCTION(BlueprintCallable)
	void RotateModel(RotationType rotType,float delta);//旋转模型
	UFUNCTION(BlueprintCallable)
	void RotateModelSelf(float newAngle);//旋转模型
	UFUNCTION(BlueprintCallable,BlueprintPure)
	float TransAngle(float oldAngle);//限制模型的角度
};
