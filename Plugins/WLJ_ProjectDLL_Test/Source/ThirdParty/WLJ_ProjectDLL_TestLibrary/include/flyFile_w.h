#pragma once
#include "flyString.h"
#include <string>
class __declspec(dllexport) flyFile_w
{
	void build_ziplist_rec(flyString_w path);

public:
	std::string RowFileName;       //去处相对路径和后缀名的文件名
	unsigned char* buf;		//!< the file data buffer
	int pos,				//!< current position in the file
		len;				//!< length of the file
	//! List of all files included in all zip packs inside the current data folder
	flyStringArray_w* ziplist;
	//! Table with text file itens as an array of array of strings
	//! Each group (named like '[groupname]') has an array of 
	//! parameter  strings (in the form 'param=data')
///flyStringArray2 *table;
//! Default constructor
	flyFile_w();
	//! Default destructor
	virtual ~flyFile_w();
	//! Build the zip list with all packed files 
	void build_ziplist(flyString_w path);
	//! Build a array or array of strings for fast text reads of fly files
	void build_table();
	//! Unzip a zipped file
	void unzip(const char*);
	//! Seek a position in the file
	void seek(int offset);
	//! Open a file by its given file name
	bool open(const char* filename = NULL);
	//! Close the file
	void close();
	//! Read 'size' bytes from the file (beginning at the current position 'pos') into the 'dest' buffer
	int read(void* dest, int size);
	//! Get a string from the profile
	int get_profile_string(const char* section, const char* key, flyString_w& dest);
	//! Get a string from the file
	int get_string(flyString_w& dest);
	//! Get an integer from the file
	int get_int();
	//! Get an floating point from the file
	float get_float();
	bool SetZipList(const char* filename);//为该flyFile_w确定其所绑定的zip文件，同时生成与此对应的ziplist

	//对每个字符串进行处理，将值写入相应的interface属性中
	//void ProcessStr(flyString str,CObjectInterface *pObject);
	//从zip的属性文件中获取该对象的属性表
	//jiang gai 2014.9.1 不需要了，不会从压缩文件中读数据	
	/*
	bool InitPropertyList();
	//与属性有关的函数
	int  SetProperty(char name[],char value[]);             //修改属性name的值为value
	bool GetProperty(char name[],char value[]);             //获得属性name的值
	bool GetPropertyRealValue(char Name[],char value_type[],void *pvalue);//与获得物体属性的实际值有关的函数
	CPropertyAndEvent* ReturnPropAndEvent() {return &m_PropEvent;};
	*/
	bool Write(const char* filename);
private:
	//jiang gai 2014.9.1 CPropertyAndEvent m_PropEvent;                          ////物体属性事件列表,对应于该zip文件所包含的属性文件

	//暂存每种类型的属性值
	//保存的数据为上一次调用GetPropertyRealValue()函数后所得到的值
	union
	{
		int   int_value;
		float float_value;
		bool  bool_value;
	};
	char string_value[256];



public:
	/***********************************************
	作者： 王庆利   创建时间 ： 2009.11.10
	功能描述：判断是否有所给后缀的文件，若有则将该文件写入指定路径
	输入参数：path:指定相对路径，写完后返回完整路径，extra：要写入的文件的后缀
	输出参数：无
	返回参数说明：有并保存成功返回true， 反之false
	************************************************/
	bool FindAndWriteFile(const char* path, void* file,const char* extra= ".xml");
	bool FindAndWriteFile(std::string& path, std::string extra = ".xml");
};

