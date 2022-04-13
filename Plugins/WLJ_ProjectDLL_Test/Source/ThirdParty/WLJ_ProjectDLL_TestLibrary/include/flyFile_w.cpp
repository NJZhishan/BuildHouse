#include "flyFile_w.h"
#include "unzip.h"
#include <stdlib.h>
#include <algorithm>

//#include "../projHead/JudgeInput.h"
//#include <mmsystem.h>

flyFile_w::flyFile_w() :
	buf(0),pos(0), len(0), ziplist(0)
{
}

flyFile_w::~flyFile_w()
{
	//delete table;
	delete[] buf;
	delete[] ziplist;
}

//为该flyFile_w确定其所绑定的zip文件，同时生成与此对应的ziplist
//为该flyFile_w确定其所绑定的zip文件，同时生成与此对应的ziplist
bool flyFile_w::SetZipList(const char* filename)
{
	std::string tempF(filename);
	transform(tempF.begin(), tempF.end(), tempF.begin(),tolower);
	std::replace(tempF.begin(), tempF.end(), '/', '\\');
	flyString_w str, str2;
	std::string strTemp = tempF;
	str = strTemp.c_str();
	int p = -1;
	//下面确认是压缩格式
	p = str.find(".txt");
	if (p == -1)
		p = str.find(".hom");

	if (p == -1)
		p = str.find(".shp");

	if (p != -1)
	{//如果是压缩文件
		if (ziplist)
		{//如果已经捆绑了，清除
			delete ziplist;
			ziplist = 0;
		}
		//给RowFileName赋值
		int pos1 = str.find_reverse('.');
		int pos2 = str.find_reverse('\\');;
		RowFileName = str.mid(pos2 + 1, pos1 - pos2 - 1);

		//创建新的ziplist对象，将压缩包中的所有文件名提出
		ziplist = new flyStringArray_w;
		ziplist->add(str);// zip文件全路径压入，作为第一个元素

		unzFile file;
		unz_file_info info;
		flyString_w name;
		int ok;
		file = unzOpen(str);
		if (file == NULL)///如果无法打开该压缩文件  wang 10.29
		{
			return false;
		}
		ok = unzGoToFirstFile(file);
		while (ok == UNZ_OK)
		{
			char tmp[255] = { 0 };
			unzGetCurrentFileInfo(file, &info, tmp, 255, 0, 0, 0, 0);
			name = tmp;
			while ((p = name.find('/')) != -1) name.set_char(p, '\\');
			name.lower();
			ziplist->add(name);	//zip文件里所有文件以相对路径方式(相对与zip文件目录)压入
			ok = unzGoToNextFile(file);
		}
		unzClose(file);
		//如果压缩包中是否有flt文件，将文件名赋给RowFileName
		bool flag = false;
		for (int i = 0; i < ziplist->num; i++)
		{
			if (-1 != (*ziplist)[i].find(".flt"))
			{
				//这里逻辑好像有问题？
				//jiang gai 2014.9.1 原来是返回文件名，不包含后缀，现在返回全文件名，包含后缀
				RowFileName = (*ziplist)[i];
				//RowFileName = (*ziplist)[i].left((*ziplist)[i].find(".flt"));
				flag = true;
				if (-1 != (*ziplist)[i].find("关.flt"))//如果是门的话，找到关门的模型直接返回
				{
					//如果当前是关门的模型，直接返回
					break;
				}
			}
			//jiang gai 2015.12.14 
			//if (flag)
			//   break; //找到第一个flt就跳出
			//jiang 2014.9.1 如果压缩包中没有flt文件，寻找是否有“.jia”文件
			if (-1 != (*ziplist)[i].find(".jia"))
			{
				//jiang gai 2014.9.1 原来是返回文件名，不包含后缀，现在返回全文件名，包含后缀
				RowFileName = (*ziplist)[i];
				//RowFileName = (*ziplist)[i].left((*ziplist)[i].find(".flt"));
				flag = true;
				if (-1 != (*ziplist)[i].find("关.jia"))
				{
					break;
				}
			}
			//jiang gai 2015.12.14 
			//if (flag)
			//   break; //找到第一个jia就跳出
			//jiang gai 2015.9.24 增加对zyj的支持
			//jiang 2014.9.1 如果压缩包中没有flt文件，寻找是否有“.jia”文件
			if (-1 != (*ziplist)[i].find(".zyj"))
			{
				//jiang gai 2014.9.1 原来是返回文件名，不包含后缀，现在返回全文件名，包含后缀
				RowFileName = (*ziplist)[i];
				flag = true;
				if (-1 != (*ziplist)[i].find("关.zyj"))
				{
					break;
				}
			}
			//wqq 2016.8.22 如果压缩包中没有flt文件，寻找是否有“.thd”文件
			if (-1 != (*ziplist)[i].find(".thd"))
			{
				RowFileName = (*ziplist)[i];
				flag = true;
				if (-1 != (*ziplist)[i].find("关.thd"))
				{
					break;
				}
			}
			//jiang gai 2015.12.14 
			//if (flag)
			//   break; //找到第一个zyj就跳出
			//jiang 2015.5.26 如果压缩包中没有flt，jia文件，寻找是否有“.mod”文件
			if (-1 != (*ziplist)[i].find(".mod"))
			{
				//jiang gai 2014.9.1 原来是返回文件名，不包含后缀，现在返回全文件名，包含后缀
				RowFileName = (*ziplist)[i];
				flag = true;
				break;
			}
			//jiang gai 2015.9.24 如果压缩包中没有flt，jia文件，寻找是否有“.sdk”文件
			if (-1 != (*ziplist)[i].find(".sdk"))
			{
				//jiang gai 2014.9.1 原来是返回文件名，不包含后缀，现在返回全文件名，包含后缀
				RowFileName = (*ziplist)[i];
				flag = true;
				break;
			}

		}
		//如果不是模型，就是贴图，找到第一tex.bmp文件赋给RowFileName
		if (!flag)
		{
			for (int i = 0; i < ziplist->num; i++)
			{
				if (-1 != (*ziplist)[i].find("tex.bmp"))
				{
					RowFileName = (*ziplist)[i]/*.left((*ziplist)[i].find("tex.bmp"))*/;
					flag = true;
					break;  //jiang gai 2013.4.27 找到第一tex.bmp文件赋给RowFileName就出来了
				}
			}
		}

		//下面的代码意义不大
		if (!flag)
		{
			for (int i = 0; i < ziplist->num; i++)
			{
				if (-1 != (*ziplist)[i].find("In.bmp"))
				{
					RowFileName = (*ziplist)[i]/*.left((*ziplist)[i].find("In.bmp"))*/;
					flag = true;
					break;///wql 2018.4.11 找到后跳出
				}
			}
		}
		if (!flag)
		{
			for (int i = 0; i < ziplist->num; i++)
			{
				if (-1 != (*ziplist)[i].find("Out.bmp"))
				{
					RowFileName = (*ziplist)[i]/*.left((*ziplist)[i].find("Out.bmp"))*/;
					flag = true;
					break;///wql 2018.4.11 找到后跳出
				}
			}
		}
		if (!flag)
		{
			for (int i = 0; i < ziplist->num; i++)
			{
				if (-1 != (*ziplist)[i].find(".bmp") && (*ziplist)[i].find("bmpjointself") < 0) //wqq 2020.1.15 将个性拼图排除 //jiang gai 2015.2.9 暂时不改，只支持bmp
				{
					RowFileName = (*ziplist)[i]/*.left((*ziplist)[i].find(".bmp"))*/;
					flag = true;
					break;///wql 2018.4.11 找到后跳出
				}
			}
		}
		if (!flag)
		{//wqq 2017.9.8 获得名字是txt的文件，因为庭好的版本的压缩包中没有在产品列表中显示的图片，都是txt文件
			for (int i = 0; i < ziplist->num; i++)
			{
				if (-1 != (*ziplist)[i].find(".txt"))
				{
					RowFileName = (*ziplist)[i];
					if (ziplist->num > 2)
					{
						int npos1, npos2;
						npos1 = RowFileName.find("相对");
						npos2 = RowFileName.find("相邻");
						if (npos1 > -1)
							RowFileName = RowFileName.substr(0, npos1);
						else if (npos2 > -1)
							RowFileName = RowFileName.substr(0, npos2);
						if (-1 != npos1 && -1 != npos2)///wql 2018.8.30 避免有重复的后缀
						{
							RowFileName = RowFileName + ".txt";
						}
					}
					flag = true;
				}
			}
		}
		///wql 2019.1.4 对jpg的判断放在最后面，只有其它类型的模型都不对应才判断jpg
		//jiang gai 2015.2.9 增加对jpg的支持
		//如果不是模型，就是贴图，找到第一tex.jpg文件赋给RowFileName
		if (!flag)
		{
			for (int i = 0; i < ziplist->num; i++)
			{
				if (-1 != (*ziplist)[i].find(".jpg"))   //wqq 2019.12.3 修改为使用.jpg
				{
					RowFileName = (*ziplist)[i];
					flag = true;
					break;  //jiang gai 2015.2.9 找到第一tex.jpg文件赋给RowFileName就出来了
				}
			}
		}
		//压缩文件，返回
		return true;
	}
	else
	{//不是压缩文件，返回
		return false;
	}
}

