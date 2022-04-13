/*********************************************/
/*              全局定义头文件               */
/*              文件名:Main.h                */
#pragma once

#include <vector>
#include <string>
//#include <gl/gl.h>
#include <fstream>


#ifndef DLLImport
#define DllImport    __declspec( dllimport )
#endif
#ifndef DLLExport
#define DllExport    __declspec( dllexport )
#endif

//二维向量
//用来描述纹理坐标
typedef class tagCVector2
{
	// public:
	// 	tagCVector2():u(0.0f), v(0.0f){}
public:
	float u, v;
}CVector2;

//三维向量
//用来描述坐标、法线
class DllExport CVector3  //jiang gai 2015.8.8
{
	// public:
	//CVector3(float x1=1.0,float y1=1.0,float z1=1.0);//jiang gai 2016.1.9 
public:
	float x, y, z;
public:
	CVector3& operator=(const CVector3& p1)//jiang 10.1 改
	{
		x = p1.x;
		y = p1.y;
		z = p1.z;
		return *this;
	}
	//jiang gai 2015.8.8
	bool operator==(const CVector3& p1);
	//jiang gai 2015.8.8
	CVector3& operator=(const CVector2& p1);//jiang gai 2016.1.9 
};

const CVector3 ZERO_VECTOR3 = { 0.0f, 0.0f, 0.0f };
const CVector3 ONE_VECTOR3 = { 1.0f, 1.0f, 1.0f };
const CVector3 X_VECTOR3 = { 1.0f, 0.0f, 0.0f };
const CVector3 Y_VECTOR3 = { 0.0f, 1.0f, 0.0f };
const CVector3 Z_VECTOR3 = { 0.0f, 0.0f, 1.0f };
const CVector2 ZERO_VECTOR2 = { 0.0f, 0.0f };
const CVector2 ONE_VECTOR2 = { 1.0f, 1.0f };

typedef class tagCPropertyDescriptor
{
public:
	char  PropertyName[256];           //属性名
	char  TypeName[256];               //属性类型名(BOOL,字符串,整型,浮点型)
	char  CanbeChanged[8];             //属性能否被修改
	char  CanbeSeen[8];                //属性是否可见
	char  value[256];                  //具体的属性值
private:
	int m_iVersion;

public:
	tagCPropertyDescriptor() :m_iVersion(1)
	{
		memset(PropertyName, 0, 256);
		memset(TypeName, 0, 256);
		memset(CanbeChanged, 0, 8);
		memset(CanbeSeen, 0, 8);
		memset(value, 0, 256);
	}
	/***********************************************
	作者： 王庆利   创建时间 ： 2010.1.11
	功能描述：保存函数
	输入参数：file保存文件
	输出参数：无
	返回参数说明：无
	************************************************/
	void SaveToFile(std::ofstream& file)
	{
		file.write((char*)&m_iVersion, sizeof(int));
		switch (m_iVersion)
		{
		case 1:
		{
			SaveToFile_1(file);
		}
		break;
		default:
			break;
		}
	}

	/***********************************************
	作者： 王庆利   创建时间 ： 2010.1.11
	功能描述：读取函数
	输入参数：file读取文件
	输出参数：无
	返回参数说明：无
	************************************************/
	void ReadFromFile(std::ifstream& file)
	{
		int version = m_iVersion;
		file.read((char*)&version, sizeof(int));
		switch (version)
		{
		case 1:
		{
			ReadFromFile_1(file);
		}
		break;
		default:
			break;
		}
	}

	/***********************************************
	作者： 王庆利   创建时间 ： 2010.1.11
	功能描述：按版本保存函数
	输入参数：file保存文件
	输出参数：无
	返回参数说明：无
	************************************************/
	void SaveToFile_1(std::ofstream& file)
	{
		file.write(PropertyName, 256);
		file.write(TypeName, 256);
		file.write(CanbeChanged, 8);
		file.write(CanbeSeen, 8);
		file.write(value, 256);
	}

	/***********************************************
	作者： 王庆利   创建时间 ： 2010.1.11
	功能描述：按版本读取函数
	输入参数：file读取文件
	输出参数：无
	返回参数说明：无
	************************************************/
	void ReadFromFile_1(std::ifstream& file)
	{
		file.read(PropertyName, 256);
		file.read(TypeName, 256);
		file.read(CanbeChanged, 8);
		file.read(CanbeSeen, 8);
		file.read(value, 256);
	}
}CPropertyDescriptor;                  //物体类属性描述

