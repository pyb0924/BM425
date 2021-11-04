#include <dcmtk/dcmjpeg/djrplol.h>
#include <dcmtk/dcmjpeg/djencode.h>
#include "DicomSCU.h"

Uint8 findUncompressedPC(const OFString& sopClass, DcmSCU& scu)
{
	Uint8 pc;
	pc = scu.findPresentationContextID(sopClass, UID_LittleEndianExplicitTransferSyntax);
	if (!pc)
		pc = scu.findPresentationContextID(sopClass, UID_BigEndianExplicitTransferSyntax);
	if (!pc)
		pc = scu.findPresentationContextID(sopClass, UID_LittleEndianImplicitTransferSyntax);
	return pc;
}

DcmDataset* GetDatasetPtr(const OFString& filePath)
{
	DcmFileFormat fileFormat;
	OFFilename dcmFileName = OFFilename(filePath.c_str());
	OFCondition status = fileFormat.loadFile(dcmFileName);
	if (status.bad())
	{
		return nullptr;
	}
	return fileFormat.getDataset();
}

bool ExecuteCompress(DcmDataset*& datasetPtr, CompressType type)
{
	DJ_RPLossless params;
	OFCondition result;
	switch (type)
	{
	case CompressType::JPEG_COMPRESSION:
		std::cout << "Compressing: JPEG" << std::endl;
		return datasetPtr->chooseRepresentation(EXS_JPEGProcess14SV1, &params).good();
	case CompressType::JPEG2000_COMPRESSION:
		std::cout << "Compressing: JPEG2000" << std::endl;
		return datasetPtr->chooseRepresentation(EXS_JPEG2000LosslessOnly, &params).good();
	default:
		break;
	}
	
	return false;
}

bool ExecuteCStore(DcmSCU& scu, const OFString& sopClass, const OFString& filePath, CompressType type)
{
	T_ASC_PresentationContextID presID = findUncompressedPC(sopClass, scu);
	if (presID == 0)
	{
		DCMNET_ERROR("There is no uncompressed presentation context for C-STORE");
		return false;
	}
	Uint16 rspStatusCode;
	OFCondition result;
	DcmDataset* datasetPtr = nullptr;
	switch (type)
	{
	case CompressType::NO_COMPORESSION:
		result = scu.sendSTORERequest(presID, filePath, NULL, rspStatusCode);
		break;
	case CompressType::JPEG_COMPRESSION:
	case CompressType::JPEG2000_COMPRESSION:
		datasetPtr = GetDatasetPtr(filePath);
		// TODO compression ERROR
		DJEncoderRegistration::registerCodecs();
		assert(ExecuteCompress(datasetPtr, type));
		DJEncoderRegistration::cleanup();
		result = scu.sendSTORERequest(presID, NULL, datasetPtr, rspStatusCode);
		break;
	default:
		break;
	}

	if (result.bad()) {
		DCMNET_ERROR("C-STORE Operation failed.");
		return false;
	}
	else {
		DCMNET_INFO("C-STORE Operation completed successfully.");
	}
	return true;
}
