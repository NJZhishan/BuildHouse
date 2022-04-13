#include "GeoMaths.h"
#include <fstream>
int m_mark = 1;
CGeoMaths::CGeoMaths()
{

}

CGeoMaths::~CGeoMaths()
{

}

///***********************************************
//作者： 王庆利   
//功能描述：判断点是否在某边界内或相交
//输入参数：无
//输出参数：无
//返回参数说明：在外返回0, 在边上返回1,在内返回2
//************************************************/
//int CGeoMaths::bPointInEdge( CVector3 &pt, std::vector<CVector3> &edgePoint, int axis )
//{
//	CVector2 temp;
//	std::vector<CVector2> polygon;
//	for(int i=0; i<(int)edgePoint.size(); i++)
//	{
//		temp.u = edgePoint[i].x;
//		temp.v = edgePoint[i].z;
//		polygon.push_back(temp);
//	}
//
//	temp.u = pt.x;
//	temp.v = pt.z;
//	CCollision cll;
//	int t = cll.bPointIsInPolygon2D(temp, polygon);
//	return t;///(t==0)?false:true;
//
//	return 0;
//}
//
///***********************************************
//作者： 王庆利   
//功能描述：判断线段是否在某边界内或相交
//输入参数：无
//输出参数：无
//返回参数说明：无
//************************************************/
//bool CGeoMaths::UncrossAndIn(PointStruct &p1, PointStruct &p2, std::vector<PointStruct> &edgePoint, bool bHole ) //jiang gai 2019.9.25  增加参数bHole
//{
//	//CMaths math;
//	//float equalZero = 0.00001f;
//	//float error = equalZero;//0.001f;//jiang gai 2015.7.21 减少误差范围，防止破面三角出现
//	//CCollision collision;
//	//CVector2 middlePoint = { 0.0, 0.0 }, pt1 = { 0.0, 0.0 }, pt2 = { 0.0, 0.0 };///连线的中点、及两端点
//	//pt1.u = p1.points.x; pt1.v = p1.points.z;
//	//pt2.u = p2.points.x; pt2.v = p2.points.z;
//	//middlePoint.u = (p1.points.x + p2.points.x) / 2;
//	//middlePoint.v = (p1.points.z + p2.points.z) / 2;
//	//std::vector<CVector2> tempPoint;
//	//CVector2 temp = { 0.0, 0.0 };
//	//for (int i = 0; i < (int)edgePoint.size(); i++)//转化成collision函数所需要的参数类型
//	//{
//	//	temp.u = edgePoint[i].points.x;
//	//	temp.v = edgePoint[i].points.z;
//	//	tempPoint.push_back(temp);
//	//}
//
//	//if (bPointIsInPolygon(middlePoint, tempPoint)) //先判断是否在多边形内，在外肯定不行
//	//{///在内则检测是否有相交
//	//	for (int i = 0; i < (int)tempPoint.size(); i++)
//	//	{
//	//		// 重复点排除 rao改 2014.9.25，因为pt1，pt2也是轮廓上的点，所以肯定也相交，但是这样的相交没有关系，所以continue
//	//		if (math.Length2D(tempPoint[i], pt1)<equalZero || math.Length2D(tempPoint[i], pt2)<equalZero
//	//			|| math.Length2D(tempPoint[(i + 1) % tempPoint.size()], pt1)<equalZero
//	//			|| math.Length2D(tempPoint[(i + 1) % tempPoint.size()], pt2)<equalZero)
//	//			//continue;
//	//		{
//	//			//jiang gai 2015.6.25 只有在排除了pt1-pt2边和tempPoint[i]-tempPoint[(i+1)完全重合之后，才能continue
//	//			// 判断是否共线 rao改 2014.9.1
//	//			CVector3 point1 = { pt1.u, 0.0, pt1.v };
//	//			CVector3 point2 = { pt2.u, 0.0, pt2.v };
//	//			CVector3 point3 = { tempPoint[i].u, 0.0, tempPoint[i].v };
//	//			CVector3 point4 = { tempPoint[(i + 1) % tempPoint.size()].u, 0.0, tempPoint[(i + 1) % tempPoint.size()].v };
//	//			CVector3 point5;
//	//			float dis1 = collision.ClosestDistanceOnLineSegment(point1, point2, point3, point5);
//	//			//=math.Length(point3,point5);
//	//			float dis2 = collision.ClosestDistanceOnLineSegment(point1, point2, point4, point5);
//	//			//float dis2=math.Length(point4,point5);
//	//			if (dis1<error && dis2<error)// 共线
//	//			{
//	//				int tt = 0;					
//	//				if (bHole)
//	//				{///wql 2019.9.26 如果是挖洞则返回
//	//					return false;
//	//				}
//	//				continue;
//	//			}
//	//			else
//	//				continue;
//	//		}
//	//		CVector2 crosspoint = { 0.0, 0.0 };
//
//	//		// 如果有条边相交
//	//		if (collision.CrossPointonTwoLines2D(pt1, pt2, tempPoint[i], tempPoint[(i + 1) % tempPoint.size()], crosspoint))
//	//		{
//	//			if (math.Length2D(crosspoint, pt1)>error
//	//				&& math.Length2D(crosspoint, pt2)>error
//	//				&& math.Length2D(crosspoint, tempPoint[i])>error
//	//				&& math.Length2D(crosspoint, tempPoint[(i + 1) % tempPoint.size()])>error)
//	//			{	// 并且交点不是检测线断的端点，四个点都要判断 rao改 2014.8.10
//	//				return false;///有除端点外的其它交点，则不能剖分
//	//			}
//
//	//			// 判断是否共线 rao改 2014.9.1，共线也返回false
//	//			CVector3 point1 = { pt1.u, 0.0, pt1.v };
//	//			CVector3 point2 = { pt2.u, 0.0, pt2.v };
//	//			CVector3 point3 = { tempPoint[i].u, 0.0, tempPoint[i].v };
//	//			CVector3 point4 = { tempPoint[(i + 1) % tempPoint.size()].u, 0.0, tempPoint[(i + 1) % tempPoint.size()].v };
//	//			CVector3 point5;// 距离直线最近的点
//	//			collision.ClosestDistanceOnLineSegment(point1, point2, point3, point5);
//	//			float dis1 = math.Length(point3, point5);
//	//			collision.ClosestDistanceOnLineSegment(point1, point2, point4, point5);
//	//			float dis2 = math.Length(point4, point5);
//	//			if (dis1<error && dis2<error)// 共线
//	//				return false;
//	//		}
//
//	//	}
//	//}
//	//else{///不在内则肯定不能剖分
//	//	return false;
//	//}
//	//return true;
//	////jiang gai 2019.8.25 引入cutpolygon中的函数
//	//float equalZero = 0.00001f;
//	//CMaths math;
//	//float error = equalZero;//0.001f;//jiang gai 2015.7.21 减少误差范围，防止破面三角出现
//	//CCollision collision;
//	//CVector2 middlePoint = { 0.0, 0.0 }, pt1 = { 0.0, 0.0 }, pt2 = { 0.0, 0.0 };///连线的中点、及两端点
//	//pt1.u = p1.points.x; pt1.v = p1.points.z;
//	//pt2.u = p2.points.x; pt2.v = p2.points.z;
//	//middlePoint.u = (p1.points.x + p2.points.x) / 2;
//	//middlePoint.v = (p1.points.z + p2.points.z) / 2;
//	//std::vector<CVector2> tempPoint;
//	//CVector2 temp = { 0.0, 0.0 };
//	//for (int i = 0; i < (int)edgePoint.size(); i++)//转化成collision函数所需要的参数类型
//	//{
//	//	temp.u = edgePoint[i].points.x;
//	//	temp.v = edgePoint[i].points.z;
//	//	tempPoint.push_back(temp);
//	//}
//
//	//if (bPointIsInPolygon(middlePoint, tempPoint)) //先判断是否在多边形内，在外肯定不行
//	//{///在内则检测是否有相交
//	//	for (int i = 0; i < (int)tempPoint.size(); i++)
//	//	{
//	//		// 重复点排除 rao改 2014.9.25，因为pt1，pt2也是轮廓上的点，所以肯定也相交，但是这样的相交没有关系，所以continue
//	//		if (math.Length2D(tempPoint[i], pt1)<equalZero || math.Length2D(tempPoint[i], pt2)<equalZero
//	//			|| math.Length2D(tempPoint[(i + 1) % tempPoint.size()], pt1)<equalZero
//	//			|| math.Length2D(tempPoint[(i + 1) % tempPoint.size()], pt2)<equalZero)
//	//			//continue;
//	//		{
//	//			//jiang gai 2015.6.25 只有在排除了pt1-pt2边和tempPoint[i]-tempPoint[(i+1)完全重合之后，才能continue
//	//			// 判断是否共线 rao改 2014.9.1
//	//			CVector3 point1 = { pt1.u, 0.0, pt1.v };
//	//			CVector3 point2 = { pt2.u, 0.0, pt2.v };
//	//			CVector3 point3 = { tempPoint[i].u, 0.0, tempPoint[i].v };
//	//			CVector3 point4 = { tempPoint[(i + 1) % tempPoint.size()].u, 0.0, tempPoint[(i + 1) % tempPoint.size()].v };
//	//			CVector3 point5;
//	//			float dis1 = collision.ClosestDistanceOnLineSegment(point1, point2, point3, point5);
//	//			//=math.Length(point3,point5);
//	//			float dis2 = collision.ClosestDistanceOnLineSegment(point1, point2, point4, point5);
//	//			//float dis2=math.Length(point4,point5);
//	//			if (dis1<error && dis2<error)// 共线
//	//			{
//	//				int tt = 0;
//	//				return false;
//	//			}
//	//			else
//	//				continue;
//	//		}
//	//		CVector2 crosspoint = { 0.0, 0.0 };
//
//	//		// 如果有条边相交
//	//		if (collision.CrossPointonTwoLines2D(pt1, pt2, tempPoint[i], tempPoint[(i + 1) % tempPoint.size()], crosspoint))
//	//		{
//	//			if (math.Length2D(crosspoint, pt1)>error
//	//				&& math.Length2D(crosspoint, pt2)>error
//	//				&& math.Length2D(crosspoint, tempPoint[i])>error
//	//				&& math.Length2D(crosspoint, tempPoint[(i + 1) % tempPoint.size()])>error)
//	//			{	// 并且交点不是检测线断的端点，四个点都要判断 rao改 2014.8.10
//	//				return false;///有除端点外的其它交点，则不能剖分
//	//			}
//
//	//			// 判断是否共线 rao改 2014.9.1，共线也返回false
//	//			CVector3 point1 = { pt1.u, 0.0, pt1.v };
//	//			CVector3 point2 = { pt2.u, 0.0, pt2.v };
//	//			CVector3 point3 = { tempPoint[i].u, 0.0, tempPoint[i].v };
//	//			CVector3 point4 = { tempPoint[(i + 1) % tempPoint.size()].u, 0.0, tempPoint[(i + 1) % tempPoint.size()].v };
//	//			CVector3 point5;// 距离直线最近的点
//	//			collision.ClosestDistanceOnLineSegment(point1, point2, point3, point5);
//	//			float dis1 = math.Length(point3, point5);
//	//			collision.ClosestDistanceOnLineSegment(point1, point2, point4, point5);
//	//			float dis2 = math.Length(point4, point5);
//	//			if (dis1<error && dis2<error)// 共线
//	//				return false;
//	//		}
//
//	//	}
//	//}
//	//else{///不在内则肯定不能剖分
//	//	return false;
//	//}
//	//return true;
//	
//	CMaths math;
//	///double error = 0.00000000001;///误差
//	float error = 0.001f;
//	CCollision collision;
//	CVector2 middlePoint={0.0,0.0}, pt1={0.0,0.0}, pt2={0.0,0.0};///连线的中点、及两端点
//	pt1.u = p1.points.x; pt1.v = p1.points.z;
//	pt2.u = p2.points.x; pt2.v = p2.points.z;
//	middlePoint.u = (p1.points.x + p2.points.x)/2;
//	middlePoint.v = (p1.points.z + p2.points.z)/2;
//	std::vector<CVector2> tempPoint;
//	CVector2 temp={0.0,0.0};
//	for( int i = 0; i < (int)edgePoint.size(); i++ )///转化成collision函数所需要的参数类型
//	{
//		temp.u = edgePoint[i].points.x;
//		temp.v = edgePoint[i].points.z;
//		tempPoint.push_back( temp );
//	}
//
//	if( collision.bPointIsInPolygon2D( middlePoint,tempPoint ) )///先判断是否在多边形内
//	{///在内则检测是否有相交
//		for( int i = 0; i < (int)tempPoint.size(); i++ )
//		{
//			CVector2 crosspoint={0.0,0.0};
//			if( collision.CrossPointonTwoLines2D( pt1, pt2, tempPoint[i], tempPoint[ (i+1)%tempPoint.size() ], crosspoint) )
//			{///如果有条边相交
//				if( math.Length2D(crosspoint, pt1)>error
//					&& math.Length2D(crosspoint, pt2)>error)
//				{///并且交点不是检测线断的端点
//					return false;///有除端点外的其它交点，则不能剖分
//				}
//			}			
//		}
//	}else{///不在内则肯定不能剖分
//		return false;
//	}
//
//	return true;
//	
//}
//
///***********************************************
//作者： 王庆利   
//功能描述：线段是否在一个多边形内或者是多边形一条边
//输入参数：point1, point2线段, polygonGroup多边形
//输出参数：无
//返回参数说明：在内返回1, 是一条边返回2, 否则返回0
//************************************************/
//int CGeoMaths::InOrIsOneLineOfPolygon(const CVector3 &point1, const CVector3 &point2, std::vector<CPointsGroup>  &polygonGroup)
//{
//	CMaths math;
//	for(int i=0; i<(int)polygonGroup.size(); i++)
//	{
//		bool pt1In = false, pt2In = false;
//		for(int j=0; j<(int)polygonGroup[i].points.size(); j++)
//		{
//			if(!pt1In)
//			{
//				if(math.Length(point1, polygonGroup[i].points[j])<0.0001f)
//				{
//					pt1In = true;
//				}
//			}
//			if(!pt2In)
//			{
//				if(math.Length(point2, polygonGroup[i].points[j])<0.0001f)
//				{
//					pt2In = true;
//				}
//			}
//			if(pt1In && pt2In)
//			{
//				//是polygonGroup中的某个多边形的一条边
//				return true;
//			}
//		}
//	}
//	CCollision cll;
//	CVector2 middlePoint={0.0,0.0}, pt1={0.0,0.0}, pt2={0.0,0.0};///连线的中点、及两端点
//	pt1.u = point1.x; pt1.v = point1.z;
//	pt2.u = point2.x; pt2.v = point2.z;
//	middlePoint.u = (point1.x + point2.x)/2;
//	middlePoint.v = (point1.z + point2.z)/2;
//	for(int i=0; i<(int)polygonGroup.size(); i++)
//	{
//		std::vector<CVector2> tempPoint;
//		CVector2 temp={0.0,0.0};
//		for( int j = 0; j < (int)polygonGroup[i].points.size(); j++ )///转化成collision函数所需要的参数类型
//		{
//			temp.u = polygonGroup[i].points[j].x;
//			temp.v = polygonGroup[i].points[j].z;
//			tempPoint.push_back( temp );
//		}
//
//		if( 2==cll.bPointIsInPolygon2D( middlePoint,tempPoint ) )///先判断是否在多边形内
//		{
//			return true;
//		}
//	}
//
//	return 0;
//}
//
///***********************************************
//作者： 王庆利   
//功能描述：一组点中是否有点在三角形内
//输入参数：polygon三角形, polygonGroup点组
//输出参数：无
//返回参数说明：没有返回true
//************************************************/
//bool CGeoMaths::NoPointInTriangle(CVector3 polygon[], std::vector<CPointsGroup>  &polygonGroup)
//{
//	CCollision cll;
//	std::vector<CVector2> temp;
//	for(int i=0; i<3; i++)
//	{
//		CVector2 tp = {polygon[i].x, polygon[i].z};
//		temp.push_back(tp);
//	}
//	for(int i=0; i<(int)polygonGroup.size(); i++)///其他元素是否有点在新加元素边界内
//	{
//		for(int j=0; j<(int)polygonGroup[i].points.size(); j++)
//		{
//			CVector2 tp = {polygonGroup[i].points[j].x, polygonGroup[i].points[j].z};
//			if(cll.bPointIsInPolygon2D(tp, temp)==2)///.InsidePolygon(polygonGroup[i].points[j], polygon, 3))
//			{
//				return false;
//			}
//		}
//	}
//// 	for(int i=0; i<(int)m_vEdgePoint.size(); i++)
//// 	{
//// 		CVector2 tp = {m_vEdgePoint[i].x, m_vEdgePoint[i].z};
//// 		if(cll.bPointIsInPolygon2D(tp, temp)==2)///.InsidePolygon(m_vEdgePoint[i], polygon, 3))
//// 		{
//// 			return false;
//// 		}
//// 	}
//	return true;
//}
//
/////wql 2019.11.14 从三角化工具中移植过来
//bool CGeoMaths::UncrossAndIn(PointStruct &p1, PointStruct &p2, PointStruct &p3, std::vector<PointStruct> &edgePoint/*,int &num,int &storeNum*/)
//{
//	CMaths math;
//	float error = 0.001f;
//	const float equalZero = 0.00001f;
//	CCollision collision;
//	CVector2 middlePoint = { 0.0, 0.0 }, pt1 = { 0.0, 0.0 }, pt2 = { 0.0, 0.0 };///连线的中点、及两端点
//	pt1.u = p1.points.x; pt1.v = p1.points.z;
//	pt2.u = p2.points.x; pt2.v = p2.points.z;
//	//jiang gai 2015.7.31
//	CVector2 pt3 = { 0.0, 0.0 };//形成三角形的另一个点
//	pt3.u = p3.points.x; pt3.v = p3.points.z;
//	std::vector<CVector2> triPolygon;//三角形
//	triPolygon.push_back(pt1);
//	triPolygon.push_back(pt2);
//	triPolygon.push_back(pt3);
//
//	middlePoint.u = (p1.points.x + p2.points.x) / 2;
//	middlePoint.v = (p1.points.z + p2.points.z) / 2;
//	std::vector<CVector2> tempPoint;
//	CVector2 temp = { 0.0, 0.0 };
//	for (int i = 0; i < (int)edgePoint.size(); i++)///转化成collision函数所需要的参数类型
//	{
//		temp.u = edgePoint[i].points.x;
//		temp.v = edgePoint[i].points.z;
//		tempPoint.push_back(temp);
//	}
//
//	if (collision.bPointIsInPolygon(middlePoint, tempPoint))///先判断是否在多边形内
//	{///在内则检测是否有相交
//		for (int i = 0; i < (int)tempPoint.size(); i++)
//		{
//			// 重复点排除 rao改 2014.9.25
//			if (math.Length2D(tempPoint[i], pt1)<equalZero || math.Length2D(tempPoint[i], pt2)<equalZero
//				|| math.Length2D(tempPoint[(i + 1) % tempPoint.size()], pt1)<equalZero
//				|| math.Length2D(tempPoint[(i + 1) % tempPoint.size()], pt2)<equalZero)
//				continue;
//
//			CVector2 crosspoint = { 0.0, 0.0 };
//			// 如果有条边相交
//			if (collision.CrossPointonTwoLines2D(pt1, pt2, tempPoint[i], tempPoint[(i + 1) % tempPoint.size()], crosspoint))
//			{
//				if (math.Length2D(crosspoint, pt1)>equalZero///wql 2020.3.2 很小的区域划分时，端点的判断需要提供精度
//					&& math.Length2D(crosspoint, pt2)>equalZero
//					&& math.Length2D(crosspoint, tempPoint[i])>equalZero
//					&& math.Length2D(crosspoint, tempPoint[(i + 1) % tempPoint.size()])>equalZero)
//				{	// 并且交点不是检测线断的端点，四个点都要判断 rao改 2014.8.10  
//					return false;///有除端点外的其它交点，则不能剖分
//				}
//
//				// 判断是否共线 rao改 2014.9.1
//				CVector3 point1 = { pt1.u, 0.0, pt1.v };
//				CVector3 point2 = { pt2.u, 0.0, pt2.v };
//				CVector3 point3 = { tempPoint[i].u, 0.0, tempPoint[i].v };
//				CVector3 point4 = { tempPoint[(i + 1) % tempPoint.size()].u, 0.0, tempPoint[(i + 1) % tempPoint.size()].v };
//				CVector3 point5;// 距离直线最近的点
//				collision.ClosestDistanceOnLineSegment(point1, point2, point3, point5);
//				float dis1 = math.Length(point3, point5);
//				collision.ClosestDistanceOnLineSegment(point1, point2, point4, point5);
//				float dis2 = math.Length(point4, point5);
//				if (dis1<error && dis2<error)// 共线
//					return false;
//
//				//// add by dyt
//				//point1.x=pt1.u;
//				//point1.y=0;
//				//point1.z=pt1.v;
//				//point2.x=pt3.u;
//				//point2.y=0;
//				//point2.z=pt3.v;
//				//collision.ClosestDistanceOnLineSegment(point1,point2,point3,point5);
//				//dis1=math.Length(point3,point5);
//				//collision.ClosestDistanceOnLineSegment(point1,point2,point4,point5);
//				//dis2=math.Length(point4,point5);
//				//if(dis1<error && dis2<error)// 共线
//				//	return false;
//				//point1.x=pt2.u;
//				//point1.y=0;
//				//point1.z=pt2.v;
//				//point2.x=pt3.u;
//				//point2.y=0;
//				//point2.z=pt3.v;
//				//collision.ClosestDistanceOnLineSegment(point1,point2,point3,point5);
//				//dis1=math.Length(point3,point5);
//				//collision.ClosestDistanceOnLineSegment(point1,point2,point4,point5);
//				//dis2=math.Length(point4,point5);
//				//if(dis1<error && dis2<error)// 共线
//				//	return false;
//			}
//		}
//	}
//	else
//	{///不在内则肯定不能剖分
//		return false;
//	}
//	//jiang gai 2015.7.31 再判断是否有点在三角形的内部，如果有的话返回false
//	for (int i = 0; i<(int)tempPoint.size(); i++)
//	{
//		CVector3 point1 = { triPolygon.at(0).u, 0, triPolygon.at(0).v };
//		CVector3 point2 = { triPolygon.at(1).u, 0, triPolygon.at(1).v };
//		CVector3 point3 = { triPolygon.at(2).u, 0, triPolygon.at(2).v };
//		CVector3 point4 = { tempPoint.at(i).u, 0, tempPoint.at(i).v };
//		CVector3 point5;
//		collision.ClosestDistanceOnLineSegment(point1, point2, point4, point5);
//		float dist1 = math.Length(point4, point5);
//		collision.ClosestDistanceOnLineSegment(point1, point3, point4, point5);
//		float dist2 = math.Length(point4, point5);
//		collision.ClosestDistanceOnLineSegment(point2, point3, point4, point5);
//		float dist3 = math.Length(point4, point5);
//		if (m_mark == 1)
//		{
//			if (collision.bPointIsInPolygon(tempPoint[i], triPolygon) == 2)
//			{
//				/*edgePoint[i].isStored=false;*/
//				return false;
//			}
//		}
//		if (m_mark == 2)
//		{
//			if (collision.bPointIsInPolygon(tempPoint[i], triPolygon) == 2 && dist1>0.02&&dist2>0.02&&dist3>0.02)
//			{
//				/*edgePoint[i].isStored=false;*/
//				return false;
//			}
//		}
//	}
//	return true;
//}
//
/////wql 2019.9.18 私有函数，限制递归次数
//bool CGeoMaths::PolygonToTriangels_Recurse(std::vector<PointStruct> &edgePoint, std::vector<int> & faceVerId, int &recurseCount, bool bHole)  //jiang gai 2019.9.25  增加参数bHole
//{
//	int num = (int)edgePoint.size();///多边形顶点数
//	int storeNum = 0;///已处理的顶点数
//	//将点序列反向
//	if (m_mark == 2)
//	{
//		vector<PointStruct> tmppoint;
//		for (int i = 0; i<num; i++)
//		{
//			tmppoint.push_back(edgePoint.at(i));
//		}
//
//		for (int i = 0; i<num; i++)
//		{
//			edgePoint.at(i).points.x = tmppoint.at(num - 1 - i).points.x;
//			edgePoint.at(i).points.y = tmppoint.at(num - 1 - i).points.y;
//			edgePoint.at(i).points.z = tmppoint.at(num - 1 - i).points.z;
//		}
//	}
//
//	//add by dyt
//	while ((num - storeNum)>2 && m_mark<3)
//	{
//		///wql 2020.2.12 应该继续分割，而不是把前一次的分割清空
//		//if (m_mark>1)
//		//{
//		//	for (int i = 0; i<num; i++)
//		//		edgePoint.at(i).isStored = false;
//		//	faceVerId.clear();
//		//}
//		for (int i = 0; i < num; i++)///对每一点，先逆时针剖分，再顺时针剖分
//		{
//			if (!edgePoint[i].isStored)///该顶点是可划分顶点
//			{
//				if ((num - storeNum) > 3)///并且是多边形
//				{
//					int next1Indx = 0, next2Indx = 0;///下一点及下2点的索引基址
//					int nextNum = 0;///下两点之间的间隔数  rao改
//					bool turn = false;///是否需要反方向剖分
//					while (!turn)///不反向，则一直按逆时针剖分
//					{
//						if (!edgePoint[(i + 2 + next2Indx) % num].isStored) ///是可划分顶点
//						{
//							if (!edgePoint[(i + 1 + next1Indx + num) % num].isStored)// 邻点已被处理
//							{
//								//jiang gai 2015.7.31
//								if (UncrossAndIn(edgePoint[i], edgePoint[(i + 2 + next2Indx) % num], edgePoint[(i + 1 + next1Indx) % num], edgePoint))///  ,num,storeNum))///可以作为一个划分
//								{
//									if (edgePoint[(i + 1 + next1Indx) % num].isStored)
//									{
//										next1Indx++;
//									}   //这个判断意义是什么
//
//									faceVerId.push_back(i);///存储划分的三角面的顶点索引
//									faceVerId.push_back((i + 1 + next1Indx) % num);
//									faceVerId.push_back((i + 2 + next2Indx) % num);
//									int sid = 0;
//									sid = i;
//									///	fprintf(fp,"%d\n",sid);
//									sid = (i + 1 + next1Indx) % num;
//									///	fprintf(fp,"%d\n",sid);
//									sid = (i + 2 + next2Indx) % num;
//									///	fprintf(fp,"%d\n",sid);
//									///	fprintf(fp,"\n");
//
//									sid = (i + 1 + next1Indx) % num;
//									///	fprintf(fp,"被删除的点 %d\n",sid);
//									///	fprintf(fp,"\n");
//
//#ifdef _DEBUG ///wang 12.3
//									if (edgePoint[(i + 1 + next1Indx) % num].isStored)
//									{
//										MessageBox(_T("error"), NULL, NULL);
//									}
//#endif
//									edgePoint[(i + 1 + next1Indx) % num].isStored = true;///标志该顶点已被分割，不再参与以后的划分
//									storeNum++;///处理顶点数  + 
//									next1Indx++;///新剖分三角面的新边顶点索引  +
//									next2Indx++;
//
//									// raogai 2013.9.22
//									next1Indx += nextNum;
//									nextNum = 0;
//
//								}
//								else
//								{///不可划分，反向剖分
//									turn = true;
//								}
//							}
//							else
//							{
//								next1Indx++;///新剖分三角面的新边顶点索引  +
//								next2Indx++;
//							}
//						}
//						else{//隔点已被处理，则选再下一点
//							if (edgePoint[(i + 1 + next1Indx) % num].isStored)///如果邻点也被处理
//							{
//								next1Indx++;///新剖分三角面的新边顶点索引  +
//								next2Indx++;
//							}
//							else{
//								next2Indx++;
//								nextNum++;
//							}
//						}
//						if ((num - storeNum) == 3)
//						{
//							int tempIndx[3] = { 0, 0, 0 };///对于只剩余三个顶点的情况原来存点的索引方法有误 wang 7.28
//							int tempid = 0;
//							for (int j = 0; j < num; j++)
//							{
//								if (!edgePoint[j].isStored)
//								{
//									if (tempid>2)
//									{
//										break;;
//									}
//									tempIndx[tempid] = j;
//									tempid++;
//								}
//							}
//							if ((tempIndx[1] - tempIndx[0])*(tempIndx[2] - tempIndx[1])*(tempIndx[0] - tempIndx[2]) < 0)///逆时针排序
//							{
//								for (int j = 0; j<3; j++)
//								{
//									faceVerId.push_back(tempIndx[j]);
//									int sid = 0;
//									sid = tempIndx[j];
//									///fprintf(fp,"%d\n",sid);
//								}
//							}
//							else
//							{
//								for (int j = 0; j<3; j++)
//								{
//									faceVerId.push_back(tempIndx[2 - j]);
//									int sid = 0;
//									sid = tempIndx[j];
//									///fprintf(fp,"%d\n",sid);
//								}
//							}
//							///fprintf(fp,"\n");
//							///fclose(fp);		
//							return true;///划分完毕
//						}
//						else if ((num - storeNum) < 3)
//						{
//#ifdef _DEBUG ///wang 12.3
//							AfxMessageBox(_T("顶点数<3,划分方法需测试"));
//#endif
//						}
//					}
//					//反向剖分
//					next1Indx = 0;
//					next2Indx = 0;
//					nextNum = 0;///下两点之间的间隔数
//					turn = false;///是否执行下一个for循环 顶点
//					while (!turn)
//					{
//						if (!edgePoint[(i - 2 - next2Indx + num) % num].isStored) ///是可划分顶点
//						{
//							if (!edgePoint[(i - 1 - next1Indx + num) % num].isStored)
//							{
//								//jiang gai 2015.7.31
//								if (UncrossAndIn(edgePoint[i], edgePoint[(i - 2 - next2Indx + num) % num], edgePoint[(i - 1 - next1Indx + num) % num], edgePoint))/// ,num,storeNum))///可以作为一个划分
//								{
//									if (edgePoint[(i - 1 - next1Indx + num) % num].isStored)
//									{
//										next1Indx++;
//									}
//									faceVerId.push_back((i - 2 - next2Indx + num) % num);///存储划分的三角面的顶点索引
//									faceVerId.push_back((i - 1 - next1Indx + num) % num);
//									faceVerId.push_back(i);
//									int sid = 0;
//									sid = (i - 2 - next2Indx + num) % num;
//									///fprintf(fp,"%d\n",sid);
//									sid = (i - 1 - next1Indx + num) % num;
//									///fprintf(fp,"%d\n",sid);
//									sid = i;
//									///fprintf(fp,"%d\n",sid);
//									///fprintf(fp,"\n");
//
//									sid = (i - 1 - next1Indx + num) % num;
//									///fprintf(fp,"被删除的点 %d\n",sid);
//									///fprintf(fp,"\n");
//#ifdef _DEBUG ///wang 12.3
//									if (edgePoint[(i - 1 - next1Indx + num) % num].isStored)
//									{
//										MessageBox(_T("error"), NULL, NULL);
//									}
//#endif
//									edgePoint[(i - 1 - next1Indx + num) % num].isStored = true;///标志该顶点已被分割，不再参与以后的划分
//									storeNum++;///处理顶点数  + 
//									next1Indx++;///新剖分三角面的新边顶点索引  +
//									next2Indx++;
//									next1Indx += nextNum;
//									nextNum = 0;
//								}
//								else{///不可划分，反向剖分
//									turn = true;
//								}
//							}
//							else{
//								next1Indx++;///新剖分三角面的新边顶点索引  +
//								next2Indx++;
//							}
//						}
//						else{///隔点已被处理，则选再下一点
//							if (edgePoint[(i - 1 - next1Indx + num) % num].isStored)///如果邻点也被处理
//							{
//								next1Indx++;///新剖分三角面的新边顶点索引  +
//								next2Indx++;
//							}
//							else{
//								next2Indx++;
//								nextNum++;
//							}
//						}
//						if ((num - storeNum) == 3)
//						{
//							int tempIndx[3] = { 0, 0, 0 }; ///对于只剩余三个顶点的情况原来存点的索引方法有误 wang 7.28
//							int tempid = 0;
//							for (int j = 0; j < num; j++)
//							{
//								if (!edgePoint[j].isStored)
//								{
//									if (tempid>2)
//									{
//										break;;
//									}
//									tempIndx[tempid] = j;
//									tempid++;
//								}
//							}
//							if ((tempIndx[1] - tempIndx[0])*(tempIndx[2] - tempIndx[1])*(tempIndx[0] - tempIndx[2]) < 0)///逆时针排序
//							{
//								for (int j = 0; j<3; j++)
//								{
//									faceVerId.push_back(tempIndx[j]);
//									int sid = 0;
//									sid = tempIndx[j];
//									///fprintf(fp,"%d\n",sid);
//
//								}
//							}
//							else{
//								for (int j = 0; j<3; j++)
//								{
//									faceVerId.push_back(tempIndx[2 - j]);
//									int sid = 0;
//									sid = tempIndx[j];
//									///fprintf(fp,"%d\n",sid);
//								}
//							}
//							///fprintf(fp,"\n");
//							///fclose(fp);			
//							return true;///划分完毕
//						}
//
//						else if ((num - storeNum) < 3)
//						{
//#ifdef _DEBUG ///wang 12.3
//							AfxMessageBox(_T("顶点数<3,划分方法需测试"));
//#endif
//						}
//					}
//				}
//				else if ((num - storeNum) == 3)
//				{
//					for (int j = 0; j < num; j++)
//					{
//						if (!edgePoint[j].isStored)
//						{
//							faceVerId.push_back(j);
//							int sid = 0;
//							sid = j;
//							///fprintf(fp,"%d\n",sid);
//						}
//					}
//					//fprintf(fp,"\n");
//					//fclose(fp);						
//					return true;//划分完毕
//				}
//				else if ((num - storeNum) < 3)
//				{
//#ifdef _DEBUG ///wang 12.3
//					AfxMessageBox(_T("顶点数<3,划分方法需测试"));
//#endif
//				}
//			}
//		}
//		if (num - storeNum == 2)
//			m_mark += 2;
//		else
//			m_mark++;
//	}
//	return 0;
//	/*
//	recurseCount++;
//	//jiang gai 2019.8.25 下面代码先屏蔽，从cutpolygon中移植代码过来
//	int num = (int)edgePoint.size();///多边形顶点数
//	int storeNum = 0;///已处理的顶点数
//	bool second = false;///wql 2019.9.5 可能是第二轮划分
//	for (int i = 0; i < num; i++)///对每一点，先逆时针剖分，再顺时针剖分
//	{
//		if (edgePoint[i].isStored)
//		{
//			storeNum++;
//			second = true;
//		}
//	}
//	if (!second)
//	{///wql 2019.9.5 第一次划分，将完全重合的点剔除
//		std::vector<int> repeat;
//		for (int i = 0; i < num; i++)
//		{
//			if (Length(edgePoint[i].points, edgePoint[(i + 1) % num].points) < 0.00001f)///wql 2019.9.5 误差现在都改小了
//			{
//				repeat.push_back(i);
//			}
//		}
//		for (int i = (int)repeat.size() - 1; i >= 0; i--)
//		{
//			edgePoint.erase(edgePoint.begin() + repeat[i]);///wql 2019.9.5 删除重复点后，调用方在使用顶点时应该使用这个调整后的顶点
//		}
//		num = (int)edgePoint.size();///wql 2019.9.5 使用新的个数
//	}
//	for (int i = 0; i < num; i++)///对每一点，先逆时针剖分，再顺时针剖分
//	{
//		if (!edgePoint[i].isStored)///该顶点是可划分顶点
//		{
//			if ((num - storeNum) > 3)///并且是多边形
//			{
//				//如果还有超过3个点没有处理，那么继续处理，如果只有3个点直接手工处理
//				int next1Indx = 0, next2Indx = 0;///下一点及下2点的索引基址
//				int nextNum = 0;///下两点之间的间隔数  rao改可能有多个点
//				bool turn = false;///是否需要反方向剖分
//				while (!turn)//不反向，则一直按顺时针剖分，因为多边形都是顺时针方向
//				{
//					if (!edgePoint[(i + 2 + next2Indx) % num].isStored) ///是可划分顶点
//					{
//						if (!edgePoint[(i + 1 + next1Indx + num) % num].isStored)// 邻点未被处理
//						{
//							if (UncrossAndIn(edgePoint[i], edgePoint[(i + 2 + next2Indx) % num], edgePoint, bHole))
//							{
//								//edgePoint[i],edgePoint[(i+2+next2Indx)%num ]没有跨相交，可以作为一个划分
//								if (edgePoint[(i + 1 + next1Indx) % num].isStored)
//								{
//									//多余，上面已判断为false了
//									next1Indx++;
//								}
//								faceVerId.push_back(i);///存储划分的三角面的顶点索引
//								faceVerId.push_back((i + 1 + next1Indx) % num);
//								faceVerId.push_back((i + 2 + next2Indx) % num);
//								//调试用的
//								int sid = 0;
//								sid = i;
//								///	fprintf(fp,"%d\n",sid);
//								sid = (i + 1 + next1Indx) % num;
//								///	fprintf(fp,"%d\n",sid);
//								sid = (i + 2 + next2Indx) % num;
//								///	fprintf(fp,"%d\n",sid);
//								///	fprintf(fp,"\n");
//
//								sid = (i + 1 + next1Indx) % num;
//								///	fprintf(fp,"被删除的点 %d\n",sid);
//								///	fprintf(fp,"\n");
//
//#ifdef _DEBUG ///wang 12.3
//								if (edgePoint[(i + 1 + next1Indx) % num].isStored)
//								{
//									MessageBox(NULL, _T("error"), NULL, NULL);
//								}
//#endif
//								edgePoint[(i + 1 + next1Indx) % num].isStored = true;///标志该顶点已被分割，不再参与以后的划分
//								storeNum++;//处理顶点数  + 
//								next1Indx++;//新剖分三角面的新边顶点索引  +
//								next2Indx++;
//
//								// raogai 2013.9.22
//								next1Indx += nextNum;//next1Indx和next2Indx大家又持平了好继续往前走
//								nextNum = 0;
//
//							}
//							else
//							{///不可划分，反向剖分
//								turn = true;
//							}
//						}
//						else
//						{
//							next1Indx++;///新剖分三角面的新边顶点索引  +
//							next2Indx++;
//						}
//					}
//					else{///隔点已被处理，则选再下一点
//						if (edgePoint[(i + 1 + next1Indx) % num].isStored)///如果邻点也被处理
//						{
//							next1Indx++;///新剖分三角面的新边顶点索引  +
//							next2Indx++;
//						}
//						else{
//							next2Indx++;
//							nextNum++;//next1Indx和next2Indx之间的间隔点数
//						}
//					}
//					if ((num - storeNum) == 3)
//					{
//						int tempIndx[3] = { 0, 0, 0 };///对于只剩余三个顶点的情况原来存点的索引方法有误 wang 7.28
//						int tempid = 0;
//						for (int j = 0; j < num; j++)
//						{
//							if (!edgePoint[j].isStored)
//							{
//								if (tempid>2)
//								{
//									break;;
//								}
//								tempIndx[tempid] = j;
//								tempid++;
//							}
//						}
//						//保证3个点的序号是从小到大，但是好像多余了，上面就是按序号压入的
//						if ((tempIndx[1] - tempIndx[0])*(tempIndx[2] - tempIndx[1])*(tempIndx[0] - tempIndx[2]) < 0)///逆时针排序
//						{
//							for (int j = 0; j<3; j++)
//							{
//								faceVerId.push_back(tempIndx[j]);
//								int sid = 0;
//								sid = tempIndx[j];
//								///fprintf(fp,"%d\n",sid);
//							}
//						}
//						else
//						{
//							for (int j = 0; j<3; j++)
//							{
//								faceVerId.push_back(tempIndx[2 - j]);
//								int sid = 0;
//								sid = tempIndx[j];
//								///fprintf(fp,"%d\n",sid);
//							}
//						}
//						///fprintf(fp,"\n");
//						///fclose(fp);		
//						return true;///划分完毕
//					}
//					else if ((num - storeNum) < 3)
//					{
//#ifdef _DEBUG ///wang 12.3
//						AfxMessageBox(_T("顶点数<3,划分方法需测试"));
//#endif
//					}
//				}
//
//				///反向剖分
//				next1Indx = 0;
//				next2Indx = 0;
//				nextNum = 0;///下两点之间的间隔数
//				turn = false;///是否执行下一个for循环 顶点
//				while (!turn)
//				{
//					//下面是从另一个方向走了，所以减
//					if (!edgePoint[(i - 2 - next2Indx + num) % num].isStored) //是可划分顶点
//					{
//						if (!edgePoint[(i - 1 - next1Indx + num) % num].isStored)
//						{
//							if (UncrossAndIn(edgePoint[i], edgePoint[(i - 2 - next2Indx + num) % num], edgePoint, bHole))///可以作为一个划分
//							{
//								if (edgePoint[(i - 1 - next1Indx + num) % num].isStored)
//								{
//									next1Indx++;
//								}
//								//方向没有限定
//								faceVerId.push_back((i - 2 - next2Indx + num) % num);///存储划分的三角面的顶点索引
//								faceVerId.push_back((i - 1 - next1Indx + num) % num);
//								faceVerId.push_back(i);
//								int sid = 0;
//								sid = (i - 2 - next2Indx + num) % num;
//								///fprintf(fp,"%d\n",sid);
//								sid = (i - 1 - next1Indx + num) % num;
//								///fprintf(fp,"%d\n",sid);
//								sid = i;
//								///fprintf(fp,"%d\n",sid);
//								///fprintf(fp,"\n");
//
//								sid = (i - 1 - next1Indx + num) % num;
//								///fprintf(fp,"被删除的点 %d\n",sid);
//								///fprintf(fp,"\n");
//#ifdef _DEBUG ///wang 12.3
//								if (edgePoint[(i - 1 - next1Indx + num) % num].isStored)
//								{
//									MessageBox(NULL, _T("error"), NULL, NULL);
//								}
//#endif
//								edgePoint[(i - 1 - next1Indx + num) % num].isStored = true;///标志该顶点已被分割，不再参与以后的划分
//								storeNum++;///处理顶点数  + 
//								next1Indx++;///新剖分三角面的新边顶点索引  +
//								next2Indx++;
//								next1Indx += nextNum;
//								nextNum = 0;
//							}
//							else{///不可划分，反向剖分
//								turn = true;
//							}
//						}
//						else{
//							next1Indx++;///新剖分三角面的新边顶点索引  +
//							next2Indx++;
//						}
//					}
//					else{///隔点已被处理，则选再下一点
//						if (edgePoint[(i - 1 - next1Indx + num) % num].isStored)///如果邻点也被处理
//						{
//							next1Indx++;///新剖分三角面的新边顶点索引  +
//							next2Indx++;
//						}
//						else{
//							next2Indx++;
//							nextNum++;
//						}
//					}
//					if ((num - storeNum) == 3)
//					{
//						int tempIndx[3] = { 0, 0, 0 }; ///对于只剩余三个顶点的情况原来存点的索引方法有误 wang 7.28
//						int tempid = 0;
//						for (int j = 0; j < num; j++)
//						{
//							if (!edgePoint[j].isStored)
//							{
//								if (tempid>2)
//								{
//									break;;
//								}
//								tempIndx[tempid] = j;
//								tempid++;
//							}
//						}
//						//保证3个点的序号是从小到大，但是好像多余了，上面就是按序号压入的
//						if ((tempIndx[1] - tempIndx[0])*(tempIndx[2] - tempIndx[1])*(tempIndx[0] - tempIndx[2]) < 0)//顺时针排序
//						{
//							for (int j = 0; j<3; j++)
//							{
//								faceVerId.push_back(tempIndx[j]);
//								int sid = 0;
//								sid = tempIndx[j];
//								///fprintf(fp,"%d\n",sid);
//							}
//						}
//						else{
//							for (int j = 0; j<3; j++)
//							{
//								faceVerId.push_back(tempIndx[2 - j]);
//								int sid = 0;
//								sid = tempIndx[j];
//								///fprintf(fp,"%d\n",sid);
//							}
//						}
//						///fprintf(fp,"\n");
//						///fclose(fp);			
//						return true;///划分完毕
//					}
//					else if ((num - storeNum) < 3)
//					{
//#ifdef _DEBUG ///wang 12.3
//						AfxMessageBox(_T("顶点数<3,划分方法需测试"));
//#endif
//					}
//				}
//			}
//			else if ((num - storeNum) == 3)
//			{
//				//剩下的3个点自动生成三角形
//				for (int j = 0; j < num; j++)
//				{
//					if (!edgePoint[j].isStored)
//					{
//						faceVerId.push_back(j);
//						int sid = 0;
//						sid = j;
//						///fprintf(fp,"%d\n",sid);
//					}
//				}
//				///fprintf(fp,"\n");
//				///fclose(fp);						
//				return true;///划分完毕
//			}
//			else if ((num - storeNum) < 3)
//			{
//#ifdef _DEBUG ///wang 12.3
//				//		AfxMessageBox( _T("顶点数<3,划分方法需测试") ); //jiang gai 2017.6.3 屏蔽，否则会触发墙面重绘死掉
//#endif
//			}
//		}
//	}
//	
//	if (recurseCount<2 && (num - storeNum) > 3)///wql 2019.9.5 不是第二轮划分，并且为划分的顶点数大于3 ///wql 2019.9.18 最多划分5次
//	{
//		return PolygonToTriangels_Recurse(edgePoint, faceVerId, recurseCount, bHole);
//	}
//	return 1;*/
//}
//
///***********************************************
//作者： 王庆利
//功能描述：将多边形三角化
//输入参数：edgePoint多边形顶点， faceVerId三角化后的顶点索引，每三个一个面
//输出参数：无
//返回参数说明：三角化成功返回true
//************************************************/
//bool CGeoMaths::PolygonToTriangels(std::vector<PointStruct> &edgePoint, std::vector<int> & faceVerId,bool bHole) //jiang gai 2019.9.25  增加参数bHole
//{
//	int recurseCount = 0;
//	m_mark = 1;
//	bool flag = PolygonToTriangels_Recurse(edgePoint, faceVerId, recurseCount, bHole);///wql 2019.9.26 挖洞和不挖洞的分割要区分开
//	if (edgePoint.size() >= 24)
//	{
//		FILE* fp = fopen("D:\\now.txt", "wb");
//		for (int i = 0; i < faceVerId.size(); i++)
//		{
//			if (0==i % 3)
//			{
//				fprintf(fp, "\n");
//			}
//			fprintf(fp, "%d\n", faceVerId[i]);
//		}
//		fclose(fp);
//	}
//	return flag;
//}
//
///*
//bool CGeoMaths::PolygonToTriangels(std::vector<PointStruct> &edgePoint, std::vector<int> & faceVerId)
//{
//	//jiang gai 2019.8.25 下面代码先屏蔽，从cutpolygon中移植代码过来
//	int num = (int)edgePoint.size();///多边形顶点数
//	int storeNum = 0;///已处理的顶点数
//	bool second = false;///wql 2019.9.5 可能是第二轮划分
//	for (int i = 0; i < num; i++)///对每一点，先逆时针剖分，再顺时针剖分
//	{
//		if (edgePoint[i].isStored)
//		{
//			storeNum++;
//			second = true;
//		}
//	}
//	if (!second)
//	{///wql 2019.9.5 第一次划分，将完全重合的点剔除
//		std::vector<int> repeat;
//		for (int i = 0; i < num; i++)
//		{
//			if (Length(edgePoint[i].points, edgePoint[(i + 1) % num].points) < 0.00001f)///wql 2019.9.5 误差现在都改小了
//			{
//				repeat.push_back(i);
//			}
//		}
//		for (int i = (int)repeat.size() - 1; i >= 0; i--)
//		{
//			edgePoint.erase(edgePoint.begin() + repeat[i]);///wql 2019.9.5 删除重复点后，调用方在使用顶点时应该使用这个调整后的顶点
//		}
//		num = (int)edgePoint.size();///wql 2019.9.5 使用新的个数
//	}
//	for (int i = 0; i < num; i++)///对每一点，先逆时针剖分，再顺时针剖分
//	{
//		if (edgePoint[i].isStored)
//		{
//			storeNum++;
//			second = true;
//		}
//	}
//	if (!second)
//	{///wql 2019.9.5 第一次划分，将完全重合的点剔除
//		std::vector<int> repeat;
//		for (int i = 0; i < num; i++)
//		{
//			if (Length(edgePoint[i].points, edgePoint[(i + 1) % num].points) < 0.00001f)///wql 2019.9.5 误差现在都改小了
//			{
//				repeat.push_back(i);
//			}
//		}
//		for (int i = (int)repeat.size() - 1; i >= 0; i--)
//		{
//			edgePoint.erase(edgePoint.begin() + repeat[i]);///wql 2019.9.5 删除重复点后，调用方在使用顶点时应该使用这个调整后的顶点
//		}
//		num = (int)edgePoint.size();///wql 2019.9.5 使用新的个数
//	}
//	for (int i = 0; i < num; i++)///对每一点，先逆时针剖分，再顺时针剖分
//	{
//		if (!edgePoint[i].isStored)///该顶点是可划分顶点
//		{
//			if ((num - storeNum) > 3)///并且是多边形
//			{
//				//如果还有超过3个点没有处理，那么继续处理，如果只有3个点直接手工处理
//				int next1Indx = 0, next2Indx = 0;///下一点及下2点的索引基址
//				int nextNum = 0;///下两点之间的间隔数  rao改可能有多个点
//				bool turn = false;///是否需要反方向剖分
//				while (!turn)//不反向，则一直按顺时针剖分，因为多边形都是顺时针方向
//				{
//					if (!edgePoint[(i + 2 + next2Indx) % num].isStored) ///是可划分顶点
//					{
//						if (!edgePoint[(i + 1 + next1Indx + num) % num].isStored)// 邻点未被处理
//						{
//							if (UncrossAndIn(edgePoint[i], edgePoint[(i + 2 + next2Indx) % num], edgePoint))
//							{
//								//edgePoint[i],edgePoint[(i+2+next2Indx)%num ]没有跨相交，可以作为一个划分
//								if (edgePoint[(i + 1 + next1Indx) % num].isStored)
//								{
//									//多余，上面已判断为false了
//									next1Indx++;
//								}
//								faceVerId.push_back(i);///存储划分的三角面的顶点索引
//								faceVerId.push_back((i + 1 + next1Indx) % num);
//								faceVerId.push_back((i + 2 + next2Indx) % num);
//								//调试用的
//								int sid = 0;
//								sid = i;
//								///	fprintf(fp,"%d\n",sid);
//								sid = (i + 1 + next1Indx) % num;
//								///	fprintf(fp,"%d\n",sid);
//								sid = (i + 2 + next2Indx) % num;
//								///	fprintf(fp,"%d\n",sid);
//								///	fprintf(fp,"\n");
//
//								sid = (i + 1 + next1Indx) % num;
//								///	fprintf(fp,"被删除的点 %d\n",sid);
//								///	fprintf(fp,"\n");
//
//#ifdef _DEBUG ///wang 12.3
//								if (edgePoint[(i + 1 + next1Indx) % num].isStored)
//								{
//									MessageBox(NULL, _T("error"), NULL, NULL);
//								}
//#endif
//								edgePoint[(i + 1 + next1Indx) % num].isStored = true;///标志该顶点已被分割，不再参与以后的划分
//								storeNum++;//处理顶点数  + 
//								next1Indx++;//新剖分三角面的新边顶点索引  +
//								next2Indx++;
//
//								// raogai 2013.9.22
//								next1Indx += nextNum;//next1Indx和next2Indx大家又持平了好继续往前走
//								nextNum = 0;
//
//							}
//							else
//							{///不可划分，反向剖分
//								turn = true;
//							}
//						}
//						else
//						{
//							next1Indx++;///新剖分三角面的新边顶点索引  +
//							next2Indx++;
//						}
//					}
//					else{///隔点已被处理，则选再下一点
//						if (edgePoint[(i + 1 + next1Indx) % num].isStored)///如果邻点也被处理
//						{
//							next1Indx++;///新剖分三角面的新边顶点索引  +
//							next2Indx++;
//						}
//						else{
//							next2Indx++;
//							nextNum++;//next1Indx和next2Indx之间的间隔点数
//						}
//					}
//					if ((num - storeNum) == 3)
//					{
//						int tempIndx[3] = { 0, 0, 0 };///对于只剩余三个顶点的情况原来存点的索引方法有误 wang 7.28
//						int tempid = 0;
//						for (int j = 0; j < num; j++)
//						{
//							if (!edgePoint[j].isStored)
//							{
//								if (tempid > 2)
//								{
//									break;;
//								}
//								tempIndx[tempid] = j;
//								tempid++;
//							}
//						}
//						//保证3个点的序号是从小到大，但是好像多余了，上面就是按序号压入的
//						if ((tempIndx[1] - tempIndx[0])*(tempIndx[2] - tempIndx[1])*(tempIndx[0] - tempIndx[2]) < 0)///逆时针排序
//						{
//							for (int j = 0; j < 3; j++)
//							{
//								faceVerId.push_back(tempIndx[j]);
//								int sid = 0;
//								sid = tempIndx[j];
//								///fprintf(fp,"%d\n",sid);
//							}
//						}
//						else
//						{
//							for (int j = 0; j < 3; j++)
//							{
//								faceVerId.push_back(tempIndx[2 - j]);
//								int sid = 0;
//								sid = tempIndx[j];
//								///fprintf(fp,"%d\n",sid);
//							}
//						}
//						///fprintf(fp,"\n");
//						///fclose(fp);		
//						return true;///划分完毕
//					}
//					else if ((num - storeNum) < 3)
//					{
//#ifdef _DEBUG ///wang 12.3
//						AfxMessageBox(_T("顶点数<3,划分方法需测试"));
//#endif
//					}
//				}
//
//				///反向剖分
//				next1Indx = 0;
//				next2Indx = 0;
//				nextNum = 0;///下两点之间的间隔数
//				turn = false;///是否执行下一个for循环 顶点
//				while (!turn)
//				{
//					//下面是从另一个方向走了，所以减
//					if (!edgePoint[(i - 2 - next2Indx + num) % num].isStored) //是可划分顶点
//					{
//						if (!edgePoint[(i - 1 - next1Indx + num) % num].isStored)
//						{
//							if (UncrossAndIn(edgePoint[i], edgePoint[(i - 2 - next2Indx + num) % num], edgePoint))///可以作为一个划分
//							{
//								if (edgePoint[(i - 1 - next1Indx + num) % num].isStored)
//								{
//									next1Indx++;
//								}
//								//方向没有限定
//								faceVerId.push_back((i - 2 - next2Indx + num) % num);///存储划分的三角面的顶点索引
//								faceVerId.push_back((i - 1 - next1Indx + num) % num);
//								faceVerId.push_back(i);
//								int sid = 0;
//								sid = (i - 2 - next2Indx + num) % num;
//								///fprintf(fp,"%d\n",sid);
//								sid = (i - 1 - next1Indx + num) % num;
//								///fprintf(fp,"%d\n",sid);
//								sid = i;
//								///fprintf(fp,"%d\n",sid);
//								///fprintf(fp,"\n");
//
//								sid = (i - 1 - next1Indx + num) % num;
//								///fprintf(fp,"被删除的点 %d\n",sid);
//								///fprintf(fp,"\n");
//#ifdef _DEBUG ///wang 12.3
//								if (edgePoint[(i - 1 - next1Indx + num) % num].isStored)
//								{
//									MessageBox(NULL, _T("error"), NULL, NULL);
//								}
//#endif
//								edgePoint[(i - 1 - next1Indx + num) % num].isStored = true;///标志该顶点已被分割，不再参与以后的划分
//								storeNum++;///处理顶点数  + 
//								next1Indx++;///新剖分三角面的新边顶点索引  +
//								next2Indx++;
//								next1Indx += nextNum;
//								nextNum = 0;
//							}
//							else{///不可划分，反向剖分
//								turn = true;
//							}
//						}
//						else{
//							next1Indx++;///新剖分三角面的新边顶点索引  +
//							next2Indx++;
//						}
//					}
//					else{///隔点已被处理，则选再下一点
//						if (edgePoint[(i - 1 - next1Indx + num) % num].isStored)///如果邻点也被处理
//						{
//							next1Indx++;///新剖分三角面的新边顶点索引  +
//							next2Indx++;
//						}
//						else{
//							next2Indx++;
//							nextNum++;
//						}
//					}
//					if ((num - storeNum) == 3)
//					{
//						int tempIndx[3] = { 0, 0, 0 }; ///对于只剩余三个顶点的情况原来存点的索引方法有误 wang 7.28
//						int tempid = 0;
//						for (int j = 0; j < num; j++)
//						{
//							if (!edgePoint[j].isStored)
//							{
//								if (tempid > 2)
//								{
//									break;;
//								}
//								tempIndx[tempid] = j;
//								tempid++;
//							}
//						}
//						//保证3个点的序号是从小到大，但是好像多余了，上面就是按序号压入的
//						if ((tempIndx[1] - tempIndx[0])*(tempIndx[2] - tempIndx[1])*(tempIndx[0] - tempIndx[2]) < 0)//顺时针排序
//						{
//							for (int j = 0; j < 3; j++)
//							{
//								faceVerId.push_back(tempIndx[j]);
//								int sid = 0;
//								sid = tempIndx[j];
//								///fprintf(fp,"%d\n",sid);
//							}
//						}
//						else{
//							for (int j = 0; j < 3; j++)
//							{
//								faceVerId.push_back(tempIndx[2 - j]);
//								int sid = 0;
//								sid = tempIndx[j];
//								///fprintf(fp,"%d\n",sid);
//							}
//						}
//						///fprintf(fp,"\n");
//						///fclose(fp);			
//						return true;///划分完毕
//					}
//					else if ((num - storeNum) < 3)
//					{
//#ifdef _DEBUG ///wang 12.3
//						AfxMessageBox(_T("顶点数<3,划分方法需测试"));
//#endif
//					}
//				}
//			}
//			else if ((num - storeNum) == 3)
//			{
//				//剩下的3个点自动生成三角形
//				for (int j = 0; j < num; j++)
//				{
//					if (!edgePoint[j].isStored)
//					{
//						faceVerId.push_back(j);
//						int sid = 0;
//						sid = j;
//						///fprintf(fp,"%d\n",sid);
//					}
//				}
//				///fprintf(fp,"\n");
//				///fclose(fp);						
//				return true;///划分完毕
//			}
//			else if ((num - storeNum) < 3)
//			{
//#ifdef _DEBUG ///wang 12.3
//				//		AfxMessageBox( _T("顶点数<3,划分方法需测试") ); //jiang gai 2017.6.3 屏蔽，否则会触发墙面重绘死掉
//#endif
//			}
//		}
//	}
//	if (!second && (num - storeNum) > 3)///wql 2019.9.5 不是第二轮划分，并且为划分的顶点数大于3
//	{
//		PolygonToTriangels(edgePoint, faceVerId);
//	}
//
//	return 1;
//}
//*/	
//
///***********************************************
//作者： 王庆利   
//功能描述：根据边界点创建一个平面形体
//输入参数：edgePt平面多边形顶点， ptGeom创建的形体
//输出参数：无
//返回参数说明：无
//************************************************/
//void CGeoMaths::CreateTopFace(std::vector<CVector3> &edgePt, CGeomentry_Information *ptGeom)
//{
//	//点纹理信息,暂时不考虑多重纹理
//	CVMapCoords coords;
//	coords.numberOfMapCoords=1;
//	CVertexMapCoords tex,*pTex;
//	tex.MapCoords.u = tex.MapCoords.v = 0; //jiang gai 2014.8.5
//	coords.UVWCoords.push_back(tex);
//	pTex = &coords.UVWCoords[coords.UVWCoords.size()-1];
//	pTex->MapIndex = 0;
//
//	//面纹理信息,目前每个形体只有一个纹理，且作用于所有的面
//	CMatAndFace mf1,*pMf1;
//	mf1.Materials_Index=0;
//	ptGeom->Materials.numberOfMaterials=1;//目前每个形体只有一个纹理
//	ptGeom->Materials.MaterialProp.push_back(mf1);
//	pMf1 = &ptGeom->Materials.MaterialProp[ptGeom->Materials.MaterialProp.size()-1];
//	pMf1->numberOfFaces = 0;
//
//	//下面压入点
//	CVector3 point={0.0,0.0,0.0},color;
//	color.x = color.y= color.z =1.0;
//
//	int i=0,j=0,k=0;
//
//	std::vector < CVector3 >  dir;///各边方向，随着顶点的删除，方向动态改变
//	CMaths math;
//	CCollision collision; ///根据偏转方向检测凹凸点
//	std::vector< int > turnPoint;///凹凸转折点标志及顶点序号, 开始置为没有转折 
//	std::vector< int > faceVerIndx;///临时存储三角片的面索引
//	std::vector< PointStruct >  tempEdgePoint;///临时存储边界点，以免更改类中成员floorEdgePoint，在更改地板时还要用到原边界
//
//	int aa=(int)edgePt.size(),bb=0;
//	///首先存入边界点及其纹理坐标, 顺便计算各边方向向量
//	for( i=0; i<(int)edgePt.size(); i++)
//	{	
//		point.x = edgePt[i].x;
//		point.y = edgePt[i].y;
//		point.z = edgePt[i].z;
//
//		///该点所对应的纹理坐标
//		pTex->MapCoords.u = 0;
//		pTex->MapCoords.v = 0;
//
//		ptGeom->Vertices.VPoint.push_back(point);
//		ptGeom->Vertices.VColors.push_back(color);
//		ptGeom->Vertices.VMaps.push_back(coords);
//
//		CVector3 point2={0.0,0.0,0.0};
//		point2.x = edgePt[(i+1)%edgePt.size()].x;
//		point2.y = edgePt[(i+1)%edgePt.size()].y;
//		point2.z = edgePt[(i+1)%edgePt.size()].z;
//
//		dir.push_back( math.Vector( point, point2 ) );///计算方向
//
//
//		PointStruct temp;
//		temp.points = edgePt[i];
//		temp.isStored = false;
//		tempEdgePoint.push_back( temp );///同时存储进临时边界容器
//	}
//	ptGeom->Vertices.numberOfVertices += (unsigned long)edgePt.size();  //点的总数加 
//
//	PolygonToTriangels( tempEdgePoint, faceVerIndx );
//
//	int ptNum = 0;
//	for( k = (int)faceVerIndx.size()-1; k >-1 ; k -- ) //存储三角片的面索引
//	{
//		ptGeom->Faces.Vertices_Index.push_back( faceVerIndx[ k ] );
//		if( !( ptNum%3 ) )///每三个点序号一个面
//		{
//			pMf1->Face_Index.push_back(pMf1->numberOfFaces);
//			pMf1->numberOfFaces++;
//		}
//		ptNum ++;
//	}
//	ptGeom->Faces.numberOfFaces += pMf1->numberOfFaces;//面的总数加2
//}
//
///***********************************************
//作者： 王庆利   
//功能描述：计算多边形面积
//输入参数：edgePoint多边形顶点
//输出参数：无
//返回参数说明：返回面积
//************************************************/
//float CGeoMaths::ComputePolygonArea(const std::vector<CVector3> &edgePt)
//{
//	//jiang gai 2017.9.9 确保多边形边数大于等于3
//	if ((int)edgePt.size()<3)
//		return 0;
//	
//	float area = 0.0f;
//	std::vector<int> faceVerId;
//	int num = (int)edgePt.size();///多边形顶点数
//	std::vector<PointStruct> edgePoint;
//	for(int i=0; i<num; i++)
//	{
//		PointStruct temp;
//		temp.points = edgePt[i];
//		temp.isStored = false;
//		edgePoint.push_back(temp);
//	}
//	PolygonToTriangels(edgePoint, faceVerId);
//	for (int i = 0; i < faceVerId.size()/3; i++)
//	{
//		CVector3 p1 = { edgePoint[faceVerId[i * 3]].points.x, 0.0f, edgePoint[faceVerId[i * 3]].points.z },
//			p2 = { edgePoint[faceVerId[i * 3 + 1]].points.x, 0.0f, edgePoint[faceVerId[i * 3 + 1]].points.z },
//			p3 = { edgePoint[faceVerId[i * 3 + 2]].points.x, 0.0f, edgePoint[faceVerId[i * 3 + 2]].points.z };
//		area += TriangleArea(p1, p2, p3);
//	}
//	return area;
//	int storeNum = 0;///已处理的顶点数
//	for( int i = 0; i < num; i++ )///对每一点，先逆时针剖分，再顺时针剖分
//	{
//		if(!edgePoint[i].isStored)///该顶点是可划分顶点
//		{
//			if( (num - storeNum) > 3  )///并且是多边形
//			{
//				int next1Indx = 0, next2Indx = 0;///下一点及下2点的索引基址
//				bool turn = false;///是否需要反方向剖分
//				while( !turn )///不反向，则一直按逆时针剖分
//				{
//					if(!edgePoint[ (i+2+next2Indx)%num ].isStored) ///是可划分顶点
//					{
//						if( UncrossAndIn( edgePoint[i], edgePoint[ (i+2+next2Indx)%num ], edgePoint ) )///可以作为一个划分
//						{
//							if (edgePoint[ (i+1+next1Indx)%num].isStored)
//							{
//								 next1Indx ++;
//							}
//
//							CVector3 p1 = {edgePoint[i].points.x, 0.0f, edgePoint[i].points.z},
//								p2 = {edgePoint[ (i+1+next1Indx)%num ].points.x, 0.0f, edgePoint[ (i+1+next1Indx)%num ].points.z},
//								p3 = {edgePoint[ (i+2+next2Indx)%num ].points.x, 0.0f, edgePoint[ (i+2+next2Indx)%num ].points.z};
//							area += TriangleArea(p1,p2,p3);
//							faceVerId.push_back( i );///存储划分的三角面的顶点索引
//							faceVerId.push_back( (i+1+next1Indx)%num );
//							faceVerId.push_back( (i+2+next2Indx)%num );
//							int sid;
//							sid = i;
//							///	fprintf(fp,"%d\n",sid);
//							sid = (i+1+next1Indx)%num;
//							///	fprintf(fp,"%d\n",sid);
//							sid = (i+2+next2Indx)%num;
//							///	fprintf(fp,"%d\n",sid);
//							///	fprintf(fp,"\n");
//
//							sid = (i+1+next1Indx)%num;
//							///	fprintf(fp,"被删除的点 %d\n",sid);
//							///	fprintf(fp,"\n");
//
//							edgePoint[ (i+1+next1Indx)%num ].isStored = true;///标志该顶点已被分割，不再参与以后的划分
//							storeNum ++;///处理顶点数  + 
//							next1Indx ++;///新剖分三角面的新边顶点索引  +
//							next2Indx ++;
//						}else{///不可划分，反向剖分
//							turn =true;
//						}
//					}else{///隔点已被处理，则选再下一点
//						if(edgePoint[ (i+1+next1Indx)%num ].isStored)///如果邻点也被处理
//						{
//							next1Indx ++;///新剖分三角面的新边顶点索引  +
//							next2Indx ++;
//						}else{
//							next2Indx ++;
//						}
//					}
//					if( (num - storeNum) == 3 )
//					{
//						int tempIndx[3];///对于只剩余三个顶点的情况原来存点的索引方法有误 wang 7.28
//						int tempid = 0;
//						for( int j = 0; j < num; j++ )
//						{
//							if(!edgePoint[j].isStored)
//							{
//								if (tempid>2)
//								{
//									break;;
//								}
//								tempIndx[tempid] = j;
//								tempid ++;
//							}
//						}
//						if( (tempIndx[1]-tempIndx[0])*(tempIndx[2]-tempIndx[1])*(tempIndx[0]-tempIndx[2]) < 0 )///逆时针排序
//						{
//							CVector3 p1 = {edgePoint[tempIndx[0]].points.x, 0.0f, edgePoint[tempIndx[0]].points.z},
//								p2 = {edgePoint[tempIndx[1]].points.x, 0.0f, edgePoint[tempIndx[1]].points.z},
//								p3 = {edgePoint[tempIndx[2]].points.x, 0.0f, edgePoint[tempIndx[2]].points.z};
//							area += TriangleArea(p1,p2,p3);
//
//							for(int j=0; j<3; j++)
//							{
//								faceVerId.push_back( tempIndx[j] );
//								int sid;
//								sid = tempIndx[j];
//								///	fprintf(fp,"%d\n",sid);
//							}
//						}else{
//							CVector3 p1 = {edgePoint[tempIndx[0]].points.x, 0.0f, edgePoint[tempIndx[0]].points.z},
//								p2 = {edgePoint[tempIndx[1]].points.x, 0.0f, edgePoint[tempIndx[1]].points.z},
//								p3 = {edgePoint[tempIndx[2]].points.x, 0.0f, edgePoint[tempIndx[2]].points.z};
//							area += TriangleArea(p1,p2,p3);
//
//							for(int j=0; j<3; j++)
//							{
//								faceVerId.push_back( tempIndx[2-j] );
//								int sid;
//								sid = tempIndx[j];
//								///	fprintf(fp,"%d\n",sid);
//							}
//						}
//						///fprintf(fp,"\n");
//						///fclose(fp);		
//						return area;///划分完毕
//					}else if( (num - storeNum) < 3 )
//					{
//#ifdef _DEBUG ///wang 12.3
//						AfxMessageBox( _T("顶点数<3,划分方法需测试") );
//#endif
//					}
//				}
//
//				///反向剖分
//				next1Indx = 0;
//				next2Indx = 0;
//				int nextNum = 0;///下两点之间的间隔数
//				turn = false;///是否执行下一个for循环 顶点
//				while( !turn )
//				{
//					if(!edgePoint[ (i-2-next2Indx+num)%num ].isStored) ///是可划分顶点
//					{
//						if( !edgePoint[ (i-1-next1Indx+num)%num ].isStored )
//						{
//							if( UncrossAndIn( edgePoint[i], edgePoint[ (i-2-next2Indx+num)%num ], edgePoint ) )///可以作为一个划分
//							{
//								CVector3 p1 = {edgePoint[i].points.x, 0.0f, edgePoint[i].points.z},
//									p2 = {edgePoint[ (i-2-next2Indx+num)%num ].points.x, 0.0f, edgePoint[ (i-2-next2Indx+num)%num ].points.z},
//									p3 = {edgePoint[ (i-1-next1Indx+num)%num ].points.x, 0.0f, edgePoint[ (i-1-next1Indx+num)%num ].points.z};
//								area += TriangleArea(p1,p2,p3);
//
//								faceVerId.push_back( (i-2-next2Indx+num)%num );///存储划分的三角面的顶点索引
//								faceVerId.push_back( (i-1-next1Indx+num)%num );
//								faceVerId.push_back( i );
//								int sid;
//								sid = (i-2-next2Indx+num)%num;
//								///	fprintf(fp,"%d\n",sid);
//								sid = (i-1-next1Indx+num)%num;
//								///	fprintf(fp,"%d\n",sid);
//								sid = i;
//								///	fprintf(fp,"%d\n",sid);
//								///	fprintf(fp,"\n");
//
//								sid = (i-1-next1Indx+num)%num;
//								///	fprintf(fp,"被删除的点 %d\n",sid);
//								///	fprintf(fp,"\n");
//#ifdef _DEBUG ///wang 12.3
//								if (edgePoint[ (i-1-next1Indx+num)%num ].isStored)
//								{
//									MessageBox(NULL, _T("error"), NULL, NULL);
//								}
//#endif
//								edgePoint[ (i-1-next1Indx+num)%num ].isStored = true;///标志该顶点已被分割，不再参与以后的划分
//								storeNum ++;///处理顶点数  + 
//								next1Indx ++;///新剖分三角面的新边顶点索引  +
//								next2Indx ++;
//								next1Indx +=nextNum;
//								nextNum = 0;
//							}else{///不可划分，反向剖分
//								turn =true;
//							}
//						}else{
//							next1Indx ++;///新剖分三角面的新边顶点索引  +
//							next2Indx ++;
//						}
//					}else{///隔点已被处理，则选再下一点
//						if(edgePoint[ (i-1-next1Indx+num)%num ].isStored)///如果邻点也被处理
//						{
//							next1Indx ++;///新剖分三角面的新边顶点索引  +
//							next2Indx ++;
//						}else{
//							next2Indx ++;
//							nextNum ++;
//						}
//					}
//					if( (num - storeNum) == 3 )
//					{
//						int tempIndx[3]; ///对于只剩余三个顶点的情况原来存点的索引方法有误 wang 7.28
//						int tempid = 0;
//						for( int j = 0; j < num; j++ )
//						{
//							if(!edgePoint[j].isStored)
//							{
//								if (tempid>2)
//								{
//									break;;
//								}
//								tempIndx[tempid] = j;
//								tempid ++;				
//							}
//						}
//						if( (tempIndx[1]-tempIndx[0])*(tempIndx[2]-tempIndx[1])*(tempIndx[0]-tempIndx[2]) < 0 )///逆时针排序
//						{
//							CVector3 p1 = {edgePoint[tempIndx[0]].points.x, 0.0f, edgePoint[tempIndx[0]].points.z},
//								p2 = {edgePoint[tempIndx[1]].points.x, 0.0f, edgePoint[tempIndx[1]].points.z},
//								p3 = {edgePoint[tempIndx[2]].points.x, 0.0f, edgePoint[tempIndx[2]].points.z};
//							area += TriangleArea(p1,p2,p3);
//
//							for(int j=0; j<3; j++)
//							{
//								faceVerId.push_back( tempIndx[j] );
//								int sid;
//								sid = tempIndx[j];
//								///	fprintf(fp,"%d\n",sid);
//
//							}
//						}else{
//							CVector3 p1 = {edgePoint[tempIndx[0]].points.x, 0.0f, edgePoint[tempIndx[0]].points.z},
//								p2 = {edgePoint[tempIndx[1]].points.x, 0.0f, edgePoint[tempIndx[1]].points.z},
//								p3 = {edgePoint[tempIndx[2]].points.x, 0.0f, edgePoint[tempIndx[2]].points.z};
//							area += TriangleArea(p1,p2,p3);
//
//							for(int j=0; j<3; j++)
//							{
//								faceVerId.push_back( tempIndx[2-j] );
//								int sid;
//								sid = tempIndx[j];
//								///	fprintf(fp,"%d\n",sid);
//							}
//						}
//						///fprintf(fp,"\n");
//						///fclose(fp);			
//						return area;///划分完毕
//					}else if( (num - storeNum) < 3 )
//					{
//#ifdef _DEBUG ///wang 12.3
//						AfxMessageBox( _T("顶点数<3,划分方法需测试") );
//#endif
//					}
//				}
//			}else if( (num - storeNum) == 3 )
//			{
//				CVector3 p[3];
//				int tempId=0;
//				for( int j = 0; j < num; j++ )
//				{
//					if(!edgePoint[j].isStored)
//					{
//						if (tempId>2)
//						{
//							break;;
//						}
//						faceVerId.push_back( j );
//						p[tempId].x = edgePoint[j].points.x;
//						p[tempId].y = 0.0f;
//						p[tempId].z = edgePoint[j].points.z;
//
//						int sid;
//						sid = j;
//						tempId ++;
//						///fprintf(fp,"%d\n",sid);
//					}
//				}
//				area += TriangleArea(p[0],p[1],p[2]);
//				///fprintf(fp,"\n");
//				///fclose(fp);						
//				return area;///划分完毕
//			}else if( (num - storeNum) < 3 )
//			{
//#ifdef _DEBUG ///wang 12.3
//				AfxMessageBox( _T("顶点数<3,划分方法需测试") );
//#endif
//			}
//		}
//	}
//	return area;
//}
//
//float CGeoMaths::TriangleArea(const CVector3 &p1, const CVector3 &p2, const CVector3 &p3)///根据三个顶点计算三角形的面积。
//{
//	CMaths math;
//	CCollision cll;
//	float area = 0.0f, len1 = 0.0f, len2 = 0.0f, len3 = 0.0f, length = 0.0f;
//	len1 = math.Length(p1, p2);
//	len2 = math.Length(p2, p3);
//	len3 = math.Length(p3, p1);
//	length = (len1+len2+len3)*0.5f;
//	area = sqrt(length*(length-len1)*(length-len2)*(length-len3));///=√[P(P-A)(P-B)(P-C)]
//	if (_isnan(area))
//	{///wql 2019.12.3 避免返回无效值
//		return 0.0f;
//	}
//	return area;
//}
///***********************************************
//作者： 王庆利   
//功能描述：计算将非连通域平面分割为三角片后的面索引
//输入参数：lineGroup该面上所有顶点，按一定顺序划分行和列，即包括边界点和内部镂空边界与其他所有边的交点，
//此所有点按照某一轴坐标划分为多个行，每一行又按顺序排列
//polygon面内部的所有镂空边界顶点
//输出参数：无
//返回参数说明：返回三角片后的面索引
//************************************************/
//std::vector<int> CGeoMaths::CreateFaceIndex(std::vector<CPointsGroup> &lineGroup, std::vector<CPointsGroup>  &polygon, 
//											std::vector<CVector3> edgePoint, int axis)
//{
//	m_points = edgePoint;
//	std::vector<int> faceId;
//	LeftToRight(faceId, lineGroup, polygon, axis);
//	RightToLeft(faceId, lineGroup, polygon, axis);
//	return faceId;
//}
//
///***********************************************
//作者： 王庆利   
//功能描述：根据非连通域平面计算划分索引
//输入参数：lineGroup该面上所有顶点，按一定顺序划分行和列，即包括边界点和内部镂空边界与其他所有边的交点，
//此所有点按照某一轴坐标划分为多个行，每一行又按顺序排列
//polygon面内部的所有镂空边界顶点
//faceId面索引
//输出参数：无
//返回参数说明：无
//************************************************/
//void CGeoMaths::LeftToRight(std::vector<int> &faceId, std::vector<CPointsGroup> &lineGroup, std::vector<CPointsGroup>  &polygon, int axis)///扫描线法从左至右
//{
//	int totalId = 0;
//	float x=0.0f, y=0.0f;///充参数
//	for(int i=0; i<(int)lineGroup.size()-1; i++)//对每一行,除最后一行外
//	{
//		for(int initDownIndex = i+1; initDownIndex<(int)lineGroup.size(); initDownIndex++)///先将以后各行的初始下边处理索引置零
//		{
//			lineGroup[initDownIndex].nowDownIndex = 0;
//		}
//		int tempId[3]={0,0,0};///预存当前面的索引
//		CVector3 tempPoints[3];///预存当前面的三个顶点 
//		int ptNum = (int)lineGroup[i].points.size()-1;
//		for(int j=0; j<ptNum; j++)///对每行中每个点，除最后一个点外
//		{
//			if(lineGroup[i].nowUpIndex >= ptNum)
//			{
//				break;
//			}
//			///if(lineGroup[i].nowUpIndex<j)
//			{
//				lineGroup[i].nowUpIndex = j;
//			}
//			if(InOrIsOneLineOfPolygon(lineGroup[i].points[j],lineGroup[i].points[j+1], polygon))///该点与下一点是否同一镂空元素中的点
//			{
//				j++;
//			}
//			if(j>=ptNum)
//			{
//				break;
//			}
//			tempId[0] = totalId + j;
//			tempPoints[0] = lineGroup[i].points[j];
//			bool crossFirst = true;///默认相交
//			int baseThisLine = ptNum;///该行点基数，尽量远离首顶点
//			while(crossFirst)///一直找到连线不与镂空元素相交的两个点作三角形的第一条边
//			{
//				if(baseThisLine <= j)///超前当前点则跳出
//				{
//					break;
//				}
//				crossFirst = CrossWithOthers(lineGroup[i].points[j], lineGroup[i].points[baseThisLine], polygon);///判断该行的首顶点与最后顶点的连线是否与镂空元素有相交
//				baseThisLine --;
//			}
//			lineGroup[i].nowUpIndex = baseThisLine+1;
//			if(!crossFirst)///不相交
//			{
//				///还要检验后确定此三角片的第一条边（横边）
//				///tempId[1] = toltalId + lineGroup[i].nowIndex;
//				int subTotal = (int)lineGroup[i].points.size(), line2 = i+1;///寻找竖边顶点的索引, 竖边顶点所在的行
//				bool id1Ok = false, id2Ok = false;
//				CVector3 point3={0.0,0.0,0.0};
//				for(line2=i+1; line2<(int)lineGroup.size(); line2++)///从下一行开始查找有效竖边
//				{
//					for(int k=lineGroup[line2].nowDownIndex; k<(int)lineGroup[line2].points.size(); k++)
//					{///int k=0; k<lineGroup[line2].points.size(); k++)///
//						// 						if(k<(int)lineGroup[line2].points.size()-1)
//						// 						{
//						// 							CVector3 midPos = {(lineGroup[line2].points[k+1].points.x+lineGroup[line2].points[k].points.x)/2.0f,0.0f,
//						// 								(lineGroup[line2].points[k+1].points.z+lineGroup[line2].points[k].points.z)/2.0f};
//						// 							if(!bPointInEdge(midPos, m_points))
//						// 							{
//						// 								continue;
//						// 							}
//						// 						}
//						point3 = lineGroup[line2].points[k];///三角片的第三个顶点
//						CVector3 midPos = {(lineGroup[i].points[j].x+point3.x)/2.0f,0.0f,(lineGroup[i].points[j].z+point3.z)/2.0f};
//						if(!CrossWithOthers(lineGroup[i].points[j], point3, polygon) && bPointInEdge(midPos, m_points))///不与其他相交，并中点在吊顶边界内
//						{
//							lineGroup[line2].nowDownIndex = k;
//							id2Ok = true;
//							break;
//						}
//					}	
//					if(id2Ok)
//					{
//						tempId[2] = totalId + subTotal + lineGroup[line2].nowDownIndex;
//						tempPoints[2] = point3;
//						lineGroup[line2].nowDownIndex ++;
//						break;
//					}
//					else
//					{
//						subTotal += (int)lineGroup[line2].points.size();
//					}
//				}
//
//				if(id2Ok)///找到了第三个点
//				{
//					for(int k=lineGroup[i].nowUpIndex; k>j; k--)
//					{
//						CVector3 midPos = {(lineGroup[i].points[k].x+point3.x)/2.0f,0.0f,(lineGroup[i].points[k].z+point3.z)/2.0f};
//						if(!CrossWithOthers(lineGroup[i].points[k], point3, polygon) && bPointInEdge(midPos, m_points))///不与其他相交，并中点在吊顶边界内
//						{
//							lineGroup[i].nowUpIndex = k;
//							tempId[1] = totalId + lineGroup[i].nowUpIndex;
//							tempPoints[1] = lineGroup[i].points[k];
//							id1Ok = true;
//							break;
//						}
//					}
//
//					if(id1Ok)
//					{
//						if(NoPointInTriangle(tempPoints, polygon))
//						{
//							for(int k=0; k<3; k++)
//							{
//								faceId.push_back(tempId[k]);
//							}
//						}
//						else
//						{
//							lineGroup[i].nowUpIndex = j;
//							lineGroup[line2].nowDownIndex = 0;
//						}
//					}
//					else
//					{
//						///是否有必要从line2的下一行重新找竖点？？待定///YES
//						id2Ok = false;
//						int ssubNum = 0;
//						int line = line2;
//						for(line = line2; line<(int)lineGroup.size()-1; line++)
//						{
//							int tempDown = lineGroup[line].nowDownIndex;
//							for(int k=tempDown; k<(int)lineGroup[line].points.size(); k++)
//							{
//								point3 = lineGroup[line].points[k];///三角片新的第三个顶点
//								CVector3 midPos = {(lineGroup[i].points[j].x+point3.x)/2.0f,0.0f,(lineGroup[i].points[j].z+point3.z)/2.0f};
//								bool inside = bPointInEdge(midPos, m_points);
//								if(!CrossWithOthers(lineGroup[i].points[j], point3, polygon) && inside)///不与其他相交，并中点在吊顶边界内
//								{///再次检验竖边是否有相交///不相交则找到临时第三点
//									tempId[2] = totalId + subTotal + ssubNum + k;///lineGroup[line].nowDownIndex;
//									tempPoints[2] = point3;
//									lineGroup[line].nowDownIndex ++;
//									id2Ok = true;
//								}
//								else
//								{///相交，
//									if(inside)
//									{
//										lineGroup[line].nowDownIndex ++;
//									}
//									continue;
//								}
//
//								if(id2Ok)
//								{
//									int tempUpIndex = lineGroup[i].nowUpIndex;
//									for(int new1Pt=tempUpIndex; new1Pt>j; new1Pt--)
//									{
//										CVector3 midPos = {(lineGroup[i].points[new1Pt].x+point3.x)/2.0f,0.0f,(lineGroup[i].points[new1Pt].z+point3.z)/2.0f};
//										if(!CrossWithOthers(lineGroup[i].points[new1Pt], point3, polygon) && bPointInEdge(midPos, m_points))///不与其他相交，并中点在吊顶边界内
//										{
//											tempUpIndex = new1Pt;
//											tempId[1] = totalId + tempUpIndex;
//											tempPoints[1] = lineGroup[i].points[new1Pt];
//											id1Ok = true;
//											break;
//										}
//									}
//									if(id1Ok)
//									{
//										lineGroup[i].nowUpIndex = tempUpIndex;
//										break;
//									}
//									else
//									{
//										lineGroup[line2].nowDownIndex = 0;
//									}
//								}
//								else
//								{
//									lineGroup[line2].nowDownIndex = 0;
//								}
//							}
//							if(id1Ok)
//							{
//								line2 = line;
//								break;
//							}
//							ssubNum += (int)lineGroup[line].points.size();
//						}
//						if(id1Ok)
//						{
//							if(NoPointInTriangle(tempPoints, polygon))
//							{
//								for(int k=0; k<3; k++)
//								{
//									faceId.push_back(tempId[k]);
//								}
//							}
//							else
//							{
//								lineGroup[i].nowUpIndex = j;
//								lineGroup[line2].nowDownIndex = 0;
//							}
//						}
//						else
//						{
//							lineGroup[i].nowUpIndex = j;
//							///是否有必要从line2的下一行重新找竖点？？待定///YES
//							// 							id2Ok = false;
//							// 							int ssubNum = 0;
//							// 							int line = line2;
//							// 							for(line = line2; line<(int)lineGroup.size()-1; line++)
//							// 							{
//							// 								int tempDown = lineGroup[line].nowDownIndex;
//							// 								for(int k=tempDown; k<(int)lineGroup[line].points.size(); k++)
//							// 								{
//							// 									point3 = lineGroup[line].points[k];///三角片新的第三个顶点
//							// 									CVector3 midPos = {(lineGroup[i].points[j].points.x+point3.x)/2.0f,0.0f,(lineGroup[i].points[j].points.z+point3.z)/2.0f};
//							// 									bool inside = bPointInEdge(midPos, m_points);
//							// 									if(!CrossWithOthers(lineGroup[i].points[j], point3) && inside)///不与其他相交，并中点在吊顶边界内
//							// 									{///再次检验竖边是否有相交///不相交则找到临时第三点
//							// 										tempId[2] = totalId + subTotal + ssubNum + lineGroup[line].nowDownIndex;
//							// 										tempPoints[2] = point3;
//							// 										lineGroup[line].nowDownIndex ++;
//							// 										id2Ok = true;
//							// 									}
//							// 									else
//							// 									{///相交，
//							// 										if(inside)
//							// 										{
//							// 											lineGroup[line].nowDownIndex ++;
//							// 										}
//							// 										continue;
//							// 									}
//							// 
//							// 									if(id2Ok)
//							// 									{
//							// 										int tempUpIndex = lineGroup[i].nowUpIndex;
//							// 										for(int new1Pt=tempUpIndex; new1Pt>j; new1Pt--)
//							// 										{
//							// 											CVector3 midPos = {(lineGroup[i].points[new1Pt].points.x+point3.x)/2.0f,0.0f,(lineGroup[i].points[new1Pt].points.z+point3.z)/2.0f};
//							// 											if(!CrossWithOthers(lineGroup[i].points[new1Pt], point3) && bPointInEdge(midPos, m_points))///不与其他相交，并中点在吊顶边界内
//							// 											{
//							// 												tempUpIndex = new1Pt;
//							// 												tempId[1] = totalId + tempUpIndex;
//							// 												tempPoints[1] = lineGroup[i].points[new1Pt];
//							// 												id1Ok = true;
//							// 												break;
//							// 											}
//							// 										}
//							// 										if(id1Ok)
//							// 										{
//							// 											lineGroup[i].nowUpIndex = tempUpIndex;
//							// 											break;
//							// 										}
//							// 										else
//							// 										{
//							// 											lineGroup[line2].nowDownIndex = 0;
//							// 										}
//							// 									}
//							// 									else
//							// 									{
//							// 										lineGroup[line2].nowDownIndex = 0;
//							// 									}
//							// 								}
//							// 								if(id1Ok)
//							// 								{
//							// 									line2 = line;
//							// 									break;
//							// 								}
//							// 								ssubNum += (int)lineGroup[line].points.size();
//							// 							}
//							// 							if(id1Ok)
//							// 							{
//							// 								if(NoPointInTriangle(tempPoints))
//							// 								{
//							// 									for(int k=0; k<3; k++)
//							// 									{
//							// 										faceId.push_back(tempId[k]);
//							// 									}
//							// 								}
//							// 								else
//							// 								{
//							// 									lineGroup[i].nowUpIndex = j;
//							// 									lineGroup[line2].nowDownIndex = 0;
//							// 								}
//							// 							}
//							// 							else
//							// 							{
//							// 								lineGroup[i].nowUpIndex = j;
//							// 							}
//						}
//					}
//				}
//				else
//				{
//					lineGroup[i].nowUpIndex = j;
//					///break;
//					///MessageBox(NULL, _T("镂空吊顶竖点有错，竖边不通过，应该不会出现此种情况"),TEXT("错误"),MB_OK);///即肯定能找到不相交的点
//				}
//			}
//			else///相交
//			{
//				lineGroup[i].nowUpIndex = j;
//				break;
//				///MessageBox(NULL, _T("镂空吊顶每行求交有错，横边不通过，应该不会出现此种情况"),TEXT("错误"),MB_OK);///即肯定能找到不相交的点
//			}
//		}
//		totalId += (int)lineGroup[i].points.size();///点基数增加本行数目
//	}
//}
//
///***********************************************
//作者： 王庆利   
//功能描述：根据非连通域平面计算划分索引
//输入参数：lineGroup该面上所有顶点，按一定顺序划分行和列，即包括边界点和内部镂空边界与其他所有边的交点，
//此所有点按照某一轴坐标划分为多个行，每一行又按顺序排列
//polygon面内部的所有镂空边界顶点
//faceId面索引
//输出参数：无
//返回参数说明：无
//************************************************/
//void CGeoMaths::RightToLeft(std::vector<int> &faceId, std::vector<CPointsGroup> &lineGroup, std::vector<CPointsGroup>  &polygon, int axis)///扫描线法从右至左
//{
//	int totalId = 0;
//	float x=0.0f, y=0.0f;
//	for(int i=0; i<(int)lineGroup.size(); i++)
//	{
//		lineGroup[i].nowUpIndex = (int)lineGroup[i].points.size()-1;
//		lineGroup[i].nowDownIndex = (int)lineGroup[i].points.size()-1;
//		totalId += (int)lineGroup[i].points.size();
//	}
//	for(int i=(int)lineGroup.size()-1; i>0; i--)///对每一行,除第一行外
//	{
//		for(int initDownIndex = i-1; initDownIndex>=0; initDownIndex--)///先将以上各行的初始下边处理索引置最大
//		{
//			lineGroup[initDownIndex].nowDownIndex = (int)lineGroup[initDownIndex].points.size()-1;
//		}
//		int tempId[3]={0.0,0.0,0.0};///预存当前面的索引
//		CVector3 tempPoints[3];///预存当前面的三个顶点
//		int ptNum = 0;
//		for(int j=(int)lineGroup[i].points.size()-1; j>0; j--)///对每行中每个点，除最后一个点外
//		{
//			if(lineGroup[i].nowUpIndex <= ptNum)
//			{
//				break;
//			}
//			///if(j>lineGroup[i].nowUpIndex)
//			{
//				lineGroup[i].nowUpIndex = j;
//			}
//			if(InOrIsOneLineOfPolygon(lineGroup[i].points[j],lineGroup[i].points[j-1], polygon))///该点与下一点是否同一镂空元素中的点
//			{
//				j--;
//			}
//			if(j<=ptNum)
//			{
//				break;
//			}
//			tempId[0] = totalId + j - (int)lineGroup[i].points.size();
//			tempPoints[0] = lineGroup[i].points[j];
//			bool crossFirst = true;///默认相交
//			int baseThisLine = ptNum;///该行点基数，尽量远离首顶点
//			while(crossFirst)///一直找到连线不与镂空元素相交的两个点作三角形的第一条边
//			{
//				if(baseThisLine >= j)///超前当前点则跳出
//				{
//					break;
//				}
//				crossFirst = CrossWithOthers(lineGroup[i].points[j], lineGroup[i].points[baseThisLine], polygon);///判断该行的首顶点与最后顶点的连线是否与镂空元素有相交
//				baseThisLine ++;
//			}
//			lineGroup[i].nowUpIndex = baseThisLine-1;
//			if(!crossFirst)///不相交
//			{
//				///还要检验后确定此三角片的第一条边（横边）
//				///tempId[1] = toltalId + lineGroup[i].nowIndex;
//				int subTotal = (int)lineGroup[i].points.size(), line2 = i-1;///寻找竖边顶点的索引, 竖边顶点所在的行
//				bool id1Ok = false, id2Ok = false;
//				CVector3 point3={0.0,0.0,0.0};
//				for(line2=i-1; line2>=0; line2--)///从下一行开始查找有效竖边
//				{
//					for(int k=lineGroup[line2].nowDownIndex; k>=0; k--)
//					{
//						point3 = lineGroup[line2].points[k];///三角片的第三个顶点
//						CVector3 midPos = {(lineGroup[i].points[j].x+point3.x)/2.0f,0.0f,(lineGroup[i].points[j].z+point3.z)/2.0f};
//						if(!CrossWithOthers(lineGroup[i].points[j], point3, polygon) && bPointInEdge(midPos, m_points))///不与其他相交，并中点在吊顶边界内
//						{
//							lineGroup[line2].nowDownIndex = k;
//							id2Ok = true;
//							break;
//						}
//					}	
//					if(id2Ok)
//					{
//						tempId[2] = totalId - subTotal - (int)lineGroup[line2].points.size() + lineGroup[line2].nowDownIndex;
//						tempPoints[2] = point3;
//						lineGroup[line2].nowDownIndex --;
//						break;
//					}
//					else
//					{
//						subTotal += (int)lineGroup[line2].points.size();
//					}
//				}
//
//				if(id2Ok)///找到了第三个点
//				{
//					for(int k=lineGroup[i].nowUpIndex; k<j; k++)
//					{
//						CVector3 midPos = {(lineGroup[i].points[k].x+point3.x)/2.0f,0.0f,(lineGroup[i].points[k].z+point3.z)/2.0f};
//						if(!CrossWithOthers(lineGroup[i].points[k], point3, polygon) && bPointInEdge(midPos, m_points))///不与其他相交，并中点在吊顶边界内
//						{
//							lineGroup[i].nowUpIndex = k;
//							tempId[1] = totalId + lineGroup[i].nowUpIndex - (int)lineGroup[i].points.size();
//							tempPoints[1] = lineGroup[i].points[k];
//							id1Ok = true;
//							break;
//						}
//					}
//
//					if(id1Ok)
//					{
//						if(NoPointInTriangle(tempPoints, polygon))
//						{
//							for(int k=0; k<3; k++)
//							{
//								faceId.push_back(tempId[k]);
//							}
//						}
//						else
//						{
//							lineGroup[i].nowUpIndex = j;
//							lineGroup[line2].nowDownIndex = (int)lineGroup[line2].points.size()-1;
//						}
//					}
//					else
//					{
//						///是否有必要从line2的下一行重新找竖点？？待定///YES
//						id2Ok = false;
//						int ssubNum = 0;
//						for(int line = line2; line>=0; line--)
//						{
//							int tempDown = lineGroup[line].nowDownIndex;
//							for(int k=tempDown; k>0; k--)
//							{
//								point3 = lineGroup[line].points[k];///三角片新的第三个顶点
//								CVector3 midPos = {(lineGroup[i].points[j].x+point3.x)/2.0f,0.0f,(lineGroup[i].points[j].z+point3.z)/2.0f};
//								bool inside = bPointInEdge(midPos, m_points);
//								if(!CrossWithOthers(lineGroup[i].points[j], point3, polygon) && inside)///不与其他相交，并中点在吊顶边界内
//								{///再次检验竖边是否有相交///不相交则找到临时第三点
//									tempId[2] = totalId - subTotal - ssubNum + k - (int)lineGroup[line].points.size();
//									tempPoints[2] = point3;
//									lineGroup[line].nowDownIndex --;
//									id2Ok = true;
//								}
//								else
//								{///相交，
//									if(inside)
//									{
//										lineGroup[line].nowDownIndex --;
//									}
//									continue;
//								}
//
//								if(id2Ok)
//								{
//									int tempUpIndex = lineGroup[i].nowUpIndex;
//									for(int new1Pt=tempUpIndex; new1Pt<j; new1Pt++)
//									{
//										CVector3 midPos = {(lineGroup[i].points[new1Pt].x+point3.x)/2.0f,0.0f,(lineGroup[i].points[new1Pt].z+point3.z)/2.0f};
//										if(!CrossWithOthers(lineGroup[i].points[new1Pt], point3, polygon) && bPointInEdge(midPos, m_points))///不与其他相交，并中点在吊顶边界内
//										{
//											tempUpIndex = new1Pt;
//											tempId[1] = totalId + tempUpIndex - (int)lineGroup[i].points.size();
//											tempPoints[1] = lineGroup[i].points[new1Pt];
//											id1Ok = true;
//											break;
//										}
//									}
//									if(id1Ok)
//									{
//										lineGroup[i].nowUpIndex = tempUpIndex;
//										break;
//									}
//									else
//									{
//										lineGroup[line2].nowDownIndex = (int)lineGroup[line2].points.size()-1;
//									}
//								}
//								else
//								{
//									lineGroup[line2].nowDownIndex = (int)lineGroup[line2].points.size()-1;
//								}
//							}
//							if(id1Ok)
//							{
//								break;
//							}
//							ssubNum += (int)lineGroup[line].points.size();
//						}
//						if(id1Ok)
//						{
//							if(NoPointInTriangle(tempPoints, polygon))
//							{
//								for(int k=0; k<3; k++)
//								{
//									faceId.push_back(tempId[k]);
//								}
//							}
//							else
//							{
//								lineGroup[i].nowUpIndex = j;
//								lineGroup[line2].nowDownIndex = (int)lineGroup[line2].points.size()-1;
//							}
//						}
//						else
//						{
//							lineGroup[i].nowUpIndex = j;
//						}
//						///MessageBox(NULL, _T("镂空吊顶竖点有错，斜边不通过，应该不会出现此种情况"),TEXT("错误"),MB_OK);///即肯定能找到不相交的点
//					}
//				}
//				else
//				{
//					lineGroup[i].nowUpIndex = j;
//					///break;
//					///MessageBox(NULL, _T("镂空吊顶竖点有错，竖边不通过，应该不会出现此种情况"),TEXT("错误"),MB_OK);///即肯定能找到不相交的点
//				}
//			}
//			else///相交
//			{
//				lineGroup[i].nowUpIndex = j;
//				///break;
//				///MessageBox(NULL, _T("镂空吊顶每行求交有错，横边不通过，应该不会出现此种情况"),TEXT("错误"),MB_OK);///即肯定能找到不相交的点
//			}
//		}
//		totalId -= (int)lineGroup[i].points.size();///点基数增加本行数目
//	}
//}
//
///***********************************************
//作者： 王庆利   
//功能描述：根据非连通域平面创建一个平面形体
//输入参数：lineGroup该面上所有顶点，按一定顺序划分行和列，即包括边界点和内部镂空边界与其他所有边的交点，
//此所有点按照某一轴坐标划分为多个行，每一行又按顺序排列
//polygon面内部的所有镂空边界顶点
//ptGeom创建的形体
//输出参数：无
//返回参数说明：无
//************************************************/
//void CGeoMaths::CreateHoleFace(std::vector<CPointsGroup> &lineGroup, std::vector<CPointsGroup>  &polygon, CGeomentry_Information *ptGeom,
//							   std::vector<CVector3> edgePoint, int axis)
//{
//	m_points.clear();
//	m_points = edgePoint;
//}
//
///***********************************************
//作者： 王庆利   
//功能描述：根据非连通域平面中的镂空侧面
//输入参数：polygon面内部的所有镂空边界顶点， ptGeom创建的形体
//输出参数：无
//返回参数说明：无
//************************************************/
//void CGeoMaths::CreateInside(CPointsGroup &ptsGroup, CGeomentry_Information *ptGeom, int axis)
//{
//
//}
//
///***********************************************
//作者： 王庆利   创建时间 ： 2010.1.2
//功能描述：根据两组顺序点创建形体
//输入参数：upPoint上侧一周顶点， downPoint下侧一周顶点
//输出参数：无
//返回参数说明：无
//************************************************/
//void CGeoMaths::CreateGeoWithTwoPointLists(const std::vector<CVector3> &objUpPoint,const std::vector<CVector3> &objDownPoint, 
//										   CGeomentry_Information *ptGeom)
//{
//	int num = (int) objDownPoint.size();
//	if(num != (int)objUpPoint.size()) return;  
//
//	//点纹理信息
//	CVMapCoords coords;
//	coords.numberOfMapCoords=1;
//	CVertexMapCoords tex,*pTex;
//	tex.MapCoords.u = tex.MapCoords.v = 0; //jiang gai 2014.8.5
//	coords.UVWCoords.push_back(tex);
//	pTex = &coords.UVWCoords[coords.UVWCoords.size()-1];
//	pTex->MapIndex = 0;
//
//	//面纹理信息,目前每个形体只有一个纹理，且作用于所有的面
//	CMatAndFace mf1,*pMf1;
//	mf1.Materials_Index=0;
//	ptGeom->Materials.numberOfMaterials=1;//目前每个形体只有一个纹理
//	ptGeom->Materials.MaterialProp.push_back(mf1);
//	pMf1 = &ptGeom->Materials.MaterialProp[ptGeom->Materials.MaterialProp.size()-1];
//	pMf1->numberOfFaces = 0;
//
//	CVector3 point;
//	CVector3 color = {1.0f, 1.0f, 1.0f};
//	///存入饰物所有顶点
//	for(int i=0; i<num; i++)
//	{
//		point = objUpPoint[i];				
//		///该点所对应的纹理坐标
//		pTex->MapCoords.u = 0;
//		pTex->MapCoords.v = 0;
//		ptGeom->Vertices.VPoint.push_back(point);
//		ptGeom->Vertices.VColors.push_back(color);
//		ptGeom->Vertices.VMaps.push_back(coords);
//
//		point = objUpPoint[ (i+1)%num ];		
//		///该点所对应的纹理坐标
//		pTex->MapCoords.u = 0;
//		pTex->MapCoords.v = 0;
//		ptGeom->Vertices.VPoint.push_back(point);
//		ptGeom->Vertices.VColors.push_back(color);
//		ptGeom->Vertices.VMaps.push_back(coords);
//
//		point = objDownPoint[ (i+1)%num ];		
//		///该点所对应的纹理坐标
//		pTex->MapCoords.u = 0;
//		pTex->MapCoords.v = 0;
//		ptGeom->Vertices.VPoint.push_back(point);
//		ptGeom->Vertices.VColors.push_back(color);
//		ptGeom->Vertices.VMaps.push_back(coords);
//
//		point = objDownPoint[i];				
//		///该点所对应的纹理坐标
//		pTex->MapCoords.u = 0;
//		pTex->MapCoords.v = 0;
//		ptGeom->Vertices.VPoint.push_back(point);
//		ptGeom->Vertices.VColors.push_back(color);
//		ptGeom->Vertices.VMaps.push_back(coords);
//
//		int nn = num*4;
//		ptGeom->Faces.Vertices_Index.push_back(4*i);
//		ptGeom->Faces.Vertices_Index.push_back((4*i+1)%nn);
//		ptGeom->Faces.Vertices_Index.push_back((4*i+2)%nn);
//		pMf1->Face_Index.push_back(pMf1->numberOfFaces);
//		pMf1->numberOfFaces ++;
//
//		ptGeom->Faces.Vertices_Index.push_back(4*i);
//		ptGeom->Faces.Vertices_Index.push_back((4*i+2)%nn);
//		ptGeom->Faces.Vertices_Index.push_back((4*i+3)%nn);
//		pMf1->Face_Index.push_back(pMf1->numberOfFaces);
//		pMf1->numberOfFaces ++;
//	}
//
//	ptGeom->Vertices.numberOfVertices += 4*num;  //点的总数加 
//	ptGeom->Faces.numberOfFaces += pMf1->numberOfFaces;//面的总数加2
//}
//
///***********************************************
//作者： 王庆利   创建时间 ： 2010.1.2
//功能描述：对于实体元素，创建该元素的模型的上或下侧形体
//输入参数：points上或下侧一周顶点
//输出参数：无
//返回参数说明：无
//************************************************/
//void CGeoMaths::CreateSolidUpAndDownside(const std::vector<CVector3> &points, CGeomentry_Information *ptGeom)
//{
//	//点纹理信息
//	CVMapCoords coords;
//	coords.numberOfMapCoords=1;
//	CVertexMapCoords tex,*pTex;
//	tex.MapCoords.u = tex.MapCoords.v = 0; //jiang gai 2014.8.5
//	coords.UVWCoords.push_back(tex);
//	pTex = &coords.UVWCoords[coords.UVWCoords.size()-1];
//	pTex->MapIndex = 0;
//
//	//面纹理信息,目前每个形体只有一个纹理，且作用于所有的面
//	CMatAndFace mf1,*pMf1;
//	mf1.Materials_Index=0;
//	ptGeom->Materials.numberOfMaterials=1;//目前每个形体只有一个纹理
//	ptGeom->Materials.MaterialProp.push_back(mf1);
//	pMf1 = &ptGeom->Materials.MaterialProp[ptGeom->Materials.MaterialProp.size()-1];
//	pMf1->numberOfFaces = 0;
//
//	CVector3 color = {1.0f, 1.0f, 1.0f};
//	int num = (int)points.size();
//	///存入饰物所有顶点
//	for(int j=0;j<num;j++)///前cirNum*2个是外圆周，0-----cirNum*2-1,后cirNum*2是内圆周，cirNum*2-------cirNum*4-1
//	{	
//		ptGeom->Vertices.VPoint.push_back(points[j]);///饰物所有点
//		ptGeom->Vertices.VColors.push_back(color);
//
//		pTex->MapCoords.u = 0.0;
//		pTex->MapCoords.v = 0.0;
//		ptGeom->Vertices.VMaps.push_back(coords);
//	}
//	ptGeom->Vertices.numberOfVertices += num;  //点的总数加 
//
//	for(int j=0; j<num-2; j++)///圆盘上面,用最后二点（上下圆心） 连接圆周点形成圆面
//	{
//		ptGeom->Faces.Vertices_Index.push_back(0);///上圆面每个三角形以上圆心作起点
//		ptGeom->Faces.Vertices_Index.push_back((j+1)%num);
//		ptGeom->Faces.Vertices_Index.push_back((j+2)%num);
//		pMf1->Face_Index.push_back(pMf1->numberOfFaces);
//		pMf1->numberOfFaces++;
//	}
//	ptGeom->Faces.numberOfFaces += pMf1->numberOfFaces;///面的总数加
//}
//
///***********************************************
//作者： 王庆利   创建时间 ： 2010.1.2
//功能描述：对于中间有一个孔的元素，创建该元素的模型的上或下侧形体
//输入参数：outPts上或下侧的外侧一周顶点，inPts上或下侧的内侧一周顶点；当面法线朝上时outPts和inPts要按内外侧反过来应用
//输出参数：无
//返回参数说明：无
//************************************************/
//void CGeoMaths::CreateLoopUpAndDownside(const std::vector<CVector3> &objOutPts,const std::vector<CVector3> &objInPts, CGeomentry_Information *ptGeom)
//{
//	int num = objOutPts.size();
//	if(num != objInPts.size()) return;
//
//	//点纹理信息
//	CVMapCoords coords;
//	coords.numberOfMapCoords=1;
//	CVertexMapCoords tex,*pTex;
//	tex.MapCoords.u = tex.MapCoords.v = 0; //jiang gai 2014.8.5
//	coords.UVWCoords.push_back(tex);
//	pTex = &coords.UVWCoords[coords.UVWCoords.size()-1];
//	pTex->MapIndex = 0;
//
//	//面纹理信息,目前每个形体只有一个纹理，且作用于所有的面
//	CMatAndFace mf1,*pMf1;
//	mf1.Materials_Index=0;
//	ptGeom->Materials.numberOfMaterials=1;//目前每个形体只有一个纹理
//	ptGeom->Materials.MaterialProp.push_back(mf1);
//	pMf1 = &ptGeom->Materials.MaterialProp[ptGeom->Materials.MaterialProp.size()-1];
//	pMf1->numberOfFaces = 0;
//
//	CVector3 point;
//	CVector3 color = {1.0f, 1.0f, 1.0f};
//	///存入饰物所有顶点
//	for(int i=0;i<num;i++)
//	{
//		point = objOutPts[i];				
//		///该点所对应的纹理坐标
//		pTex->MapCoords.u = 0;
//		pTex->MapCoords.v = 0;
//		ptGeom->Vertices.VPoint.push_back(point);
//		ptGeom->Vertices.VColors.push_back(color);
//		ptGeom->Vertices.VMaps.push_back(coords);
//
//		point = objOutPts[ (i+1)%num ];		
//		///该点所对应的纹理坐标
//		pTex->MapCoords.u = 0;
//		pTex->MapCoords.v = 0;
//		ptGeom->Vertices.VPoint.push_back(point);
//		ptGeom->Vertices.VColors.push_back(color);
//		ptGeom->Vertices.VMaps.push_back(coords);
//
//		point = objInPts[ (i+1)%num ];		
//		///该点所对应的纹理坐标
//		pTex->MapCoords.u = 0;
//		pTex->MapCoords.v = 0;
//		ptGeom->Vertices.VPoint.push_back(point);
//		ptGeom->Vertices.VColors.push_back(color);
//		ptGeom->Vertices.VMaps.push_back(coords);
//
//		point = objInPts[i];				
//		///该点所对应的纹理坐标
//		pTex->MapCoords.u = 0;
//		pTex->MapCoords.v = 0;
//		ptGeom->Vertices.VPoint.push_back(point);
//		ptGeom->Vertices.VColors.push_back(color);
//		ptGeom->Vertices.VMaps.push_back(coords);
//
//		int nn = num*4;
//		ptGeom->Faces.Vertices_Index.push_back(4*i);
//		ptGeom->Faces.Vertices_Index.push_back((4*i+1)%nn);
//		ptGeom->Faces.Vertices_Index.push_back((4*i+2)%nn);
//		pMf1->Face_Index.push_back(pMf1->numberOfFaces);
//		pMf1->numberOfFaces ++;
//
//		ptGeom->Faces.Vertices_Index.push_back(4*i);
//		ptGeom->Faces.Vertices_Index.push_back((4*i+2)%nn);
//		ptGeom->Faces.Vertices_Index.push_back((4*i+3)%nn);
//		pMf1->Face_Index.push_back(pMf1->numberOfFaces);
//		pMf1->numberOfFaces ++;
//	}
//
//	ptGeom->Vertices.numberOfVertices += num*4;  //点的总数加 
//	ptGeom->Faces.numberOfFaces += pMf1->numberOfFaces;///面的总数加
//}
//
///***********************************************
//作者： 王庆利   
//功能描述：线段是否与多边形相交
//输入参数：polygon三角形, polygonGroup点组
//输出参数：无
//返回参数说明：没有返回true
//************************************************/
//bool CGeoMaths::CrossWithOthers(const CVector3 &point1, const CVector3 &point2, std::vector<CPointsGroup> &polygonGroup)///判断该线段是否与镂空元素相交
//{
//	CVector3 dir1 = Normalize(Vector(point1, point2));
//	float error = 0.00002f;///误差
//	CCollision cll;///用于相交测试
//	CVector2 pt1 = {point1.x, point1.z}, pt2 = {point2.x, point2.z};
//	CVector2 midPt = {(pt1.u+pt2.u)/2, (pt1.v+pt2.v)/2};
//	for( int i = 0; i < (int)polygonGroup.size(); i++ )
//	{
//		std::vector<CVector2> tempPolygon;
//		int ptNum = (int)polygonGroup[i].points.size();
//		for(int j=0; j<ptNum; j++)
//		{
//			CVector2 crosspoint={0.0f,0.0f}, polygonPt1 = {polygonGroup[i].points[j].x, polygonGroup[i].points[j].z},
//				polygonPt2 = {polygonGroup[i].points[ (j+1)%ptNum ].x, polygonGroup[i].points[ (j+1)%ptNum ].z};
//			tempPolygon.push_back(polygonPt1);///搜集该元素所有边界的二维点，便于检测中点是否在边界内
//			CVector3 dir2 = Normalize(Vector(polygonGroup[i].points[j], polygonGroup[i].points[ (j+1)%ptNum ]));
//			if(fabs(dir1.x-dir2.x)<error && fabs(dir1.y-dir2.y)<error && fabs(dir1.z-dir2.z)<error)///平行
//			{
//				continue;
//			}
//			if(fabs(polygonPt1.v - polygonPt2.v)<error)///水平方向的边可以不进行相交测试
//			{
//				continue;
//			}
//			if( cll.CrossPointonTwoLines2D( pt1, pt2, polygonPt1, polygonPt2, crosspoint) )
//			{///如果有条边相交
//				float a = Length2D(crosspoint, pt1),
//					b = Length2D(crosspoint, pt2),
//					c = Length2D(crosspoint, polygonPt1),
//					d = Length2D(crosspoint, polygonPt2);
//				if(
//					a<error///( ( (fabs(crosspoint.u-pt1.u)<error) && (fabs(crosspoint.v-pt1.v)<error) ) 
//					|| b<error///( (fabs(crosspoint.u-pt2.u)<error) && (fabs(crosspoint.v-pt2.v)<error) ) )
//					||
//					c<error///( ( (fabs(crosspoint.u-polygonPt1.u)<error) && (fabs(crosspoint.v-polygonPt1.v)<error) ) 
//					|| d<error///( (fabs(crosspoint.u-polygonPt2.u)<error) && (fabs(crosspoint.v-polygonPt2.v)<error) ) ) 
//					)
//				{///但交点是两求交线段的端点
//					continue;
//				}
//				else
//				{///有非端点的交点
//					return true;
//				}
//			}	
//		}		
//		if(cll.bPointIsInPolygon2D(midPt, tempPolygon)==2)
//		{
//			return true;
//		}
//	}
//
//// 	//////////////////////////////////////////////////////////////////////////
//// 	///下面判断是否与吊顶边界相交
//// 	int ptNum = (int)m_vEdgePoint.size();
//// 	for(int j=0; j<ptNum; j++)
//// 	{
//// 		CVector3 dir2 = math.Normalize(math.Vector(m_vEdgePoint[j], m_vEdgePoint[ (j+1)%ptNum ]));
//// 		if(fabs(dir1.x-dir2.x)<error && fabs(dir1.y-dir2.y)<error && fabs(dir1.z-dir2.z)<error)///平行
//// 		{
//// 			continue;
//// 		}
//// 		CVector2 crosspoint={0.0f,0.0f}, polygonPt1 = {m_vEdgePoint[j].x, m_vEdgePoint[j].z},
//// 			polygonPt2 = {m_vEdgePoint[ (j+1)%ptNum ].x, m_vEdgePoint[ (j+1)%ptNum ].z};
//// 		if(fabs(polygonPt1.v - polygonPt2.v)<error)///水平方向的边可以不进行相交测试
//// 		{
//// 			continue;
//// 		}
//// 		if( cll.CrossPointonTwoLines2D( pt1, pt2, polygonPt1, polygonPt2, crosspoint) )
//// 		{///如果有条边相交
//// 			if(
//// 				Length2D(crosspoint, pt1)<error 
//// 				|| Length2D(crosspoint, pt2)<error
//// 				||
//// 				Length2D(crosspoint, polygonPt1)<error 
//// 				|| Length2D(crosspoint, polygonPt2)<error 
//// 				)
//// 			{///但交点是两求交线段的端点
//// 				continue;
//// 			}
//// 			else
//// 			{///有非端点的交点
//// 				return true;
//// 			}
//// 		}	
//// 	}		
//
//	return 0;
//}
//
std::string CGeoMaths::ReadString( std::ifstream &file, int type/*=0*/ )
{
	std::string rst;
	int pNum;
	switch(type)
	{
	case 0:
		file.read((char *)&(pNum),sizeof(int));
		break;
	case 1:
		file.read((char *)&(pNum),sizeof(unsigned long));
		break;
	}
	char *str = new char[pNum+1]; 
	file.read((char *)str,pNum);
	str[pNum] = 0;
	rst = str; 
	delete []str;

	return rst;
}
//
//void CGeoMaths::SaveString( std::ofstream &file, std::string &str )
//{
//	int pNum = (int)str.length();
//	file.write((char *)&(pNum),sizeof(int));
//	file.write((char *)str.c_str(),(std::streamsize)str.length());
//}

