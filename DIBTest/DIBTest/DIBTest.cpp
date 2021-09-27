// DIBTest.cpp : Defines the entry point for the console application.
//

/*
任务：
1. 完成程序注释中的各项问题？
2. 掌握单点debug方法；
3. 掌握调试时，内存查看的方法。
4. 请找出主函数内，内存泄漏的Bug？ new & delete
	方法A：用眼睛找；
	方法B：借助工具；
 5. 如何计算图像上两点之间的距离：像点（20,35）到像点（1289,675）？
 // sqrt(((x1-x2)/biXPelsPerMeter)^2+ ((y1-y2)/biXPelsPerMeter)^2)
 6. 把灰度lena512.bmp改成biBitCount=8的彩色图像
 7. 图像垂直方向翻转
*/

#include "stdafx.h" 
#include <iostream>
using namespace std;

#include <Windows.h>
#include <fstream>
#include <string>

unsigned char* pData;
int width;
int height;
int bitCount;
RGBQUAD* pRGBQUAD;
BITMAPFILEHEADER bitMapFileHeader;
BITMAPINFOHEADER bitMapInfoHeader;

bool readBmpFile(char* filename)
{
	ifstream file;
	file.open(filename, ios::in | ios::binary);
	if (!file.is_open())
	{
		cout << "文件打开失败!" << endl;
		return false;
	}
	constexpr int FILE_SIZE = 300000;
	auto buffer = new char[FILE_SIZE];
	file.read(buffer, FILE_SIZE);
	file.close();

	auto curPos = buffer;

	memcpy(&bitMapFileHeader, curPos, sizeof(BITMAPFILEHEADER));//此处是否有漏洞，如何保护？ 
	curPos += sizeof(BITMAPFILEHEADER);
	//! 需要判断文件是否读取成功或是否达到文件末尾，可以通过fread的返回值来判断

	if (0x4D42 != bitMapFileHeader.bfType) // 这个判断的目的是什么？ 
	//! Ascii码中0x4D42=BM，标识了文件类型，BM为位图。该判断的目的是判断文件是不是位图
	{
		cout << "Error！" << endl;
		return false;
	}

	memcpy(&bitMapInfoHeader, curPos, sizeof(BITMAPINFOHEADER));//此处是否有漏洞，如何保护？
	curPos += sizeof(BITMAPINFOHEADER);
	//! 需要判断文件是否读取成功或是否达到文件末尾，可以通过fread的返回值来判断

	cout << "位图文件头：" << endl;
	cout << "位图文件类型: " << bitMapFileHeader.bfType << endl;
	cout << "位图文件大小: " << bitMapFileHeader.bfSize << endl;//如何判断bitMapFileHeader.bfSize是否有误
	//! 判断bfSize==biWidth*biHeight*bitCount/8+bfOffBits是否成立，若相等则无误
	cout << "偏移的字节数: " << bitMapFileHeader.bfOffBits << endl; //什么叫偏移字节数？
	//! 偏移的字节数即文件头的大小，标识图像的像素值信息的起始位置相对文件开始的偏移量

	width = bitMapInfoHeader.biWidth;
	height = bitMapInfoHeader.biHeight;
	bitCount = bitMapInfoHeader.biBitCount;
	cout << "\n位图信息头：" << endl;
	cout << "信息头占用字节数：" << bitMapInfoHeader.biSize << endl;
	cout << "位图宽度： " << bitMapInfoHeader.biWidth << endl;
	cout << "位图高度： " << bitMapInfoHeader.biHeight << endl;
	cout << "位图压缩类型： " << bitMapInfoHeader.biCompression << endl;
	cout << "位图每像素占用位数： " << bitMapInfoHeader.biBitCount << endl;  //什么叫每像素占用位数？
	//! 每像素占用位数表示用多少位可以存储一个像素的信息，如此图为Gray8图像，共有2^8=256个灰度级
	cout << "位图数据占用字节数： " << bitMapInfoHeader.biSizeImage << endl;//如何判断bitMapInfoHeader.biSizeImage是否有误？
	//! biSizeImage衡量的是位图文件中真正存储像素信息的大小，判断biSizeImage==biWidth*biHeight是否成立即可

	if (8 == bitMapInfoHeader.biBitCount) //此处判断，目的是什么？
	//! 判断是否为8位图像
	{
		pRGBQUAD = new RGBQUAD[256];
		memcpy(pRGBQUAD, curPos, sizeof(RGBQUAD) * 256);

	}

	int lineByte = (bitMapInfoHeader.biWidth * bitMapInfoHeader.biBitCount / 8 + 3) / 4 * 4; //此处的逻辑是什么？请解释？
	//! 计算每行像素所占的空间大小，将其调整至4的倍数（BMP格式要求）
	pData = new unsigned char[bitMapInfoHeader.biHeight * lineByte];	//此处是否有漏洞，如何保护？
	//! 需要判断申请的空间大小是否为0，或者很大
	memcpy(pData, buffer + bitMapFileHeader.bfOffBits, bitMapInfoHeader.biSizeImage);//此处是否有漏洞，如何保护？
	//! 需要判断文件是否读取成功或是否达到文件末尾，可以通过fread的返回值来判断
	return true;
	//此段函授有三处fread，如果批量读取文件，效率不高，请给出读取一次文件，完成本函数功能的方案？
	//! 可以使用fseek和ftell获得整个文件的大小，直接使用fread将一整个BMP文件读入一个足够大的buffer中，
	//! 然后从buffer中对应的offset取值，分几个部分（FILEHEADER，INFOHEADER，pRGBQUAD，pData）分别赋给对应的变量即可。
}

