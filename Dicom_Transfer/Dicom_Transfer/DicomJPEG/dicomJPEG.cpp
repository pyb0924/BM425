#include "dicomJPEG.h"
#include <dcmtk/dcmjpeg/djrplol.h>
#include <dcmtk/dcmjpeg/djencode.h>

bool ExecuteCompress(DcmFileFormat& fileformat, E_TransferSyntax type)
{
	DJ_RPLossless params;
	OFCondition result;
	DJEncoderRegistration::registerCodecs();
	bool flag = false;

	DcmDataset* datasetPtr = fileformat.getDataset();
	DcmMetaInfo* metaInfo = fileformat.getMetaInfo();

	// this causes the lossless JPEG version of the dataset to be created
	flag = (datasetPtr->chooseRepresentation(type, &params).good() && datasetPtr->canWriteXfer(type));
	DJEncoderRegistration::cleanup();
	return flag;
}