#ifndef _OBJECTINSCENE_H_
#define _OBJECTINSCENE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>
#include <vector>
#include <string>
#include "Main.h"

//#define WM_LOADNEWOBJ   WM_USER+13///由动态库发送消息，下载本地不存在的模型  wang 10.29

class __declspec(dllexport) CObjectInScene
{
public:
	CObjectInScene(char* configPath);
	virtual ~CObjectInScene();
public:
	BOOL ReturnSceneCanbeUseFlag();;  //返回场景是否可创建信息
	std::string ReturnSceneVersion();                              //返回创建的场景的版本号
	int ReturnNumberOfObjectStyle();;       //返回可用的物体类型数
	CObjectStyleDescriptor ReturnObjectStyleDescriptor(int index);; //返回序号为index物体类型信息  

	CScenePropertyAndEvent* ReturnSceneDescriptor();;

	CPropertyAndEvent &ReturnPropertyAndEvent(int index);;//返回序号为index的物体的属性与事件信息

	//bool CheckS();
private:
	BOOL GetAllObjectStyleFromObjectStyleFile();//从文件中获得所有的可用物体类型
	BOOL GetObjectPropertyAndEvent(int index);  //从文件中获得序号为index的物体类型的属性与事件列表
	BOOL GetScenePropertyAndEvent();            //从文件中获得场景的属性与事件列表

private:
	std::vector<CObjectStyleDescriptor> StyleDescriptorList;    //类型描述器列表,列举可用的物体类型
	std::vector<CPropertyAndEvent> PropertyAndEventList;        //所有类型的属性事件的列表
	int NumberOfStyle;                                          //这个场景成功载入的类型数量

	CScenePropertyAndEvent *ScenePropAndEvent;                   //场景的属性与事件
	BOOL AddSceneSucceedFlag;                                   //加载场景属性与事件成功标志

	std::string m_configFile;   //配置文件的路径
};
#endif //_OBJECTINSCENE_H_