double get_distance(double x1, double x2, double y1, double y2)
{
	if (bitMapInfoHeader.biXPelsPerMeter == 0 && bitMapInfoHeader.biYPelsPerMeter == 0)
	{
		cout << "Error! resolution==0!  ";
	}

	const double dist_x = (x2 - x1) * bitMapInfoHeader.biXPelsPerMeter;
	const double dist_y = (y2 - y1) * bitMapInfoHeader.biYPelsPerMeter;
	return sqrt(dist_x * dist_x + dist_y * dist_y);
}

bool writeBmpFile(const char* filename, unsigned char* pData, int biWidth, int biHeight, int biBitCount)
{
	FILE* pf = fopen(filename, "wb");
	if (NULL == pf)
	{
		cout << "文件打开失败!" << endl;
		return false;
	}

	int colorTablesize = 0;
	if (biBitCount == 8)
		colorTablesize = 1024;

	//请说明此处的逻辑？
	//! 计算每行像素所占的空间大小，将其调整至4的倍数（BMP格式要求）
	int lineByte = (biWidth * biBitCount / 8 + 3) / 4 * 4;

	//申请位图文件头结构变量，填写文件头信息，请逐项说明BITMAPFILEHEADER各个字段含义？
	bitMapFileHeader.bfType = 0x4D42; //! bmp类型
	bitMapFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + colorTablesize + lineByte * biHeight; //! BMP文件的总大小
	bitMapFileHeader.bfReserved1 = 0;  //! 保留字段1，恒为0
	bitMapFileHeader.bfReserved2 = 0;  //! 保留字段2，恒为0
	bitMapFileHeader.bfOffBits = 54 + colorTablesize;  //! 偏移字节数

	//申请位图信息头结构变量，填写信息头信息，请逐项说明BITMAPINFOHEADER各个字段含义？
	bitMapInfoHeader.biBitCount = biBitCount; //! 每像素所占的位数
	bitMapInfoHeader.biClrImportant = 0; //! 显示位图所需要的“重要”索引数，设置为0则说明所有的均“重要”
	bitMapInfoHeader.biClrUsed = 0;  //! 颜色表中真正被位图用到的颜色索引数，设置为0则默认为2^n
	bitMapInfoHeader.biCompression = 0; //! 压缩方式 BI_RGB=0
	bitMapInfoHeader.biHeight = biHeight; //! 位图宽度
	bitMapInfoHeader.biPlanes = 1; //! 目标设备的显示平面数，必须设为1
	bitMapInfoHeader.biSize = 40; //! bipMapInfoHeader所需大小
	bitMapInfoHeader.biSizeImage = lineByte * biHeight; //! 图像大小，在未压缩的RGB图中可设为0
	bitMapInfoHeader.biWidth = biWidth; //! 位图宽度
	bitMapInfoHeader.biXPelsPerMeter = 0; //! 水平方向分辨率，单位为像素/m
	bitMapInfoHeader.biYPelsPerMeter = 0; //! 垂直方向分辨率，单位为像素/m

	//写文件头进文件
	fwrite(&bitMapFileHeader, sizeof(BITMAPFILEHEADER), 1, pf);
	//写位图信息头进内存
	fwrite(&bitMapInfoHeader, sizeof(BITMAPINFOHEADER), 1, pf);

	//此处是何逻辑？
	//! 写入8位图像的颜色表
	if (biBitCount == 8)
	{
		fwrite(pRGBQUAD, sizeof(RGBQUAD), 256, pf);
	}

	auto p_newData = new unsigned char[biHeight * lineByte];

	for (auto i = 0; i < biHeight; i++)
	{
		for (auto j = 0; j < lineByte; j++)
		{
			p_newData[i * lineByte + j] = pData[i * lineByte + biWidth - 1 - j];
		}
	}

	fwrite(p_newData, sizeof(unsigned char), biHeight * lineByte, pf);

	fclose(pf);
	delete[] p_newData;
	return true;
}

int main()
{
	char* toReadfilename = "..\\lena512.bmp";
	bool ret = readBmpFile(toReadfilename);
	if (!ret)
	{
		cout << "读BMP文件失败！" << endl;
		return -1;
	}
	else
	{
		cout << "读BMP文件成功！" << endl;
	}
	cout << endl;

	for (auto i = 0; i < 256; i++)
	{
		pRGBQUAD[i].rgbRed = i;
		pRGBQUAD[i].rgbGreen = i;
		pRGBQUAD[i].rgbBlue = 255 - i;
	}

	const char* toWritefilename = "..\\write.bmp";
	ret = writeBmpFile(toWritefilename, pData, width, height, bitCount);
	if (!ret)
	{
		cout << "写BMP文件失败！" << endl;
		return -1;
	}
	else
	{
		cout << "写BMP文件成功！" << endl;
	}

	cout << "距离为" << get_distance(20, 35, 1289, 675) << endl;

	delete[] pRGBQUAD;
	pRGBQUAD = NULL;

	delete[] pData;
	pData = NULL;

	return 0;
}

/*int _tmain(int argc, _TCHAR* argv[])
{

	return 0;
}
*/
