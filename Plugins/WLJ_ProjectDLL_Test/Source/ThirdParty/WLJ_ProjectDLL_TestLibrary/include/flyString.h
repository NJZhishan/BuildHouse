/*! \file */
#pragma once
#include "flyArray.h"
#include <string>
#define FLY_STR_MINSIZE 16	//!< flyString's minimum size in bytes
//#include "main.h"

#ifndef DLLImport
#define DllImport   __declspec(dllimport)
#endif
#ifndef DLLExport
#define DllExport   
#endif

//! Fly3D in-house string class
/*!
	This class implements variable-size strings, and auto-manages its memory space, expanding the buffer 
	whenever necessary. The class supplies methods for constructing a flyString from a char pointer, 
	concatenating, copying and comparing strings, indexing a specific character and finding a sub-string.
*/
class DllExport flyString_w
{
	private:
		char* buf;
		int size;
		int len;

	public:

	//! Default constructor
	flyString_w();

	//! Construct a string from a char pointe
	flyString_w(const char* in);

	//! Copy-constructor
	flyString_w(const flyString_w& in);

	//! Construct a string of length 'n' from a char pointer, beginning at position 'p'
	flyString_w(const char* in,int p,int n);
		
	//! Default destructor
	virtual ~flyString_w();

	//! Type cast to char pointer
	inline operator const char *() const
	{ return buf; }

	//! Indexing operator
	inline char operator[](int i) const
	{ return buf[i]; }

	//! Char pointer atribuition operator
	void operator=(const char* in);

	//! Atribuition operator
	void operator=(const flyString_w& in);

	//! Self-concatenation operator with a char pointer
	void operator+=(const char* in);

	//! Self-concatenation operator
	void operator+=(const flyString_w& in);

	//! Concatenation operator with a char pointer
	flyString_w operator+(const char* in);

	//! Concatenation operator
	flyString_w operator+(const flyString_w& in);

	//! Copy the 'n'-sized contents of a char pointer beginning at position 'p'
	void copy(const char* in,int p,int n);

	//! Find a sub-string in the string
	inline int find(const char *str) const
	{ 
		char *pos=strstr(buf,str);
		if (pos==0)
			return -1;
		return (int)(pos-buf); 
	}

	//! Find the first occurrence of a character in the string
	inline int find(char c) const
	{ 
		char *pos=strchr(buf,c);
		if (pos==0)
			return -1;
		return (int)(pos-buf); 
	}

	//! Find the last occurrence of a character in the string
	inline int find_reverse(char c) const
	{
		char *pos=strrchr(buf,c);
		if (pos==0)
			return -1;
		return (int)(pos-buf); 
	}

	//! Change the 'i'-th character of the string
	inline void set_char(int i,char c)
	{ 
		if (i<len) 
			buf[i]=c;
		if (c==0)
			if (len>i)
				len=i;
	}

	//! Crop the first 'n' characters of the string
	inline void crop_begin(int n)
	{ 
		if (n<len)
		{
			len-=n;
			strcpy_s(buf,len+1,&buf[n]);
		}
	}

	//! Return a string consisting of the first 'n' characters of the original one
	flyString_w left(int n) const;

	//! Return a string consisting of the last 'n' characters of the original one
	flyString_w right(int n) const;

	//! Return a string consisting of the 'n' characters of the original one that follow position 'p', inclusive
	flyString_w mid(int p,int n) const;

	//! Format the string using a format template
	void format(const char *fmt, ...);

	//! equal compare operator
	inline int operator==(const char *str) const 
	{ return strcmp(buf,str)==0; }

	inline int operator!=(const char *str) const 
	{ return strcmp(buf,str)!=0; }

	inline int operator>(const char *str) const 
	{ return strcmp(buf,str)>0; }

	inline int operator<(const char *str) const 
	{ return strcmp(buf,str)<0; }

	inline int operator>=(const char *str) const 
	{ return strcmp(buf,str)>=0; }

	inline int operator<=(const char *str) const 
	{ return strcmp(buf,str)<=0; }

	//! Compare with a char pointer
	inline int compare(const char *str) const
	{ return strcmp(buf,str); }

	//! Compare the first 'n' characters of the string with a char pointer
	inline int compare(const char *str,int n) const
	{ return strncmp(buf,str,n); }

	//! Compare with a char pointer, case-insensitive flavour
	inline int compare_nocase(const char *str) const
	{ return _stricmp(buf,str); }

	//! Change all characters to lower-case
	inline void lower()
	{ _strlwr_s(buf,len+1); }

	//! Change all characters to upper-case
	inline void upper()
	{ _strupr_s(buf, len+1); }

	//! Return the length of the string in bytes
	inline int length() const
	{ return len; }

	void GetBuffer(std::string &str);//»ñµÃ×Ö·û´®
	char* returnBuffer();///wang 11.3
};

typedef flyArray_w<flyString_w> flyStringArray_w;
typedef flyArray_w<flyStringArray_w> flyStringArray2_w;
