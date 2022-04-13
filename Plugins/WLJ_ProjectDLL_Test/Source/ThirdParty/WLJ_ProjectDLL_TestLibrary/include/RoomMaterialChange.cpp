#include "RoomMaterialChange.h"

//jiang gai 2015.11.16
CTextureChange::CTextureChange()
{
	m_opreType = NULL_OPUV;//纹理调整方式
	m_Translation.x = m_Translation.y = m_Translation.z = 0;//纹理坐标三轴平移
	m_Scale.x = m_Scale.y = m_Scale.z = 1.0;  //缩放
    m_fRotation = 0; //纹理旋转
	m_MaterialIndex = -1; //对应的geometry中材质的序号
	m_GeometryIndex = -1;//对应的geometry序号
	m_SelFaceIndex = 0; //jiang gai 2015.11.27
    m_version = 2;
}

/***********************************************
jiang gai 2015.11.16 
功能描述：保存函数
输入参数：file保存文件
输出参数：无
返回参数说明：无
************************************************/
void CTextureChange::SaveToFile(std::ofstream &file)
{
	file.write((char *)&m_version,sizeof(int));
	switch(m_version)
	{
	case 1:
		{
			SaveToFile_1(file);
		}
		break;
	case 2://jiang gai 2015.11.27
		{
			SaveToFile_2(file);
		}
		break;	
	default:
		break;
	}
}

/***********************************************
jiang gai 2015.11.16 
功能描述：读取函数
输入参数：file读取文件
输出参数：无
返回参数说明：无
************************************************/
void CTextureChange::ReadFromFile(std::ifstream &file)
{
	int version = m_version;
	file.read((char *)&version,sizeof(int));
	switch(version)
	{
	case 1:
		{
			ReadFromFile_1(file);
		}
		break;	
	case 2://jiang gai 2015.11.27
		{
			ReadFromFile_2(file);
		}
		break;
	default:
		break;
	}
}

/***********************************************
jiang gai 2015.11.16
功能描述：按版本保存函数
输入参数：file保存文件
输出参数：无
返回参数说明：无
************************************************/
void CTextureChange::SaveToFile_1(std::ofstream &file)
{
	file.write((char *)&m_opreType,sizeof(TxtureOprationType));
	file.write((char *)&m_Translation,sizeof(CVector3));
	file.write((char *)&m_Scale,sizeof(CVector3));
	file.write((char *)&m_fRotation,sizeof(float));
	file.write((char *)&m_GeometryIndex,sizeof(int));
	file.write((char *)&m_MaterialIndex,sizeof(int));
}

/***********************************************
jiang gai 2015.11.16
功能描述：按版本读取函数
输入参数：file读取文件
输出参数：无
返回参数说明：无
************************************************/
void CTextureChange::ReadFromFile_1(std::ifstream &file)
{
	file.read((char *)&m_opreType,sizeof(TxtureOprationType));
	file.read((char *)&m_Translation,sizeof(CVector3));
	file.read((char *)&m_Scale,sizeof(CVector3));
	file.read((char *)&m_fRotation,sizeof(float));
	file.read((char *)&m_GeometryIndex,sizeof(int));
	file.read((char *)&m_MaterialIndex,sizeof(int));
}

/***********************************************
jiang gai 2015.11.27
功能描述：按版本保存函数
输入参数：file保存文件
输出参数：无
返回参数说明：无
************************************************/
void CTextureChange::SaveToFile_2(std::ofstream &file)
{
    SaveToFile_1(file);
	file.write((char *)&m_SelFaceIndex,sizeof(int));
}

/***********************************************
jiang gai 2015.11.27
功能描述：按版本读取函数
输入参数：file读取文件
输出参数：无
返回参数说明：无
************************************************/
void CTextureChange::ReadFromFile_2(std::ifstream &file)
{
	ReadFromFile_1(file);
	file.read((char *)&m_SelFaceIndex,sizeof(int));
}


