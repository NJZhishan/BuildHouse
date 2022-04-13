// Fill out your copyright notice in the Description page of Project Settings.


#include "CHouseWall.h"

CHouseWall::CHouseWall()
{
	wallModelBase = nullptr;
}

CHouseWall::~CHouseWall()
{
	middle_vertex.clear();         
	BeginLinkInfo.clear(); 
	EndlinkInfo.clear();     
	mergeList.clear();    
	wallModelBase = nullptr; //是否是在清空场景时释放，不在这里释放
}
