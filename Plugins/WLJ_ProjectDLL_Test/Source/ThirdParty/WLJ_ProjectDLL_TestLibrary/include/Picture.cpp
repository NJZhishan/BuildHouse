#include "Picture.h"
#include <locale.h>
#include "flyPicture.h"
/* PixelFormat */
#define GL_COLOR_INDEX                    0x1900
#define GL_STENCIL_INDEX                  0x1901
#define GL_DEPTH_COMPONENT                0x1902
#define GL_RED                            0x1903
#define GL_GREEN                          0x1904
#define GL_BLUE                           0x1905
#define GL_ALPHA                          0x1906
#define GL_RGB                            0x1907
#define GL_RGBA                           0x1908
#define GL_LUMINANCE                      0x1909
#define GL_LUMINANCE_ALPHA                0x190A

/* EXT_bgra */
#define GL_BGR_EXT                        0x80E0
#define GL_BGRA_EXT                       0x80E1

static int formatComponents(int fmt)
{
	switch (fmt)
	{
	case GL_RGBA:
	case GL_BGRA_EXT:
		return 4;
	case GL_RGB:
	case GL_BGR_EXT:
		return 3;
	case GL_LUMINANCE_ALPHA:
		/*
		case GL_DSDT_NV:
		*/
		return 2;
	case GL_ALPHA:
	case GL_LUMINANCE:
		return 1;

		/*
		// Compressed formats
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
			return 3;
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			return 4;
		*/

		// Unknown format
	default:
		return 0;
	}
}

static int calcMipLevelSize(int fmt, int w, int h, int mip)
{
	w = max(w >> mip, 1);
	h = max(h >> mip, 1);

	/*
	switch (fmt)
	{
	case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
		// 4x4 blocks, 8 bytes per block
		return ((w + 3) / 4) * ((h + 3) / 4) * 8;
	case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
		// 4x4 blocks, 16 bytes per block
		return ((w + 3) / 4) * ((h + 3) / 4) * 16;
	default:
		return h * w * formatComponents(fmt);
	}
	*/

	return h * w * formatComponents(fmt);
}

CPicture::CPicture(int fmt, int w, int h, int mips) :
	m_width(w),
	m_height(h),
	m_mipLevels(mips),
	m_format(fmt),
	m_pPixels(NULL)
{
	m_components = formatComponents(fmt);
	//assert(m_components != 0);

	m_size = 0;
	for (int i = 0; i < m_mipLevels; i++)
		m_size += calcMipLevelSize(fmt, w, h, i);
	m_pPixels = new unsigned char[m_size];
}

CPicture::~CPicture()
{
	if (m_pPixels != NULL)
	{
		delete[] m_pPixels;
		m_pPixels = NULL;
	}
}

//*****************************************************
//获取第row行的图象数据的起始位置
unsigned char* CPicture::GetPixelRow(int mip, int row)
{
	int w = max(m_width >> mip, 1);
	int h = max(m_height >> mip, 1);
	if (mip >= m_mipLevels || row >= h)
		return NULL;

	//不允许行压缩
	if (IsCompressed())
		return NULL;

	return GetMipLevel(mip) + row * w * m_components;
}

//*****************************************************
//获取第row行的图象数据的起始位置(针对只有1层的图象)
unsigned char* CPicture::GetPixelRow(int row)
{
	return GetPixelRow(0, row);
}

//*****************************************************
//获取第row行第col列的图象的数据(针对只有1层的图象)
bool CPicture::GetPixelPixel(int row, int col, unsigned char& r, unsigned char& g, unsigned char& b)
{
	unsigned char* rowStart = GetPixelRow(row);
	if (rowStart == NULL)
		return false;

	int fmt = GetFormat();

	//根据不同的像素格式创建纹理对象
	switch (fmt)
	{
	case GL_RGBA:
	{
		r = *(rowStart + col * 4 + 1);
		g = *(rowStart + col * 4 + 2);
		b = *(rowStart + col * 4 + 3);
	}
	return true;
	case GL_BGRA_EXT:
	{
		r = *(rowStart + col * 4 + 3);
		g = *(rowStart + col * 4 + 2);
		b = *(rowStart + col * 4 + 1);
	}
	return true;
	case GL_RGB:
	{
		r = *(rowStart + col * 3);
		g = *(rowStart + col * 3 + 1);
		b = *(rowStart + col * 3 + 2);
	}
	return true;
	case GL_BGR_EXT:
	{
		r = *(rowStart + col * 3 + 2);
		g = *(rowStart + col * 3 + 1);
		b = *(rowStart + col * 3);
	}
	return true;
	case GL_LUMINANCE_ALPHA:
	{
		r = *(rowStart + col * 2);
		g = r;
		b = r;
	}
	return true;
	case GL_LUMINANCE:
	{
		r = *(rowStart + col);
		g = r;
		b = r;
	}
	return true;
	}

	return false;
}