//由zip文件包中读取一个文件数据，放入buf中
bool flyFile_w::open(const char* filename)
{
	//请空buf
	close();

	if (ziplist == 0)
	{
		return false;
	}

	flyString_w str;
	int p, j;
	if (filename != NULL)
	{
		std::string tempF(filename);
		transform(tempF.begin(), tempF.end(), tempF.begin(), tolower);
		std::replace(tempF.begin(), tempF.end(), '/', '\\');
		std::string strTemp = tempF;

		str = strTemp.c_str();
		//从zip的第二个文件起，检索欲读取的数据文件
		for (j = 1; j < ziplist->num; j++)
			if ((*ziplist)[j].compare_nocase(str) == 0)
				break;
		//如果没有找到，直接返回
		if (j == ziplist->num)
			return false;
	}
	else
	{
		str = (*ziplist)[1];
	}

	//下面是实现由zip文件中读取数据的功能
	unzFile zipfile = unzOpen((*ziplist)[0]);//由zip文件全名获取压缩文件句柄

	while ((p = str.find('\\')) != -1)
		str.set_char(p, '/');//将windows下路径号恢复为unix下路径号（zip认）
	if (unzLocateFile(zipfile, (char*)(const char*)str, 0) != UNZ_OK)//在zip中定位该文件
	{
		unzClose(zipfile);
		return false;
	}

	unz_file_info pfile_info;//获得该文件对应的信息头
	unzGetCurrentFileInfo(zipfile, &pfile_info, 0, 0, 0, 0, 0, 0);
	len = pfile_info.uncompressed_size;

	unzOpenCurrentFile(zipfile);
	buf = new unsigned char[len + 1];//sll 2020.01.09 Bugfix 627xsn
	unzReadCurrentFile(zipfile, buf, len);//将当前定位文件的压缩数据解压并复制到buf中
	unzCloseCurrentFile(zipfile);
	unzClose(zipfile);
	buf[len] = '\0';
	pos = 0;
	return true;
}

