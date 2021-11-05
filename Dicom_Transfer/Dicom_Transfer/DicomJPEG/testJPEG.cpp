#include <iostream>

#include "dicomJPEG.h"

int main(int argc, char* argv[])
{
	DcmFileFormat fileformat;
	E_TransferSyntax type = EXS_JPEGProcess14SV1;
	fileformat.loadFile("E:\\BM425\\pre_DICOM_axis\\test.dcm");
	if (ExecuteCompress(fileformat, type))
	{
		fileformat.saveFile("test_jpeg.dcm",type);
	}
	return 0;
}