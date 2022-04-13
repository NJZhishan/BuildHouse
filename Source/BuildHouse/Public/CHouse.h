// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CHouseWall.h"
#include <vector>
/**
 * 
 */
class BUILDHOUSE_API CHouse
{
public:
	CHouse();
	~CHouse();
public:
	float m_height;    //ǽ��
	std::vector<CHouseWall>m_wall;  //���е�ǽ��
public:
	//jiang 2.11 �ж����Ӷ˵�ǽ���Ƿ�ɼ�,��begin-end���ڵ�ͬ��ǽ����Ϊ���ɼ�
	void ADJustWallVisible(FVector begin, FVector end, std::vector<CWallIlnkInfo>& walllink, std::vector<bool>& wallflag);
	//�������ǽ����point��������ľ����Լ�������Ӧ��ǽ������
	float GetMinDistanceToPoint(FVector point, int& WallIndex, FVector& MinDisPoint);
};