CRoomMaterialChange::CRoomMaterialChange()
{
	m_version = 8;//jiang gai 2017.5.20 //jiang gai 2015.11.16
	m_iStyleIndex = -1;
	m_iObjectIndex = -1;  //对象的类型序号、对象序号
	m_iTextureIndex = -1;    //被替换的材质在该对象CModelBase中的CTexture中的序号

	m_objName = "";
	m_bTexture = true;  //新的材质是否是纹理
	m_iRoomStyle = -1;
	m_texScaleAndAngle.x = -1.0f;
	m_texScaleAndAngle.y = -1.0f;
	m_texScaleAndAngle.z = 0.0f;
	m_regSize = ONE_VECTOR2;
	strcpy_s(m_ErMaterialID, "");//jiang gai 2015.9.16
}

/***********************************************
作者： 王庆利   创建时间 ： 2010.1.26
功能描述：拷贝构造函数
输入参数：file保存文件
输出参数：无
返回参数说明：无
************************************************/
// CRoomMaterialChange::CRoomMaterialChange(const CRoomMaterialChange &mat)
// {
// 	m_version = 1;       //版本号
// 	m_iStyleIndex = mat.m_iStyleIndex;
// 	m_iObjectIndex = mat.m_iObjectIndex;  //对象的类型序号、对象序号
// 	m_iTextureIndex = mat.m_iTextureIndex;    //被替换的材质在该对象CModelBase中的CTexture中的序号
// 
// 	m_bTexture = mat.m_bTexture;  //新的材质是否是纹理
// 	m_Color = mat.m_Color; //新的材质的颜色
// 	m_TexName = mat.m_TexName; //新纹理的文件名
// 	m_TexURL = mat.m_TexURL;  //如果是新的纹理，其对应的url
// }

void CRoomMaterialChange::SaveToXML() //写入xml文件
{

}

void CRoomMaterialChange::SaveToXML1() //写入xml文件（第一个版本）
{

}

void CRoomMaterialChange::ReadFromXML()// 从xml文件中读取相关信息
{

}

void CRoomMaterialChange::ReadFromXML1()// 从xml文件中读取相关信息　　
{

}

/***********************************************
作者： 王庆利   创建时间 ： 2010.1.26
功能描述：保存函数
输入参数：file保存文件
输出参数：无
返回参数说明：无
************************************************/
void CRoomMaterialChange::SaveToFile(std::ofstream &file)
{
	file.write((char *)&m_version,sizeof(int));
	switch(m_version)
	{
	case 1:
		{
			SaveToFile_1(file);
		}
		break;
	case 2:
		{
			SaveToFile_2(file);
		}
		break;
	case 3:
		{
			SaveToFile_3(file);
		}
		break;
	case 4:
		{
			SaveToFile_4(file);
		}
		break;
	case 5:
		{
			SaveToFile_5(file);
		}
		break;
		//jiang gai 2015.9.16 
	case 6:
		{
			SaveToFile_6(file);
		}
		break;
	//jiang gai 2015.11.16 
	case 7:
		{
			SaveToFile_7(file);
		}
		break;
		//jiang gai 2017.5.20 
	case 8:
		{
			SaveToFile_8(file);
		}
		break;
	default:
		break;
	}
}

/***********************************************
作者： 王庆利   创建时间 ： 2010.1.26
功能描述：读取函数
输入参数：file读取文件
输出参数：无
返回参数说明：无
************************************************/
void CRoomMaterialChange::ReadFromFile(std::ifstream &file)
{
	int version = m_version;
	file.read((char *)&version,sizeof(int));
	switch(version)
	{
	case 1:
		{
			ReadFromFile_1(file);
		}
		break;
	case 2:
		{
			ReadFromFile_2(file);
		}
		break;
	case 3:
		{
			ReadFromFile_3(file);
		}
		break;
	case 4:
		{
			ReadFromFile_4(file);
		}
		break;
	case 5:
		{
			ReadFromFile_5(file);
		}
		break;
	//jiang gai 2015.9.16
	case 6:
		{
			ReadFromFile_6(file);
		}
		break;
	//jiang gai 2015.11.16
	case 7:
		{
			ReadFromFile_7(file);
		}
		break;
		//jiang gai 2017.5.20
	case 8:
		{
			ReadFromFile_8(file);
		}
		break;
	default:
		break;
	}
}

