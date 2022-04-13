// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <vector>
#include "ModulBaseActor.h"
//墙体端连通信息
class BUILDHOUSE_API CWallIlnkInfo
{
public:
	int WallIndex;  //所连接墙体的序号
	int PointIndex; //所连接墙体端点的序号：0或1 
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
	FString wallName;                           //墙体名字，和fbx文件中的一致
	AModulBaseActor* wallModelBase;             //墙体
	std::vector<FVector>middle_vertex;          //墙体中轴线
	std::vector<CWallIlnkInfo> BeginLinkInfo;   //墙体前端连通信息
	std::vector<CWallIlnkInfo> EndlinkInfo;     //墙体后端连通信息
	std::vector<int>mergeList;                  //墙体对应的融合面
};
