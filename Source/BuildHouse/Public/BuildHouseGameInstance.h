// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "TextureManager.h"
#include "ModulBaseActor.h"
#include "DownloadFile.h"
#include "CScene.h"
#include "HouseGroupActor.h"
#include "BuildHouseGameInstance.generated.h"


UCLASS()
class BUILDHOUSE_API UBuildHouseGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual void Init() override;  //������Ϸ

public:
	UFUNCTION()
	void AddPic(FString picName);  //�����ͼ����ͼ���Ա����ļ�����
	bool AddZipPic(const FString& texURL, void* picture); //����������������ѹ���ļ���
	UFUNCTION()
	UTexture* FindPic(FString picName);  //������ͼ
	UFUNCTION()
	void SetSelectModulBaseActor(AModulBaseActor* actor);   //���ñ�ѡ�е�ģ��
	UFUNCTION(BlueprintCallable)
	AModulBaseActor* ReturnSelectModulBaseActor();   //���ر�ѡ�е�ģ��
	UFUNCTION()
	void SetSelectGroupActor(AHouseGroupActor* group);   //���ñ�ѡ�е�Ⱥ��
	UFUNCTION(BlueprintCallable)
	AHouseGroupActor* ReturnSelectGroupActor();   //���ر�ѡ�е�Ⱥ��
	UFUNCTION(BlueprintCallable)
	void MoveSelThing();   //�ƶ���ѡ�е�ģ�ͻ���Ⱥ��
	UFUNCTION()
	void SetDragInfor(const FString& infor) { m_sDragInfor = infor; }  //������ק��Ϣ
	UFUNCTION()
	void HandleDragOver();  //������ק���
	UFUNCTION()
	void HandleReplaceTexture(FString texURL,FString matName,FVector specularColor); //�滻��ͼ
	UFUNCTION()
	AModulBaseActor* HandlePutModel(FString texURL, FDownloadInforStruct prodInfor,FVector location,FVector scale,FRotator rotate,int style=-1); //����ģ��
	UFUNCTION()
	void HandlePutGroup(FString texURL, FDownloadInforStruct prodInfor, FVector location, FVector scale, FVector normal); //�������
	UFUNCTION()
	void OnDownloadEnd(FString txtPath); //�������
	UFUNCTION()
	void OnDownloadBegin(FString txtPath, DownloadType style); //׼������
	UFUNCTION()
	bool IsTheModelInLocal(FString& fileName, bool changeExt = true);  //�ļ��Ƿ��ڱ��ش���
	UFUNCTION()
	void SetWallAdsorb(bool adsorb) { m_bWallAdsorb = adsorb; }  //����ǽ������
	UFUNCTION()
	void SetObjectAdsorb(bool adsorb) { m_bObjectAdsorb = adsorb; } //������������
	UFUNCTION()
	bool GetWallAdsorb() { return m_bWallAdsorb;}  //����ǽ������
	UFUNCTION()
	bool GetObjectAdsorb() { return m_bObjectAdsorb; }  //������������
private:
	bool ParseDragInfor();  //������ק��Ϣ
private:
	UPROPERTY()
	UTextureManager* m_sceneTextureManager;  //������ʹ�õ���ͼ
	UPROPERTY()
	UDownloadFile* m_DownLoadFileTool;       //���������ļ��Ĺ���
	UPROPERTY()
	TArray<FReCreateStruct>m_DownLoadList;   //�����ļ�����
	UPROPERTY()
	AModulBaseActor* m_SelectModulBase;      //��ǰ��ѡ�е�modulbase
	UPROPERTY()
	AHouseGroupActor* m_SelectGroup;         //��ǰ��ѡ�е�Ⱥ��
	UPROPERTY()
	FString m_sDragInfor;                    //��ק��Ϣ
	UPROPERTY()
	FDownloadInforStruct m_CurProdInfor;     //��ק��Ʒ����Ϣ
	UPROPERTY()
	bool m_bWallAdsorb;     //ǽ������
	UPROPERTY()
	bool m_bObjectAdsorb;   //��������
public:
	CScene* m_pScene;       //��ǰ�ĳ���
};