///*********************************************
//作者：rao    jiang gai 2019.8.25 
//功能描述：点在多边形的位置
//输入参数：要判断的点,多边形点序列
//输出参数：空
//返回参数说明：返回0-外部，2-内部，1-边上
//************************************************/
//int CGeoMaths::bPointIsInPolygon(CVector2 point, std::vector<CVector2> PointList)
//{
//	float minDisDeal = 0.00001f;/// 0.001f;			// 用户输入所需要处理的偏差，比如，输入点的误差 
//	// 射线法：从被判断点向左方作一条射线，计算该射线与多边形各条
//	//         边的交点个数。若交点个数为奇数则点位于多边形内部,
//	//         若没有交点或者交点个数为偶数则点位于多边形外部
//
//	if ((int)PointList.size() < 3)// 确定是多边形  rap改 2013.12.18
//		return 0;//jiang gai 2016.10.15 不规范的情况统一为0 解决一条线分割整个面没了的问题 return -1;
//	// 判断是否是多边形顶点，若是，直接返回1
//	if (IsPointOfPolygon(point, PointList))
//		return 1;
//	// 若点不在包围多边形的矩形内，则必不在多边形内
//	// 求外接多边形的坐标
//	float maxX, minX, maxY, minY;
//	maxX = PointList[0].u;
//	minX = PointList[0].u;
//	maxY = PointList[0].v;
//	minY = PointList[0].v;
//	for (int i = 0; i<PointList.size(); i++)
//	{
//		if (PointList[i].u>maxX)
//			maxX = PointList[i].u;
//		if (PointList[i].u<minX)
//			minX = PointList[i].u;
//		if (PointList[i].v>maxY)
//			maxY = PointList[i].v;
//		if (PointList[i].v<minY)
//			minY = PointList[i].v;
//	}
//	// 在外接多边形外部
//	//jiang gai 2017.5.28 为了允许多边形的一些边不是完全平整的情况下降低判断错误的概率，将
//	//误差改为1mm
//	if ((point.u<minX && abs(point.u - minX)>minDisDeal) ||//equalZero)||
//		(point.u>maxX && abs(point.u - maxX)>minDisDeal) ||//equalZero)||
//		(point.v<minY && abs(point.v - minY)>minDisDeal) ||//equalZero)||
//		(point.v>maxY && abs(point.v - maxY)>minDisDeal))//equalZero))
//		return 0;
//	// 计算该点向左方向的水平射线与多边形各边的交点
//	// 交点个数
//	int nCount = 0;
//	for (int i = 0; i<PointList.size(); i++)
//	{
//		int j = (i + 1) % PointList.size();
//		int nFlag = bPointIsInLine(point, PointList[i], PointList[j]);
//		if (nFlag<0) return 1;
//		nCount += nFlag;
//	}
//	if (nCount % 2 == 0)
//		return 0;
//	else
//		return 2;
//	//return nCount&1;
//}
//
///*********************************************
//作者：jiang gai 2019.8.25 
//功能描述：判断是否是多边形顶点
//输入参数：要判断的点、多边形点序列
//输出参数：空
//返回参数说明：是顶点，返回顶点序号+1；否则，返回0
//************************************************/
//int CGeoMaths::IsPointOfPolygon(CVector2 point, std::vector<CVector2> polygonList)
//{
//	CMaths math;
//	for (int i = 0; i<polygonList.size(); i++)
//	{
//		if (math.Length2D(polygonList[i], point)<0.001)//jiang gai 2017.5.28 统一为0.001 //0.00001)
//		{
//			return i + 1;
//		}
//	}
//	return 0;
//}
//
///*********************************************
//作者：rao   jiang gai 2019.8.25
//功能描述：判断点所在射线（方向向左）与线段的关系
//输入参数：要判断的点,线段的类个端点
//输出参数：空
//返回参数说明：返回-1-在线段上，0-不相交，1-相交
//************************************************/
//int CGeoMaths::bPointIsInLine(CVector2 point, CVector2 pointBegin, CVector2 pointEnd)
//{
//	float minDisDeal = 0.001f;			// 用户输入所需要处理的偏差，比如，输入点的误差 
//	CMaths math;
//	// 获取线段的maxX,minX,maxY,minY
//	float maxX, minX, maxY, minY;
//	maxX = pointBegin.u;
//	minX = pointEnd.u;
//	if (pointBegin.u<pointEnd.u)
//	{
//		maxX = pointEnd.u;
//		minX = pointBegin.u;
//	}
//	maxY = pointBegin.v;
//	minY = pointEnd.v;
//	if (pointBegin.v<pointEnd.v)
//	{
//		maxY = pointEnd.v;
//		minY = pointBegin.v;
//	}
//	// 射线和线段没有交点
//	//jiang gai 2017.5.28 为了允许多边形的一些边不是完全平整的情况下降低判断错误的概率，将
//	//误差改为1mm
//	if ((point.v<minY && abs(point.v - minY)>minDisDeal) ||//equalZero*2)||
//		(point.v>maxY && abs(point.v - maxY)>minDisDeal) ||//equalZero*2)||
//		(point.u<minX && abs(point.u - minX)>minDisDeal))//equalZero*2))//因为是向左发射线，所以只要判断point.u<minX即可
//		return 0;
//
//	// rao改 2013.11.18
//	// 判断是否是端点
//	//jiang gai 2017.5.28 误差放到0.001
//	if (math.Length2D(point, pointBegin)<minDisDeal || math.Length2D(point, pointEnd)<minDisDeal)
//		//if(math.Length2D(point,pointBegin)<equalZero || math.Length2D(point,pointEnd)<equalZero)
//		return -1;
//
//	// 如果是水平线段，则点在线段上返回-1，否则返回0
//	//jiang gai 2015.8.3 判断水平会有些误差，这里需要把误差放大，因为上面已经排除大于maxy和小于miny，能走到这里的，肯定是距离水平直线0.001内 的
//	//jiang gai 2017.5.28 误差放到0.001
//	if (abs(minY - maxY)<minDisDeal) //equalZero*100)
//		return (point.u >= minX&&point.u <= maxX) ? (-1) : 0;
//
//	//jiang gai 2015.11.20 会出现pointBegin--pointEnd比较平，point在直线上，但是由于计算误差造成point不在直线上的情况
//	//为了避免，进行如下判断
//	if (point.u >= minX&&point.u <= maxX)
//	{
//		CCollision cll;
//		//确保point在x轴范围内
//		CVector3 p1, p2, cross, p3;
//		p1.x = pointBegin.u;
//		p1.y = 0;
//		p1.z = pointBegin.v;
//		p2.x = pointEnd.u;
//		p2.y = 0;
//		p2.z = pointEnd.v;
//		p3.x = point.u;
//		p3.y = 0;
//		p3.z = point.v;
//		//如点距离线段的距离小于0.01，认为点在线段上
//		float dis = cll.ClosestDistanceOnLineSegment(p1, p2, p3, cross);
//		if (dis<0.001)//minDistancePointAndPoint
//			return -1;
//	}
//
//	float x0 = pointBegin.u + double(point.v - pointBegin.v)*(pointEnd.u - pointBegin.u) / (pointEnd.v - pointBegin.v);
//	// 计算射线所在直线与边所在直线交点横坐标
//	// rao改 2013.11.15  修改偏差值，//jiang gai 2017.5.28 误差放到0.001
//	if (x0>point.u && abs(x0 - point.u)>minDisDeal)//0.0005)// 交点在射线起点右侧（射线与线段不相交）
//		return 0;
//	if (abs(x0 - point.u) <= minDisDeal)//0.0005)// 交点与射线起点相同---点在线段上，返回-1
//		return -1;
//	// 交点在射线起点的左侧，但射线穿过线段下端点，记为不相交，返回0
//	if (abs(point.v - minY)<minDisDeal)//equalZero*2)
//		return 0;
//	// 交点在边的中间点或者上端点，记为交点，返回1
//	return 1;
//}
