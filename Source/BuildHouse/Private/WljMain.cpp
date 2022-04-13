#include "../Public/WljMain.h"
#include <math.h>
//jiang gai 2016.1.9
CVector3& CVector3::operator=(const CVector2& p1)
{
	x = p1.u;
	y = 0;
	z = p1.v;
	return *this;
}

//jiang gai 2015.8.8
bool CVector3::operator==(const CVector3& p1)
{
	if (fabs(x - p1.x) < 0.0001 && fabs(y - p1.y) < 0.0001 && fabs(z - p1.z) < 0.0001)
	{
		return true;
	}
	else
		return false;
}

//jiang gai 2015.7.9
tagCMatElements::tagCMatElements()
{
	memset(Mat_Name, 0, 256);
	Ambient.x = 1.0f;  Ambient.y = 1.0f;  Ambient.z = 1.0f;
	Diffuse.x = 1.0f;  Diffuse.y = 1.0f;  Diffuse.z = 1.0f;
	Specular.x = 1.0f;  Specular.y = 1.0f;  Specular.z = 1.0f;
	Emission.x = 0.0f;  Emission.y = 0.0f;  Emission.z = 0.0f;
	Shininess = 46.0f;
	Alpha = 1.0f;
	m_version = 3;  //jiang gai 2016.5.25 挽救章文晴的错误 //zwq 2016.5.12 添加
	SpecularColor.x = 1.0f;  SpecularColor.y = 1.0f;  SpecularColor.z = 1.0f;
	Bumpweight = 0.0f;
}

//zwq 2016.5.12
void tagCMatElements::ReadFromFile_2(flyFile_w* flyfile)
{
	ReadFromFile_1(flyfile);
	flyfile->read((char*)&Bumpweight, sizeof(float));
}

//jiang gai 2015.9.24
void tagCMatElements::ReadFromFile(flyFile_w* flyfile)
{
	int version;
	flyfile->read((char*)&version, sizeof(int));
	switch (version)
	{
	case 1:
		ReadFromFile_1(flyfile);
		break;
	case 2://zwq 2016.5.12 添加
		ReadFromFile_2(flyfile);
		break;
	case 3://jiang gai 2016.5.25 挽救章文晴的错误
		ReadFromFile_2(flyfile);
		break;
	}
}

//jiang gai 2015.9.24
void tagCMatElements::ReadFromFile_1(flyFile_w* flyfile)
{
	flyfile->read((char*)Mat_Name, sizeof(char) * 256);
	flyfile->read((char*)&Ambient, sizeof(CVector3));
	flyfile->read((char*)&Diffuse, sizeof(CVector3));
	flyfile->read((char*)&Specular, sizeof(CVector3));
	flyfile->read((char*)&Emission, sizeof(CVector3));
	flyfile->read((char*)&SpecularColor, sizeof(CVector3));
	flyfile->read((char*)&Shininess, sizeof(float));
	flyfile->read((char*)&Alpha, sizeof(float));
}

//jiang gai 2015.7.9
tagCMapElements::tagCMapElements()
{
	memset(FileName, 0, 256);
	alpha = 255;
	type = 0;
	u_scale = 1.0f;
	v_scale = 1.0f;
	u_offset = 0.0f;
	v_offset = 0.0f;
	rotation = 0.0f;
	m_version = 2;//jiang gai 2017.4.24
	//CGetInfoBasedSerSignal getSignal; //wqq 2017.6.6 注释掉
	//serSignal = getSignal.GetDefaultSignal(); //wqq 2017.4.24 
}

//jiang gai 2015.9.24
void tagCMapElements::ReadFromFile(flyFile_w* flyfile)
{
	int version;
	flyfile->read((char*)&version, sizeof(int));
	switch (version)
	{
	case 1:
		ReadFromFile_1(flyfile);
		break;
		//jiang gai 2017.4.24
	case 2:
		ReadFromFile_2(flyfile);
		break;
	}
}

