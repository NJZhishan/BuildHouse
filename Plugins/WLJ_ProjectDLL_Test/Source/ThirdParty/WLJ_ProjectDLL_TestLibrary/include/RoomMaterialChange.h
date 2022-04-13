#pragma once
#include "main.h"

/*************************************************************************/
/*版权(C), 2007-2017, Ltd
/*版本 0.01
/*作者：jiang
/*2010.1.14 ( created )
/*2010.1.15 ( modified )
功能描述: 用于描述与三维体验馆的一间房子中某个风格中一个对象（普通物体、窗）的某个材质被新的纹理或材质替换
/**************************************************************************/

//jiang gai 2015.11.3 增加一个用于记录针对物体表面纹理操作属性的类
typedef enum {NULL_OPUV,PLANE_CHANGE,BOX_CHANGE,SPHERE_CHANGE} TxtureOprationType;
//enum TxtureOprationType {NULL,PLANE,BOX,SPHERE};
class DllExport CTextureChange
{
public:
	CTextureChange();
	void SaveToFile(std::ofstream &file);
	void ReadFromFile(std::ifstream &file);
	void SaveToFile_1(std::ofstream &file);	
	void ReadFromFile_1(std::ifstream &file);
	//jiang gai 2015.11.27
	void SaveToFile_2(std::ofstream &file);	
	void ReadFromFile_2(std::ifstream &file);
public:
    TxtureOprationType m_opreType;//纹理调整方式
    CVector3 m_Translation,m_Scale; //纹理坐标三轴平移、缩放
    float m_fRotation; //纹理旋转
	int m_version;
	//jiang gai 2015.11.16
	int m_MaterialIndex; //对应的geometry中材质的序号
	int m_GeometryIndex;//对应的geometry序号 
	int m_SelFaceIndex; // 对应的选中面 jiang gai 2015.11.27
};

class DllExport CRoomMaterialChange
{
public:
    int m_version;       //版本号
    int m_iStyleIndex, m_iObjectIndex;  //对象的类型序号、对象序号
	int m_iRoomStyle;  //当前房间所处的风格
	std::string m_objName;///要将更改材质的物体名称记录下来，方便核对
	int m_iTextureIndex;    //被替换的材质在该对象CModelBase中的CTexture中的序号
    char m_ErMaterialID[30];//jiang gai 2015.9.16 ess渲染材质
	std::string m_price; //jiang gai 2015.9.16 价格

	bool m_bTexture;  //新的材质是否是纹理
	CVector3 m_Color; //新的材质的颜色
	std::string m_TexName; //新纹理的文件名
	std::string m_TexClass;  //如果是新的纹理，其对应的类型
	CVector2 m_regSize;		///纹理的原始尺寸
	CVector3 m_texScaleAndAngle;
    CTextureChange m_TexUVChange; //jiang gai 2015.11.3 纹理坐标改变属性
	std::string m_sSerSignal; //jiang gai 2017.5.20 增加服务器版本信息
public:
	CRoomMaterialChange();
	~CRoomMaterialChange();
	/***********************************************
	作者： 王庆利   创建时间 ： 2010.1.26
	功能描述：拷贝构造函数
	输入参数：file保存文件
	输出参数：无
	返回参数说明：无
	************************************************/
	//CRoomMaterialChange(const CRoomMaterialChange &mat);

	/***********************************************
	作者： 王庆利   创建时间 ： 2010.1.26
	功能描述：保存函数
	输入参数：file保存文件
	输出参数：无
	返回参数说明：无
	************************************************/
	void SaveToFile(std::ofstream &file);

	/***********************************************
	作者： 王庆利   创建时间 ： 2010.1.26
	功能描述：读取函数
	输入参数：file读取文件
	输出参数：无
	返回参数说明：无
	************************************************/
	void ReadFromFile(std::ifstream &file);

	/***********************************************
	作者： 王庆利   创建时间 ： 2010.1.26
	功能描述：按版本保存函数
	输入参数：file保存文件
	输出参数：无
	返回参数说明：无
	************************************************/
	void SaveToFile_1(std::ofstream &file);

	/***********************************************
	作者： 王庆利   创建时间 ： 2010.1.26
	功能描述：按版本读取函数
	输入参数：file读取文件
	输出参数：无
	返回参数说明：无
	************************************************/
	void ReadFromFile_1(std::ifstream &file);

	/***********************************************
	作者： 王庆利   创建时间 ： 2010.1.26
	功能描述：按版本保存函数
	输入参数：file保存文件
	输出参数：无
	返回参数说明：无
	************************************************/
	void SaveToFile_2(std::ofstream &file);

	/***********************************************
	作者： 王庆利   创建时间 ： 2010.1.26
	功能描述：按版本读取函数
	输入参数：file读取文件
	输出参数：无
	返回参数说明：无
	************************************************/
	void ReadFromFile_2(std::ifstream &file);

	/***********************************************
	作者： 王庆利   创建时间 ： 2010.1.26
	功能描述：按版本保存函数
	输入参数：file保存文件
	输出参数：无
	返回参数说明：无
	************************************************/
	void SaveToFile_3(std::ofstream &file);

	/***********************************************
	作者： 王庆利   创建时间 ： 2010.1.26
	功能描述：按版本读取函数
	输入参数：file读取文件
	输出参数：无
	返回参数说明：无
	************************************************/
	void ReadFromFile_3(std::ifstream &file);

	/***********************************************
	作者： 王庆利   创建时间 ： 2010.1.26
	功能描述：按版本保存函数
	输入参数：file保存文件
	输出参数：无
	返回参数说明：无
	************************************************/
	void SaveToFile_4(std::ofstream &file);

	/***********************************************
	作者： 王庆利   创建时间 ： 2010.1.26
	功能描述：按版本读取函数
	输入参数：file读取文件
	输出参数：无
	返回参数说明：无
	************************************************/
	void ReadFromFile_4(std::ifstream &file);
	/***********************************************
	作者： 王庆利   创建时间 ： 2010.1.26
	功能描述：按版本保存函数
	输入参数：file保存文件
	输出参数：无
	返回参数说明：无
	************************************************/
	void SaveToFile_5(std::ofstream &file);

	/***********************************************
	作者： 王庆利   创建时间 ： 2010.1.26
	功能描述：按版本读取函数
	输入参数：file读取文件
	输出参数：无
	返回参数说明：无
	************************************************/
	void ReadFromFile_5(std::ifstream &file);
	//jiang gai 2015.9.16
    void ReadFromFile_6(std::ifstream &file);
	void SaveToFile_6(std::ofstream &file);

	//jiang gai 2015.11.3
    void ReadFromFile_7(std::ifstream &file);
	void SaveToFile_7(std::ofstream &file);

	//jiang gai 2017.5.20
	void ReadFromFile_8(std::ifstream &file);
	void SaveToFile_8(std::ofstream &file);

	void SaveToXML(); //写入xml文件
	void SaveToXML1(); //写入xml文件（第一个版本）
	void SaveToXML3(); //写入xml文件（第一个版本）
	void ReadFromXML();// 从xml文件中读取相关信息
	void ReadFromXML1();// 从xml文件中读取相关信息　　
	void ReadFromXML3();// 从xml文件中读取相关信息　　
}; 