//jiang gai 2017.5.20
void CRoomMaterialChange::ReadFromFile_8(std::ifstream &file)
{
	ReadFromFile_7(file); //wqq 2017.5.23 唉
	int num = 0;
	file.read((char *)&num,sizeof(int));
	char *temp = new char[num+1];
	file.read((char *)temp, num);
	temp[num] = 0;
	m_sSerSignal = temp;
	delete temp;
}

//jiang gai 2017.5.20 
void CRoomMaterialChange::SaveToFile_8(std::ofstream &file)
{
	SaveToFile_7(file); //wqq 2017.5.23 唉
	int num = (int)m_sSerSignal.length();
	file.write((char *)&num,sizeof(int));
	file.write((char *)m_sSerSignal.c_str(),num);
}

/***********************************************
作者： 王庆利   创建时间 ： 2010.1.26
功能描述：按版本保存函数
输入参数：file保存文件
输出参数：无
返回参数说明：无
************************************************/
void CRoomMaterialChange::SaveToFile_1(std::ofstream &file)
{
	file.write((char *)&m_iStyleIndex,sizeof(int));
	file.write((char *)&m_iObjectIndex,sizeof(int));
	file.write((char *)&m_iTextureIndex,sizeof(int));

	file.write((char *)&m_bTexture,sizeof(bool));
	file.write((char *)&m_Color,sizeof(CVector3));

	int num = (int)m_TexName.length();
	file.write((char *)&num,sizeof(int));
	file.write((char *)m_TexName.c_str(),num);
	num = (int)m_TexClass.length();
	file.write((char *)&num,sizeof(int));
	file.write((char *)m_TexClass.c_str(),num);
}

/***********************************************
作者： 王庆利   创建时间 ： 2010.1.26
功能描述：按版本读取函数
输入参数：file读取文件
输出参数：无
返回参数说明：无
************************************************/
void CRoomMaterialChange::ReadFromFile_1(std::ifstream &file)
{
	file.read((char *)&m_iStyleIndex,sizeof(int));
	file.read((char *)&m_iObjectIndex,sizeof(int));
	file.read((char *)&m_iTextureIndex,sizeof(int));
	file.read((char *)&m_bTexture,sizeof(bool));
	file.read((char *)&m_Color,sizeof(CVector3));

	int num = 0;
	file.read((char *)&num,sizeof(int));
	char *temp = new char[num+1];
	file.read((char *)temp, num);
	temp[num] = 0;
	m_TexName = temp;
	delete temp;
	int pp = m_TexName.find("tex.bmp");
	if (-1!=pp)
	{
		m_TexName = m_TexName.substr(0, pp) + ".txt";
	}
	pp = m_TexName.rfind('\\')-1;
	pp = m_TexName.rfind('\\', pp);
	if (-1!=pp)
	{
		m_TexName = m_TexName.substr(pp);
	}
	//if (m_iStyleIndex == 1) //wqq 2018.7.10 
	//{
	//	CString str;
	//	str.Format(_T("%d,%s"), m_iObjectIndex, NetApp::S2WS(m_TexName));
	//	MessageBox(NULL,str,_T(""),0);
	//}
	num = 0;
	file.read((char *)&num,sizeof(int));
	temp = new char[num+1];
	file.read((char *)temp, num);
	temp[num] = 0;
	m_TexClass = temp;
	delete temp;
}

/***********************************************
作者： 王庆利   创建时间 ： 2010.1.26
功能描述：按版本保存函数
输入参数：file保存文件
输出参数：无
返回参数说明：无
************************************************/
void CRoomMaterialChange::SaveToFile_2(std::ofstream &file)
{
	SaveToFile_1(file);
	file.write((char *)&m_iRoomStyle, sizeof(int));
}

