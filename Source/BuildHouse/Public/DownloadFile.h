// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Runtime/Online/HTTP/Public/Interfaces/IHttpRequest.h"
#include "Runtime/Online/HTTP/Public/HttpModule.h"
#include "DownloadFile.generated.h"

/**
 * �����زĵ�������Ϣ
 */
UENUM(BlueprintType)
enum class DownloadType :uint8
{
	DT_PIC,   //��ͼ
	DT_MODEL, //ģ��
	DT_GROUP  //���
};
/**
 * ������Ϣ
 */
USTRUCT(BlueprintType)
struct BUILDHOUSE_API FReCreateStruct
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		FString url; //�ز����֣�����5667.txt
	UPROPERTY()
		DownloadType style; //�Ƿ���ģ�ͣ�0��ʾ��ͼ,1��ʾģ��
};
/**
 * �ز���Ϣ
 */
USTRUCT(BlueprintType)
struct BUILDHOUSE_API FDownloadInforStruct
{
	GENERATED_USTRUCT_BODY()
public:
	FDownloadInforStruct() { info.SetNum(30); }
public:
		FString url; //�ز�id
		FString type; //�ز����� 0����Ʒ��1,2,3,4��¥�̡����͡����巿��
		TArray<FString>info;  // //�ز����������Ϣ
		
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
	void DownloadFile(FString URL); //�����ļ�

private:
	void HandleDownloadRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded);//������ɵ���
	void HandleHttpRequestProgress(FHttpRequestPtr HttpRequest, int32 BytesSent, int32 BytesReceived); //�������ؽ���
public:
	UPROPERTY()
	TArray<FString>m_DownloadList;  //��������
};
