// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModulBaseActor.h"
#include <vector>
/**
 * 
 */
class BUILDHOUSE_API CStyle
{
public:
	CStyle();
	~CStyle();
public:
	std::vector<AModulBaseActor*>pObjectList;
};
