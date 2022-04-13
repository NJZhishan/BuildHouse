#pragma once
#include<windows.h>
#include "flyFile_w.h"

enum ContentType {
	Content_JPEG = 1,
	Content_BMP = 2,
	Content_GIF = 3,
	Content_PNG = 4,
	Content_Targa = 5,
	Content_3DStudio = 7,
	Content_AVI = 9,
	Content_DDS = 11,
	Content_Unknown = -1,
};

class __declspec(dllexport) CPicture
{
public:
	//fmt:ͼ�����ظ�ʽ
	//w:ͼ�����ؿ�
	//h:ͼ�����ظ�
	//mips:ͼ��ͼ��(Ĭ��Ϊ1��)
	CPicture(int fmt, int w, int h, int mips = 1);
	virtual ~CPicture();

	int GetHeight() const { return m_height; };
	int GetWidth() const { return m_width; };
	int GetMipLevelCount() const { return m_mipLevels; };
	int GetFormat() const { return m_format; };
	int GetComponents() const { return m_components; };
	unsigned char* GetPixels() const { return m_pPixels; };
	unsigned char* GetPixelRow(int row);
	unsigned char* GetPixelRow(int mip, int row);
	bool GetPixelPixel(int row, int col, unsigned char& r, unsigned char& g, unsigned char& b);
	unsigned char* GetMipLevel(int mip);
	int getSize() const { return m_size; };
	int getMipLevelSize(int mip) const;

	bool IsCompressed() const;
	bool hasAlpha() const;
	bool LoadPictureData(BYTE* pBuffer, int nSize);
	enum {
		ColorChannel = 1,
		AlphaChannel = 2
	};

	void CheckSize(int type, COLORREF color);

private:
	unsigned char* m_pPixels;
	int m_mipLevels; //ͼ��ķֱ��ʵĲ���
	int m_components;//ÿ������ռ�����ֽ�
	int m_format;//ͼ���ļ���ʽ
	int m_size;
	int m_height;
	int m_width;
};
//�������ļ���alphaֵ����CPicture����
__declspec(dllexport) CPicture* CreateImageFromZipFileWithAlpha(const char filename[], flyFile_w& fp, float alpha);
__declspec(dllexport) CPicture* LoadImageFromZipFile(const std::string& filename, flyFile_w& fp, bool selfPixel = false);//��zip�л�ȡ�������ݣ�����CPicture����

