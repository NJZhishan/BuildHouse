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
	//��������άƽ���ڣ�xoy,xoz,yoz�������߶εĽ���ֱ����������ϵĲ���
	static bool GetT_CrossPointonTwoLinesOnXOZ(FVector b1, FVector e1, FVector b2, FVector e2, float& t1, float& t2);
	//��������άƽ���ڣ�xoz�������߶εĽ��㣺���ò�������������߶εĽ���
	static int CrossPointonTwoLinesOnXOZ(FVector b1, FVector e1, FVector b2, FVector e2, FVector& crosspoint);///����ֵ��boolΪint
	//��vPoint�����߶�(vA<->vB)����ľ���
	static float ClosestDistanceOnLineSegment(FVector vA, FVector vB, FVector vPoint, FVector& CorrespontPoint);
};