//释放当前文件的数据缓存
void flyFile_w::close()
{
	if (buf)
	{//wqq 2016.10.27 指针不为空才能删除
		delete[] buf;
		buf = 0;
	}
	pos = 0;
	len = 0;
}

//从当前文件的数据缓存中读取size长度的数据
int flyFile_w::read(void* dest, int size)
{
	if (pos + size >= len) size = len - pos;
	memcpy(dest, &buf[pos], size);
	pos += size;
	return size;
}

//从当前文件的数据缓存中当前位置开始读取一行文本
int flyFile_w::get_string(flyString_w& dest)
{
	int i = pos;
	while (i < len && buf[i] != '\r' && buf[i] != '\n')
		i++;
	int size = i - pos;
	if (0 == size && i < len - 1)
	{///wql 2018.12.18 如果为0并且没结束，可能文本中间有空行，需要排除空行继续读取下一行
		pos = i + 1;///wql 2018.12.18 要往前移一个字符，否则会陷入死循环
		return get_string(dest);
	}
	dest.copy((char*)buf, pos, i - pos);

	pos = i;
	if (pos < len && buf[pos] == '\r')
		pos++;
	if (pos < len && buf[pos] == '\n')
		pos++;
	//jiang gai 2016.7.13 挺好版本，解密
	return size;
}

int flyFile_w::get_profile_string(const char* section, const char* key, flyString_w& dest)
{
	/*int i,j,k;

	static int last=0;
	if(table==0)
	{
		build_table();
		last=0;
	}

	k=strlen(key);

	for(i=0;i<table->num;i++)
	{
		j=(last+i)%table->num;
		if(stricmp((*table)[j][0],section)==0)
			break;
	}

	if (i<table->num)
	{
		i=j;
		for(j=1;j<(*table)[i].num;j++)
			if (strchr((*table)[i][j],'=')-(*table)[i][j]==k &&
				strnicmp((*table)[i][j],key,k)==0)
				{
					dest=(*table)[i][j].right((*table)[i][j].length()-k-1);
					last=i;
					return 1;
				}
	}

	last=0;
	dest="";*/
	return 0;
}

