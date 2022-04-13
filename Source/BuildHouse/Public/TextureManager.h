// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Runtime/ImageWrapper/Public/IImageWrapper.h"
#include "Runtime/ImageWrapper/Public/IImageWrapperModule.h"
#include "Runtime/CoreUObject/Public/UObject/SoftObjectPtr.h"
#include "TextureManager.generated.h"


/**
 *��ʾ��ͼ
 */
USTRUCT(BlueprintType)
struct BUILDHOUSE_API FTextureStruct
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
	FString texName;   //��ͼ������
	UPROPERTY()
	TSoftObjectPtr<UTexture>texturePtr;//��ͼ��Դ
public:
	void InitTexture()
	{//��ʼ��
		texName = "";
		texturePtr = nullptr;
	}
};

/**
 * 
 */
UCLASS()
class BUILDHOUSE_API UTextureManager : public UObject
{
	GENERATED_BODY()


public:
	UFUNCTION()
	void AddPic(FString picName);  //�����ͼ����ͼ���Ա����ļ�����
	UFUNCTION()
	UTexture* FindPic(FString picName);  // ������ͼ
	bool AddZipPic(const FString& texURL, void* picture); //����������������ѹ���ļ���

private:
	bool PicExit(FString picName);  //��ͼ�Ƿ��Ѿ�����
	UTexture* LoadTexture2D(const FString& ImagePath, bool& IsValid, int32& OutWidth, int32& OutHeight);  //������ͼ,��ͼ�ڱ����ļ�����
	UTexture* LoadTexture2D(void *picture);  //������ͼ,��ͼ���ڴ���
	TSharedPtr<IImageWrapper> GetImageWrapperByExtention(const FString InImagePath);//���ͼƬ��ʽ
public:
	UPROPERTY()
	TArray<FTextureStruct>m_textureList;   //��ͼ����
};