//jiang gai 2015.9.24
void tagCMapElements::ReadFromFile_1(flyFile_w* flyfile)
{
	flyfile->read((char*)FileName, sizeof(char) * 256);
	flyfile->read((char*)&alpha, sizeof(unsigned char));
	flyfile->read((char*)&type, sizeof(short int));
	flyfile->read((char*)&u_scale, sizeof(float));
	flyfile->read((char*)&v_scale, sizeof(float));
	flyfile->read((char*)&u_offset, sizeof(float));
	flyfile->read((char*)&v_offset, sizeof(float));
	flyfile->read((char*)&rotation, sizeof(float));
}

//jiang gai 2017.4.24
//jiang gai 2015.9.24
void tagCMapElements::ReadFromFile_2(flyFile_w* flyfile)
{
	ReadFromFile_1(flyfile);
	int num = 0;
	flyfile->read((char*)&num, sizeof(int));
	char* tp = new char[num + 1];
	flyfile->read((char*)tp, num);
	tp[num] = 0;
	serSignal = tp;
}

//jiang gai 2015.7.9
tagCTextureProperty::tagCTextureProperty()
{
	m_version = 1;
}

//jiang gai 2015.9.24
void tagCTextureProperty::ReadFromFile(flyFile_w* flyfile)
{
	int version;
	flyfile->read((char*)&version, sizeof(int));
	switch (version)
	{
	case 1:
		ReadFromFile_1(flyfile);
		break;
	}
}

//jiang gai 2015.9.24
void tagCTextureProperty::ReadFromFile_1(flyFile_w* flyfile)
{
	material.ReadFromFile(flyfile);
	map.ReadFromFile(flyfile);
}

CMaps::CMaps()
{
	memset(map_name, 0, 256);
	//jiang gai 2014.9.1 完成初始化
	memset(m_sPovMatirealID, 0, 30);
	memset(m_sEssMatirealID, 0, 30);
	m_sProdSize = "0*0";///wql 2019.4.25 原来的贴图没有该值，都为0
	map_index = -1;
	m_version = 5;///wql 2019.4.25 新版保存读取单片产品的尺寸 //jiang gai 2016.9.14 //zwq 2016.5.12//jiang gai 2015.9.24 增加对从压缩文件中直接读取cmps的支持
}

void CMaps::CopyFromOld(CMapsOld& map)
{
	map_index = map.map_index;
	memcpy(map_name, map.map_name, sizeof(char) * 256);
	texture_type = map.texture_type;
	two_side = map.two_side;					//是否为双面纹理
	//memcpy((char *)&texture_property,(char *)&map.texture_property,sizeof(CTextureProperty1));
	//jiang gai 2015.7.9
	texture_property.material.Alpha = map.texture_property.material.Alpha;
	texture_property.material.Ambient = map.texture_property.material.Ambient;
	texture_property.material.Diffuse = map.texture_property.material.Diffuse;
	texture_property.material.Emission = map.texture_property.material.Emission;
	memcpy(texture_property.material.Mat_Name, map.texture_property.material.Mat_Name, sizeof(char) * 256);
	texture_property.material.Shininess = map.texture_property.material.Shininess;
	texture_property.material.Specular = map.texture_property.material.Specular;
	texture_property.map.alpha = map.texture_property.map.alpha;
	memcpy(texture_property.map.FileName, map.texture_property.map.FileName, sizeof(char) * 256);
	texture_property.map.rotation = map.texture_property.map.rotation;
	texture_property.map.type = map.texture_property.map.type;
	texture_property.map.u_offset = map.texture_property.map.u_offset;
	texture_property.map.u_scale = map.texture_property.map.u_scale;
	texture_property.map.v_offset = map.texture_property.map.v_offset;
	texture_property.map.v_scale = map.texture_property.map.v_scale;
	//jiang gai 2015.9.25 
	texture_property.material.SpecularColor = map.texture_property.material.Specular;

	//jiang gai 2015.2.2 在这里，除去map_name的后两位，来赋材质属性的值
	if (strlen(map_name) < 32 && strlen(map_name) > 2)//jiang gai 2015.4.16
	{
		//因为最多30个字母
		memcpy(m_sEssMatirealID, map_name, strlen(map_name) - 2);
		memcpy(m_sPovMatirealID, map_name, strlen(map_name) - 2);
	}
}

