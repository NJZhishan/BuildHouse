// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ModulBaseActor.h"
#include "HouseGroupActor.h"
#include "ProdInforWidget.generated.h"

/**
 * ���µ��ز���Ϣ
 */
USTRUCT(BlueprintType)
struct BUILDHOUSE_API FProdInforStruct
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	bool bModel;      //�Ƿ���ģ��
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	FString prodName;  //��Ʒ����
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	FString brand;     //Ʒ��
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	float length;      //����  y
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	float width;       //���  x
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	float height;      //�߶�  z
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	float lengthScale;      //��������
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	float widthScale;       //�������
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	float heightScale;      //�߶�����
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	bool equalScale;       //�ȱ�������
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	float pitch;            //��תy
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	float yaw;       //��תz
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	float roll;      //��תx
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
	void UpdateProdInfor(AModulBaseActor* selModel, AHouseGroupActor* selGroup);//���²�Ʒ��Ϣ
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateHouseUI();     //���·�����ϢUI
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateUI(FProdInforStruct prodInfor);     //���²�Ʒ��Ϣ��UI
	UFUNCTION(BlueprintCallable)
	void ScaleModel(float lenthScale,float widthScale,float heightScale);//����ģ��
	UFUNCTION(BlueprintCallable)
	void RotateModel(RotationType rotType,float delta);//��תģ��
	UFUNCTION(BlueprintCallable)
	void RotateModelSelf(float newAngle);//��תģ��
	UFUNCTION(BlueprintCallable,BlueprintPure)
	float TransAngle(float oldAngle);//����ģ�͵ĽǶ�
};
