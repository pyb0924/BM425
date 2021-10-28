#include "dicomTransform.h"
#include <cassert>

void GetDicomTransformInfo(DcmDataset*& dataSetPtr, DicomTransformInfo& info)
{
	Uint16 uIntBuffer;
	const Uint16* uIntPtrBuffer;
	if (dataSetPtr->findAndGetUint16(DCM_Columns, uIntBuffer).good())
	{
		info.width = uIntBuffer;
	}
	if (dataSetPtr->findAndGetUint16(DCM_Rows, uIntBuffer).good())
	{
		info.height = uIntBuffer;
	}
	if (dataSetPtr->findAndGetUint16Array(DCM_PixelData, uIntPtrBuffer).good())
	{
		info.data = uIntPtrBuffer;
	}
}

void SetNewDataset(DcmDataset*& datasetPtr, DicomTransformInfo& info)
{
	assert(datasetPtr->putAndInsertFloat64(DCM_WindowWidth, 1300.0).good());
	assert(datasetPtr->putAndInsertUint16(DCM_Columns, info.width).good());
	assert(datasetPtr->putAndInsertUint16(DCM_Rows, info.height).good());
	assert(datasetPtr->putAndInsertUint16Array(DCM_PixelData, info.data, info.width * info.height).good());
}

void Flip(DicomTransformInfo& data, FlipType type)
{

}

void Rotate(DicomTransformInfo& data, RotateType type)
{
}
