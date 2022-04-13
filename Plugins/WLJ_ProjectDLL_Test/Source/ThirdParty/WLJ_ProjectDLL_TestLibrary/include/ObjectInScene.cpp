#include "ObjectInScene.h"
#include "Main.h"
//#include "../Licence/LicenceHead/clicence.h"

#include <fstream>
#include <string.h>
#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//
//std::string TCHAR2STRING(TCHAR* STR)
//{
//	int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);
//	char* chRtn = new char[iLen * sizeof(char)];
//	WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);
//	std::string str(chRtn);
//	delete chRtn;
//	return str;
//}
//
//std::string WChar2Ansi(LPCWSTR pwszSrc)
//{
//	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
//
//	if (nLen <= 0) return std::string("");
//
//	char* pszDst = new char[nLen];
//	if (NULL == pszDst) return std::string("");
//
//	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
//	pszDst[nLen - 1] = 0;
//
//	std::string strTemp(pszDst);
//	delete[] pszDst;
//	return strTemp;
//}
//
//std::string ws2s(std::wstring& inputws) { return WChar2Ansi(inputws.c_str()); }
//
////Converting a Ansi string to WChar string
//
//std::wstring Ansi2WChar(LPCSTR pszSrc, int nLen)
//{
//	if (nLen <= 0) return std::wstring(TEXT(""));
//	int nSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, 0, 0);
//	if (nSize <= 0) return std::wstring(TEXT(""));
//
//	WCHAR* pwszDst = new WCHAR[nSize + 1];
//	if (NULL == pwszDst) return std::wstring(TEXT(""));
//
//	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, pwszDst, nSize);
//	pwszDst[nSize] = 0;
//
//	if (pwszDst[0] == 0xFEFF) // skip Oxfeff
//		for (int i = 0; i < nSize; i++)
//			pwszDst[i] = pwszDst[i + 1];
//
//	std::wstring wcharString(pwszDst);
//	delete pwszDst;
//
//	return wcharString;
//}
//
//std::wstring s2ws(const std::string& s) { return Ansi2WChar(s.c_str(), (int)s.size()); }

CObjectInScene::CObjectInScene(char* configPath)
{
	m_configFile = configPath;
	ScenePropAndEvent = new CScenePropertyAndEvent;
	//获得场景的属性与事件
	AddSceneSucceedFlag = GetScenePropertyAndEvent();
	NumberOfStyle = 0;
	//获得所有可用的物体的类型与这些类型的属性与事件
	GetAllObjectStyleFromObjectStyleFile();
}

CObjectInScene::~CObjectInScene()
{
	if (ScenePropAndEvent!=NULL)
	{
		delete ScenePropAndEvent;
		ScenePropAndEvent = NULL;
	}
}

//*************************************
//从类型描述文件中获得可用的物体类型
//类型描述文件中记录了所有当前可用的
//物体类型,通过增加该文件的内容可使可用
//的物体类型增加
///////////////////////////////////////
BOOL CObjectInScene::GetAllObjectStyleFromObjectStyleFile()
{
	//打开文件
	std::string currentPath = m_configFile+ "/ROUTE/STYLE/StyleFile.vinf";
	setlocale(LC_ALL,"Chinese-simplified");//设置中文环境
	std::ifstream input_file(currentPath.c_str(),std::ios::in);/// | std::ios::nocreate
	setlocale(LC_ALL,"C");//还原

	//若无法打开
	//返回失败
	if(input_file.fail())
	{
		NumberOfStyle=0;
		//MessageBox(NULL,TEXT("无法打开物体类型描述文件StyleFile.vinf!"),"错误",MB_OK);
		//AfxMessageBox(TEXT("无法打开物体类型描述文件StyleFile.vinf!"));
		return FALSE;
	}
	
	//读入总的物体类型数目
	input_file>>NumberOfStyle;

	//对每个物体类型
	int i=0;
	for(i=0; i<NumberOfStyle; i++)
	{
		CObjectStyleDescriptor style;
		//读入类型名称
		input_file>>style.ObjectStyleName;
		//读入该类型物体的描述文件的位置
		input_file>>style.ObjectStyleFile;
		//读入该类型物体的DLL位置
		input_file>>style.ObjectStyleDLL;
		//读入该类模型所在的模型库位置
		input_file>>style.ModelDatabaseDir;
		if(strcmp(style.ModelDatabaseDir,"无")==0)
			style.ModelDatabaseDir[0]='\0';
		//压入该类型到可用类型列表中
		StyleDescriptorList.push_back(style);
	}

	//关闭类型描述文件
	input_file.close();
	//获得每类物体的属性事件描述列表
	for(i=0; i<NumberOfStyle; i++)
	{
		//如果成功
		if(GetObjectPropertyAndEvent(i))
		{
			//标志该类型的物体可用
			//StyleDescriptorList[i].AddSucceedFlag=TRUE;
		}
		//否则
		else
		{
			//标志该类型的物体不可用
			//StyleDescriptorList[i].AddSucceedFlag=FALSE;
		}
	}

	//返回成功
	return TRUE;
}

