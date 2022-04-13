// Fill out your copyright notice in the Description page of Project Settings.


#include "CollisionBFL.h"

int UCollisionBFL::CrossPointonTwoLinesOnXOZ(FVector b1, FVector e1, FVector b2, FVector e2, FVector& crosspoint)
{
	//jiang gai 2018.11.1 采用新的算法，复制cutpolygon中的
	double tempX1 = e1.X - b1.X;
	double tempY1 = e1.Y - b1.Y;
	double tempX2 = e2.X - b2.X;
	double tempY2 = e2.Y - b2.Y;

	//jiang gai 2019.6.30 消除误差干扰,认为小于0.0001就在一条线上
	if (fabs(tempX1) < 0.0001)
		tempX1 = 0;
	if (fabs(tempY1) < 0.0001)
		tempY1 = 0;
	if (fabs(tempX2) < 0.0001)
		tempX2 = 0;
	if (fabs(tempY2) < 0.0001)
		tempY2 = 0;


	double d = tempX2 * tempY1 - tempY2 * tempX1;  //求t2时的分母；
	double mindeal = 0.0001;// 设置误差 rao改 2014.8.10
	mindeal = mindeal / 10;//jiang gai 2016.3.18  平行误差放为/10，否则还会存在把相交认为是平行或重合 原来为/2  //jiang gai 2015.10.8 把平行误差再放一半jsfe
	//if(fabs(d)>0.001f) //d=0;两线平行或重合
	if (fabs(d) > mindeal) // 两线平行或重合 rao改 2013.12.24
	{ // 两线不平行或重合
		double   t2 = (tempY1 * b1.X - tempX1 * b1.Y - b2.X * tempY1 + b2.Y * tempX1) / d; //交点在首线段的参数
		double   t1 = (b1.X * tempY2 - b1.Y * tempX2 - tempY2 * b2.X + b2.Y * tempX2) / d; //交点次线段上的参数

		if ((t1 >= 0.0f - mindeal && t1 <= 1.0f + mindeal) &&
			(t2 >= 0.0f - mindeal && t2 <= 1.0f + mindeal))// 在线段上的交点 
		{
			//交点在两条线段上
			// rao改 2013.11.22
			// 取误差小的点
			if (fabs(t1 - 1.0) > mindeal && fabs(t1 - 0.0) > mindeal)
			{
				crosspoint.X = b1.X + tempX1 * t1;//交点坐标 
				crosspoint.Y = b1.Y + tempY1 * t1;
				crosspoint.Z = 0.0f;
			}
			else
			{
				crosspoint.X = b2.X + tempX2 * t2;//交点坐标 
				crosspoint.Y = b2.Y + tempY2 * t2;
				crosspoint.Z = 0.0f;
			}
			if (fabs(crosspoint.X) < 0.00001f)
			{
				crosspoint.X = 0.0f;
			}
			if (fabs(crosspoint.Y) < 0.00001f)
			{
				crosspoint.Y = 0.0f;
			}
			return 1;
		}
		else// 延长线上的交点
		{
			// rao改 2013.11.22
			// 取误差小的点
			if (fabs(t1 - 1.0) > mindeal && fabs(t1 - 0.0) > mindeal)
			{
				crosspoint.X = b1.X + tempX1 * t1;//交点坐标 
				crosspoint.Y = b1.Y + tempY1 * t1;
				crosspoint.Z = 0.0f;
			}
			else
			{
				crosspoint.X = b2.X + tempX2 * t2;//交点坐标 
				crosspoint.Y = b2.Y + tempY2 * t2;
				crosspoint.Z = 0.0f;
			}
			if (fabs(crosspoint.X) < 0.00001f)
			{
				crosspoint.X = 0.0f;
			}
			if (fabs(crosspoint.Y) < 0.00001f)
			{
				crosspoint.Y = 0.0f;
			}
			return 2;
		}
	}
	else
	{
		//两线平行或重合
		return 0;
	}
}

//检测特殊二维平面内（xoy,xoz,yoz）两条线段的交点分别在两条线上的参数
bool UCollisionBFL::GetT_CrossPointonTwoLinesOnXOZ(FVector b1, FVector e1, FVector b2, FVector e2, float& t1, float& t2)
{
	float   tempX1 = e1.X - b1.X;
	float   tempY1 = e1.Y - b1.Y;
	float   tempX2 = e2.X - b2.X;
	float   tempY2 = e2.Y - b2.Y;

	float   d = tempX2 * tempY1 - tempY2 * tempX1;  //求t2时的分母；   
	//jiang 1.19
	if (fabs(d) > 0.001) //d=0;两线平行或重合
	{ // 两线不平行或重合
		t2 = (tempY1 * b1.X - tempX1 * b1.Y - b2.X * tempY1 + b2.Y * tempX1) / d; //交点在次线段的参数
		t1 = (b1.X * tempY2 - b1.Y * tempX2 - tempY2 * b2.X + b2.Y * tempX2) / d; //交点首线段上的参数
		return true;
	}
	else
	{
		//两线平行或重合
		return false;
	}
}

//********************************************************
//点vPoint距离线段(vA<->vB)最近的距离：如果vPoint在(vA<->vB)上的投影点在vA外侧，则返回vPoint-vA的长度
//如果vPoint在(vA<->vB)上的投影点在vB外侧，则返回vPoint-vB的长度;如果投影点在vA-vB内，则返回vPoint和投影点的
//距离//jiang 12.28 CorrespontPoint为vpoint在vA-vB上的投影点
float UCollisionBFL::ClosestDistanceOnLineSegment(FVector vA, FVector vB, FVector vPoint, FVector& CorrespontPoint)
{
	//获得从vA指向vPoint的向量vVector1
	FVector vVector1 = vPoint-vA;

	//获得有向线段vA->vB的方向向量vVector2
	FVector vVector2 = vB - vA;
	vVector2.Normalize();

	//有向线段vA->vB的长度d
	float d = FVector(vA-vB).Size();

	//利用点积获得向量vVector1在向量vVector2上的投影
	//该投影代表了最近点离vA的距离,正代表在vA->vB的方向上
	//负代表在vB->vA的方向上
	float t = FVector::DotProduct(vVector1, vVector2);

	//如果最近点在线段上,则计算vA到最近点的向量vVector3
	FVector vVector3 = vVector2 * t;
	//计算最近点vClosestPoint
	CorrespontPoint = vA+ vVector3;

	//如果小于0
	//则最近点线段端点vA
	if (t <= 0.0f)
	{
		return FVector(vA-vPoint).Size();
	}

	//如果大于线段长度d
	//则最近点线段端点vB
	if (t >= d)
		return FVector(vB - vPoint).Size();

	float dis = FVector(CorrespontPoint - vPoint).Size();///wang 11.1.8 sqrt(fabs(math.Length(vA,vPoint)*math.Length(vA,vPoint)-t*t));
	//返回距离
	return dis;
}