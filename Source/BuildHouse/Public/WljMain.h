#pragma once
#include "Windows/AllowWindowsPlatformTypes.h" //一般为Windows中的宏和UE4冲突所致,将可能会导致冲突的宏所在的hom文件包裹起来
#include "Windows/PreWindowsApi.h"
#include "Picture.h"
#include "flyfile_w.h"
#include "main.h"
#include "Windows/PostWindowsApi.h"            //一般为Windows中的宏和UE4冲突所致,将可能会导致冲突的宏所在的hom文件包裹起来
#include "Windows/HideWindowsPlatformTypes.h"
#include<vector>



//材质状态//jiang 2.3
typedef enum { MATERIALS, MAPS, NONE } TEXTURE_TYPE;

/*************************************************************/
/*                     ExtStruct.h                           */
/*         ------包含3D定义文件格式的结构------              */
/*************************************************************/
//说明:
//此头文件中定义的各个结构体是用来描述限于本平台内部使用的
//3D文件结构.从其它软件中导入的模型均将转换成该文件结构.
//本文件结构如下所述:
//          +--FileHeader(0xFFFF)
//          |  |
//          |  +--FileInformation(0xFEFE)
//          |
//          +--Group1_Geomentry_Information(0xFDFD)
//          |  |
//          |  +--Vertices_Information(0x7474)
//          |  +--Face_Information(0x7F7F)
//          |
//          +--Group2_Geomentry_Information(0xFDFD)
//          +…
//          +--Groupn_Geomentry_Information(0xFDFD)
//          |
//          +--Texture and Map(0x0D0D)
//          |
//          +--Collision_Information(0x3F3F)
//          |
//          +--
//
//内定三维文件信息部分

typedef class tagCFileInformation
{
public:
	unsigned short     ID;                      //文件信息ID号: 0xFEFE
	unsigned long      Length;                  //块长度
	char               FileName[256];           //文件名
	unsigned long      vertices_number;         //总的顶点数
	unsigned long      faces_number;            //总的面数
	unsigned short     group_number;            //顶点组数或称物体数,下面将有group_number个Geomentry_Information
	CVector3           min_coord;               //所有点的最小xyz坐标
	CVector3           max_coord;               //所有点的最大xyz坐标,与上述确定整个模型的空间范围
	float              radius;                  //整个模型包围球半径
	char               has_LOD;                 //是否有相应的LOD信息
	char               has_Anim;                //是否有帧动画
	char               has_Bone;                //是否有骨骼动画
public:
	tagCFileInformation()
	{
		memset(FileName, 0, 256);
		min_coord = ZERO_VECTOR3;
		min_coord = ZERO_VECTOR3;
		radius = 0.0f;
	}
}CFileInformation;

//内定三维文件头部分
typedef class tagCFileHeader
{
public:
	unsigned short     ID;                       //文件头ID号: 0xFFFF
	unsigned long      Length;                   //块长度,包括子块、文件头ID块长度部分
	char               ext_name[4];				 //文件扩展名:flt
	unsigned short     version;					 //文件版本号:0
	CFileInformation   FileInfo;                 //文件信息部分
public:
	tagCFileHeader()
	{
		memset(ext_name, 0, 4);
	}
}CFileHeader;


typedef class tagCVertexMapCoords
{
public:
	unsigned short MapIndex;
	CVector2       MapCoords;
}CVertexMapCoords;

typedef class tagCVMapCoords
{
public:
	unsigned short numberOfMapCoords;
	std::vector<CVertexMapCoords>   UVWCoords;
}CVMapCoords;

//内定三维文件顶点部分
//顶点信息中包括顶点个数
//每个顶点的位置坐标,包括xyz值
//每个顶点的法向量,平滑的顶点向量的xyz值
//每个顶点的纹理坐标
//以及每个顶点的颜色坐标
typedef class tagCVertices
{
public:
	unsigned short             ID;                 //顶点块识别号: 0x7474
	unsigned long              Length;             //块长度
	unsigned long              numberOfVertices;   //物体顶点数目
	std::vector<CVector3>      VPoint;             //顶点的坐标
	std::vector<CVector3>      VNormals;           //顶点的法向量
	std::vector<CVMapCoords>   VMaps;              //顶点的帖图坐标向量
	std::vector<CVector3>      VColors;            //顶点颜色
}CVertices;

//内定三维文件面部分
//面信息部分包括面的个数
//面的顶点索引,每三个一组
typedef class tagCFaces
{
public:
	unsigned short              ID;                 //三角面块识别号: 0x7F7F
	unsigned long               Length;             //块长度 
	unsigned long               numberOfFaces;      //物体面个数
	std::vector<unsigned long>  Vertices_Index;     //物体面顶点的索引,每三个一组
}CFaces;

