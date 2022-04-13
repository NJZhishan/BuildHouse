// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Runtime/Online/HTTP/Public/Interfaces/IHttpRequest.h"
#include "Runtime/Online/HTTP/Public/HttpModule.h"
#include "DownloadFile.generated.h"

/**
 * 下载素材的类型信息
 */
UENUM(BlueprintType)
enum class DownloadType :uint8
{
	DT_PIC,   //贴图
	DT_MODEL, //模型
	DT_GROUP  //组合
};
/**
 * 下载信息
 */
USTRUCT(BlueprintType)
struct BUILDHOUSE_API FReCreateStruct
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		FString url; //素材名字，比如5667.txt
	UPROPERTY()
		DownloadType style; //是否是模型，0表示贴图,1表示模型
};
/**
 * 素材信息
 */
USTRUCT(BlueprintType)
struct BUILDHOUSE_API FDownloadInforStruct
{
	GENERATED_USTRUCT_BODY()
public:
	FDownloadInforStruct() { info.SetNum(30); }
public:
		FString url; //素材id
		FString type; //素材类型 0：产品；1,2,3,4：楼盘、户型、样板房等
		TArray<FString>info;  // //素材所有相关信息
		
};
/**
 *
 */
/**
 * 
 */
UCLASS()
class BUILDHOUSE_API UDownloadFile : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION()
	void DownloadFile(FString URL); //下载文件

private:
	void HandleDownloadRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);//下载完成调用
	void HandleHttpRequestProgress(FHttpRequestPtr HttpRequest, int32 BytesSent, int32 BytesReceived); //更新下载进度
public:
	UPROPERTY()
	TArray<FString>m_DownloadList;  //下载序列
};
