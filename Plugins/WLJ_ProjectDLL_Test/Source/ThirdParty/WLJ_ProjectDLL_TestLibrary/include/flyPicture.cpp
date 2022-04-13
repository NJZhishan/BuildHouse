//#include "stdafx.h"
#include "flyPicture.h"
//#include "../projHead/ijl.h" //wqq 2018.12.3 将对ijl的引用注释掉
#include "jpeglib.h" //wqq 2018.12.3 
//复制构造函数
flyPicture::flyPicture(flyPicture& in) :
	sx(in.sx),sy(in.sy),
	bytespixel(in.bytespixel),
	size(in.size)
{ 
	buf=new unsigned char[size];
	memcpy(buf,in.buf,size);
}

//对=进行重载
void flyPicture::operator=(const flyPicture& in)
{
	FreePicture();

	sx=in.sx;
	sy=in.sy;
	bytespixel=in.bytespixel;
	size=size;
	strcpy_s(name,256,in.name);

	if (size)
	{
		buf=new unsigned char[size];
		memcpy(buf,in.buf,size);
	}
}

//释放内存
void flyPicture::FreePicture(void)
{
	delete[] buf;
	buf=0;
}

//初始化创建每个象素四个字节的内存数据
void flyPicture::CreatePicture32(int xd,int yd)
{
    sx=xd; sy=yd; bytespixel=4;
	size=sx*sy*bytespixel;

	buf=new unsigned char[size];
}

//初始化创建每个象素三个字节的内存数据
void flyPicture::CreatePicture24(int xd,int yd)
{
    sx=xd; sy=yd; bytespixel=3;
	size=sx*sy*bytespixel;

	buf=new unsigned char[sx*sy*bytespixel];
}

//对行列宽度不是2的次幂的图象数据进行调整
void flyPicture::CheckSize(int droplevel)
{
	int levelx=-1,levely=-1;
	int newsx,newsy;
	int i,j,k;

	i=1;
	while(levelx==-1)
	{
		j=1<<i;
		if (sx==j)
			levelx=i;
		else if (sx<j)
			levelx=i-1;
		i++;
	}
	i=1;
	while(levely==-1)
	{
		j=1<<i;
		if (sy==j)
			levely=i;
		else if (sy<j)
			levely=i-1;
		i++;
	}

	levelx-=droplevel;
	levely-=droplevel;

	if (levelx<0)
		levelx=0;
	if (levely<0)
		levely=0;
	newsx=1<<levelx;
	newsy=1<<levely;
	if (newsx==sx && newsy==sy)
		return;

	unsigned char *data=new unsigned char [newsx*newsy*bytespixel];
	int x,y,xx,yy,xy,p=0,rgba[4];
	for( y=0;y<newsy;y++ )
	for( x=0;x<newsx;x++ )
	{
		xx=(x+1)*sx/newsx;
		yy=(y+1)*sy/newsy;
		xy=(xx-x*sx/newsx)*(yy-y*sy/newsy);
		rgba[0]=rgba[1]=rgba[2]=rgba[3]=0;
		for( j=y*sy/newsy;j<yy;j++ )
			for( i=x*sx/newsx;i<xx;i++ )
				for( k=0;k<bytespixel;k++ )
					rgba[k]+=buf[(j*sx+i)*bytespixel+k];
		
		for( k=0;k<bytespixel;k++ )
			data[p++]=rgba[k]/xy;
	}
	
	delete[] buf;
	buf=data;
	sx=newsx;
	sy=newsy;
	size=sx*sy*bytespixel;
}

//对数据进行倒序排列
void flyPicture::FlipY()
{
	unsigned char *line=new unsigned char[bytespixel*sx];
	for( int i=0;i<sy/2;i++ )
		{
		memcpy(line,&buf[i*sx*bytespixel],bytespixel*sx);
		memcpy(&buf[i*sx*bytespixel],&buf[(sy-i-1)*sx*bytespixel],bytespixel*sx);
		memcpy(&buf[(sy-i-1)*sx*bytespixel],line,bytespixel*sx);
		}
	delete[] line;
}

