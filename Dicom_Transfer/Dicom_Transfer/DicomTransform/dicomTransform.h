#ifndef DCM_TRANSFORM_H
#define DCM_TRANSFORM_H

#include "dcmtk/dcmdata/dctk.h"
struct DicomTransformInfo
{
	Uint16 width;
	Uint16 height;
	const Uint16* data;
};

enum class FlipType
{
	VERTICAL=0,
	HORIZONTAL
};

enum class RotateType
{
	LEFT=0,
	RIGHT
};

void GetDicomTransformInfo(DcmDataset*& dataSetPtr, DicomTransformInfo& info);

void SetNewDataset(DcmDataset*& datasetPtr, DicomTransformInfo& info);

void Flip(DicomTransformInfo& data, FlipType type);

void Rotate(DicomTransformInfo& data, RotateType type);

#endif // !DCM_TRANSFORM_H