void flyFile_w::build_table()
{
	/*delete table;
	table=new flyStringArray2;

	char *c1,*c2;
	flyString_w s;
	flyArray_w<flyString_w> section;

	c1=(char *)buf;
	while((c1-(char *)buf)<len)
	{
		if(*c1==10)
			c1++;
		if ((c1-(char *)buf)>=len)
			break;

		c2=strchr(c1,13);
		if(c2==0)
			c2=&c1[strlen(c1)];
		*c2=0;

		if(*c1=='[' && *(c2-1)==']')
		{
			if(section.num>0)
				table->add(section);

			section.clear();
			section.add(flyString_w(c1,1,c2-c1-2));
		}
		else
			if(strchr(c1,'='))
				section.add(flyString_w(c1,0,c2-c1));

		c1=c2+1;
	}

	if(section.num>0)
		table->add(section);*/
}

void flyFile_w::build_ziplist(flyString_w path)
{
	/*delete ziplist;
	ziplist=new flyStringArray2;
	build_ziplist_rec(path);*/
}

void flyFile_w::build_ziplist_rec(flyString_w path)
{
	/*flyString_w str,str2;
	int p;

	while((p=path.find('/'))!=-1) path.set_char(p,'\\');

	str2=path;
	str2+="*";

	HANDLE hFindFile;
	WIN32_FIND_DATA FindFileData;
	hFindFile=FindFirstFile(str2, &FindFileData);
	if(hFindFile!=INVALID_HANDLE_VALUE)
	do
	{
		flyStringArray a;
		str=path;
		str2=FindFileData.cFileName;

		while((p=str2.find('/'))!=-1) str2.set_char(p,'\\');

		if(str2.compare(".")==0 || str2.compare("..")==0)
			continue;

		if(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			str2+="\\";
			str+=str2;
			build_ziplist_rec(str);
			continue;
		}

		str2.lower();
		int p=str2.find(".zip");
		if (p!=-1)
		{
			str+=str2;
			a.add(str);

			unzFile file;
			unz_file_info info;
			flyString_w name;
			int ok;

			file=unzOpen(str);
			ok=unzGoToFirstFile(file);
			while(ok==UNZ_OK)
			{
				char tmp[255];
				unzGetCurrentFileInfo(file,&info,tmp,255,0,0,0,0);

				name=tmp;
				while((p=name.find('/'))!=-1) name.set_char(p,'\\');

				a.add(name);

				ok=unzGoToNextFile(file);
			}
			unzClose(file);

			ziplist->add(a);
		}

	} while(FindNextFile(hFindFile, &FindFileData));*/
}

//从当前文件（文本）的数据缓存中读取一个浮点数
float flyFile_w::get_float()
{
	while (pos < len && !((buf[pos] >= 48 && buf[pos] <= 57) || buf[pos] == '.' || buf[pos] == '-'))
		pos++;

	if (pos == len) return 0;

	int i = pos;
	do i++;
	while (i < len && ((buf[i] >= 48 && buf[i] <= 57) || buf[i] == '.'));//'-'只在第一位，所以下面就不需要负号了

	flyString_w str((char*)buf, pos, i - pos);
	pos = i;

	return (float)atof(str);
}

//从当前文件（文本）的数据缓存中读取一个整数
int flyFile_w::get_int()
{
	while (pos < len && !((buf[pos] >= 48 && buf[pos] <= 57) || buf[pos] == '-'))
		pos++;

	if (pos == len) return 0;

	int i = pos;
	do i++;
	while (i < len && buf[i] >= 48 && buf[i] <= 57);

	flyString_w str((char*)buf, pos, i - pos);
	pos = i;

	return atoi(str);
}

void flyFile_w::seek(int offset)
{
	pos = pos + offset;
	if (pos < 0)
		pos = 0;
	if (pos >= len)
		pos = len - 1;
}