//贴图的属性
typedef class tagCMatAndFace
{
public:
	unsigned short               Materials_Index;    //贴图的序号.;
	unsigned long                numberOfFaces;      //使用该贴图的面的数量
	std::vector<unsigned long>   Face_Index;         //面的索引号
}CMatAndFace;

//此部分为三维几何体中的材质贴图部分,
//包括该三维物体所用到的材质贴图数目
//所有该物体用到的贴图的属性
//每个属性包括纹理序号、该纹理对应的面数、所对应面的序号
//画图时是通过该部分信息找到所包含的面,
//再由面查到点,最后由点的信息画出
typedef class tagCMaterials
{//针对于每个形体
public:
	unsigned short                  ID;               //贴图部分识别号: 0x1F1F
	unsigned long                   Length;           //块长度
	unsigned short                  numberOfMaterials;//贴图数
	std::vector<CMatAndFace>        MaterialProp;     //贴图属性
}CMaterials;

//内定三维文件几何体信息部分
//此部分中包含了物体所有的静态几何体部分信息
//包括
//物体名称
//顶点信息
//面信息
//材质信息
//中心点
//bounding box的坐标范围
//bounding circle的半径
typedef class tagCGeomentry_Information
{
public:
	unsigned short     ID;                       //物体信息ID号: 0xFDFD
	unsigned long      Length;                   //块长度
	char               Geomentry_name[256];      //物体名
	CVertices          Vertices;                 //物体顶点块
	CFaces             Faces;                    //物体面块
	CMaterials         Materials;                //物体表面纹理
	CVector3           Center_Point;             //该物体中心点坐标
	CVector3           min;                      //与max形成物体的包围盒
	CVector3           max;
	float              radius;                   //物体包围球半径
public:
	tagCGeomentry_Information()
	{
		memset(Geomentry_name, 0, 256);
	}
}CGeomentry_Information;

//带有纹理的三维点
struct Point
{
	CVector3 pos;
	float coordX, coordY;
};//点结构


typedef struct tagCMatElements
{
public:
	char     Mat_Name[256];
	CVector3 Ambient;
	CVector3 Diffuse;
	CVector3 Specular;
	CVector3 Emission;
	float    Shininess;
	float    Alpha;
	CVector3 SpecularColor;//jiang gai 2015.7.9
	int m_version;//jiang gai 2015.7.9
	float    Bumpweight;  //zwq 2016.5.12 bmp贴图权重
public:
	//jiang gai 2015.7.9
	tagCMatElements();
	//jiang gai 2015.9.24
	void ReadFromFile(flyFile_w* flyfile);
	void ReadFromFile_1(flyFile_w* flyfile);
	void ReadFromFile_2(flyFile_w* flyfile);
}CMatElements;


typedef class tagCMapElements
{
public:
	char			FileName[256];
	unsigned char	alpha;//不透明读
	short int		type;
	float			u_scale;//u,v方向的放缩比例//jiang gai 2013.11.1 现改为当前纹理的实际尺寸
	float			v_scale;
	float			u_offset;//u,v方向偏移
	float			v_offset;
	float			rotation;//旋转属性
	int m_version;//jiang gai 2015.7.9
	std::string     serSignal; //wqq 2017.4.24 服务器标记
public:
	//jiang gai 2015.7.9
	tagCMapElements();
	//jiang gai 2015.9.24
	void ReadFromFile(flyFile_w* flyfile);
	void ReadFromFile_1(flyFile_w* flyfile);
	//jiang gai 2017.4.24 
	void ReadFromFile_2(flyFile_w* flyfile);
}CMapElements;//涉及纹理坐标的转换


typedef struct tagCMatElements0
{
public:
	char     Mat_Name[256];
	CVector3 Ambient;
	CVector3 Diffuse;
	CVector3 Specular;
	CVector3 Emission;
	float    Shininess;
	float    Alpha;
public:
	tagCMatElements0()
	{
		memset(Mat_Name, 0, 256);
		Ambient.x = 1.0f;  Ambient.y = 1.0f;  Ambient.z = 1.0f;
		Diffuse.x = 1.0f;  Diffuse.y = 1.0f;  Diffuse.z = 1.0f;
		Specular.x = 1.0f;  Specular.y = 1.0f;  Specular.z = 1.0f;
		Emission.x = 0.0f;  Emission.y = 0.0f;  Emission.z = 0.0f;
		Shininess = 46.0f;
		Alpha = 1.0f;
	}
}CMatElements0;

typedef class tagCMapElements0
{
public:
	char			FileName[256];
	unsigned char	alpha;//不透明读
	short int		type;
	float			u_scale;//u,v方向的放缩比例//jiang gai 2013.11.1 现改为当前纹理的实际尺寸
	float			v_scale;
	float			u_offset;//u,v方向偏移
	float			v_offset;
	float			rotation;//旋转属性
public:
	tagCMapElements0()
	{
		memset(FileName, 0, 256);
		alpha = 255;
		type = 0;
		u_scale = 1.0f;
		v_scale = 1.0f;
		u_offset = 0.0f;
		v_offset = 0.0f;
		rotation = 0.0f;
	}
}CMapElements0;//涉及纹理坐标的转换

