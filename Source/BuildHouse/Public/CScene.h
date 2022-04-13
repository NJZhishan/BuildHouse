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
#include "Windows/PostWindowsApi.h"            //һ��ΪWindows�еĺ��UE4��ͻ����,�����ܻᵼ�³�ͻ�ĺ����ڵ�hom�ļ���������
/**
 * ��������
 */
class BUILDHOUSE_API CScene
{
public:
	CScene();
	~CScene();
public:
	void InitScene();  //��ʼ������
	bool ReadSceneFile(FString sceneFile, UWorld* worldContex);  //��ȡ������Ϣ�ļ�
	bool Init3DFirstPersonCharator();                            //��ʼ����ά�е�һ�˳�����ʱcharator��λ��
	void UpdateRoomVisiblity(TArray<int>floorVisible);            //���¿ɼ���
	void ModifyVisibility(const FVector& cameraPos, const FVector& cameraDir); //�����ӵ�λ�õ����ɼ���
	void DelObjectFlagForFloors(OBJECT_STYLE styleindex, AModulBaseActor*delActor);//�ڷ���ɾ�������ƾߣ������е�floor�����ù������
	void AddObjectFlagForFloors(OBJECT_STYLE styleindex, AModulBaseActor* newSpawnActor,FVector pos);//�ڷ��з��������ƾߣ������е�floor�����ù������
	void ModifyObjectFlagForFloors(OBJECT_STYLE styleindex, AModulBaseActor* movedActor, FVector pos);//�ڷ����ƶ������ƾߣ������е�floor�����ù������ jiang10.18
	AHouseGroupActor* GetInWhichGroup(AActor *model);  //�ж����ĸ�Ⱥ����
	void DeleteGroup(AHouseGroupActor* delGroup);      //ɾ��Ⱥ��
	void InitSceneLight();   //��ʼ�������ƹ�
private:
	bool InitSceneModelBase();         //��ʼ�������е�modulbase
	void ClearVisibility();            //��տɼ���
	int InWhichRoom(FVector  pos);    //�����ĸ�����
public:
	UWorld* MyWorldContex;                 //�൱��GetWorld

	CHouse* MyHouse;                       //����������ë����
	std::vector<AModulBaseActor*> m_CeilingList; //���������ĵ���
	std::vector<AModulBaseActor*> m_FloorList; //���������ĵ���
	std::vector<AModulBaseActor*> m_MergeWallfaceList; //�����������ں���
	std::vector<CStyle>StyleList;           //���������е�ģ��
	std::vector<CFloorObjectFlag> m_FloorObjectFlagList;  //ÿ������Ŀɼ���
	std::vector<AHouseGroupActor*> m_GroupList;     //�����е�Ⱥ��

	TArray<int> m_floorVisibleIndexList;  //�ɼ���������
	std::vector<bool> FloorFlags;         //����ɼ��ԣ�ʵ�ʿɼ�����m_floorVisibleIndexList����)
	std::vector<bool> CeilingFlags;		  // �����ɼ����� ��ʵ�ʿɼ���
	std::vector<bool> HouseWallFlag;	  // ǽ��ɼ���ǣ�ʵ�ʿɼ���
	std::vector<bool> MergeWallFaceFlags; //�ں���ɼ����У�ʵ�ʿɼ���
	std::vector<CStyleObjectFlag> StyleFlag;// ���ж���ɼ����(ʵ�ʿɼ�)

	CObjectInScene* pInformation;                 //��������ʱ�������Ϣ
};