void CMaps::ReadFromFile(flyFile_w* flyfile)
{
	if (flyfile == NULL)
		return;
	//jiang gai 2015.9.24 从文件中读取临时version，m_version不变
	int version;
	flyfile->read((char*)&version, sizeof(int));
	switch (version)
	{
	case 1:
		ReadFromFile_1(flyfile);
		break;
		//jiang gai 2015.9.24 增加对从压缩文件中直接读取cmps的支持
	case 2:
		ReadFromFile_2(flyfile);
		break;
	case 3://zwq 2016.5.12
		ReadFromFile_3(flyfile);
		break;
	case 4://jiang gai 2016.9.14
		ReadFromFile_4(flyfile);
		break;
	}
}

void CMaps::ReadFromFile_1(flyFile_w* flyfile)
{
	flyfile->read((char*)&map_index, sizeof(int));
	flyfile->read((char*)map_name, sizeof(char) * 256);
	flyfile->read((char*)&texture_type, sizeof(TEXTURE_TYPE));
	flyfile->read((char*)&two_side, sizeof(bool));
	//jiang gai 2015.7.9 这里有隐患,jia文件中的CTextureProperty是最老的
	CTextureProperty0 property0;
	flyfile->read((char*)&property0, sizeof(CTextureProperty0));
	//再把相关的属性赋给texture_property，因为texture_property有了m_version，不能直接复制
	texture_property.material.Alpha = property0.material.Alpha;
	texture_property.material.Ambient = property0.material.Ambient;
	texture_property.material.Diffuse = property0.material.Diffuse;
	texture_property.material.Emission = property0.material.Emission;
	memcpy(texture_property.material.Mat_Name, property0.material.Mat_Name, sizeof(char) * 256);
	texture_property.material.Shininess = property0.material.Shininess;
	texture_property.material.Specular = property0.material.Specular;
	//jiang gai 2015.9.25
	texture_property.material.SpecularColor = property0.material.Specular;
	texture_property.map.alpha = property0.map.alpha;
	memcpy(texture_property.map.FileName, property0.map.FileName, sizeof(char) * 256);
	texture_property.map.rotation = property0.map.rotation;
	texture_property.map.type = property0.map.type;
	texture_property.map.u_offset = property0.map.u_offset;
	texture_property.map.u_scale = property0.map.u_scale;
	texture_property.map.v_offset = property0.map.v_offset;
	texture_property.map.v_scale = property0.map.v_scale;
	flyfile->read((char*)m_sPovMatirealID, sizeof(char) * 30);
	flyfile->read((char*)m_sEssMatirealID, sizeof(char) * 30);
}

