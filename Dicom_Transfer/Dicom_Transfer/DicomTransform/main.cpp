// DicomTransform.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <iostream>
#include <cassert>
#include <opencv2/opencv.hpp>

//DCMTK
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"

#include "dicomTransform.h"

int main()
{
	std::string filePath = "E:\\BM425\\data\\FACT3d_tra_bh_FF_NDC_8003\\00000001.dcm";
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

	Flip(dcmInfo, FlipType::HORIZONTAL);
	Rotate(dcmInfo, RotateType::LEFT);

	std::cout << dcmInfo.height << "\t" << dcmInfo.width << std::endl;
	SetNewDataset(datasetPtr, dcmInfo);

	status = fileFormat.saveFile("result.dcm", EXS_LittleEndianExplicit);
	
	if (status.bad())
	{
		std::cout << "DCM file create failed!" << std::endl;
	}

	return 0;
}