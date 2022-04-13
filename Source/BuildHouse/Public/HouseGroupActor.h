// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DownloadFile.h"
#include <vector>
#include <fstream>
#include <iostream>
#include "HouseGroupActor.generated.h"

/**
 *表示群组单元
 */
USTRUCT(BlueprintType)
struct BUILDHOUSE_API FHouseGroupUnitStruct
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(VisibleAnywhere)
	FString groupUnitName;   //对象的名称
public:
	FHouseGroupUnitStruct() { InitHouseGroupUnitStruct(); }
	void InitHouseGroupUnitStruct()
	{//初始化
		groupUnitName = TEXT("");
	}
};

/***********************************************
作者： 姜晓彤   创建时间 ： 2010.2.13
功能描述：该类用于描述被旋转轴的信息以及出现的次数
************************************************/
class CAxisInfor
{
public:
	FVector axis; //旋转轴
	int num; //具有相同轴的数目

	CAxisInfor() { num = 0; };
};

/************************************群组******************************************/
UCLASS(Blueprintable)
class BUILDHOUSE_API AHouseGroupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHouseGroupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	void SetGroupName(FString name);            //设置group的名字
	FString ReturnGroupName();                  //返回group的名字
	void SetProdInfor(FDownloadInforStruct infor);  //设置产品信息
	FDownloadInforStruct ReturnProdInfor();         //获得产品信息
	void SetOutline(bool outLine);              //描边
	void GetAllUnit(TArray<FString>&unitList);  //获得所有单元
	void AddGroupUnit(FString name);            //添加单元
	void ReComputeMinMax();                     //计算群组的位置
	void AdjustAxisLists(std::vector<CAxisInfor>& axislists, FVector axis);
	void GetAllAxis(std::vector<CAxisInfor>& axislists);
	void MoveGroup(FVector delta);              //移动群组
	void RotateGroup(float newAngle);           //旋转群组
	UFUNCTION(BlueprintCallable)
	void SplitGroup();                      //拆分群组

	/*********************素材库中的群组文件读取*************************************************/
	void ReadGroup(const FString& fileName); //创建群组，参数是群组所在txt文件的绝对路径
	void ReadGroup_1(std::ifstream& file);
	void ReadGroup_2(std::ifstream& file);
	void ReadGroup_3(std::ifstream& file);
	void ReadGroup_4(std::ifstream& file);
	void ReadGroup_5(std::ifstream& file);
	void ReadGroup_6(std::ifstream& file);
	void ReadGroup_7(std::ifstream& file); 
	/*********************素材库中的群组文件读取*************************************************/
public:
	UPROPERTY()
	FString m_txtPath;    //模型使用txt的名字，比如5667.txt
	UPROPERTY(VisibleAnywhere)
	TArray<FHouseGroupUnitStruct>m_GroupUnitList;   //对象列表
	FVector m_MinCoord, m_MaxCoord;                 //组的最大、最小坐标
	float m_RotateAngle;                            //群组旋转角度
	FVector m_RotateAxis;				            //群旋转轴
	FVector m_groupNormal;				            //放置群组点的法向（只有当群组txt在本地不存在时才使用这个变量）
	FVector m_groupPosition;                        //群组位置（只有当群组txt在本地不存在时才使用这个变量）
private:
	UPROPERTY()
	FString m_GroupName;  //产品名称，比如，A客厅吊灯，为了每个group的名字是唯一的，设置名字:styleIndex_ObjectIndex_产品名称_编号
	FDownloadInforStruct m_GroupInfor;   //产品信息
	int m_PositionState; //2012.8.15 受限状态：0：地面、1：墙面；2：吊顶；3，任意；4：垂直向上//jiang gai 2017.10.17 3和4对调
};
