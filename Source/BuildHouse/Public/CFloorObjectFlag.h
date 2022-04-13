// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include<vector>
//对象可见标记
class BUILDHOUSE_API CStyleObjectFlag
{
public:
	std::vector<bool> ObjectFlag;
};

//        (不存在,		 客饭厅,     卧室,		 卫生间,    厨房,			阳台,		一般房间,		衣帽间)///wql 2018.12.29 新增的房间类型要按顺序放在最后，因为模板素材中记录了这个值
enum ROOM { RM_NONE = -1, RM_PARLOR, RM_BEDROOM, RM_TOILET, RM_KITCHEN, RM_BALCONY, RM_COMMON, RM_COAT };
/**
 * 房间可见性
 */
class BUILDHOUSE_API CFloorObjectFlag
{
public:
	CFloorObjectFlag();
	~CFloorObjectFlag();

public:
	int FloorIndex;                                  // 本身所属的Floor序列的序号
	FString FloorName;                              //floor的名字，和导出到fbx中的一致
	ROOM m_roomStyle;                               //房间类型，来自fbx文件
	std::vector<bool> OldMergeWallFaceFlags;		//融合墙面可见序列，，来自fbx文件
	std::vector<bool> OldCeilingFlags;		        //吊顶可见序列（固有可见） ，来自fbx文件
	std::vector<bool> OldHouseWallFlag;	            //墙体可见标记（固有可见），来自fbx文件
	std::vector<CStyleObjectFlag> OldStyleFlag;     //屋中对象可见标记	，来自fbx文件
};