//*****************************************************
//获得第mip层图象的图象数据起始位置
unsigned char* CPicture::GetMipLevel(int mip)
{
	if (mip >= m_mipLevels)
		return NULL;

	int offset = 0;
	for (int i = 0; i < mip; i++)
		offset += calcMipLevelSize(m_format, m_width, m_height, i);

	return m_pPixels + offset;
}

int CPicture::getMipLevelSize(int mip) const
{
	if (mip >= m_mipLevels)
		return 0;
	else
		return calcMipLevelSize(m_format, m_width, m_height, mip);
}

bool CPicture::IsCompressed() const
{
	/*
	switch(m_format)
	{
	case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
		return true;
	default:
		return false;
	}
	*/
	return false;
}

bool CPicture::hasAlpha() const
{
	switch (m_format)
	{
		/*
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
		*/
	case GL_RGBA:
	case GL_BGRA_EXT:
	case GL_LUMINANCE_ALPHA:
	case GL_ALPHA:
		return true;
	default:
		return false;
	}
}

const std::string JPEGExt(".jpeg");
const std::string JPGExt(".jpg");
const std::string JFIFExt(".jif");
const std::string BMPExt(".bmp");
const std::string TargaExt(".tga");
const std::string PNGExt(".png");
const std::string ThreeDSExt(".3ds");
const std::string AVIExt(".avi");
const std::string DDSExt(".dds");

int compareIgnoringCase(const std::string& s1, const std::string& s2)
{
	std::string::const_iterator i1 = s1.begin();
	std::string::const_iterator i2 = s2.begin();

	while (i1 != s1.end() && i2 != s2.end())
	{
		if (toupper(*i1) != toupper(*i2))
			return (toupper(*i1) < toupper(*i2)) ? -1 : 1;
		++i1;
		++i2;
	}

	return (int)(s2.size() - s1.size());
}

ContentType DetermineFileType(const std::string& filename)
{
	int extPos = (int)filename.rfind('.');
	if (extPos == (int)std::string::npos)
		return Content_Unknown;
	std::string ext = std::string(filename, extPos, filename.length() - extPos + 1);

	if (compareIgnoringCase(JPEGExt, ext) == 0 ||
		compareIgnoringCase(JPGExt, ext) == 0 ||
		compareIgnoringCase(JFIFExt, ext) == 0)
		return Content_JPEG;
	else if (compareIgnoringCase(BMPExt, ext) == 0
		|| compareIgnoringCase(".bck", ext) == 0)
		return Content_BMP;
	else if (compareIgnoringCase(TargaExt, ext) == 0)
		return Content_Targa;
	else if (compareIgnoringCase(PNGExt, ext) == 0)
		return Content_PNG;
	else if (compareIgnoringCase(ThreeDSExt, ext) == 0)
		return Content_3DStudio;
	else if (compareIgnoringCase(AVIExt, ext) == 0)
		return Content_AVI;
	else if (compareIgnoringCase(DDSExt, ext) == 0)
		return Content_DDS;
	else
		return Content_Unknown;
}

