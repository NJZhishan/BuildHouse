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
	void ReadFbx();      //��ȡfbx�ļ�
	UFUNCTION(BlueprintCallable, Category = "EntryWidget|ReadFbx")
	void SaveDragProdInfor(FString infor); //������ק��ģ����Ϣ
	UFUNCTION(BlueprintCallable, Category = "EntryWidget|ReadFbx")
	void DragOver(FString mousePos); //��ק���
	UFUNCTION(BlueprintCallable)
	void UpdateDownloadProgress(float prog);   //�������ؽ���
	UFUNCTION(BlueprintCallable, Category = "EntryWidget|ReadFbx")
	void ChangViewType(FString viewType);      //��ά��ά�л�
	UFUNCTION(BlueprintCallable, Category = "EntryWidget|ReadFbx")
	void SetWallAdsorb(bool adsorb);           //ǽ������
	UFUNCTION(BlueprintCallable, Category = "EntryWidget|ReadFbx")
	void SetObjectAdsorb(bool adsorb);         //��������
	UFUNCTION(BlueprintCallable, Category = "EntryWidget|ReadFbx")
	void UpdateProdInfor(AModulBaseActor* selModel, AHouseGroupActor* selGroup);   //���²�Ʒ��Ϣ
	UFUNCTION(BlueprintImplementableEvent)
	void ReturnMiniMapSize(float &width,float &height);     //���С��ͼ�ĳߴ�
	UFUNCTION(BlueprintImplementableEvent)
	void ShowMiniMap(bool showMiniMap);     //����С��ͼ����ʾ
public:
	UPROPERTY(EditAnyWhere,BlueprintReadWrite)
	FText m_downloadProgress;  //���ؽ���
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	float m_fDownloadProgress;  //���ؽ���
};
