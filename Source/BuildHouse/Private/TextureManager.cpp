// Fill out your copyright notice in the Description page of Project Settings.


#include "TextureManager.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"
#include "Windows/AllowWindowsPlatformTypes.h" //一般为Windows中的宏和UE4冲突所致,将可能会导致冲突的宏所在的hom文件包裹起来
#include "Windows/PreWindowsApi.h"
#include "Picture.h"
#include "flyfile_w.h"
#include "Windows/PostWindowsApi.h"            //一般为Windows中的宏和UE4冲突所致,将可能会导致冲突的宏所在的hom文件包裹起来
#include "Windows/HideWindowsPlatformTypes.h"

//求平均
#define AVERAGE(a, b)   (uint8)( ((a) + (b)) >> 1 )

//添加贴图，贴图来自本地文件夹中
//参数是绝对路径
void UTextureManager::AddPic(FString picName)
{
	if (picName == "")
		return;  //贴图的名字不能是空的
	bool bExit = PicExit(picName);  //贴图是否已经存在
	if (bExit)
		return;
	//创建纹理
	bool IsValid = false;//载入的图片是否有效
	int32 width = 0;
	int32 height = 0;
	UTexture* texture = LoadTexture2D(picName, IsValid, width, height);
	if (texture)
	{
		FTextureStruct textureTemp;
		textureTemp.texName = picName;
		textureTemp.texturePtr = texture;
		m_textureList.Add(textureTemp);
	}
	return;
}

//创建纹理，纹理来自压缩文件中
//参数是纹理的名字，比如 奥米茄tex.bmp
bool UTextureManager::AddZipPic(const FString& texURL, void* picture)
{
	if (texURL.IsEmpty() || !picture)
		return false;
	//贴图是否已经存在
	bool bExit = PicExit(texURL);
	if (bExit)
		return true;
	//创建纹理
	bool IsValid = false;//载入的图片是否有效
	int32 width = 0;
	int32 height = 0;
	UTexture* texture = LoadTexture2D(picture); 
	if (texture)
	{
		FTextureStruct textureTemp;
		textureTemp.texName = texURL;
		textureTemp.texturePtr = texture;
		m_textureList.Add(textureTemp);
		return true;
	}
	return false;
}

// 查找贴图
UTexture* UTextureManager::FindPic(FString picName)
{
	if (picName == "")
		return nullptr;
	for (int i = 0; i < (int)m_textureList.Num(); i++)
	{
		if (m_textureList[i].texName.Equals(picName) || m_textureList[i].texName.Contains(picName))
		{
			if (!m_textureList[i].texturePtr.IsPending()) 
			{//资源有效
				return m_textureList[i].texturePtr.Get();
			}
			else 
			{//资源无效，则重新加载
				return m_textureList[i].texturePtr.LoadSynchronous();
			}
		}
	}
	return nullptr;
}

//贴图是否已经存在
bool UTextureManager::PicExit(FString picName)
{
	for (int i = 0; i < (int)m_textureList.Num(); i++)
	{
		if (m_textureList[i].texName.Equals(picName))
		{
			if (!m_textureList[i].texturePtr.IsPending())
			{//资源有效
				return true;
			}
			else
			{
				m_textureList.RemoveAt(i);
				return false;
			}
		}
	}
	return false;
}

// 二分均值缩放 BGRA Format
void MinifyByTwoScale(const uint8* SrcData, uint8* dstData, int srcWidth, int srcHeight)
{
	int x, y, x2, y2;
	int TgtWidth, TgtHeight;
	TgtWidth = srcWidth / 2;
	TgtHeight = srcHeight / 2;
	for (y = 0; y < TgtHeight; y++)
	{
		y2 = (y << 1);
		for (x = 0; x < TgtWidth; x++)
		{
			x2 = (x << 1);

			int p_Temp = (y2 * srcWidth + x2) << 2;
			int p_TempNext = (y2 * srcWidth + x2 + 1) << 2;

			int q_Temp = ((y2 + 1) * srcWidth + x2) << 2;
			int q_TempNext = ((y2 + 1) * srcWidth + x2 + 1) << 2;

			int dst_Temp = (y * TgtWidth + x) << 2;

			uint8 pb = AVERAGE(SrcData[p_Temp + 0], SrcData[p_TempNext + 0]);
			uint8 pg = AVERAGE(SrcData[p_Temp + 1], SrcData[p_TempNext + 1]);
			uint8 pr = AVERAGE(SrcData[p_Temp + 2], SrcData[p_TempNext + 2]);
			uint8 pa = AVERAGE(SrcData[p_Temp + 3], SrcData[p_TempNext + 3]);

			uint8 qb = AVERAGE(SrcData[q_Temp + 0], SrcData[q_TempNext + 0]);
			uint8 qg = AVERAGE(SrcData[q_Temp + 1], SrcData[q_TempNext + 1]);
			uint8 qr = AVERAGE(SrcData[q_Temp + 2], SrcData[q_TempNext + 2]);
			uint8 qa = AVERAGE(SrcData[q_Temp + 3], SrcData[q_TempNext + 3]);


			dstData[dst_Temp + 0] = AVERAGE(pb, qb);
			dstData[dst_Temp + 1] = AVERAGE(pg, qg);
			dstData[dst_Temp + 2] = AVERAGE(pr, qr);
			dstData[dst_Temp + 3] = AVERAGE(pa, qa);
		}
	}
}