typedef class tagCObjectStyleDescriptor
{
public:
	tagCObjectStyleDescriptor()
	{
		memset(ObjectStyleName, 0, 256);
		memset(ObjectStyleFile, 0, 256);
		memset(ObjectStyleDLL, 0, 256);
		memset(ModelDatabaseDir, 0, 256);
		//hInstance = NULL;
		//AddSucceedFlag = FALSE;
	}
	char ObjectStyleName[256];         //物体类型名称
	char ObjectStyleFile[256];         //描述该类型物体的文件所在的路径
	char ObjectStyleDLL[256];          //实现该物体的插件(DLL)所在的路径
	char ModelDatabaseDir[256];		   //该类物体模型库所在的路径
	//HINSTANCE hInstance;               //DLL加载后的句柄
	//BOOL AddSucceedFlag;               //该类型物体描述文件加载成功标志

}CObjectStyleDescriptor;               //类型描述器

typedef class tagCEventDescriptor
{
public:
	char ReturnType[64];               //事件函数的返回值类型
	char EventName[256];               //事件函数名 
	char ParameterList[256];           //事件函数的参数列表(以逗号分隔)
private:
	int m_iVersion;

public:
	tagCEventDescriptor() :m_iVersion(1)
	{
		memset(ReturnType, 0, 64);
		memset(EventName, 0, 256);
		memset(ParameterList, 0, 256);
	}
	/***********************************************
	作者： 王庆利   创建时间 ： 2010.1.11
	功能描述：保存函数
	输入参数：file保存文件
	输出参数：无
	返回参数说明：无
	************************************************/
	void SaveToFile(std::ofstream& file)
	{
		file.write((char*)&m_iVersion, sizeof(int));
		switch (m_iVersion)
		{
		case 1:
		{
			SaveToFile_1(file);
		}
		break;
		default:
			break;
		}
	}

	/***********************************************
	作者： 王庆利   创建时间 ： 2010.1.11
	功能描述：读取函数
	输入参数：file读取文件
	输出参数：无
	返回参数说明：无
	************************************************/
	void ReadFromFile(std::ifstream& file)
	{
		int version = m_iVersion;
		file.read((char*)&version, sizeof(int));
		switch (version)
		{
		case 1:
		{
			ReadFromFile_1(file);
		}
		break;
		default:
			break;
		}
	}

	/***********************************************
	作者： 王庆利   创建时间 ： 2010.1.11
	功能描述：按版本保存函数
	输入参数：file保存文件
	输出参数：无
	返回参数说明：无
	************************************************/
	void SaveToFile_1(std::ofstream& file)
	{
		file.write(ReturnType, 64);
		file.write(EventName, 256);
		file.write(ParameterList, 256);
	}

	/***********************************************
	作者： 王庆利   创建时间 ： 2010.1.11
	功能描述：按版本读取函数
	输入参数：file读取文件
	输出参数：无
	返回参数说明：无
	************************************************/
	void ReadFromFile_1(std::ifstream& file)
	{
		file.read(ReturnType, 64);
		file.read(EventName, 256);
		file.read(ParameterList, 256);
	}
}CEventDescriptor;                     //物体类事件描述

