#pragma once
#include <vector>
#include <string>
//#include "Main.h"
//#include "Maths.h"
//#include "ComponentStruct.h"

#ifndef DLLImport
#define DllImport   __declspec(dllimport)
#endif
#ifndef DLLExport
#define DllExport   __declspec(dllexport)
#endif

/*************************************************
版权 (C), 2007-2017, 南京家装, Ltd.
作者: 王庆利     版本:1.0        
功能描述: 该类设计一些形体生成及多边形算法， 继承自基本算法
*************************************************/
class DllExport CGeoMaths/*:public CMaths*/
{
public:
	CGeoMaths();
	virtual ~CGeoMaths();

public:
	/***********************************************
	作者： 王庆利   
	功能描述：读取file中保存的字符串
	输入参数：file中的字符格式为,首先保存了字符个数(type:0,int;1,unsigned long),然后是该字符串
	输出参数：无
	返回参数说明：无
	************************************************/
	std::string ReadString(std::ifstream &file, int type=0);
//	/***********************************************
//	作者： 王庆利   
//	功能描述：保存字符串，先保存字符个数(int),然后是该字符串
//	输入参数：无
//	输出参数：无
//	返回参数说明：无
//	************************************************/
//	void SaveString(std::ofstream &file, std::string &str);
//
//	/***********************************************
//	作者： 王庆利   
//	功能描述：判断点是否在某边界内或相交
//		axis在某一个轴平面上0:xoz, 1:xoy, 2:yoz
//	输入参数：无
//	输出参数：无
//	返回参数说明：在外返回0, 在边上返回1,在内返回2
//	************************************************/
//	int bPointInEdge( CVector3 &pt, std::vector<CVector3> &edgePoint, int axis=0 );
//
//	/***********************************************
//	作者： 王庆利   
//	功能描述：判断线段是否在某边界内或相交
//	输入参数：无
//	输出参数：无
//	返回参数说明：否返回false
//	************************************************/
//	bool UncrossAndIn(PointStruct &p1, PointStruct &p2, std::vector<PointStruct> &edgePoint, bool bHole = true); //jiang gai 2019.9.25  增加参数bHole
//	///wql 2019.11.14 从三角化工具中移植过来
//	bool UncrossAndIn(PointStruct &p1, PointStruct &p2, PointStruct &p3, std::vector<PointStruct> &edgePoint/*,int &num,int &storeNum*/);
//
//	/***********************************************
//	作者： 王庆利   
//	功能描述：线段是否在一个多边形内或者是多边形一条边
//	输入参数：point1, point2线段, polygonGroup多边形
//	输出参数：无
//	返回参数说明：在内返回1, 是一条边返回2, 否则返回0
//	************************************************/
//	int InOrIsOneLineOfPolygon(const CVector3 &point1, const CVector3 &point2, std::vector<CPointsGroup>  &polygonGroup);
//
//	/***********************************************
//	作者： 王庆利   
//	功能描述：将多边形三角化
//	输入参数：edgePoint多边形顶点， faceVerId三角化后的顶点索引，每三个一个面
//	输出参数：无
//	返回参数说明：三角化成功返回true
//	************************************************/
//	bool PolygonToTriangels(std::vector<PointStruct> &edgePoint, std::vector<int> & faceVerId, bool bHole=true); //jiang gai 2019.9.25  增加参数bHole
//
//	/***********************************************
//	作者： 王庆利   
//	功能描述：根据边界点创建一个平面形体
//	输入参数：edgePt平面多边形顶点， ptGeom创建的形体
//	输出参数：无
//	返回参数说明：无
//	************************************************/
//	void CreateTopFace(std::vector<CVector3> &edgePt, CGeomentry_Information *ptGeom);
//
//	/***********************************************
//	作者： 王庆利   
//	功能描述：计算多边形面积
//	输入参数：edgePoint多边形顶点
//	输出参数：无
//	返回参数说明：返回面积
//	************************************************/
//	float ComputePolygonArea(const std::vector<CVector3> &edgePt);
//
//	/***********************************************
//	作者： 王庆利   
//	功能描述：计算三角形面积
//	输入参数：edgePoint三角形顶点
//	输出参数：无
//	返回参数说明：返回面积
//	************************************************/
//	float TriangleArea(const CVector3 &p1, const CVector3 &p2, const CVector3 &p3);
//
//	/***********************************************
//	作者： 王庆利   
//	功能描述：计算将非连通域平面分割为三角片后的面索引
//	输入参数：lineGroup该面上所有顶点，按一定顺序划分行和列，即包括边界点和内部镂空边界与其他所有边的交点，
//				此所有点按照某一轴坐标划分为多个行，每一行又按顺序排列
//			  polygon面内部的所有镂空边界顶点
//			  edgePoint整个区域的边界点
//			  axis在某一个轴平面上0:xoz, 1:xoy, 2:yoz
//	输出参数：无
//	返回参数说明：返回三角片后的面索引
//	************************************************/
//	std::vector<int> CreateFaceIndex(std::vector<CPointsGroup> &lineGroup, std::vector<CPointsGroup>  &polygon, 
//		std::vector<CVector3> edgePoint, int axis=0);
//	
//	/***********************************************
//	作者： 王庆利   
//	功能描述：根据非连通域平面创建一个平面形体
//	输入参数：lineGroup该面上所有顶点，按一定顺序划分行和列，即包括边界点和内部镂空边界与其他所有边的交点，
//				此所有点按照某一轴坐标划分为多个行，每一行又按顺序排列
//				polygon面内部的所有镂空边界顶点
//				ptGeom创建的形体
//				edgePoint整个区域的边界点
//				axis在某一个轴平面上0:xoz, 1:xoy, 2:yoz
//	输出参数：无
//	返回参数说明：无
//	************************************************/
//	void CreateHoleFace(std::vector<CPointsGroup> &lineGroup, std::vector<CPointsGroup>  &polygon, CGeomentry_Information *ptGeom, 
//		std::vector<CVector3> edgePoint, int axis=0);
//
//	/***********************************************
//	作者： 王庆利   
//	功能描述：一组点中是否有点在三角形内
//	输入参数：polygon三角形, polygonGroup点组
//	输出参数：无
//	返回参数说明：没有返回true
//	************************************************/
//	bool NoPointInTriangle(CVector3 polygon[], std::vector<CPointsGroup>  &polygonGroup);
//
//	/***********************************************
//	作者： 王庆利   
//	功能描述：线段是否与多边形相交
//	输入参数：polygon三角形, polygonGroup点组
//	输出参数：无
//	返回参数说明：没有返回true
//	************************************************/
//	bool CrossWithOthers(const CVector3 &point1, const CVector3 &point2, std::vector<CPointsGroup> &polygonGroup);///判断该线段是否与镂空元素相交
//
//	/***********************************************
//	作者： 王庆利   
//	功能描述：根据非连通域平面中的镂空侧面
//	输入参数：polygon面内部的所有镂空边界顶点， ptGeom创建的形体
//	axis在某一个轴平面上0:xoz, 1:xoy, 2:yoz
//	输出参数：无
//	返回参数说明：无
//	************************************************/
//	void CreateInside(CPointsGroup &ptsGroup, CGeomentry_Information *ptGeom, int axis=0);
//
//	/***********************************************
//	作者： 王庆利   创建时间 ： 2010.1.2
//	功能描述：根据两组顺序点创建形体
//	输入参数：upPoint上侧一周顶点， downPoint下侧一周顶点, ptGeom创建的形体， ptGeom创建的形体
//	输出参数：无
//	返回参数说明：无
//	************************************************/
//	void CreateGeoWithTwoPointLists(const std::vector<CVector3> &upPoint,const std::vector<CVector3> &downPoint, CGeomentry_Information *ptGeom);
//
//	/***********************************************
//	作者： 王庆利   创建时间 ： 2010.1.2
//	功能描述：对于实体元素，创建该元素的模型的上或下侧形体
//	输入参数：points上或下侧一周顶点， ptGeom创建的形体
//	axis在某一个轴平面上0:xoz, 1:xoy, 2:yoz
//	输出参数：无
//	返回参数说明：无
//	************************************************/
//	void CreateSolidUpAndDownside(const std::vector<CVector3> &points, CGeomentry_Information *ptGeom);
//
//	/***********************************************
//	作者： 王庆利   创建时间 ： 2010.1.2
//	功能描述：对于中间有一个孔的元素，创建该元素的模型的上或下侧形体
//	输入参数：outPts上或下侧的外侧一周顶点，inPts上或下侧的内侧一周顶点；当面法线朝上时outPts和inPts要按内外侧反过来应用
//	axis在某一个轴平面上0:xoz, 1:xoy, 2:yoz
//	输出参数：无
//	返回参数说明：无
//	************************************************/
//	void CreateLoopUpAndDownside(const std::vector<CVector3> &outPts,const std::vector<CVector3> &inPts, CGeomentry_Information *ptGeom);
//	
//
//private:
//	/***********************************************
//	作者： 王庆利   
//	功能描述：根据非连通域平面计算划分索引
//	输入参数：lineGroup该面上所有顶点，按一定顺序划分行和列，即包括边界点和内部镂空边界与其他所有边的交点，
//		此所有点按照某一轴坐标划分为多个行，每一行又按顺序排列
//		polygon面内部的所有镂空边界顶点
//		faceId面索引
//		axis在某一个轴平面上0:xoz, 1:xoy, 2:yoz
//	输出参数：无
//	返回参数说明：无
//	************************************************/
//	void LeftToRight(std::vector<int> &faceId, std::vector<CPointsGroup> &lineGroup, std::vector<CPointsGroup>  &polygon, int axis);///扫描线法从左至右
//	void RightToLeft(std::vector<int> &faceId, std::vector<CPointsGroup> &lineGroup, std::vector<CPointsGroup>  &polygon, int axis);///扫描线法从右至左
//
//	//jiang gai 2019.8.25
//	int bPointIsInPolygon(CVector2 point, std::vector<CVector2> PointList);
//	int IsPointOfPolygon(CVector2 point, std::vector<CVector2> polygonList);
//	int bPointIsInLine(CVector2 point, CVector2 pointBegin, CVector2 pointEnd);
//	///wql 2019.9.18 私有函数，限制递归次数
//	bool PolygonToTriangels_Recurse(std::vector<PointStruct> &edgePoint, std::vector<int> & faceVerId, int &recurse, bool bHole=true); //jiang gai 2019.9.25  增加参数bHole
//
//private:
//	std::vector<CVector3> m_points;
};