//jiang gai 2015.7.9 为了防止增加属性，将CTextureProperty0和TextureProperty相隔离
typedef class tagCTextureProperty0
{
public:
	CMatElements0         material;
	CMapElements0        map;
}CTextureProperty0;

class WljMain
{
};

//内定三维文件纹理属性部分
typedef class tagCTextureProperty
{
public:
	CMatElements         material;
	CMapElements         map;
	int m_version;       //jiang gai 2015.7.9
	//jiang gai 2015.7.9
	tagCTextureProperty();
	//jiang gai 2015.9.24
	void ReadFromFile(flyFile_w* flyfile);
	void ReadFromFile_1(flyFile_w* flyfile);
}CTextureProperty;

//jiang gai 2014.9.1 用于打开老的文件
//内定三维文件纹理信息部分
typedef class tagCMapsOld
{
public:
	int                 map_index;					//纹理序号
	char                map_name[256];				//纹理名,比如MATERIAL #01
	TEXTURE_TYPE        texture_type;				//纹理类型,是材质还是贴图				
	bool				two_side;					//是否为双面纹理
	CTextureProperty0   texture_property;			//纹理属性	 //jiang gai 2015.7.9
public:
	tagCMapsOld()
	{
		memset(map_name, 0, 256);
	}
}CMapsOld;

//内定三维文件纹理信息部分
//jiang gai 2014.9.1 该类用于新的纹理对象的读取
class DllExport CMaps
{
public:
	int                 map_index;					//纹理序号
	char                map_name[256];				//纹理名,比如MATERIAL #01
	char                m_HarshName_name[256];		//jiang gai 2015.12.16 纹理harsh纹理名,不需要保存，用于ess本地渲染
	TEXTURE_TYPE        texture_type;				//纹理类型,是材质还是贴图				
	bool				two_side;					//是否为双面纹理
	CTextureProperty    texture_property;			//纹理属性
	CTextureProperty    bump_property;               //zwq 2016.5.12 bmp纹理贴图
	//jiang gai 2014.9.1 增加两个材质序号
	char                m_sPovMatirealID[30];          //pov材质序号
	char                m_sEssMatirealID[30];          //Ess材质序号
	int                 m_version;                 //版本号
	//jiang gai 2016.9.14 为了方便segarea自动输出材质名称、工艺等，增加表面材质序列
	std::string  m_sGoodsName; //产品名称
	std::string  m_sScendClass; //二级分类 
	std::string  m_sThirdClass; //三级分类
	std::string  m_sSerial;   //序列号
	std::string  m_sType;     //型号
	std::string  m_sStyle;    //风格
	std::string  m_sSize;     //尺寸
	std::string  m_sPirce;    //价格
	std::string  m_sBrand;    //品牌
	std::string  m_sSmallScript;  //简单描述
	std::string  m_sBigScript;    //详细描述
	std::string  m_sBuggentPrice; //预算价格
	std::string  m_sSeries;  //系列
	std::string  m_sProdSize;///wql 2019.4.25 该值记录贴图中单片产品的尺寸，m_sSize记录贴图的图片尺寸
public:
	CMaps();
	void ReadFromFile(flyFile_w* flyfile);
	void ReadFromFile_1(flyFile_w* flyfile);
	void CopyFromOld(CMapsOld& map);
	void ReadFromFile_2(flyFile_w* flyfile);//jiang gai 2015.9.24 读直接cmaps 2.0版
	void ReadFromFile_3(flyFile_w* flyfile);
	void ReadFromFile_4(flyFile_w* flyfile);
	void ReadFromFile_5(flyFile_w* flyfile);///wql 2019.4.25 该函数为什么没有对应的保存？
};

//内定三维文件纹理部分
//该部分为总的纹理信息(非物体信息块中)
//包括
//总的纹理数
//纹理信息的列表
//如上结构所示,纹理信息列表元素包括
//此纹理序号
//纹理名
//纹理文件名(空时此纹理仅表示颜色)
//颜色信息(若纹理文件名空)
typedef class tagCTexture
{
public:
	unsigned short       ID;                     //材质纹理ID号: 0x0D0D
	unsigned long        Length;                 //块长度
	unsigned short       map_number;             //纹理数目
	std::vector<CMaps>   VMaps;                  //纹理信息列表
	std::vector<CVector2> m_regSizeList;		///每个纹理素材的原始尺寸
	std::vector<std::string> m_texClassList;	///每个纹理素材对应的类型
	std::vector<std::string> m_texPriceList;	///每个纹理素材对应的价格
}CTexture;