typedef class tagCScenePropertyAndEvent
{
public:
	char           Version[20];                   //场景定义文件的版本号
	unsigned short NumberOfProperty;              //场景属性的个数
	unsigned short NumberOfEvent;                 //场景事件的个数
	std::vector<CPropertyDescriptor> PropertyList;//场景属性列表
	std::vector<CEventDescriptor>    EventList;   //场景事件列表
private:
	int m_iVersion;
public:
	tagCScenePropertyAndEvent() :m_iVersion(1)
	{
		memset(Version, 0, 20);
		NumberOfProperty = 0;
		NumberOfEvent = 0;
	}
	/***********************************************
	作者： 王庆利   创建时间 ： 2010.1.11
	功能描述：保存函数
	输入参数：file保存文件
	输出参数：无
	返回参数说明：无
	************************************************/
	void SaveToFile(std::ofstream& file)
	{
		file.write((char*)&m_iVersion, sizeof(int));
		switch (m_iVersion)
		{
		case 1:
		{
			SaveToFile_1(file);
		}
		break;
		default:
			break;
		}
	}

	/***********************************************
	作者： 王庆利   创建时间 ： 2010.1.11
	功能描述：读取函数
	输入参数：file读取文件
	输出参数：无
	返回参数说明：无
	************************************************/
	void ReadFromFile(std::ifstream& file)
	{
		int version = m_iVersion;
		file.read((char*)&version, sizeof(int));
		switch (version)
		{
		case 1:
		{
			ReadFromFile_1(file);
		}
		break;
		default:
			break;
		}
	}

	/***********************************************
	作者： 王庆利   创建时间 ： 2010.1.11
	功能描述：按版本保存函数
	输入参数：file保存文件
	输出参数：无
	返回参数说明：无
	************************************************/
	void SaveToFile_1(std::ofstream& file)
	{
		file.write((char*)Version, 20);
		file.write((char*)&NumberOfProperty, sizeof(unsigned short));
		file.write((char*)&NumberOfEvent, sizeof(unsigned short));
		for (int i = 0; i < NumberOfProperty; i++)
		{
			PropertyList[i].SaveToFile(file);
		}
		for (int i = 0; i < NumberOfEvent; i++)
		{
			EventList[i].SaveToFile(file);
		}
	}

	/***********************************************
	作者： 王庆利   创建时间 ： 2010.1.11
	功能描述：按版本读取函数
	输入参数：file读取文件
	输出参数：无
	返回参数说明：无
	************************************************/
	void ReadFromFile_1(std::ifstream& file)
	{
		file.read((char*)Version, 20);
		unsigned short num1 = NumberOfProperty;
		unsigned short num2 = NumberOfEvent;
		file.read((char*)&num1, sizeof(unsigned short));
		file.read((char*)&num2, sizeof(unsigned short));
		PropertyList.resize(num1);
		for (int i = 0; i < num1; i++)
		{
			PropertyList[i].ReadFromFile(file);
		}
		EventList.resize(num2);
		for (int i = 0; i < num2; i++)
		{
			EventList[i].ReadFromFile(file);
		}
	}
}CScenePropertyAndEvent;

