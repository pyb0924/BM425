// DIBTest.cpp : Defines the entry point for the console application.
//

/*
����
1. ��ɳ���ע���еĸ������⣿
2. ���յ���debug������
3. ���յ���ʱ���ڴ�鿴�ķ�����
4. ���ҳ��������ڣ��ڴ�й©��Bug�� new & delete
	����A�����۾��ң�
	����B���������ߣ�
 5. ��μ���ͼ��������֮��ľ��룺��㣨20,35������㣨1289,675����
 // sqrt(((x1-x2)/biXPelsPerMeter)^2+ ((y1-y2)/biXPelsPerMeter)^2)
 6. �ѻҶ�lena512.bmp�ĳ�biBitCount=8�Ĳ�ɫͼ��
 7. ͼ��ֱ����ת
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
		cout << "�ļ���ʧ��!" << endl;
		return false;
	}
	constexpr int FILE_SIZE = 300000;
	auto buffer = new char[FILE_SIZE];
	file.read(buffer, FILE_SIZE);
	file.close();

	auto curPos = buffer;

	memcpy(&bitMapFileHeader, curPos, sizeof(BITMAPFILEHEADER));//�˴��Ƿ���©������α����� 
	curPos += sizeof(BITMAPFILEHEADER);
	//! ��Ҫ�ж��ļ��Ƿ��ȡ�ɹ����Ƿ�ﵽ�ļ�ĩβ������ͨ��fread�ķ���ֵ���ж�

	if (0x4D42 != bitMapFileHeader.bfType) // ����жϵ�Ŀ����ʲô�� 
	//! Ascii����0x4D42=BM����ʶ���ļ����ͣ�BMΪλͼ�����жϵ�Ŀ�����ж��ļ��ǲ���λͼ
	{
		cout << "Error��" << endl;
		return false;
	}

	memcpy(&bitMapInfoHeader, curPos, sizeof(BITMAPINFOHEADER));//�˴��Ƿ���©������α�����
	curPos += sizeof(BITMAPINFOHEADER);
	//! ��Ҫ�ж��ļ��Ƿ��ȡ�ɹ����Ƿ�ﵽ�ļ�ĩβ������ͨ��fread�ķ���ֵ���ж�

	cout << "λͼ�ļ�ͷ��" << endl;
	cout << "λͼ�ļ�����: " << bitMapFileHeader.bfType << endl;
	cout << "λͼ�ļ���С: " << bitMapFileHeader.bfSize << endl;//����ж�bitMapFileHeader.bfSize�Ƿ�����
	//! �ж�bfSize==biWidth*biHeight*bitCount/8+bfOffBits�Ƿ�����������������
	cout << "ƫ�Ƶ��ֽ���: " << bitMapFileHeader.bfOffBits << endl; //ʲô��ƫ���ֽ�����
	//! ƫ�Ƶ��ֽ������ļ�ͷ�Ĵ�С����ʶͼ�������ֵ��Ϣ����ʼλ������ļ���ʼ��ƫ����

	width = bitMapInfoHeader.biWidth;
	height = bitMapInfoHeader.biHeight;
	bitCount = bitMapInfoHeader.biBitCount;
	cout << "\nλͼ��Ϣͷ��" << endl;
	cout << "��Ϣͷռ���ֽ�����" << bitMapInfoHeader.biSize << endl;
	cout << "λͼ��ȣ� " << bitMapInfoHeader.biWidth << endl;
	cout << "λͼ�߶ȣ� " << bitMapInfoHeader.biHeight << endl;
	cout << "λͼѹ�����ͣ� " << bitMapInfoHeader.biCompression << endl;
	cout << "λͼÿ����ռ��λ���� " << bitMapInfoHeader.biBitCount << endl;  //ʲô��ÿ����ռ��λ����
	//! ÿ����ռ��λ����ʾ�ö���λ���Դ洢һ�����ص���Ϣ�����ͼΪGray8ͼ�񣬹���2^8=256���Ҷȼ�
	cout << "λͼ����ռ���ֽ����� " << bitMapInfoHeader.biSizeImage << endl;//����ж�bitMapInfoHeader.biSizeImage�Ƿ�����
	//! biSizeImage��������λͼ�ļ��������洢������Ϣ�Ĵ�С���ж�biSizeImage==biWidth*biHeight�Ƿ��������

	if (8 == bitMapInfoHeader.biBitCount) //�˴��жϣ�Ŀ����ʲô��
	//! �ж��Ƿ�Ϊ8λͼ��
	{
		pRGBQUAD = new RGBQUAD[256];
		memcpy(pRGBQUAD, curPos, sizeof(RGBQUAD) * 256);

	}

	int lineByte = (bitMapInfoHeader.biWidth * bitMapInfoHeader.biBitCount / 8 + 3) / 4 * 4; //�˴����߼���ʲô������ͣ�
	//! ����ÿ��������ռ�Ŀռ��С�����������4�ı�����BMP��ʽҪ��
	pData = new unsigned char[bitMapInfoHeader.biHeight * lineByte];	//�˴��Ƿ���©������α�����
	//! ��Ҫ�ж�����Ŀռ��С�Ƿ�Ϊ0�����ߺܴ�
	memcpy(pData, buffer + bitMapFileHeader.bfOffBits, bitMapInfoHeader.biSizeImage);//�˴��Ƿ���©������α�����
	//! ��Ҫ�ж��ļ��Ƿ��ȡ�ɹ����Ƿ�ﵽ�ļ�ĩβ������ͨ��fread�ķ���ֵ���ж�
	return true;
	//�˶κ���������fread�����������ȡ�ļ���Ч�ʲ��ߣ��������ȡһ���ļ�����ɱ��������ܵķ�����
	//! ����ʹ��fseek��ftell��������ļ��Ĵ�С��ֱ��ʹ��fread��һ����BMP�ļ�����һ���㹻���buffer�У�
	//! Ȼ���buffer�ж�Ӧ��offsetȡֵ���ּ������֣�FILEHEADER��INFOHEADER��pRGBQUAD��pData���ֱ𸳸���Ӧ�ı������ɡ�
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
		cout << "�ļ���ʧ��!" << endl;
		return false;
	}

	int colorTablesize = 0;
	if (biBitCount == 8)
		colorTablesize = 1024;

	//��˵���˴����߼���
	//! ����ÿ��������ռ�Ŀռ��С�����������4�ı�����BMP��ʽҪ��
	int lineByte = (biWidth * biBitCount / 8 + 3) / 4 * 4;

	//����λͼ�ļ�ͷ�ṹ��������д�ļ�ͷ��Ϣ��������˵��BITMAPFILEHEADER�����ֶκ��壿
	bitMapFileHeader.bfType = 0x4D42; //! bmp����
	bitMapFileHeader.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + colorTablesize + lineByte * biHeight; //! BMP�ļ����ܴ�С
	bitMapFileHeader.bfReserved1 = 0;  //! �����ֶ�1����Ϊ0
	bitMapFileHeader.bfReserved2 = 0;  //! �����ֶ�2����Ϊ0
	bitMapFileHeader.bfOffBits = 54 + colorTablesize;  //! ƫ���ֽ���

	//����λͼ��Ϣͷ�ṹ��������д��Ϣͷ��Ϣ��������˵��BITMAPINFOHEADER�����ֶκ��壿
	bitMapInfoHeader.biBitCount = biBitCount; //! ÿ������ռ��λ��
	bitMapInfoHeader.biClrImportant = 0; //! ��ʾλͼ����Ҫ�ġ���Ҫ��������������Ϊ0��˵�����еľ�����Ҫ��
	bitMapInfoHeader.biClrUsed = 0;  //! ��ɫ����������λͼ�õ�����ɫ������������Ϊ0��Ĭ��Ϊ2^n
	bitMapInfoHeader.biCompression = 0; //! ѹ����ʽ BI_RGB=0
	bitMapInfoHeader.biHeight = biHeight; //! λͼ���
	bitMapInfoHeader.biPlanes = 1; //! Ŀ���豸����ʾƽ������������Ϊ1
	bitMapInfoHeader.biSize = 40; //! bipMapInfoHeader�����С
	bitMapInfoHeader.biSizeImage = lineByte * biHeight; //! ͼ���С����δѹ����RGBͼ�п���Ϊ0
	bitMapInfoHeader.biWidth = biWidth; //! λͼ���
	bitMapInfoHeader.biXPelsPerMeter = 0; //! ˮƽ����ֱ��ʣ���λΪ����/m
	bitMapInfoHeader.biYPelsPerMeter = 0; //! ��ֱ����ֱ��ʣ���λΪ����/m

	//д�ļ�ͷ���ļ�
	fwrite(&bitMapFileHeader, sizeof(BITMAPFILEHEADER), 1, pf);
	//дλͼ��Ϣͷ���ڴ�
	fwrite(&bitMapInfoHeader, sizeof(BITMAPINFOHEADER), 1, pf);

	//�˴��Ǻ��߼���
	//! д��8λͼ�����ɫ��
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
		cout << "��BMP�ļ�ʧ�ܣ�" << endl;
		return -1;
	}
	else
	{
		cout << "��BMP�ļ��ɹ���" << endl;
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
		cout << "дBMP�ļ�ʧ�ܣ�" << endl;
		return -1;
	}
	else
	{
		cout << "дBMP�ļ��ɹ���" << endl;
	}

	cout << "����Ϊ" << get_distance(20, 35, 1289, 675) << endl;

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
