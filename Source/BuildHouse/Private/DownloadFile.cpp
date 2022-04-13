// Fill out your copyright notice in the Description page of Project Settings.


#include "../Public/DownloadFile.h"
#include "Runtime/Online/HTTP/Public/HttpModule.h"
#include "Runtime/Online/HTTP/Public/Interfaces/IHttpResponse.h"
#include "EntryWidget.h"
#include "../Public/BuildHouseGameInstance.h"
#include "Runtime/UMG/Public/Blueprint/WidgetBlueprintLibrary.h"
#include "Runtime/Core/Public/Misc/FileHelper.h"
//�����ļ�
void UDownloadFile::DownloadFile(FString URL)
{
	TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
	HttpRequest->OnProcessRequestComplete().BindUObject(this, &UDownloadFile::HandleDownloadRequest);
	HttpRequest->OnRequestProgress().BindUObject(this, &UDownloadFile::HandleHttpRequestProgress);
	HttpRequest->SetURL(URL);
	HttpRequest->SetVerb(TEXT("GET"));
	HttpRequest->ProcessRequest();
}

//������ɵ���
void UDownloadFile::HandleDownloadRequest(FHttpRequestPtr HttpRequest, FHttpResponsePtr HttpResponse, bool bSucceeded)
{
	if (!HttpRequest.IsValid() || !HttpResponse.IsValid()) 
	{

	}
	else if (bSucceeded && EHttpResponseCodes::IsOk(HttpResponse->GetResponseCode())) 
	{
		FString Filename = FPaths::GetCleanFilename(HttpRequest->GetURL());
		//�������������Ƴ�
		for (int i = m_DownloadList.Num() - 1; i >= 0; i--)
		{
			if (m_DownloadList[i].Find(Filename) > -1)
			{
				m_DownloadList.RemoveAt(i);
			}
		}
		//�����ļ�
		FString FilePath = FString::Printf(TEXT("%s%s%s"), *FPaths::ProjectDir(), TEXT("model_zy/"), *Filename);
		FFileHelper::SaveArrayToFile(HttpResponse->GetContent(), *FilePath);
		if (GetWorld())
		{
			UBuildHouseGameInstance* instance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
			instance->OnDownloadEnd(FilePath);
		}
	}
	if (m_DownloadList.Num() > 0)
	{//��������
		DownloadFile(m_DownloadList[0]);
	}
	return;
}

//�������ؽ���
void UDownloadFile::HandleHttpRequestProgress(FHttpRequestPtr HttpRequest, int32 BytesSent, int32 BytesReceived) 
{
	if (GetWorld())
	{
		const FHttpResponsePtr Response = HttpRequest->GetResponse();
		if (Response.IsValid())
		{
			const int32 FullSize = Response->GetContentLength();  //�ļ����ܴ�С
			for (TObjectIterator<UUserWidget> Itr; Itr; ++Itr)
			{
				UUserWidget* LiveWidget = *Itr;
				UEntryWidget* entryWidget = Cast<UEntryWidget>(LiveWidget);
				if (entryWidget && entryWidget->IsInViewport())
				{
					entryWidget->UpdateDownloadProgress((float)BytesReceived / (float)FullSize);
				}
			}
		}
	}
	return;
}