//由PNG图象生成CPicture类
CPicture* LoadPNGImage(const std::string& filename)
{
#ifndef PNG_SUPPORT
	return NULL;
#else
	char header[8];
	png_structp png_ptr;
	png_infop info_ptr;
	png_uint_32 width, height;
	int bit_depth, color_type, interlace_type;
	FILE* fp = NULL;
	CPicture* img = NULL;
	png_bytep* row_pointers = NULL;

	fp = fopen(filename.c_str(), "rb");
	if (fp == NULL)
	{
		//clog << "Error opening image file " << filename << '\n';
		return NULL;
	}

	fread(header, 1, sizeof(header), fp);
	if (png_sig_cmp((unsigned char*)header, 0, sizeof(header)))
	{
		//clog << "Error: " << filename << " is not a PNG file.\n";
		fclose(fp);
		return NULL;
	}

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
		NULL, NULL, NULL);
	if (png_ptr == NULL)
	{
		fclose(fp);
		return NULL;
	}

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
		fclose(fp);
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		return NULL;
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		fclose(fp);
		if (img != NULL)
			delete img;
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		//clog << "Error reading PNG image file " << filename << '\n';
		return NULL;
	}

	// png_init_io(png_ptr, fp);
	png_set_read_fn(png_ptr, (void*)fp, PNGReadData);
	png_set_sig_bytes(png_ptr, sizeof(header));

	png_read_info(png_ptr, info_ptr);

	png_get_IHDR(png_ptr, info_ptr,
		&width, &height, &bit_depth,
		&color_type, &interlace_type,
		NULL, NULL);

	GLenum glformat = GL_RGB;
	switch (color_type)
	{
	case PNG_COLOR_TYPE_GRAY:
		glformat = GL_LUMINANCE;
		break;
	case PNG_COLOR_TYPE_GRAY_ALPHA:
		glformat = GL_LUMINANCE_ALPHA;
		break;
	case PNG_COLOR_TYPE_RGB:
		glformat = GL_RGB;
		break;
	case PNG_COLOR_TYPE_PALETTE:
	case PNG_COLOR_TYPE_RGB_ALPHA:
		glformat = GL_RGBA;
		break;
	default:
		// badness
		break;
	}

	img = new CPicture(glformat, width, height);
	if (img == NULL)
	{
		fclose(fp);
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		return NULL;
	}

	// TODO: consider using paletted textures if they're available
	if (color_type == PNG_COLOR_TYPE_PALETTE)
	{
		png_set_palette_to_rgb(png_ptr);
	}

	if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
	{
		png_set_gray_1_2_4_to_8(png_ptr);
	}

	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
	{
		png_set_tRNS_to_alpha(png_ptr);
	}

	// TODO: consider passing images with < 8 bits/component to
	// GL without expanding
	if (bit_depth == 16)
		png_set_strip_16(png_ptr);
	else if (bit_depth < 8)
		png_set_packing(png_ptr);

	row_pointers = new png_bytep[height];
	for (unsigned int i = 0; i < height; i++)
		row_pointers[i] = (png_bytep)img->GetPixelRow(i);

	png_read_image(png_ptr, row_pointers);

	delete[] row_pointers;

	png_read_end(png_ptr, NULL);
	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

	fclose(fp);

	return img;
#endif
}

//由zip中获取纹理数据，创建CPicture对象
CPicture* LoadImageFromZipFile(const std::string& filename1, flyFile_w& fp, bool selfPixel)
{
	// 	std::string realFile(filename1);
		//首先提取出纯的文件名
	int pos = (int)filename1.rfind("\\");
	int pos1 = (int)filename1.rfind('/');
	if (pos < pos1)
	{
		pos = pos1;
	}
	std::string filename = filename1.substr(pos + 1);
	if (NULL != &fp && !fp.FindAndWriteFile(std::string("find"), filename))
	{
		filename = fp.RowFileName/*+"tex.bmp"*/;
	}
	ContentType type = DetermineFileType(filename);//确定图像的格式
	CPicture* img = NULL;
	flyPicture* p = new flyPicture;
	int ret = 0;
	int droplevel = 0;//进行2次幂扩展方式
	int flipy;//纹理数据是否Y方向翻转标记
	int format;//纹理象素的格式
	//clog << "Loading image from file " << filename << '\n';

	TCHAR dir[256];
	GetCurrentDirectory(256, dir);
	//由不同格式的文件中获取纹理数据及其属性，填入flyPicture对象
	switch (type)
	{
	case Content_JPEG:
		flipy = 1;//jpeg图象纹理数据Y方向要翻转
		strcpy_s(p->name, 256, filename.c_str());
		if (fp.open(p->name))//打开压缩包中的文件
			ret = p->LoadJPG(fp.buf, fp.len, flipy);//从fp.buf数据中获得jpg图像原始数据
		break;
	case Content_BMP:
		//img = LoadBMPImage(filename);
		strcpy_s(p->name, 256, filename.c_str());
		if (fp.open(p->name))
			ret = p->LoadBMP(fp.buf, fp.len, 0);
		break;
	case Content_PNG:
		img = LoadPNGImage(filename);
		//ret = 1;
		break;
	case Content_Targa://jiang 8.14
		/*img = LoadTGAImage(filename);
		ret =1;*/
		flipy = 1;//jpeg图象纹理数据Y方向要翻转
		strcpy_s(p->name, 256, filename.c_str());
		if (fp.open(p->name))
			ret = p->LoadTGA(fp.buf, fp.len, flipy);
		break;
	default:
		//clog << filename << ": unrecognized or unsupported image file type.\n";
		break;
	}

	if (ret == 0)
	{
		ret = -1;
		if (p != NULL)
		{
			delete p;
			p = NULL;
		}
		//	MessageBox(NULL,"texture format is pronlem!","LoadImageFromZipFile,eroor",MB_OK);
		return NULL;
	}
	else
	{
		if (!selfPixel)
		{
			p->CheckSize(droplevel);//进行2次幂扩展
		}
	}
	switch (p->bytespixel)
	{
	case 3:
		format = GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		break;
	default:
	{
#ifdef _DEBUG ///wang 12.3
		//MessageBox(NULL, "texture format has problem!", "LoadImageFromZipFile,eroor", MB_OK);
#endif
		if (p != NULL)
		{
			delete p;
			p = NULL;
		}
		return NULL;
	}
	}

	//由flyPicture对象创建CPicture对象
	img = new CPicture(format, p->sx, p->sy);

	//将flyPicture中的数据复制到CPicture中
	int width = img->GetWidth();
	int height = img->GetHeight();
	unsigned char* pdata, * psdata;
	pdata = p->buf;
	int perPixel = p->bytespixel;

	for (int i = 0; i < height; ++i)
	{
		psdata = img->GetPixelRow(i);
		for (int j = 0; j < width * perPixel; ++j)
		{
			psdata[j] = pdata[i * width * perPixel + j];
		}
	}

	if (p != NULL)
	{
		delete p;
		p = NULL;
	}
	return img;
}

