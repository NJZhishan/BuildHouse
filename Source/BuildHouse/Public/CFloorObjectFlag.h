// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include<vector>
//����ɼ����
class BUILDHOUSE_API CStyleObjectFlag
{
public:
	std::vector<bool> ObjectFlag;
};

//        (������,		 �ͷ���,     ����,		 ������,    ����,			��̨,		һ�㷿��,		��ñ��)///wql 2018.12.29 �����ķ�������Ҫ��˳����������Ϊģ���ز��м�¼�����ֵ
enum ROOM { RM_NONE = -1, RM_PARLOR, RM_BEDROOM, RM_TOILET, RM_KITCHEN, RM_BALCONY, RM_COMMON, RM_COAT };
/**
 * ����ɼ���
 */
class BUILDHOUSE_API CFloorObjectFlag
{
public:
	CFloorObjectFlag();
	~CFloorObjectFlag();

public:
	int FloorIndex;                                  // ����������Floor���е����
	FString FloorName;                              //floor�����֣��͵�����fbx�е�һ��
	ROOM m_roomStyle;                               //�������ͣ�����fbx�ļ�
	std::vector<bool> OldMergeWallFaceFlags;		//�ں�ǽ��ɼ����У�������fbx�ļ�
	std::vector<bool> OldCeilingFlags;		        //�����ɼ����У����пɼ��� ������fbx�ļ�
	std::vector<bool> OldHouseWallFlag;	            //ǽ��ɼ���ǣ����пɼ���������fbx�ļ�
	std::vector<CStyleObjectFlag> OldStyleFlag;     //���ж���ɼ����	������fbx�ļ�
};
