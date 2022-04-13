// Fill out your copyright notice in the Description page of Project Settings.


#include "CollisionBFL.h"

int UCollisionBFL::CrossPointonTwoLinesOnXOZ(FVector b1, FVector e1, FVector b2, FVector e2, FVector& crosspoint)
{
	//jiang gai 2018.11.1 �����µ��㷨������cutpolygon�е�
	double tempX1 = e1.X - b1.X;
	double tempY1 = e1.Y - b1.Y;
	double tempX2 = e2.X - b2.X;
	double tempY2 = e2.Y - b2.Y;

	//jiang gai 2019.6.30 ����������,��ΪС��0.0001����һ������
	if (fabs(tempX1) < 0.0001)
		tempX1 = 0;
	if (fabs(tempY1) < 0.0001)
		tempY1 = 0;
	if (fabs(tempX2) < 0.0001)
		tempX2 = 0;
	if (fabs(tempY2) < 0.0001)
		tempY2 = 0;


	double d = tempX2 * tempY1 - tempY2 * tempX1;  //��t2ʱ�ķ�ĸ��
	double mindeal = 0.0001;// ������� rao�� 2014.8.10
	mindeal = mindeal / 10;//jiang gai 2016.3.18  ƽ������Ϊ/10�����򻹻���ڰ��ཻ��Ϊ��ƽ�л��غ� ԭ��Ϊ/2  //jiang gai 2015.10.8 ��ƽ������ٷ�һ��jsfe
	//if(fabs(d)>0.001f) //d=0;����ƽ�л��غ�
	if (fabs(d) > mindeal) // ����ƽ�л��غ� rao�� 2013.12.24
	{ // ���߲�ƽ�л��غ�
		double   t2 = (tempY1 * b1.X - tempX1 * b1.Y - b2.X * tempY1 + b2.Y * tempX1) / d; //���������߶εĲ���
		double   t1 = (b1.X * tempY2 - b1.Y * tempX2 - tempY2 * b2.X + b2.Y * tempX2) / d; //������߶��ϵĲ���

		if ((t1 >= 0.0f - mindeal && t1 <= 1.0f + mindeal) &&
			(t2 >= 0.0f - mindeal && t2 <= 1.0f + mindeal))// ���߶��ϵĽ��� 
		{
			//�����������߶���
			// rao�� 2013.11.22
			// ȡ���С�ĵ�
			if (fabs(t1 - 1.0) > mindeal && fabs(t1 - 0.0) > mindeal)
			{
				crosspoint.X = b1.X + tempX1 * t1;//�������� 
				crosspoint.Y = b1.Y + tempY1 * t1;
				crosspoint.Z = 0.0f;
			}
			else
			{
				crosspoint.X = b2.X + tempX2 * t2;//�������� 
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
		else// �ӳ����ϵĽ���
		{
			// rao�� 2013.11.22
			// ȡ���С�ĵ�
			if (fabs(t1 - 1.0) > mindeal && fabs(t1 - 0.0) > mindeal)
			{
				crosspoint.X = b1.X + tempX1 * t1;//�������� 
				crosspoint.Y = b1.Y + tempY1 * t1;
				crosspoint.Z = 0.0f;
			}
			else
			{
				crosspoint.X = b2.X + tempX2 * t2;//�������� 
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
		//����ƽ�л��غ�
		return 0;
	}
}

//��������άƽ���ڣ�xoy,xoz,yoz�������߶εĽ���ֱ����������ϵĲ���
bool UCollisionBFL::GetT_CrossPointonTwoLinesOnXOZ(FVector b1, FVector e1, FVector b2, FVector e2, float& t1, float& t2)
{
	float   tempX1 = e1.X - b1.X;
	float   tempY1 = e1.Y - b1.Y;
	float   tempX2 = e2.X - b2.X;
	float   tempY2 = e2.Y - b2.Y;

	float   d = tempX2 * tempY1 - tempY2 * tempX1;  //��t2ʱ�ķ�ĸ��   
	//jiang 1.19
	if (fabs(d) > 0.001) //d=0;����ƽ�л��غ�
	{ // ���߲�ƽ�л��غ�
		t2 = (tempY1 * b1.X - tempX1 * b1.Y - b2.X * tempY1 + b2.Y * tempX1) / d; //�����ڴ��߶εĲ���
		t1 = (b1.X * tempY2 - b1.Y * tempX2 - tempY2 * b2.X + b2.Y * tempX2) / d; //�������߶��ϵĲ���
		return true;
	}
	else
	{
		//����ƽ�л��غ�
		return false;
	}
}

//********************************************************
//��vPoint�����߶�(vA<->vB)����ľ��룺���vPoint��(vA<->vB)�ϵ�ͶӰ����vA��࣬�򷵻�vPoint-vA�ĳ���
//���vPoint��(vA<->vB)�ϵ�ͶӰ����vB��࣬�򷵻�vPoint-vB�ĳ���;���ͶӰ����vA-vB�ڣ��򷵻�vPoint��ͶӰ���
//����//jiang 12.28 CorrespontPointΪvpoint��vA-vB�ϵ�ͶӰ��
float UCollisionBFL::ClosestDistanceOnLineSegment(FVector vA, FVector vB, FVector vPoint, FVector& CorrespontPoint)
{
	//��ô�vAָ��vPoint������vVector1
	FVector vVector1 = vPoint-vA;

	//��������߶�vA->vB�ķ�������vVector2
	FVector vVector2 = vB - vA;
	vVector2.Normalize();

	//�����߶�vA->vB�ĳ���d
	float d = FVector(vA-vB).Size();

	//���õ���������vVector1������vVector2�ϵ�ͶӰ
	//��ͶӰ�������������vA�ľ���,��������vA->vB�ķ�����
	//��������vB->vA�ķ�����
	float t = FVector::DotProduct(vVector1, vVector2);

	//�����������߶���,�����vA������������vVector3
	FVector vVector3 = vVector2 * t;
	//���������vClosestPoint
	CorrespontPoint = vA+ vVector3;

	//���С��0
	//��������߶ζ˵�vA
	if (t <= 0.0f)
	{
		return FVector(vA-vPoint).Size();
	}

	//��������߶γ���d
	//��������߶ζ˵�vB
	if (t >= d)
		return FVector(vB - vPoint).Size();

	float dis = FVector(CorrespontPoint - vPoint).Size();///wang 11.1.8 sqrt(fabs(math.Length(vA,vPoint)*math.Length(vA,vPoint)-t*t));
	//���ؾ���
	return dis;
}