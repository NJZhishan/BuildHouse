// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CHouse.h"
#include "ModulBaseActor.h"
#include "CFloorObjectFlag.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Core/Public/Containers/Array.h"
#include "CStyle.h"
#include "HouseGroupActor.h"

#include "Windows/PreWindowsApi.h"
#include "../../../ThirdParty/WLJ_ProjectDLL_TestLibrary/include/ObjectInScene.h"
#include "Windows/PostWindowsApi.h"            //一般为Windows中的宏和UE4冲突所致,将可能会导致冲突的宏所在的hom文件包裹起来
/**
 * 整个场景
 */
class BUILDHOUSE_API CScene
{
public:
	CScene();
	~CScene();
public:
	void InitScene();  //初始化场景
	bool ReadSceneFile(FString sceneFile, UWorld* worldContex);  //读取场景信息文件
	bool Init3DFirstPersonCharator();                            //初始化三维中第一人称漫游时charator的位置
	void UpdateRoomVisiblity(TArray<int>floorVisible);            //更新可见性
	void ModifyVisibility(const FVector& cameraPos, const FVector& cameraDir); //根据视点位置调整可见性
	void DelObjectFlagForFloors(OBJECT_STYLE styleindex, AModulBaseActor*delActor);//在房中删除物体或灯具，在所有的floor中设置关联标记
	void AddObjectFlagForFloors(OBJECT_STYLE styleindex, AModulBaseActor* newSpawnActor,FVector pos);//在房中放了物体或灯具，在所有的floor中设置关联标记
	void ModifyObjectFlagForFloors(OBJECT_STYLE styleindex, AModulBaseActor* movedActor, FVector pos);//在房中移动物体或灯具，在所有的floor中设置关联标记 jiang10.18
	AHouseGroupActor* GetInWhichGroup(AActor *model);  //判断在哪个群组中
	void DeleteGroup(AHouseGroupActor* delGroup);      //删除群组
	void InitSceneLight();   //初始化场景灯光
private:
	bool InitSceneModelBase();         //初始化场景中的modulbase
	void ClearVisibility();            //清空可见性
	int InWhichRoom(FVector  pos);    //点在哪个房间
public:
	UWorld* MyWorldContex;                 //相当于GetWorld

	CHouse* MyHouse;                       //整个场景的毛坯房
	std::vector<AModulBaseActor*> m_CeilingList; //整个场景的吊顶
	std::vector<AModulBaseActor*> m_FloorList; //整个场景的地面
	std::vector<AModulBaseActor*> m_MergeWallfaceList; //整个场景的融合面
	std::vector<CStyle>StyleList;           //整个场景中的模型
	std::vector<CFloorObjectFlag> m_FloorObjectFlagList;  //每个房间的可见性
	std::vector<AHouseGroupActor*> m_GroupList;     //场景中的群组

	TArray<int> m_floorVisibleIndexList;  //可见房间的序号
	std::vector<bool> FloorFlags;         //地面可见性（实际可见，由m_floorVisibleIndexList控制)
	std::vector<bool> CeilingFlags;		  // 吊顶可见序列 （实际可见）
	std::vector<bool> HouseWallFlag;	  // 墙体可见标记（实际可见）
	std::vector<bool> MergeWallFaceFlags; //融合面可见序列（实际可见）
	std::vector<CStyleObjectFlag> StyleFlag;// 屋中对象可见标记(实际可见)

	CObjectInScene* pInformation;                 //创建对象时所需的信息
};
