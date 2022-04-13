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
	virtual void Init() override;  //启动游戏

public:
	UFUNCTION()
	void AddPic(FString picName);  //添加贴图，贴图来自本地文件夹中
	bool AddZipPic(const FString& texURL, void* picture); //创建纹理，纹理来自压缩文件中
	UFUNCTION()
	UTexture* FindPic(FString picName);  //查找贴图
	UFUNCTION()
	void SetSelectModulBaseActor(AModulBaseActor* actor);   //设置被选中的模型
	UFUNCTION(BlueprintCallable)
	AModulBaseActor* ReturnSelectModulBaseActor();   //返回被选中的模型
	UFUNCTION()
	void SetSelectGroupActor(AHouseGroupActor* group);   //设置被选中的群组
	UFUNCTION(BlueprintCallable)
	AHouseGroupActor* ReturnSelectGroupActor();   //返回被选中的群组
	UFUNCTION(BlueprintCallable)
	void MoveSelThing();   //移动被选中的模型或者群组
	UFUNCTION()
	void SetDragInfor(const FString& infor) { m_sDragInfor = infor; }  //设置拖拽信息
	UFUNCTION()
	void HandleDragOver();  //处理拖拽完成
	UFUNCTION()
	void HandleReplaceTexture(FString texURL,FString matName,FVector specularColor); //替换贴图
	UFUNCTION()
	AModulBaseActor* HandlePutModel(FString texURL, FDownloadInforStruct prodInfor,FVector location,FVector scale,FRotator rotate,int style=-1); //放置模型
	UFUNCTION()
	void HandlePutGroup(FString texURL, FDownloadInforStruct prodInfor, FVector location, FVector scale, FVector normal); //放置组合
	UFUNCTION()
	void OnDownloadEnd(FString txtPath); //下载完成
	UFUNCTION()
	void OnDownloadBegin(FString txtPath, DownloadType style); //准备下载
	UFUNCTION()
	bool IsTheModelInLocal(FString& fileName, bool changeExt = true);  //文件是否在本地存在
	UFUNCTION()
	void SetWallAdsorb(bool adsorb) { m_bWallAdsorb = adsorb; }  //设置墙体吸附
	UFUNCTION()
	void SetObjectAdsorb(bool adsorb) { m_bObjectAdsorb = adsorb; } //设置物体吸附
	UFUNCTION()
	bool GetWallAdsorb() { return m_bWallAdsorb;}  //返回墙体吸附
	UFUNCTION()
	bool GetObjectAdsorb() { return m_bObjectAdsorb; }  //返回物体吸附
private:
	bool ParseDragInfor();  //解析拖拽信息
private:
	UPROPERTY()
	UTextureManager* m_sceneTextureManager;  //场景中使用的贴图
	UPROPERTY()
	UDownloadFile* m_DownLoadFileTool;       //用于下载文件的工具
	UPROPERTY()
	TArray<FReCreateStruct>m_DownLoadList;   //下载文件序列
	UPROPERTY()
	AModulBaseActor* m_SelectModulBase;      //当前被选中的modulbase
	UPROPERTY()
	AHouseGroupActor* m_SelectGroup;         //当前被选中的群组
	UPROPERTY()
	FString m_sDragInfor;                    //拖拽信息
	UPROPERTY()
	FDownloadInforStruct m_CurProdInfor;     //拖拽产品的信息
	UPROPERTY()
	bool m_bWallAdsorb;     //墙体吸附
	UPROPERTY()
	bool m_bObjectAdsorb;   //物体吸附
public:
	CScene* m_pScene;       //当前的场景
};