typedef class DllExport tagCPropertyAndEvent
{
public:
	unsigned short NumberOfProperty;              //某个物体类的属性个数
	unsigned short NumberOfEvent;                 //某个物体类的事件个数
	std::vector<CPropertyDescriptor> PropertyList;//某个物体类的属性描述列表
	std::vector<CEventDescriptor>    EventList;   //某个物体类的事件描述列表
private:
	int m_iVersion;

public:
	tagCPropertyAndEvent() :m_iVersion(1)
	{
		NumberOfProperty = 0;
		NumberOfEvent = 0;
	}
	/***********************************************
	作者： 王庆利   创建时间 ： 2010.1.11
	功能描述：保存函数
	输入参数：file保存文件
	输出参数：无
	返回参数说明：无
	************************************************/
	void SaveToFile(std::ofstream& file)
	{
		file.write((char*)&m_iVersion, sizeof(int));
		switch (m_iVersion)
		{
		case 1:
		{
			SaveToFile_1(file);
		}
		break;
		default:
			break;
		}
	}

	/***********************************************
	作者： 王庆利   创建时间 ： 2010.1.11
	功能描述：读取函数
	输入参数：file读取文件
	输出参数：无
	返回参数说明：无
	************************************************/
	void ReadFromFile(std::ifstream& file)
	{
		int version = m_iVersion;
		file.read((char*)&version, sizeof(int));
		switch (version)
		{
		case 1:
		{
			ReadFromFile_1(file);
		}
		break;
		default:
			break;
		}
	}

	/***********************************************
	作者： 王庆利   创建时间 ： 2010.1.11
	功能描述：按版本保存函数
	输入参数：file保存文件
	输出参数：无
	返回参数说明：无
	************************************************/
	void SaveToFile_1(std::ofstream& file)
	{
		file.write((char*)&NumberOfProperty, sizeof(unsigned short));
		file.write((char*)&NumberOfEvent, sizeof(unsigned short));
		for (int i = 0; i < NumberOfProperty; i++)
		{
			PropertyList[i].SaveToFile(file);
		}
		for (int i = 0; i < NumberOfEvent; i++)
		{
			EventList[i].SaveToFile(file);
		}
	}

	/***********************************************
	作者： 王庆利   创建时间 ： 2010.1.11
	功能描述：按版本读取函数
	输入参数：file读取文件
	输出参数：无
	返回参数说明：无
	************************************************/
	void ReadFromFile_1(std::ifstream& file)
	{
		unsigned short num1 = NumberOfProperty;
		unsigned short num2 = NumberOfEvent;
		file.read((char*)&num1, sizeof(unsigned short));
		file.read((char*)&num2, sizeof(unsigned short));
		PropertyList.resize(num1);
		for (int i = 0; i < num1; i++)
		{
			PropertyList[i].ReadFromFile(file);
		}
		EventList.resize(num2);
		for (int i = 0; i < num2; i++)
		{
			EventList[i].ReadFromFile(file);
		}
	}

	/***********************************************
	作者： 王庆利   创建时间 ： 2010.1.29
	功能描述：根据现有属性脚本读取函数
	输入参数：file读取文件
	输出参数：无
	返回参数说明：无
	************************************************/
	void ReadFromFileReferScript(std::ifstream& file)
	{
		int version = m_iVersion;
		file.read((char*)&version, sizeof(int));
		switch (version)
		{
		case 1:
		{
			ReadFromFileReferScript_1(file);
		}
		break;
		default:
			break;
		}
	}
	/***********************************************
	作者： 王庆利   创建时间 ： 2010.1.11
	功能描述：按版本根据现有属性脚本读取函数
	输入参数：file读取文件
	输出参数：无
	返回参数说明：无
	************************************************/
	void ReadFromFileReferScript_1(std::ifstream& file)
	{
		unsigned short num1 = NumberOfProperty;
		unsigned short num2 = NumberOfEvent;
		file.read((char*)&num1, sizeof(unsigned short));
		file.read((char*)&num2, sizeof(unsigned short));
		for (int i = 0; i < num1; i++)
		{
			CPropertyDescriptor p1;
			p1.ReadFromFile(file);
			for (int j = 0; j < (int)PropertyList.size(); j++)
			{
				if (std::string(PropertyList[j].PropertyName) == std::string(p1.PropertyName))
				{
					PropertyList[j] = p1;
				}
			}
		}
		for (int i = 0; i < num2; i++)
		{
			CEventDescriptor p1;
			p1.ReadFromFile(file);
			for (int j = 0; j < (int)EventList.size(); j++)
			{
				if (std::string(EventList[j].EventName) == std::string(p1.EventName))
				{
					EventList[j] = p1;
				}
			}
		}
	}
	//******************************************
//获得属性Name的值(实际的值)
//成功获取,返回TRUE,否则,返回FALSE
//!!指针pvalue所需空间在该函数外面分配!! 
	bool GetPropertyRealValue(const char name[], char value_type[], void* pvalue)
	{
		//循环所有的属性
		for (unsigned short i = 0; i < (int)PropertyList.size(); i++)
		{
			//判断是否有属性的名字为name
			if (strcmp(PropertyList[i].PropertyName, name) == 0)
			{
				//如果有
				//判断该类型是否为所传递的指针pvalue的类型
				if (strcmp(PropertyList[i].TypeName, value_type) == 0)
				{
					//如果是
					//则对不同的类型分别处理
					if (strcmp(value_type, "integer") == 0)//整型
					{
						*((int*)pvalue) = atoi(PropertyList[i].value);
					}
					else if (strcmp(value_type, "float") == 0)
					{
						*((float*)pvalue) = (float)atof(PropertyList[i].value);
					}
					else if (strcmp(value_type, "charcluster") == 0)
					{
						strcpy((char*)pvalue, PropertyList[i].value);
					}
					else if (strcmp(value_type, "boolean") == 0)
					{
						bool bool_value;
						if (strcmp(PropertyList[i].value, "真") == 0)
							bool_value = true;
						else if (strcmp(PropertyList[i].value, "假") == 0)
							bool_value = false;
						else
							return false;
						*((bool*)pvalue) = bool_value;
					}
					else
						return false;

					return true;
				}
				else
				{
					//否则返回FALSE
					return false;
				}
			}
		}
		return false;
	}
}CPropertyAndEvent;