// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <vector>
#include "ModulBaseActor.h"
//ǽ�����ͨ��Ϣ
class BUILDHOUSE_API CWallIlnkInfo
{
public:
	int WallIndex;  //������ǽ������
	int PointIndex; //������ǽ��˵����ţ�0��1 
	CWallIlnkInfo()
	{
		WallIndex = -1;
		PointIndex = -1;
	}
};

/**
 * 
 */
class BUILDHOUSE_API CHouseWall
{
public:
	CHouseWall();
	~CHouseWall();
public:
	FString wallName;                           //ǽ�����֣���fbx�ļ��е�һ��
	AModulBaseActor* wallModelBase;             //ǽ��
	std::vector<FVector>middle_vertex;          //ǽ��������
	std::vector<CWallIlnkInfo> BeginLinkInfo;   //ǽ��ǰ����ͨ��Ϣ
	std::vector<CWallIlnkInfo> EndlinkInfo;     //ǽ������ͨ��Ϣ
	std::vector<int>mergeList;                  //ǽ���Ӧ���ں���
};
