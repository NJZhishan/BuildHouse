// Fill out your copyright notice in the Description page of Project Settings.


#include "ProdInforWidget.h"
#include "../Public/BuildHouseGameInstance.h"     

//限制模型的角度,限制模型的角度在0到360之间
float UProdInforWidget::TransAngle(float oldAngle)
{
	if (FMath::IsNearlyZero(oldAngle))
		return oldAngle;
	int bPositive = oldAngle / fabs(oldAngle);  //角度是正数还是负数
	double temp2;
	float temp1 = modf(fabs(oldAngle)/360.0f,&temp2);//获得整数部分
	return  bPositive * (fabs(oldAngle) - temp2 * 360);
}

//更新产品信息
void UProdInforWidget::UpdateProdInfor(AModulBaseActor* selModel, AHouseGroupActor* selGroup)
{
	if (selModel == nullptr && selGroup == nullptr)
		return;
	FProdInforStruct prodInfor;
	FString prodUrl;   //群组或者模型的url
	FString prodName;  //群组或者模型的名字
	if (selGroup)
	{//显示群组信息
		prodUrl = selGroup->ReturnProdInfor().url;
		prodName = selGroup->ReturnGroupName();
		prodInfor.bModel = true;//是否是模型
		//群组的旋转
		prodInfor.yaw = selGroup->m_RotateAngle;  //群组只绕着旋转轴旋转
		//群组的缩放
		FVector min = selGroup->m_MinCoord * 10; //转化成mm
		FVector max = selGroup->m_MaxCoord * 10;
		prodInfor.length = max.Y - min.Y;
		prodInfor.width = max.X - min.X;
		prodInfor.height = max.Z - min.Z;
	}
	else
	{//显示单个模型信息
		prodUrl = selModel->ReturnProdInfor().url;
		prodName = selModel->ReturnModelName();
		prodInfor.bModel = selModel->ActorHasTag(TEXT("普通模型"));  //是否是模型
		FRotator rot = selModel->GetActorRotation();
		for (int i = 0; i < (int)selModel->MeshComponentList.Num(); i++)
		{
			rot = selModel->MeshComponentList[i]->GetRelativeRotation();
			break;
		}
		prodInfor.pitch = TransAngle(rot.Pitch);  //旋转，保证角度在0到360之间
		prodInfor.yaw = TransAngle(rot.Yaw);
		prodInfor.roll = TransAngle(rot.Roll);
	}
	if (prodInfor.bModel)
	{//模型或者群组
		if (prodUrl.IsEmpty())
		{//模型是从fbx文件中读入的，没有具体的信息
			FString modelName = prodName;
			int index = -1;
			if (modelName.FindChar('_', index))
			{
				modelName = modelName.Right(modelName.Len() - 1 - index);
				if (modelName.FindChar('_', index))
				{
					modelName = modelName.Right(modelName.Len() - 1 - index);
				}
			}
			prodInfor.prodName = modelName;  //产品名称
			if (!selGroup && selModel)
			{
				FBox box = selModel->CalculateComponentsBoundingBoxInLocalSpace(true, false);
				FVector min = box.Min * 10;  //转化成mm
				FVector max = box.Max * 10;
				prodInfor.length = max.Y - min.Y;
				prodInfor.width = max.X - min.X;
				prodInfor.height = max.Z - min.Z;
			}
		}
		else
		{//模型是从素材库中拖入的，有具体信息
			prodInfor.prodName = selModel->ReturnProdInfor().info[11];  //产品名称
			prodInfor.brand = selModel->ReturnProdInfor().info[0];   //品牌
			FString sizeStr = selModel->ReturnProdInfor().info[3];   //尺寸
			int npos = sizeStr.Find(TEXT("*"));
			if (npos > -1)
			{
				FString lenthStr = sizeStr.Left(npos);
				prodInfor.length = FCString::Atof(*lenthStr); //长度
				sizeStr = sizeStr.Right(sizeStr.Len() - 1 - npos);
				npos = sizeStr.Find(TEXT("*"));
				if (npos > -1)
				{
					FString widthStr = sizeStr.Left(npos);
					prodInfor.width = FCString::Atof(*widthStr); //宽度
					FString heightStr = sizeStr.Right(sizeStr.Len() - 1 - npos);
					prodInfor.height = FCString::Atof(*widthStr); //高度
				}
			}
		}
		FVector scaleVec = FVector::OneVector;
		if (selGroup)
		{//群组的缩放
			scaleVec = selGroup->GetActorScale();
		}
		else if(selModel)
		{//单个模型的缩放
			scaleVec = selModel->GetActorScale();
		}
		prodInfor.lengthScale = scaleVec.Y;  //缩放
		prodInfor.widthScale = scaleVec.X;
		prodInfor.heightScale = scaleVec.Z;
		if (scaleVec.AllComponentsEqual())
		{
			prodInfor.equalScale = true;   //等比例缩放
		}
		else
		{
			prodInfor.equalScale = false;   //不是等比例缩放
		}
	}
	else
	{
		prodInfor.prodName = selModel->ReturnModelName();  //产品名称
	}
	UpdateUI(prodInfor);  //更新UI,太繁琐了，所以在蓝图中实现吧
}

//缩放模型
void UProdInforWidget::ScaleModel(float lenthScale, float widthScale, float heightScale)
{
	UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
	if (gameInstance)
	{
		AModulBaseActor* lastSel = gameInstance->ReturnSelectModulBaseActor();
		AHouseGroupActor* lastSelGroup = gameInstance->ReturnSelectGroupActor();
		if (lastSelGroup)
		{//缩放群组
			lastSelGroup->SetActorScale3D(FVector(widthScale, lenthScale, heightScale));
		}
		else if (lastSel && lastSel->ActorHasTag(TEXT("普通模型")))
		{//缩放单个模型
			lastSel->SetActorScale3D(FVector(widthScale, lenthScale, heightScale));
		}
	}
	return;
}

//旋转模型
void UProdInforWidget::RotateModel(RotationType rotType, float delta)
{
	UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
	if (gameInstance)
	{
		AModulBaseActor* lastSel = gameInstance->ReturnSelectModulBaseActor();
		if (lastSel)
		{
			lastSel->RotateModel(rotType, delta);
		}
	}
	return;
}

//旋转模型
void UProdInforWidget::RotateModelSelf(float newAngle)
{
	UBuildHouseGameInstance* gameInstance = Cast<UBuildHouseGameInstance>(GetWorld()->GetGameInstance());
	if (gameInstance)
	{
		AModulBaseActor* lastSel = gameInstance->ReturnSelectModulBaseActor();
		AHouseGroupActor* lastSelGroup = gameInstance->ReturnSelectGroupActor();
		if (lastSelGroup)
		{//旋转群组
			lastSelGroup->RotateGroup(newAngle);
		}
		else if (lastSel)
		{//旋转单个模型
			lastSel->RotateModel(newAngle);
		}
	}
	return;
}