void flyFile_w::unzip(const char* localfile)
{
	/*FILE *fp;
	unzFile file;
	unz_file_info info;
	static char str[512],name[512];
	int ok;

	file=unzOpen(localfile);
	ok=unzGoToFirstFile(file);
	while(ok==UNZ_OK)
	{
		unzGetCurrentFileInfo(file,&info,name,255,0,0,0,0);

		strlwr(name);
		if (!strncmp(name,"data/",5))
		{
			strcpy(str,g_flyengine->flysdkdatapath);
			strcat(str,&name[5]);
		}
		else
		if (!strncmp(name,"plugin/",7))
		{
			strcpy(str,g_flyengine->flysdkpluginpath);
			strcat(str,&name[7]);
		}
		else
			continue;

		char *c1=str,*c2;
		while( c1 )
		{
			c2=strchr(c1,'/');
			if (c2==0)
				c2=strchr(c1,'\\');
			if (c2)
			{
				*c2=0;
				_mkdir(str);
				*c2='\\';
				c2++;
			}
			c1=c2;
		};

		fp=fopen(str,"wb");
		if (fp)
			{
			char *data=new char[info.uncompressed_size];
			unzOpenCurrentFile(file);
			unzReadCurrentFile(file,data,info.uncompressed_size);
			unzCloseCurrentFile(file);
			fwrite(data,1,info.uncompressed_size,fp);
			delete[] data;
			fclose(fp);
			}

		ok=unzGoToNextFile(file);
	}
	unzClose(file);*/
}

//******************************************
//修改属性name的值为value
//若找不到属性name,返回-1
//若输入格式不正确,返回-2
//否则,返回0
//该函数可被适当的进行重载以对一些特殊的输入格式进行判定
/*
int flyFile_w::SetProperty(char name[],char value[])
{
	//找到该属性
	for(unsigned short i=0; i<m_PropEvent.NumberOfProperty; i++)
	{
		if(strcmp(m_PropEvent.PropertyList[i].PropertyName,name)==0)
		{
			//判断传递的属性值是否为正确的格式
			CJudgeInput judge;
			//如果不是,则返回-2
			//!!此处只能判断一些基本的输入格式,包括整型,浮点型,字符串型以及布尔型!!
			//!!若要判断其它的输入格式,请重载SetProperty(...)函数!!
			BOOL Result;
			if(_stricmp(name,"名称")==0)
			{
				if((Result=judge.JudgeInputName(value))==FALSE)
					return -2;
			}
			else
			{
				if((Result=judge.JudgeInput(m_PropEvent.PropertyList[i].TypeName,value))==FALSE)
					return -2;
				if(strcmp("URL",name)==0)
				{///wang 09.12.11 由于网站域名更改，而以前的模型文档里记录了完整的下载连接，所以在填充URL时只留相对路径，在主程序中将路径补齐
					std::string val(value);
					int pos = val.find("/model");
					if(pos!=-1)
					{
						val = val.substr(pos);
						strcpy_s(value,256,val.c_str());
					}
				}
			}
			//设置该属性值并返回0
			strcpy_s(m_PropEvent.PropertyList[i].value,value);
			return 0;
		}
	}
	//到此处,找不到属性name[],返回-1
	return -1;
}
*/

//******************************************
//获得属性Name的值(字符串表示的值)
//成功获取,返回TRUE,否则,返回FALSE
//!!字符串value所需空间在该函数外面分配!!    
/*
bool flyFile_w::GetProperty(char name[],char value[])
{
	//找到该属性
	for(unsigned short i=0; i<m_PropEvent.NumberOfProperty; i++)
	{
		if(strcmp(m_PropEvent.PropertyList[i].PropertyName,name)==0)
		{
			strcpy_s(value,256,m_PropEvent.PropertyList[i].value);
			return TRUE;
		}
	}
	return FALSE;
}*/