//由纹理文件和alpha值创建CPicture对象
CPicture* CreateImageFromZipFileWithAlpha(const char filename[], flyFile_w& fp, float alpha)
{
	CPicture* img, * newImg;
	img = newImg = NULL;

	//载入图象
	setlocale(LC_ALL, "Chinese-simplified");//设置中文环境
	img = LoadImageFromZipFile(filename, fp);
	setlocale(LC_ALL, "C");//还原

	if (img == NULL)
	{
		return newImg;
	}

	//判断原始图像是否为3位或4位格式
	if (img->GetComponents() != 3 &&
		img->GetComponents() != 4)
	{
		delete img;
		return newImg;
	}

	//生成一个新的图像
	//新的图像是RGBA模式,大小与层次与原始图像一致
	newImg = new CPicture(GL_RGBA, img->GetWidth(), img->GetHeight(), img->GetMipLevelCount());
	if (newImg == NULL)
	{
		delete img;
		return newImg;
	}

	int calpha = ((int)(alpha * 255.0f)) % 256;

	//拷贝所有的原始图像数据到新的图像中,每个像素数据为4个字节
	//处理所有的层
// 	FILE *fp1;
// 	if(fopen_s(&fp1,file,"wb")!=0)
// 		return 0;
///	FILE   *   fp1=fopen("2da值.txt","w");
	for (int i = 0; i < newImg->GetMipLevelCount(); ++i)
	{
		//对该层的每一行
		for (int j = 0; j < newImg->GetHeight(); ++j)
		{
			unsigned char* dest = newImg->GetPixelRow(i, j);
			unsigned char* source = img->GetPixelRow(i, j);

			//对该行的每一列
			for (int k = 0; k < newImg->GetWidth(); ++k)
			{
				if (img->GetFormat() == GL_RGBA)
				{
					*(dest + k * 4 + 0) = *(source + k * 4 + 0);//jiang 8.15
					*(dest + k * 4 + 1) = *(source + k * 4 + 1);
					*(dest + k * 4 + 2) = *(source + k * 4 + 2);
					*(dest + k * 4 + 3) = *(source + k * 4 + 3);
					int t0 = (int)(*(dest + k * 4 + 0)), t1 = (int)(*(dest + k * 4 + 1)), t2 = (int)(*(dest + k * 4 + 2)), t3 = (int)(*(dest + k * 4 + 3));
					///	fprintf(fp1,"%d %d %d %d\n", t0, t1, t2, t3);
				}
				else if (img->GetFormat() == GL_BGRA_EXT)
				{
					*(dest + k * 4 + 1) = *(source + k * 4 + 3);
					*(dest + k * 4 + 2) = *(source + k * 4 + 2);
					*(dest + k * 4 + 3) = *(source + k * 4 + 1);
					*(dest + k * 4 + 3) = calpha;
				}
				else if (img->GetFormat() == GL_RGB)
				{
					*(dest + k * 4 + 0) = *(source + k * 3 + 0);
					*(dest + k * 4 + 1) = *(source + k * 3 + 1);
					*(dest + k * 4 + 2) = *(source + k * 3 + 2);
					*(dest + k * 4 + 3) = calpha;//jiang 8.15
				}
				else if (img->GetFormat() == GL_BGR_EXT)
				{
					*(dest + k * 4 + 1) = *(source + k * 3 + 2);
					*(dest + k * 4 + 2) = *(source + k * 3 + 1);
					*(dest + k * 4 + 3) = *(source + k * 3 + 0);
					*(dest + k * 4 + 3) = calpha;//jiang 8.15
				}
			}
		}
	}
	///	fclose(fp1);

	delete img;

	return newImg;
}
