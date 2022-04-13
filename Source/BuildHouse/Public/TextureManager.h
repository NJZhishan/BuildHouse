// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Runtime/ImageWrapper/Public/IImageWrapper.h"
#include "Runtime/ImageWrapper/Public/IImageWrapperModule.h"
#include "Runtime/CoreUObject/Public/UObject/SoftObjectPtr.h"
#include "TextureManager.generated.h"


/**
 *表示贴图
 */
USTRUCT(BlueprintType)
struct BUILDHOUSE_API FTextureStruct
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
	FString texName;   //贴图的名字
	UPROPERTY()
	TSoftObjectPtr<UTexture>texturePtr;//贴图资源
public:
	void InitTexture()
	{//初始化
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
	void AddPic(FString picName);  //添加贴图，贴图来自本地文件夹中
	UFUNCTION()
	UTexture* FindPic(FString picName);  // 查找贴图
	bool AddZipPic(const FString& texURL, void* picture); //创建纹理，纹理来自压缩文件中

private:
	bool PicExit(FString picName);  //贴图是否已经存在
	UTexture* LoadTexture2D(const FString& ImagePath, bool& IsValid, int32& OutWidth, int32& OutHeight);  //创建贴图,贴图在本地文件夹中
	UTexture* LoadTexture2D(void *picture);  //创建贴图,贴图在内存中
	TSharedPtr<IImageWrapper> GetImageWrapperByExtention(const FString InImagePath);//获得图片格式
public:
	UPROPERTY()
	TArray<FTextureStruct>m_textureList;   //贴图序列
};
