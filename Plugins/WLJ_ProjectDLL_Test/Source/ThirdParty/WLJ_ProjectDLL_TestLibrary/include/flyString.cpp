#include "flyString.h"
#include <stdio.h>
#include <stdarg.h>

flyString_w::~flyString_w()
{ delete[] buf; }

flyString_w::flyString_w() : 
	size(FLY_STR_MINSIZE),len(0)
{ 
	buf=new char[size];
	buf[0]=0; 
}

flyString_w::flyString_w(const char* in) :
	len((int)strlen(in))
{
	size=FLY_STR_MINSIZE; 
	while( size<len+1 ) size<<=1;

	buf=new char[size];
	strcpy_s(buf,size,in);
}

flyString_w::flyString_w(const flyString_w& in) :
	len(in.len),size(in.size)
{
	buf=new char[size];
	strcpy_s(buf, size,(const char *)in);
}

flyString_w::flyString_w(const char* in,int p,int n) :
	len(n)
{
	size=FLY_STR_MINSIZE; 
	while( size<len+1 ) size<<=1;

	buf=new char[size];
	memcpy(buf,&in[p],len);
	buf[len]=0;
}

void flyString_w::operator=(const char* in)
{
	len=(int)strlen(in);
	if(size<len+1)
	{
		while( size<len+1 ) size<<=1;

		delete[] buf;
		buf=new char[size];
	}
	strcpy_s(buf,size,in);
}

void flyString_w::copy(const char* in,int p,int n)
{
	len=n;
	if (size<len+1)
	{
		while( size<len+1 ) size<<=1;

		char* temp=new char[size];
		strncpy_s(temp,size,&in[p],len);
		delete[] buf;
		buf=temp;
	}
	else
		strncpy_s(buf,size,&in[p],len);
	buf[len]=0;
}

void flyString_w::format(const char *fmt, ...)
{
	static char ach[2048]={0};
	
	va_list va;
	va_start( va, fmt );
	vsprintf_s( ach, fmt, va );
	va_end( va );

	operator=(ach);
}

flyString_w flyString_w::left(int n) const
{ 
	return flyString_w(buf,0,n); 
}

flyString_w flyString_w::right(int n) const
{ 
	return flyString_w(n<=len?&buf[len-n]:&buf[0]); 
}

flyString_w flyString_w::mid(int p,int n) const
{ 
	return flyString_w(buf,p,n); 
}

void flyString_w::operator=(const flyString_w& in)
{ 
	operator=((const char *)in); 
}

void flyString_w::operator+=(const char* in)
{
	len+=(int)strlen(in);
	if(size<len+1)
	{
		while( size<len+1 ) size<<=1;

		char* temp=new char[size];
		strcpy_s(temp,size,buf);
		strcat_s(temp,size,in);
		delete[] buf;
		buf=temp;
	}
	else
		strcat_s(buf,size,in);
}

flyString_w flyString_w ::operator+(const flyString_w& in)
{ 
	return operator+((const char *)in); 
}

void flyString_w::operator+=(const flyString_w& in)
{ 
	operator+=((const char *)in); 
}

flyString_w flyString_w::operator+(const char* in)
{
	flyString_w str(buf);
	str+=in;
	return str;
}

//获得字符串
void flyString_w::GetBuffer(std::string &str)
{
	str = buf;
}

///获得字符串指针  ///wang 11.3  库外调用
char* flyString_w::returnBuffer()
{
	return buf;
}