//jiang gai 2016.9.14 读新增材质属性，方便输出效果图
void CMaps::ReadFromFile_4(flyFile_w* flyfile)
{
	int n = 0;
	char* pchar = NULL;
	ReadFromFile_3(flyfile);

	flyfile->read((char*)&n, sizeof(int));
	if (n != 0)
	{
		pchar = new char[n + 1];
		flyfile->read((char*)pchar, n);
		pchar[n] = 0;
		m_sGoodsName = pchar;
		delete[]pchar;
		n = 0;
	}

	flyfile->read((char*)&n, sizeof(int));
	if (n != 0)
	{
		pchar = new char[n + 1];
		flyfile->read((char*)pchar, n);
		pchar[n] = 0;
		m_sScendClass = pchar;
		delete[]pchar;
		n = 0;
	}

	flyfile->read((char*)&n, sizeof(int));
	if (n != 0)
	{
		pchar = new char[n + 1];
		flyfile->read((char*)pchar, n);
		pchar[n] = 0;
		m_sThirdClass = pchar;
		delete[]pchar;
		n = 0;
	}

	flyfile->read((char*)&n, sizeof(int));
	if (n != 0)
	{
		pchar = new char[n + 1];
		flyfile->read((char*)pchar, n);
		pchar[n] = 0;
		m_sSerial = pchar;
		delete[]pchar;
		n = 0;
	}


	flyfile->read((char*)&n, sizeof(int));
	if (n != 0)
	{
		pchar = new char[n + 1];
		flyfile->read((char*)pchar, n);
		pchar[n] = 0;
		m_sType = pchar;
		delete[]pchar;
		n = 0;
	}

	flyfile->read((char*)&n, sizeof(int));
	if (n != 0)
	{
		pchar = new char[n + 1];
		flyfile->read((char*)pchar, n);
		pchar[n] = 0;
		m_sStyle = pchar;
		delete[]pchar;
		n = 0;
	}

	flyfile->read((char*)&n, sizeof(int));
	if (n != 0)
	{
		pchar = new char[n + 1];
		flyfile->read((char*)pchar, n);
		pchar[n] = 0;
		m_sSize = pchar;
		delete[]pchar;
		n = 0;
	}

	flyfile->read((char*)&n, sizeof(int));
	if (n != 0)
	{
		pchar = new char[n + 1];
		flyfile->read((char*)pchar, n);
		pchar[n] = 0;
		m_sPirce = pchar;
		delete[]pchar;
		n = 0;
	}

	flyfile->read((char*)&n, sizeof(int));
	if (n != 0)
	{
		pchar = new char[n + 1];
		flyfile->read((char*)pchar, n);
		pchar[n] = 0;
		m_sBrand = pchar;
		delete[]pchar;
		n = 0;
	}

	flyfile->read((char*)&n, sizeof(int));
	if (n != 0)
	{
		pchar = new char[n + 1];
		flyfile->read((char*)pchar, n);
		pchar[n] = 0;
		m_sSmallScript = pchar;
		delete[]pchar;
		n = 0;
	}

	flyfile->read((char*)&n, sizeof(int));
	if (n != 0)
	{
		pchar = new char[n + 1];
		flyfile->read((char*)pchar, n);
		pchar[n] = 0;
		m_sBigScript = pchar;
		delete[]pchar;
		n = 0;
	}

	flyfile->read((char*)&n, sizeof(int));
	if (n != 0)
	{
		pchar = new char[n + 1];
		flyfile->read((char*)pchar, n);
		pchar[n] = 0;
		m_sBuggentPrice = pchar;
		delete[]pchar;
		n = 0;
	}

	flyfile->read((char*)&n, sizeof(int));
	if (n != 0)
	{
		pchar = new char[n + 1];
		flyfile->read((char*)pchar, n);
		pchar[n] = 0;
		m_sSeries = pchar;
		delete[]pchar;
		n = 0;
	}
}

//jiang gai 2015.9.24 增加对从压缩文件中直接读取cmps的支持，直接通过cmaps的readfromfile
//函数从压缩文件中读取数据
void CMaps::ReadFromFile_2(flyFile_w* flyfile)
{
	flyfile->read((char*)&map_index, sizeof(int));
	flyfile->read((char*)map_name, sizeof(char) * 256);
	flyfile->read((char*)&texture_type, sizeof(TEXTURE_TYPE));
	flyfile->read((char*)&two_side, sizeof(bool));
	//直接读取CTextureProperty
	texture_property.ReadFromFile(flyfile);
	flyfile->read((char*)m_sPovMatirealID, sizeof(char) * 30);
	flyfile->read((char*)m_sEssMatirealID, sizeof(char) * 30);
}

//zwq 2016.5.12
void CMaps::ReadFromFile_3(flyFile_w* flyfile)
{
	ReadFromFile_2(flyfile);
	//直接读取CTextureProperty
	bump_property.ReadFromFile(flyfile);
}

///wql 2019.4.25 新版读取
void CMaps::ReadFromFile_5(flyFile_w* flyfile)
{
	ReadFromFile_4(flyfile);
	int n = 0;
	flyfile->read((char*)&n, sizeof(int));
	if (n != 0)
	{
		char* pchar = new char[n + 1];
		flyfile->read((char*)pchar, n);
		pchar[n] = 0;
		m_sProdSize = pchar;
		delete[]pchar;
		n = 0;
	}
}

