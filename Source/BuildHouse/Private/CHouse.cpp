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

//jiang 2.11 判断连接端的墙体是否可见,与begin-end相邻的同面墙都设为不可见
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
				//说明两段墙体成一条线,walllink[i].WallIndex所对应的墙体也不可见
				wallflag[walllink[i].WallIndex] = false;
				//继续对wallflag[walllink[i].WallIndex]的连接端检测
				if (walllink[i].PointIndex == 0)
					ADJustWallVisible(b1, e1, m_wall[walllink[i].WallIndex].EndlinkInfo, wallflag);
				else
					ADJustWallVisible(b1, e1, m_wall[walllink[i].WallIndex].BeginLinkInfo, wallflag);
			}
		}
	}
}

//获得所有墙体与point距离最近的距离以及其所对应的墙体的序号
float CHouse::GetMinDistanceToPoint(FVector point, int& WallIndex, FVector& MinDisPoint)
{
	point.Z = 0.0f;
	float MinDis = 1000;///maxDis = 1000;
	FVector Thepoint;
	for (int i = 0; i < (int)m_wall.size(); i++)
	{
		// qyy gai 2017.12.12 分直线墙和弧形墙进行找靠近点
		if (1/*m_wall.at(i).type == 0*/)
		{
			if (m_wall[i].middle_vertex.empty())//jiang gai 2018.7.5 确认有效
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