//******************************************
//获得属性Name的值(实际的值)
//成功获取,返回TRUE,否则,返回FALSE
//!!指针pvalue所需空间在该函数外面分配!! 
/*
bool flyFile_w::GetPropertyRealValue(char name[],char value_type[],void *pvalue)
{
	//循环所有的属性
	for(unsigned short i=0; i<m_PropEvent.NumberOfProperty; i++)
	{
		//判断是否有属性的名字为name
		if(strcmp(m_PropEvent.PropertyList[i].PropertyName,name)==0)
		{
			//如果有
			//判断该类型是否为所传递的指针pvalue的类型
			if(strcmp(m_PropEvent.PropertyList[i].TypeName,value_type)==0)
			{
				//如果是
				//则对不同的类型分别处理
				if(strcmp(value_type,"integer")==0)//整型
				{
					int_value=atoi(m_PropEvent.PropertyList[i].value);
					*((int *)pvalue)=int_value;
				}
				else if(strcmp(value_type,"float")==0)
				{
					float_value=(float)atof(m_PropEvent.PropertyList[i].value);
					*((float *)pvalue)=float_value;
				}
				else if(strcmp(value_type,"charcluster")==0)
				{
					strcpy_s((char*)pvalue,256,m_PropEvent.PropertyList[i].value);
				}
				else if(strcmp(value_type,"boolean")==0)
				{
					if(strcmp(m_PropEvent.PropertyList[i].value,"真")==0)
						bool_value=TRUE;
					else if(strcmp(m_PropEvent.PropertyList[i].value,"假")==0)
						bool_value=FALSE;
					else
						return FALSE;
					*((BOOL *)pvalue)=bool_value;
				}
				else
					return FALSE;

				return TRUE;
			}
			else
			{
				//否则返回FALSE
				return FALSE;
			}
		}
	}
	return FALSE;
}
*/

bool flyFile_w::Write(const char* filename)
{
	char* temp = (char*)filename;
	char* i = 0, c = '/';
	while ((i = (char*)strchr(temp, c)) != 0)
		temp[(int)(i - temp)] = '\\';

	FILE* fp;
	if (fopen_s(&fp, temp, "wb") != 0) return 0;
	///FILE *fp = fopen(temp,"wb");
	int nn = sizeof(buf);
	///fprintf(fp,(const char*)buf);
	if (fp != NULL && buf != NULL)
	{
		fwrite(buf, sizeof(unsigned char), len, fp);
		///fprintf(fp,"%s",buf);
	}
	fclose(fp);
	return 0;
}



/***********************************************
作者： 王庆利   创建时间 ： 2009.11.10
功能描述：判断是否有所给后缀的文件，若有则将该文件写入指定路径
输入参数：path:指定相对路径，写完后返回完整路径，extra：要写入的文件的后缀
输出参数：无
返回参数说明：有并保存成功返回true， 反之false
************************************************/
bool flyFile_w::FindAndWriteFile(const char* path, void* file,const char* extra/* = ".xml"*/)
{
	//_strlwr_s((char*)extra, std::string(extra).size() + 1);
	bool flag = false;
	//从zip的第二个文件起，检索欲读取的数据文件
	for (int i = 1; i < ziplist->num; i++)
	{
		std::string temp;
		(*ziplist)[i].GetBuffer(temp);
		_strlwr_s((char*)temp.c_str(), temp.size() + 1);
		if (temp.rfind(extra) != -1)
		{
			if ("find" == path)
			{
				if (temp.size() == std::string(extra).size())
				{
					return false;
				}
				return false;
			}
			if ("2d.bmp" == extra)
			{
				//path = temp;
				return false;
			}
			open(temp.c_str());

			std::string pathtemp = path + temp;
			Write(pathtemp.c_str());
			strcpy((char*)file, pathtemp.c_str());
			return true;
		}
	}
	return nullptr;
}

/***********************************************
作者： 王庆利   创建时间 ： 2009.11.10
功能描述：判断是否有所给后缀的文件，若有则将该文件写入指定路径
输入参数：path:指定相对路径，写完后返回完整路径，extra：要写入的文件的后缀
输出参数：无
返回参数说明：有并保存成功返回true， 反之false
************************************************/
bool flyFile_w::FindAndWriteFile(std::string& path, std::string extra)
{
	_strlwr_s((char*)extra.c_str(), extra.size() + 1);
	bool flag = false;
	//从zip的第二个文件起，检索欲读取的数据文件
	for (int i = 1; i < ziplist->num; i++)
	{
		std::string temp;
		(*ziplist)[i].GetBuffer(temp);
		_strlwr_s((char*)temp.c_str(), temp.size() + 1);
		if (temp.rfind(extra) != -1)
		{
			if ("find" == path)
			{
				if (temp.size() == extra.size())
				{
					return true;
				}
				return false;
			}
			if ("2d.bmp" == extra)
			{
				path = temp;
				return true;
			}
			open(temp.c_str());
			std::string pathtemp = path + temp;
			Write(pathtemp.c_str());
			path = pathtemp;
			flag = true;
		}
	}
	return flag;
}