#include <iostream>

#include "dicomJPEG.h"

int main(int argc, char* argv[])
{
	DcmFileFormat fileformat;
	E_TransferSyntax type = EXS_JPEGProcess14SV1;
	fileformat.loadFile("E:\\BM425\\Dicom_Transfer\\data\\test.dcm");
	if (ExecuteCompress(fileformat, type))
	{
		fileformat.saveFile("E:\\BM425\\Dicom_Transfer\\data\\test_jpeg.dcm",type);
	}
	return 0;
}