//读取jpeg纹理数据，data：jpeg文件数据；len：jpeg文件长度；flipy：文件数据是否翻转
int flyPicture::LoadJPG(const unsigned char *data,int len,int flipy)
{
	//wqq 2018.12.11 注释掉
	//JPEG_CORE_PROPERTIES jpg_data;
	//int rslt=-1;

	//FreePicture();

	//memset(&jpg_data,0, sizeof(JPEG_CORE_PROPERTIES));

	//if(ijlInit(&jpg_data)!=IJL_OK)
	//	return 0;

	//jpg_data.JPGBytes=(unsigned char *)data;
	//jpg_data.JPGSizeBytes=len;

	//if(ijlRead(&jpg_data, IJL_JBUFF_READPARAMS)==IJL_OK)
	//	if(jpg_data.JPGChannels==3)
	//	{
	//		//jpeg文件每个象素只有三个字节？？？？
	//		jpg_data.DIBColor=IJL_RGB;
	//		jpg_data.DIBWidth=jpg_data.JPGWidth;
	//		jpg_data.DIBHeight=jpg_data.JPGHeight;
	//		jpg_data.DIBChannels=jpg_data.JPGChannels;
	//		CreatePicture24(jpg_data.DIBWidth, jpg_data.DIBHeight);
	//		jpg_data.DIBBytes = buf;
	//		rslt=ijlRead(&jpg_data, IJL_JBUFF_READWHOLEIMAGE);
	//	}
	//
	//ijlFree(&jpg_data);

	//if (flipy)
	//	FlipY();

	//return rslt==IJL_OK;

	FreePicture();
	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_decompress(&cinfo);

	//从内存读取
	jpeg_mem_src(&cinfo, data, len);

	jpeg_read_header(&cinfo, TRUE);
	unsigned int m_height = cinfo.image_height;
	unsigned int m_width = cinfo.image_width;

	CreatePicture24(cinfo.image_width, cinfo.image_height);

	jpeg_start_decompress(&cinfo);

	JSAMPROW row_pointer[1];

	while (cinfo.output_scanline < cinfo.output_height)
	{
		row_pointer[0] = &buf[cinfo.output_scanline*cinfo.image_width*cinfo.num_components];
		jpeg_read_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	if (flipy)
		FlipY();
	return TRUE;
}

//载入TGA图象
int flyPicture::LoadTGA(const unsigned char *data,int len,int flipy)
{
	int x_pos1, x_pos2, y, i, p, cursize;
	unsigned char flag, pixel_order;
	int filepos=18;

	FreePicture();

	sx=sy=0;
		
	if(!((data[16]==24 || data[16]==32) && (data[2]==2 || data[2]==10)))
		return 0;

	sx=*((unsigned short *)&data[12]);
	sy=*((unsigned short *)&data[14]);

	if(data[16]==24)
		CreatePicture24(sx,sy);
	else
		CreatePicture32(sx,sy);

	pixel_order=data[17]&0x30;

	if(data[2]==2)
		memcpy(buf,&data[filepos],size);
	else
	{
		unsigned char *p=buf, *q, c;

		cursize=0;
		while(cursize<sx*sy)
		{
			c=data[filepos++];
			if(!(c&0x80))
			{
				c++;
				memcpy(p,&data[filepos],c*bytespixel);
				filepos+=c*bytespixel;
				p+=c*bytespixel;
			}
			else
			{
				c=(c&0x7f)+1;
				memcpy(p,&data[filepos],bytespixel);
				filepos+=bytespixel;
				q=p;
				for(i=1; i<c; i++)
				{
					q+=bytespixel;
					q[0]=p[0];
					q[1]=p[1];
					q[2]=p[2];
				}
				p=q;
				p+=bytespixel;
			}
			cursize+=c;
		}
	}

	if (flipy)
	{
		if (pixel_order&0x20)
			FlipY();
	}
	else
		if ((pixel_order&0x20)==0)
			FlipY();

	for( y=0;y<sy;y++ )
	{
		x_pos1=0;
		for(p=0; p<sx; p++)
		{
			flag=buf[y*sx*bytespixel+x_pos1+2];
			buf[y*sx*bytespixel+x_pos1+2]=buf[y*sx*bytespixel+x_pos1];
			buf[y*sx*bytespixel+x_pos1]=flag;
			x_pos1+=bytespixel;
		}
		if(pixel_order&0x10)
		{
			x_pos1=0;
			x_pos2=(sx-1)*bytespixel;
			for(p=0; p<sx/2; p++)
			{
				for(i=0; i<bytespixel; i++)
				{
					flag=buf[y*sx*bytespixel+x_pos2+i];
					buf[y*sx*bytespixel+x_pos2+i]=buf[y*sx*bytespixel+x_pos1+i];
					buf[y*sx*bytespixel+x_pos1+i]=flag;
				}
				x_pos1+=bytespixel;
				x_pos2-=bytespixel;
			}
		}
	}

	return 1;
}

//保存TGA图象
int flyPicture::SaveTGA(const char *file)
{
    FILE *fp;
    int a,b;
    unsigned char TGA_INI[18]={0};
    unsigned char *picline;

    picline=new unsigned char[sx*bytespixel];
    if (!picline)
       return 0;

    if ((fopen_s(&fp,file,"wb"))==0)
       {
        memset(&TGA_INI[0],0,18);
        TGA_INI[12]=(unsigned char)(sx%256);
        TGA_INI[13]=(unsigned char)(sx/256);
        TGA_INI[14]=(unsigned char)(sy%256);
        TGA_INI[15]=(unsigned char)(sy/256);
        TGA_INI[2]=2;
        TGA_INI[16]=8*bytespixel;
		TGA_INI[17]=0x20;
        fwrite((char *)&TGA_INI[0],18,1,fp);
        for( a=sy-1;a>=0;a-- )
             {
              for( b=0;b<sx;b++ )
                   {
                    picline[b*bytespixel]=buf[(a*sx+b)*bytespixel+2];
                    picline[b*bytespixel+1]=buf[(a*sx+b)*bytespixel+1];
                    picline[b*bytespixel+2]=buf[(a*sx+b)*bytespixel];
					if (bytespixel==4)
						picline[b*bytespixel+3]=buf[(a*sx+b)*bytespixel+3];
                   }
              if (fwrite((char *)picline,sx,bytespixel,fp)!=(unsigned)bytespixel)
                 {
                  fclose(fp);
                  delete[] picline;
                  return 0;
                 }
             }
        fclose(fp);
       }
    delete[] picline;
    return 1;
}

//保存JPG图象
int flyPicture::SaveJPG(const char *file,int quality)
{
	if (buf==0 || file==0)
		return 0;
	FILE *fp;
	if(fopen_s(&fp,file,"wb")!=0)
		return 0;
// 	FILE *fp=fopen(file,"wb");
// 	if (fp==0)
// 		return 0;

	int rslt=SaveJPG(fp,quality);

    fclose(fp);
	
	return rslt;
}

int flyPicture::SaveJPG(FILE *fp,int quality)
{
	return 0;
	//wqq 2018.12.3 注释掉
	//if (buf==0 || fp==0)
	//	return 0;

	//JPEG_CORE_PROPERTIES jpg_data;

	//memset(&jpg_data,0, sizeof(JPEG_CORE_PROPERTIES));

	//if(ijlInit(&jpg_data)!=IJL_OK)
	//	return 0;

	//unsigned char *jpg_buf=new unsigned char[sx*sy*3];

 //   jpg_data.DIBWidth       = sx;
 //   jpg_data.DIBHeight      = sy;
 //   jpg_data.DIBBytes       = buf;
 //   jpg_data.DIBPadBytes    = 0;
 //   jpg_data.DIBChannels    = 3;
 //   jpg_data.DIBColor       = IJL_RGB;

 //   jpg_data.JPGWidth       = sx;
 //   jpg_data.JPGHeight      = sy;
 //   jpg_data.JPGFile        = 0;
 //   jpg_data.JPGBytes       = jpg_buf;
 //   jpg_data.JPGSizeBytes   = sx*sy*3;
 //   jpg_data.JPGChannels    = 3;
 //   jpg_data.JPGColor       = IJL_YCBCR;
 //   jpg_data.JPGSubsampling = IJL_411;
 //   jpg_data.jquality       = quality;

	//int rslt = ijlWrite(&jpg_data,IJL_JBUFF_WRITEWHOLEIMAGE);

	//ijlFree(&jpg_data);

	//fwrite(jpg_buf,jpg_data.JPGSizeBytes,1,fp);

	//delete jpg_buf;

	//return rslt==IJL_OK;
}

int readInt(const unsigned char *data,int &pos)
{
    unsigned char b[4]={0};
    memcpy(reinterpret_cast<char*>(b),data+pos, 4);
	pos +=4;
    return ((int) b[3] << 24) + ((int) b[2] << 16)
        + ((int) b[1] << 8) + (int) b[0];
}

short readShort(const unsigned char *data,int &pos)
{
    unsigned char b[2]={0};
    memcpy(reinterpret_cast<char*>(b), data+pos,2);
	pos+=2;
    return ((short) b[1] << 8) + (short) b[0];
}
//! Load a .bmp image
int flyPicture:: LoadBMP(const unsigned char *data,int len,int flipy)
{
	FreePicture();
	BMPFileHeader fileHeader;
    BMPImageHeader imageHeader;
    int pos = 0;
    fileHeader.b = data[pos++];
    fileHeader.m = data[pos++];
    fileHeader.size = readInt(data,pos);
    fileHeader.reserved = readInt(data,pos);
    fileHeader.offset = readInt(data,pos);

    if(fileHeader.b != 'B' || fileHeader.m != 'M')
        return NULL;

    imageHeader.size = readInt(data,pos);
    imageHeader.width = readInt(data,pos);
    imageHeader.height = readInt(data,pos);
    imageHeader.planes = readShort(data,pos);
    imageHeader.bpp = readShort(data,pos);
    imageHeader.compression = readInt(data,pos);
    imageHeader.imageSize = readInt(data,pos);
    imageHeader.widthPPM = readInt(data,pos);
    imageHeader.heightPPM = readInt(data,pos);
    imageHeader.colorsUsed = readInt(data,pos);
    imageHeader.colorsImportant = readInt(data,pos);

    if (imageHeader.width <= 0 || imageHeader.height <= 0)
        return NULL;

    //目前不处理压缩格式的BMP
    if (imageHeader.compression != 0)
        return NULL;
    //目前不处理1-, 2-, or 4-bpp格式的BMP
    if (imageHeader.bpp != 8 && imageHeader.bpp != 24 && imageHeader.bpp != 32)
        return NULL;

    unsigned char* palette = NULL;
    if(imageHeader.bpp == 8)
    {
        //printf("Reading %d color palette\n", imageHeader.colorsUsed);
        palette = new unsigned char[imageHeader.colorsUsed * 4];
        //in.read(reinterpret_cast<char*>(palette), imageHeader.colorsUsed * 4);
		memcpy(reinterpret_cast<char*>(palette), data+pos,imageHeader.colorsUsed * 4);
		pos+= imageHeader.colorsUsed * 4;
    }
    
    //in.seekg(fileHeader.offset, ios::beg);
	pos = fileHeader.offset;

    unsigned int bytesPerRow =
        (imageHeader.width * imageHeader.bpp / 8 + 1) & ~1;
    unsigned int imageBytes = bytesPerRow * imageHeader.height;

	sx = imageHeader.width;
	sy = imageHeader.height;
	bytespixel = imageHeader.bpp/8;
	size = sx*sy*bytespixel;

    //获得图象数据
	unsigned char *pixels = new unsigned char[imageBytes];
    buf = new unsigned char[imageBytes];
    //in.read(reinterpret_cast<char*>(pixels), imageBytes);
	memcpy(reinterpret_cast<char*>(pixels), data+pos,imageBytes);
	memset(buf, 0, imageBytes);///wql 2018.7.18 初始化，避免无效
	pos+= imageBytes;  
	if (NULL == buf || NULL == pixels)///wql 2018.7.24 该值有时候为NULL，可能没有申请到空间
	{
		if (pixels)
		{
			delete[]pixels;
			pixels = NULL;
		}
		if (buf)
		{
			delete[]buf;
			buf = NULL;
		}
		return NULL;
	}
	//将图象数据拷贝到CPicture中
    for(int y = 0; y < imageHeader.height; y++)
    {
        unsigned char* src = &pixels[y * bytesPerRow];
        unsigned char* dst = &buf[y * bytesPerRow];
	
		//对8,24,32位BMP分别处理
        switch(imageHeader.bpp)
        {
        case 8:
            {
				if (pixels)
					delete[]pixels;

				if (palette)
					delete[]palette;
				return NULL;///wql 2018.7.24 如果贴图是8位的则释放内存后直接返回，避免崩溃

                for(int x = 0; x < imageHeader.width/3; x++)///wql 2018.7.18 fix-bug 有时模型素材中的图片为8位时会崩溃。
                {
                    unsigned char* color = palette + (*src << 2);
                    dst[0] = color[2];
                    dst[1] = color[1];
                    dst[2] = color[0];
                    src++;
                    dst += 3;
                }
            }
            break;
            
        case 24:
            {
                for (int x = 0; x < imageHeader.width; x++)
                {
                    dst[0] = src[2];
                    dst[1] = src[1];
                    dst[2] = src[0];
                    src += 3;
                    dst += 3;
                }
            }
            break;

        case 32:
            {
                for (int x = 0; x < imageHeader.width; x++)
                {
                    dst[0] = src[2];
                    dst[1] = src[1];
                    dst[2] = src[0];
                    dst[3] = src[3];
                    src += 4;
                    dst += 4;
                }
            }
            break;
        }
    }

	if (pixels)
		delete[]pixels;

	if (palette)
		delete[]palette;
	return 1;
}

///***********************************************
//作者： 王庆利   创建时间 ： 2009.12.16
//功能描述：从data生成bitmap
//输入参数：data：为图像素值，len：数据长度， flipy：是否上下翻转
//输出参数：无
//返回参数说明：HBITMAP
//************************************************/
//HBITMAP flyPicture::CreateBitmap(const unsigned char *data,int len,int flipy)
//{
//	FreePicture();
//	HBITMAP hBitmap = NULL;
//	HGLOBAL hGlobal = GlobalAlloc(GPTR, len);
//	void* pData = GlobalLock(hGlobal);
//	memcpy(pData, (BYTE*)data, len);
//
//	CBitmap *bitmap = new CBitmap;
//	IPicture* pic = NULL;
//	IStream*   str = NULL;
//	HGLOBAL hG = NULL;
//	void* pp = NULL;
//
//	if (!hGlobal)
//	{
//		return NULL;
//	}
//	else
//	{
//		CreateStreamOnHGlobal(hGlobal, true, &str);
//	}
//
//	if (!str)
//	{
//		return NULL;
//	}
//
//	OleLoadPicture(str, 0, false, IID_IPicture, (void**)&pic);
//
//	if (!pic)
//	{
//		str->Release();
//		return NULL;
//	}
//	str->Release();
//
//	HBITMAP hb = 0;
//	pic->get_Handle((unsigned int*)&hb);
//
//	HBITMAP hBB = (HBITMAP)CopyImage(hb, IMAGE_BITMAP, 0, 0, LR_COPYRETURNORG);
//
//	pic->Release();
//	bitmap->m_hObject = hBB;
//
//	hBitmap = *bitmap;
//	return hBitmap;
//}
