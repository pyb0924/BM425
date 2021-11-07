#include <iostream>
#include <cassert>

#include <dcmtk/config/osconfig.h>

#include "dicomTransform.h"

int main()
{
	std::string filePath = "E:\\BM425\\Dicom_Transfer\\data\\test.dcm";
	DcmFileFormat fileFormat;
	OFFilename dcmFileName = OFFilename(filePath.c_str());
	OFCondition status = fileFormat.loadFile(dcmFileName);
	if (status.bad())
	{
		throw "Dicom File Not Found!";
	}
	DcmDataset* datasetPtr = fileFormat.getDataset();
	DicomTransformInfo dcmInfo;
	GetDicomTransformInfo(datasetPtr,dcmInfo);
	//std::cout << dcmInfo.height << "\t" << dcmInfo.width << std::endl;

	Flip(dcmInfo, FlipType::VERTICAL);
	//Flip(dcmInfo, FlipType::HORIZONTAL);
	Rotate(dcmInfo, RotateType::RIGHT);
	//Rotate(dcmInfo, RotateType::LEFT);

	//std::cout << dcmInfo.height << "\t" << dcmInfo.width << std::endl;
	SetNewDataset(datasetPtr, dcmInfo);

	assert(fileFormat.saveFile("E:\\BM425\\Dicom_Transfer\\data\\transform.dcm", EXS_LittleEndianExplicit).good());

	return 0;
}