/***********************************************
作者： 王庆利   创建时间 ： 2010.1.26
功能描述：按版本读取函数
输入参数：file读取文件
输出参数：无
返回参数说明：无
************************************************/
void CRoomMaterialChange::ReadFromFile_2(std::ifstream &file)
{
	ReadFromFile_1(file);
	file.read((char *)&m_iRoomStyle, sizeof(int));
}

/***********************************************
作者： 王庆利   创建时间 ： 2010.1.26
功能描述：按版本保存函数
输入参数：file保存文件
输出参数：无
返回参数说明：无
************************************************/
void CRoomMaterialChange::SaveToFile_3(std::ofstream &file)
{
	SaveToFile_2(file);
	int num = (int)m_objName.length();
	file.write((char *)&num,sizeof(int));
	file.write((char *)m_objName.c_str(),num);
}

/***********************************************
作者： 王庆利   创建时间 ： 2010.1.26
功能描述：按版本读取函数
输入参数：file读取文件
输出参数：无
返回参数说明：无
************************************************/
void CRoomMaterialChange::ReadFromFile_3(std::ifstream &file)
{
	ReadFromFile_2(file);
	int num = 0;
	file.read((char *)&num,sizeof(int));
	char *temp = new char[num+1];
	file.read((char *)temp, num);
	temp[num] = 0;
	m_objName = temp;
	delete temp;
}

void CRoomMaterialChange::SaveToFile_4(std::ofstream &file)
{
	SaveToFile_3(file);
	file.write((char *)&m_texScaleAndAngle,sizeof(CVector3));
}

void CRoomMaterialChange::ReadFromFile_4(std::ifstream &file)
{
	ReadFromFile_3(file);
	file.read((char *)&m_texScaleAndAngle,sizeof(CVector3));
}

void CRoomMaterialChange::SaveToFile_5( std::ofstream &file )
{
	SaveToFile_4(file);
	file.write((char*)&m_regSize, sizeof(CVector2));
}

void CRoomMaterialChange::ReadFromFile_5( std::ifstream &file )
{
	ReadFromFile_4(file);
	file.read((char *)&m_regSize,sizeof(CVector2));
}

//jiang gai 2015.9.16，增加渲染材质和价格属性
void CRoomMaterialChange::ReadFromFile_6( std::ifstream &file )
{
	ReadFromFile_5(file);
	file.read((char *)m_ErMaterialID,30);
	int num = 0;
	file.read((char *)&num,sizeof(int));
	char *temp = new char[num+1];
	file.read((char *)temp, num);
	temp[num] = 0;
	m_price = temp;
	delete temp;
}

//jiang gai 2015.9.16 增加对渲染材质和价格的保存
void CRoomMaterialChange::SaveToFile_6( std::ofstream &file)
{
	SaveToFile_5(file);
	file.write((char*)m_ErMaterialID,30);
	int num = (int)m_price.length();
	file.write((char *)&num,sizeof(int));
	file.write((char *)m_price.c_str(),num);
}

//jiang gai 2015.11.16 增加对CTextureChange的支持
void CRoomMaterialChange::ReadFromFile_7( std::ifstream &file )
{
	ReadFromFile_6(file);
	m_TexUVChange.ReadFromFile(file);
}

//jiang gai 2015.11.16 增加对CTextureChange的支持
void CRoomMaterialChange::SaveToFile_7( std::ofstream &file)
{
	SaveToFile_6(file);
	m_TexUVChange.SaveToFile(file);
}

CRoomMaterialChange::~CRoomMaterialChange()
{
	m_objName = "";///要将更改材质的物体名称记录下来，方便核对
	memset(m_ErMaterialID, 0, sizeof(m_ErMaterialID));//jiang gai 2015.9.16 ess渲染材质
	m_sSerSignal = m_TexClass=m_TexName=m_price = "";
}