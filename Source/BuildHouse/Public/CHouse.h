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
	float m_height;    //墙高
	std::vector<CHouseWall>m_wall;  //所有的墙体
public:
	//jiang 2.11 判断连接端的墙体是否可见,与begin-end相邻的同面墙都设为不可见
	void ADJustWallVisible(FVector begin, FVector end, std::vector<CWallIlnkInfo>& walllink, std::vector<bool>& wallflag);
	//获得所有墙体与point距离最近的距离以及其所对应的墙体的序号
	float GetMinDistanceToPoint(FVector point, int& WallIndex, FVector& MinDisPoint);
};