//**********************************************
//获得某个类型的物体的可用属性与方法
BOOL CObjectInScene::GetObjectPropertyAndEvent(int index)
{
	//如果序号小于零或大于可用形体的个数
	//返回FALSE
	if(index<0 || index>=NumberOfStyle)
		return FALSE;
	CPropertyAndEvent PropEvent,*pPropEvent;
	PropertyAndEventList.push_back(PropEvent);
	pPropEvent=&(PropertyAndEventList[index]);

	std::string path = m_configFile + StyleDescriptorList[index].ObjectStyleFile;

	setlocale(LC_ALL,"Chinese-simplified");//设置中文环境
	std::ifstream input_file(path.c_str(),std::ios::in );///| std::ios::nocreate
	setlocale(LC_ALL,"C");//还原

	if(input_file.fail())
	{
		//无法打开第index个物体的属性与时间描述文件
		char buffer[1000]={0};
		//生成提示符
		sprintf_s(buffer,"无法打开物体类 %s 的属性事件描述文件\n%s\n物体类 %s 无法使用!",
			StyleDescriptorList[index].ObjectStyleName,
			StyleDescriptorList[index].ObjectStyleFile,
			StyleDescriptorList[index].ObjectStyleName);
		//MessageBox(NULL,buffer,TEXT("错误"),MB_OK);
		//AfxMessageBox(buffer);
		//返回失败
		return FALSE;
	}
	
	input_file>>pPropEvent->NumberOfProperty;	
	//读取属性列表
	unsigned short i=0;
	for( i=0; i<pPropEvent->NumberOfProperty; i++)
	{
		char temp[1000]={0};
		CPropertyDescriptor Prop,*pProp;
		pPropEvent->PropertyList.push_back(Prop);
		pProp=&(pPropEvent->PropertyList[i]);
		input_file>>temp;
		std::string tempString=temp;
		std::basic_string <char> :: size_type Pos1,Pos2,Length;
		
		Pos1=tempString.find(",");//找到第一个逗号的位置
		Pos2=tempString.rfind(",");//找到最后一个逗号的位置
		Length=tempString.length();//字符串的长度

		std::basic_string <char> tempName,tempType,tempValue,tempModifyFlag,tempSeeFlag,tempString1;
		tempName=tempString.substr(0,Pos1);
		tempSeeFlag=tempString.substr(Pos2+1,Length-Pos2-1);
		tempString1=tempString.substr(Pos1+1,Pos2-Pos1-1);
		tempString=tempString1;
		Pos1=tempString.find(",");
		Pos2=tempString.rfind(",");
		Length=tempString.length();
		tempType=tempString.substr(0,Pos1);
		tempModifyFlag=tempString.substr(Pos2+1,Length-Pos2-1);
		tempValue=tempString.substr(Pos1+1,Pos2-Pos1-1);
		if(tempValue=="无")
			tempValue="";
		
		strcpy_s(pProp->PropertyName,256,tempName.c_str());
		strcpy_s(pProp->TypeName,256,tempType.c_str());
		strcpy_s(pProp->CanbeChanged,8,tempModifyFlag.c_str());
		strcpy_s(pProp->CanbeSeen,8,tempSeeFlag.c_str());
		strcpy_s(pProp->value,256,tempValue.c_str());
	}	
	//读入事件的个数
	input_file>>pPropEvent->NumberOfEvent;
	//读取事件列表
	for(i=0; i<pPropEvent->NumberOfEvent; i++)
	{
		CEventDescriptor Event,*pEvent;
		pPropEvent->EventList.push_back(Event);

		pEvent=&(pPropEvent->EventList[i]);		
		char temp[5];
		input_file>>pEvent->ReturnType;
		input_file>>pEvent->EventName;
		input_file.getline(temp,4);
		input_file.getline(pEvent->ParameterList,255);
	}

	//关闭文件
	input_file.close();
	//调用相应的DLL库 wqq 库肯定是调用不到的，因为没有创建这样的库
	//path = m_configFile + StyleDescriptorList[index].ObjectStyleDLL; 
	//StyleDescriptorList[index].hInstance=LoadLibrary(s2ws(path).c_str());
	//if(StyleDescriptorList[index].hInstance==NULL)
	//{
	//	//无法加载第index个物体的DLL
	//	char buffer[1000]={0};
	//	//生成提示符
	//	sprintf_s(buffer,"无法加载物体类 %s 的DLL库\n%s\n物体类 %s 无法使用!",
	//		StyleDescriptorList[index].ObjectStyleName,
	//		StyleDescriptorList[index].ObjectStyleDLL,
	//		StyleDescriptorList[index].ObjectStyleName);
	//	//MessageBox(NULL,buffer,TEXT("错误"),MB_OK);
	//	//AfxMessageBox(buffer);
	//	//返回失败
	//	return FALSE;
	//}
	//返回成功
	return TRUE;
}

