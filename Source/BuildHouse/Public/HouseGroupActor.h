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
 *��ʾȺ�鵥Ԫ
 */
USTRUCT(BlueprintType)
struct BUILDHOUSE_API FHouseGroupUnitStruct
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(VisibleAnywhere)
	FString groupUnitName;   //���������
public:
	FHouseGroupUnitStruct() { InitHouseGroupUnitStruct(); }
	void InitHouseGroupUnitStruct()
	{//��ʼ��
		groupUnitName = TEXT("");
	}
};

/***********************************************
���ߣ� ����ͮ   ����ʱ�� �� 2010.2.13
��������������������������ת�����Ϣ�Լ����ֵĴ���
************************************************/
class CAxisInfor
{
public:
	FVector axis; //��ת��
	int num; //������ͬ�����Ŀ

	CAxisInfor() { num = 0; };
};

/************************************Ⱥ��******************************************/
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
	void SetGroupName(FString name);            //����group������
	FString ReturnGroupName();                  //����group������
	void SetProdInfor(FDownloadInforStruct infor);  //���ò�Ʒ��Ϣ
	FDownloadInforStruct ReturnProdInfor();         //��ò�Ʒ��Ϣ
	void SetOutline(bool outLine);              //���
	void GetAllUnit(TArray<FString>&unitList);  //������е�Ԫ
	void AddGroupUnit(FString name);            //��ӵ�Ԫ
	void ReComputeMinMax();                     //����Ⱥ���λ��
	void AdjustAxisLists(std::vector<CAxisInfor>& axislists, FVector axis);
	void GetAllAxis(std::vector<CAxisInfor>& axislists);
	void MoveGroup(FVector delta);              //�ƶ�Ⱥ��
	void RotateGroup(float newAngle);           //��תȺ��
	UFUNCTION(BlueprintCallable)
	void SplitGroup();                      //���Ⱥ��

	/*********************�زĿ��е�Ⱥ���ļ���ȡ*************************************************/
	void ReadGroup(const FString& fileName); //����Ⱥ�飬������Ⱥ������txt�ļ��ľ���·��
	void ReadGroup_1(std::ifstream& file);
	void ReadGroup_2(std::ifstream& file);
	void ReadGroup_3(std::ifstream& file);
	void ReadGroup_4(std::ifstream& file);
	void ReadGroup_5(std::ifstream& file);
	void ReadGroup_6(std::ifstream& file);
	void ReadGroup_7(std::ifstream& file); 
	/*********************�زĿ��е�Ⱥ���ļ���ȡ*************************************************/
public:
	UPROPERTY()
	FString m_txtPath;    //ģ��ʹ��txt�����֣�����5667.txt
	UPROPERTY(VisibleAnywhere)
	TArray<FHouseGroupUnitStruct>m_GroupUnitList;   //�����б�
	FVector m_MinCoord, m_MaxCoord;                 //��������С����
	float m_RotateAngle;                            //Ⱥ����ת�Ƕ�
	FVector m_RotateAxis;				            //Ⱥ��ת��
	FVector m_groupNormal;				            //����Ⱥ���ķ���ֻ�е�Ⱥ��txt�ڱ��ز�����ʱ��ʹ�����������
	FVector m_groupPosition;                        //Ⱥ��λ�ã�ֻ�е�Ⱥ��txt�ڱ��ز�����ʱ��ʹ�����������
private:
	UPROPERTY()
	FString m_GroupName;  //��Ʒ���ƣ����磬A�������ƣ�Ϊ��ÿ��group��������Ψһ�ģ���������:styleIndex_ObjectIndex_��Ʒ����_���
	FDownloadInforStruct m_GroupInfor;   //��Ʒ��Ϣ
	int m_PositionState; //2012.8.15 ����״̬��0�����桢1��ǽ�棻2��������3�����⣻4����ֱ����//jiang gai 2017.10.17 3��4�Ե�
};