//载入图像,图片在本地文件夹中
//参数ImagePath是绝对路径
UTexture* UTextureManager::LoadTexture2D(const FString& ImagePath, bool& IsValid, int32& OutWidth, int32& OutHeight)
{
	UTexture2D* Texture = nullptr;
	IsValid = false;
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*ImagePath))
	{
		return nullptr;
	}
	TArray<uint8> CompressedData;
	if (!FFileHelper::LoadFileToArray(CompressedData, *ImagePath))
	{
		return nullptr;
	}
	TSharedPtr<IImageWrapper> ImageWrapper = GetImageWrapperByExtention(ImagePath);
	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(CompressedData.GetData(), CompressedData.Num()))
	{
		TArray<uint8> UncompressedRGBA;
		if (ImageWrapper->GetRaw(ERGBFormat::BGRA, 8, UncompressedRGBA))
		{
			Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);//改成PF_B8G8R8A8之后正常了
			if (Texture != nullptr)
			{
				IsValid = true;
				OutWidth = ImageWrapper->GetWidth();
				OutHeight = ImageWrapper->GetHeight();
				void* TextureData = Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(TextureData, UncompressedRGBA.GetData(), UncompressedRGBA.Num());
				Texture->PlatformData->Mips[0].BulkData.Unlock();
				//生成mipmap，否则会产生摩尔纹,必须确保图片长宽都是2的N次方或为正方形图片,会导致非2的N次方的非方形图片拉伸变形
				if (1)
				{
					uint32 curW = FMath::Max<uint32>(OutWidth >> 1, 1);
					uint32 curH = FMath::Max<uint32>(OutHeight >> 1, 1);
					const uint8* pre_data = (uint8*)TextureData;
					int pow_W = (int)FMath::Log2(OutWidth);
					int pow_H = (int)FMath::Log2(OutHeight);
					int powNum = FMath::Max(pow_W, pow_H);
					for (int i = 0; i < powNum; i++)
					{
						int32 NumBytes = (curW * curH) * 4;
						FTexture2DMipMap* Mip = new FTexture2DMipMap();
						Texture->PlatformData->Mips.Add(Mip);

						Mip->SizeX = curW;
						Mip->SizeY = curH;

						Mip->BulkData.Lock(LOCK_READ_WRITE);
						Mip->BulkData.Realloc(NumBytes);
						Mip->BulkData.Unlock();

						uint8* Data = static_cast<uint8*>(Mip->BulkData.Lock(LOCK_READ_WRITE));
						int newWidth = (i < pow_W ? (curW * 2) : 1);
						int newHeight = (i < pow_H ? (curH * 2) : 1);

						MinifyByTwoScale(pre_data,Data,newWidth,newHeight);

						pre_data = Data;

						Mip->BulkData.Unlock();

						curW = FMath::Max<uint32>(curW >> 1, 1);
						curH = FMath::Max<uint32>(curH >> 1, 1);
					}
				}
				Texture->UpdateResource();
			}
		}
	}
	return Texture;
}

//获得图片格式
TSharedPtr<IImageWrapper> UTextureManager::GetImageWrapperByExtention(const FString InImagePath)
{
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	if (InImagePath.EndsWith(".png"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	}
	else if (InImagePath.EndsWith(".jpg") || InImagePath.EndsWith(".jpeg"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
	}
	else if (InImagePath.EndsWith(".bmp"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP);
	}
	else if (InImagePath.EndsWith(".ico"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::ICO);
	}
	else if (InImagePath.EndsWith(".exr"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::EXR);
	}
	else if (InImagePath.EndsWith(".icns"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::ICNS);
	}

	return nullptr;
}

//创建贴图,贴图在内存中
UTexture* UTextureManager::LoadTexture2D(void* picture)
{
	UTexture2D* Texture = nullptr;
	CPicture* pPicture = (CPicture*)picture;
	if (!pPicture)
		return Texture;
	Texture = UTexture2D::CreateTransient(pPicture->GetWidth(), pPicture->GetHeight(), PF_R8G8B8A8);
	if (Texture != nullptr)
	{
		void* TextureData = Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
		FMemory::Memcpy(TextureData, pPicture->GetPixels(), pPicture->getSize());
		Texture->PlatformData->Mips[0].BulkData.Unlock();
		//生成mipmap，否则会产生摩尔纹,必须确保图片长宽都是2的N次方或为正方形图片,会导致非2的N次方的非方形图片拉伸变形
		if (1)
		{
			uint32 curW = FMath::Max<uint32>(pPicture->GetWidth() >> 1, 1);
			uint32 curH = FMath::Max<uint32>(pPicture->GetHeight() >> 1, 1);
			const uint8* pre_data = (uint8*)TextureData;
			int pow_W = (int)FMath::Log2(pPicture->GetWidth());
			int pow_H = (int)FMath::Log2(pPicture->GetHeight());
			int powNum = FMath::Max(pow_W, pow_H);
			for (int i = 0; i < powNum; i++)
			{
				int32 NumBytes = (curW * curH) * 4;
				FTexture2DMipMap* Mip = new FTexture2DMipMap();
				Texture->PlatformData->Mips.Add(Mip);

				Mip->SizeX = curW;
				Mip->SizeY = curH;

				Mip->BulkData.Lock(LOCK_READ_WRITE);
				Mip->BulkData.Realloc(NumBytes);
				Mip->BulkData.Unlock();

				uint8* Data = static_cast<uint8*>(Mip->BulkData.Lock(LOCK_READ_WRITE));
				int newWidth = (i < pow_W ? (curW * 2) : 1);
				int newHeight = (i < pow_H ? (curH * 2) : 1);

				MinifyByTwoScale(pre_data, Data, newWidth, newHeight);

				pre_data = Data;

				Mip->BulkData.Unlock();

				curW = FMath::Max<uint32>(curW >> 1, 1);
				curH = FMath::Max<uint32>(curH >> 1, 1);
			}
		}
		Texture->UpdateResource();
	}
	return Texture;
}