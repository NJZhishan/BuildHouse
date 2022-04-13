// Fill out your copyright notice in the Description page of Project Settings.


#include "CHouse.h"
#include "../Public/CollisionBFL.h"

CHouse::CHouse()
{
}

CHouse::~CHouse()
{
	m_wall.clear();
}

//jiang 2.11 �ж����Ӷ˵�ǽ���Ƿ�ɼ�,��begin-end���ڵ�ͬ��ǽ����Ϊ���ɼ�
void CHouse::ADJustWallVisible(FVector begin, FVector end, std::vector<CWallIlnkInfo>& walllink, std::vector<bool>& wallflag)
{
	int i;
	FVector b1, e1, dir1, dir2;
	float cos;
	for (i = 0; i < (int)walllink.size(); i++)
	{
		if (wallflag[walllink[i].WallIndex])
		{
			b1 = m_wall[walllink[i].WallIndex].middle_vertex[0];
			e1 = m_wall[walllink[i].WallIndex].middle_vertex[1];
			dir1 = b1 - e1;
			dir1.Normalize();
			dir2 = begin - end;
			dir2.Normalize();
			cos = fabs(FVector::DotProduct(dir1, dir2));
			if (cos > 0.99)
			{
				//˵������ǽ���һ����,walllink[i].WallIndex����Ӧ��ǽ��Ҳ���ɼ�
				wallflag[walllink[i].WallIndex] = false;
				//������wallflag[walllink[i].WallIndex]�����Ӷ˼��
				if (walllink[i].PointIndex == 0)
					ADJustWallVisible(b1, e1, m_wall[walllink[i].WallIndex].EndlinkInfo, wallflag);
				else
					ADJustWallVisible(b1, e1, m_wall[walllink[i].WallIndex].BeginLinkInfo, wallflag);
			}
		}
	}
}

//�������ǽ����point��������ľ����Լ�������Ӧ��ǽ������
float CHouse::GetMinDistanceToPoint(FVector point, int& WallIndex, FVector& MinDisPoint)
{
	point.Z = 0.0f;
	float MinDis = 1000;///maxDis = 1000;
	FVector Thepoint;
	for (int i = 0; i < (int)m_wall.size(); i++)
	{
		// qyy gai 2017.12.12 ��ֱ��ǽ�ͻ���ǽ�����ҿ�����
		if (1/*m_wall.at(i).type == 0*/)
		{
			if (m_wall[i].middle_vertex.empty())//jiang gai 2018.7.5 ȷ����Ч
				continue;
			float dis = UCollisionBFL::ClosestDistanceOnLineSegment(m_wall[i].middle_vertex[0], m_wall[i].middle_vertex[1], point, Thepoint);
			if (dis < MinDis)
			{
				WallIndex = i;
				MinDis = dis;
				MinDisPoint = Thepoint;
				/// return 0.0f;
			}
		}
		/*else if (m_wall.at(i).type == 3)
		{
			float dis = FLT_MAX;
			if (CV3Operate::FindNearProjectPoint(point, m_wall.at(i).middle_vertex, Thepoint, dis) && dis < MinDis)
			{
				WallIndex = i;
				MinDis = dis;
				MinDisPoint = Thepoint;
			}
		}*/
	}
	return MinDis;/// maxDis;
}
