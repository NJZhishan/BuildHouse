// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CollisionBFL.generated.h"

/**
 * 
 */
UCLASS()
class BUILDHOUSE_API UCollisionBFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	//检测特殊二维平面内（xoy,xoz,yoz）两条线段的交点分别在两条线上的参数
	static bool GetT_CrossPointonTwoLinesOnXOZ(FVector b1, FVector e1, FVector b2, FVector e2, float& t1, float& t2);
	//检测特殊二维平面内（xoz）两条线段的交点：采用参数法检测两条线段的交点
	static int CrossPointonTwoLinesOnXOZ(FVector b1, FVector e1, FVector b2, FVector e2, FVector& crosspoint);///返回值改bool为int
	//点vPoint距离线段(vA<->vB)最近的距离
	static float ClosestDistanceOnLineSegment(FVector vA, FVector vB, FVector vPoint, FVector& CorrespontPoint);
};
