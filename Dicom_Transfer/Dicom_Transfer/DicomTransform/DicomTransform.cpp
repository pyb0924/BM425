#include "dicomTransform.h"
#include <cassert>

void GetDicomTransformInfo(DcmDataset*& dataSetPtr, DicomTransformInfo& info)
{
	DcmElement* elementBuffer = nullptr;
	if (dataSetPtr->findAndGetElement(DCM_Columns, elementBuffer).good())
	{
		assert(elementBuffer->getUint16(info.width).good());
	}
	if (dataSetPtr->findAndGetElement(DCM_Rows,elementBuffer).good())
	{
		assert(elementBuffer->getUint16(info.height).good());
	}
	if (dataSetPtr->findAndGetElement(DCM_PixelData, elementBuffer).good())
	{
		assert(elementBuffer->getUint16Array(info.dataPtr).good());
	}
}

void SetNewDataset(DcmDataset*& datasetPtr, const DicomTransformInfo& info)
{
	assert(datasetPtr->putAndInsertUint16(DCM_Columns, info.width).good());
	assert(datasetPtr->putAndInsertUint16(DCM_Rows, info.height).good());
	assert(datasetPtr->putAndInsertUint16Array(DCM_PixelData, info.dataPtr, info.width * info.height).good());
}

void Flip(DicomTransformInfo& info, FlipType type)
{
	Uint16* newDataPtr = new Uint16[info.height * info.width];

	switch (type)
	{
	case FlipType::VERTICAL:
		for (auto i = 0; i < info.height; i++)
		{
			for (auto j = 0; j < info.width; j++)
			{
				newDataPtr[i * info.width + j] = info.dataPtr[(info.height - 1 - i) * info.width + j];
			}
		}
		break;
	case FlipType::HORIZONTAL:
		for (auto i = 0; i < info.height; i++)
		{
			for (auto j = 0; j < info.width; j++)
			{
				newDataPtr[i * info.width + j] = info.dataPtr[i * info.width + info.width - 1 - j];
			}
		}
		break;
	default:
		break;
	}
	memcpy(info.dataPtr, newDataPtr, info.height* info.width * sizeof(Uint16));
	delete[] newDataPtr;
}

void Rotate(DicomTransformInfo& info, RotateType type)
{
	Uint16* newDataPtr = new Uint16[info.height * info.width];

	switch (type)
	{
	case RotateType::LEFT:
		for (auto i = 0; i < info.height; i++)
		{
			for (auto j = 0; j < info.width; j++)
			{
				newDataPtr[(info.width - 1 - j) * info.height + i] = info.dataPtr[i * info.width + j];
			}
		}
		break;
	case RotateType::RIGHT:
		for (auto i = 0; i < info.height; i++)
		{
			for (auto j = 0; j < info.width; j++)
			{
				newDataPtr[j * info.height + info.height - 1 - i] = info.dataPtr[i * info.width + j];
			}
		}
		break;
	default:
		break;
	}
	memcpy(info.dataPtr, newDataPtr, info.height * info.width * sizeof(Uint16));
	delete[] newDataPtr;

	//swap height and width
	Uint16 tmp = info.height;
	info.height = info.width;
	info.width = tmp;
}