//**********************************************
//从场景属性事件文件中获得场景可用的属性与事件
BOOL CObjectInScene::GetScenePropertyAndEvent()
{
	//打开文件
	std::string currentPath = m_configFile + std::string("/ROUTE/PROPANDEVENT/ScenePropAndEvent.vinf");
	setlocale(LC_ALL,"Chinese-simplified");//设置中文环境
	std::ifstream input_file(currentPath.c_str(),std::ios::in); ///| std::ios::nocreate
	setlocale(LC_ALL,"C");//还原
	//无法打开场景的属性与事件描述文件
	if(input_file.fail())
	{		
		char buffer[1000]={0};
		//生成提示符
		sprintf_s(buffer,"无法打开场景的属性事件描述文件\n.\\ROUTE\\PROPANDEVENT\\ScenePropAndEvent.vinf\n无法创建场景!");
		//MessageBox(NULL,buffer,TEXT("错误"),MB_OK);
		//AfxMessageBox(buffer);
		//返回失败
		return FALSE;
	}
	
	//读入版本号
	input_file>>ScenePropAndEvent->Version;
	//读入场景属性的个数
	input_file>>ScenePropAndEvent->NumberOfProperty;
	//逐个读入场景属性
	unsigned short i=0;
	for( i=0; i<ScenePropAndEvent->NumberOfProperty; i++)
	{
		char temp[1000]={0};

		CPropertyDescriptor Prop,*pProp;

		//为属性列表压入一个值
		ScenePropAndEvent->PropertyList.push_back(Prop);
		//获取刚压入的值
		pProp=&(ScenePropAndEvent->PropertyList[i]);
		//分别读入属性名称与属性类型
		input_file>>temp;
		std::string tempString=temp;
		std::basic_string <char> :: size_type Pos1,Pos2,Length;
		
		Pos1=tempString.find(",");//找到第一个逗号的位置
		Pos2=tempString.rfind(",");//找到最后一个逗号的位置
		Length=tempString.length();//字符串的长度

		std::basic_string <char> tempName,tempType,tempValue,tempModifyFlag,tempSeeFlag,tempString1;
		tempName=tempString.substr(0,Pos1);
		tempSeeFlag=tempString.substr(Pos2+1,Length-Pos2-1);
		tempString1=tempString.substr(Pos1+1,Pos2-Pos1-1);
		tempString=tempString1;
		Pos1=tempString.find(",");
		Pos2=tempString.rfind(",");
		Length=tempString.length();
		tempType=tempString.substr(0,Pos1);
		tempModifyFlag=tempString.substr(Pos2+1,Length-Pos2-1);
		tempValue=tempString.substr(Pos1+1,Pos2-Pos1-1);
		if(tempValue=="无")
			tempValue="";
		
		strcpy_s(pProp->PropertyName,256,tempName.c_str());
		strcpy_s(pProp->TypeName,256,tempType.c_str());
		strcpy_s(pProp->CanbeChanged,8,tempModifyFlag.c_str());
		strcpy_s(pProp->CanbeSeen,8,tempSeeFlag.c_str());
		strcpy_s(pProp->value,256,tempValue.c_str());
		
		/*
		CString tempString=temp;

		int Pos1=tempString.Find(',');//找到第一个逗号的位置
		int Pos2=tempString.ReverseFind(',');//找到第二个逗号的位置

		CString tempName,tempType,tempValue,tempModifyFlag,tempSeeFlag;
		tempName=tempString.Left(Pos1);
		tempSeeFlag=tempString.Right(tempString.GetLength()-Pos2-1);
		tempString=tempString.Mid(Pos1+1,Pos2-Pos1-1);
		Pos1=tempString.Find(',');
		Pos2=tempString.ReverseFind(',');
		tempType=tempString.Left(Pos1);
		tempModifyFlag=tempString.Right(tempString.GetLength()-Pos2-1);
		tempValue=tempString.Mid(Pos1+1,Pos2-Pos1-1);
		if(tempValue=="无")
			tempValue.Empty();

		strcpy(pProp->PropertyName,tempName.GetBuffer(tempName.GetLength()));
		strcpy(pProp->TypeName,tempType.GetBuffer(tempType.GetLength()));
		strcpy(pProp->CanbeChanged,tempModifyFlag.GetBuffer(tempModifyFlag.GetLength()));
		strcpy(pProp->CanbeSeen,tempSeeFlag.GetBuffer(tempSeeFlag.GetLength()));
		strcpy(pProp->value,tempValue.GetBuffer(tempValue.GetLength()));
		*/
	}

	//读入事件的个数
	input_file>>ScenePropAndEvent->NumberOfEvent;
	//读取事件列表
	for(i=0; i<ScenePropAndEvent->NumberOfEvent; i++)
	{
		CEventDescriptor Event,*pEvent;

		ScenePropAndEvent->EventList.push_back(Event);

		pEvent=&(ScenePropAndEvent->EventList[i]);
		
		char temp[5]={0};

		input_file>>pEvent->ReturnType;
		input_file>>pEvent->EventName;
		input_file.getline(temp,4);
		input_file.getline(pEvent->ParameterList,255);
	}

	//关闭文件
	input_file.close();

	//返回成功
	return TRUE;
}

