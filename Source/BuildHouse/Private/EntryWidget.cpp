// Fill out your copyright notice in the Description page of Project Settings.


#include "EntryWidget.h"
#include "../Public/ReadFbx.h"
#include "Runtime/Engine/Classes/Kismet/KismetTextLibrary.h"
#include "../Public/BuildHouseGameInstance.h"
#include "../Public/BuildHousePlayerController.h"
#include "../Public/BuildHouseCharacter.h"
#include "../Public/ProdInforWidget.h"


 //读取fbx文件
void UEntryWidget::ReadFbx()
{
	UReadFbx *obj = NewObject<UReadFbx>(this,TEXT("ReadFbx"));  //创建UObject对象，不需要手动释放
	obj->ReadFbxFile();
}

//更新下载进度
//prog的值在0到1之间
void UEntryWidget::UpdateDownloadProgress(float prog)
{
	m_fDownloadProgress = prog;
	FString Text1 = FString(TEXT("下载进度:")) + FString::Printf(TEXT("%.2f"), prog * 100) + FString(TEXT("%"));
	m_downloadProgress =FText::FromString(Text1);
	return;
}

//保存拖拽的模型信息
void UEntryWidget::SaveDragProdInfor(FString infor)
{
	if (GetWorld())
	{
		UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
		if (gameInstance)
		{
			//取消上次选中物体的描边
			AModulBaseActor* lastSel = gameInstance->ReturnSelectModulBaseActor();
			if (lastSel)
			{
				lastSel->SetOutline(false);
			}
			//设置没有物体被选中
			gameInstance->SetSelectModulBaseActor(nullptr);
			//保存拖拽信息
			gameInstance->SetDragInfor(infor);
		}
	}
	return;
}

//拖拽完成
void UEntryWidget::DragOver(FString mousePos)
{
	if (GetWorld())
	{
		UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
		if (gameInstance)
		{
			gameInstance->HandleDragOver();  //拖拽完成
		}
	}
	return;
}

//二维三维切换
void UEntryWidget::ChangViewType(FString viewType)
{
	if (GetWorld())
	{
		ABuildHouseCharacter* charactor = Cast<ABuildHouseCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
		if (charactor)
		{
			charactor->ChangeViewType(viewType);
		}
	}
}

//墙体吸附
void UEntryWidget::SetWallAdsorb(bool adsorb)
{
	if (GetWorld())
	{
		UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
		if (gameInstance)
		{
			gameInstance->SetWallAdsorb(adsorb);
		}
	}
	return;
}

//物体吸附
void UEntryWidget::SetObjectAdsorb(bool adsorb)
{
	if (GetWorld())
	{
		UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
		if (gameInstance)
		{
			gameInstance->SetObjectAdsorb(adsorb);
		}
	}
	return;
}

//更新产品信息
//没有选中产品，则显示户型信息
//选中产品，则显示产品信息
void UEntryWidget::UpdateProdInfor(AModulBaseActor* selModel,AHouseGroupActor* selGroup)
{
	UProdInforWidget* prodInforUI = Cast<UProdInforWidget>(GetWidgetFromName(TEXT("UI_Prodinfor")));
	if (prodInforUI)
	{
		if (selModel == nullptr && selGroup==nullptr)
		{
			prodInforUI->UpdateHouseUI();
		}
		else
		{
			prodInforUI->UpdateProdInfor(selModel,selGroup);
		}
	}
	return;
}