//*******************************************
//返回场景的版本号
std::string CObjectInScene::ReturnSceneVersion()
{
	std::string Version("");
	std::basic_string <char> tempVersion;

	if(AddSceneSucceedFlag==TRUE)
	{
		Version=ScenePropAndEvent->Version;
		tempVersion=Version.substr(8,6);
	}

	return tempVersion;
}

CObjectStyleDescriptor CObjectInScene::ReturnObjectStyleDescriptor( int index )
{
	CObjectStyleDescriptor temp;
	if((index>=0) && (index<NumberOfStyle))
	{
		return StyleDescriptorList[index];
	}
	return temp;
}

CScenePropertyAndEvent* CObjectInScene::ReturnSceneDescriptor()
{
	return ScenePropAndEvent;
}

CPropertyAndEvent &CObjectInScene::ReturnPropertyAndEvent( int index )
{
	CPropertyAndEvent temp;
	if((index>=0) && (index<NumberOfStyle))
	{
		return PropertyAndEventList[index];
	}
	return temp;
}

BOOL CObjectInScene::ReturnSceneCanbeUseFlag()
{
	return AddSceneSucceedFlag;
}

int CObjectInScene::ReturnNumberOfObjectStyle()
{
	return NumberOfStyle;
}

//bool CObjectInScene::CheckS()
//{
//	CString CurrentPath = _T("") ;
//	GetModuleFileName(NULL,CurrentPath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);   
//	CurrentPath.ReleaseBuffer();   
//	CurrentPath.MakeLower();
//	int nPos = 0;   
//	nPos     = CurrentPath.ReverseFind('\\');   
//	CurrentPath  = CurrentPath.Left(nPos);
//	HMODULE hIns = LoadLibrary(CurrentPath+_T("\\Licence.dll"));
//	if (NULL!=hIns)
//	{
//		RETURNSTATE *pFunction=NULL;		
//		pFunction=(RETURNSTATE *)::GetProcAddress(hIns, 
//			"?ReturnState@@YA?AUretstruct@@U1@0@Z");
//		if(NULL!=pFunction)
//		{
//			retstruct rs;
//			rs.para1 = NULL;
//			rs.para2 = NULL;
//			rs.para3 = NULL;
//			rs.para4 = NULL;
//			rs.para5 = NULL;
//			// 						CLicenceDlg dlg;
//			// 						CString s;
//			// 					 	INT_PTR ret = dlg.DoModal();
//			// 					 	switch(ret)
//			// 					 	{
//			// 					 	case IDOK:
//			// 					 		{
//			// 								s = dlg.ReturnS();
//			// 					 		}
//			// 					 		break;
//			// 					 	case IDCANCEL:
//			// 					 		{					 
//			// 					 		}
//			// 					 		break;
//			// 					 	default:
//			// 					 		{	
//			// 								return 0;
//			// 					 		}
//			// 					 	}		
//			rs = (*pFunction)(rs, rs);
//			FreeLibrary(hIns);
//
//			if (rs.para3)
//			{
//				if (NULL==rs.para1)
//				{
//					return 0;
//				} 
//				else
//				{
//					return 1;
//				}
//			}
//			else
//			{
//				if (NULL==rs.para1)
//				{
//				} 
//				else
//				{
//				}
//				return 0;
//			}
//		}
//	}
//	return 0;
//}
