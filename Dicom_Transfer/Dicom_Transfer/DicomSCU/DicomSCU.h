#ifndef DICOM_SCU
#define DICOM_SCU

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/diutil.h>
#include <dcmtk/dcmnet/scu.h>

constexpr auto OFFIS_CONSOLE_APPLICATION = "testscu";
constexpr auto APPLICATIONTITLE = "SCU";
constexpr auto PEERHOSTNAME = "localhost";
constexpr auto PEERPORT = 104;
constexpr auto PEERAPPLICATIONTITLE = "SERVER";
constexpr auto STORESCPPORT = 104;

enum class CompressType {
	NO_COMPORESSION = 0,
	JPEG_COMPRESSION,
	JPEG2000_COMPRESSION
};

Uint8 findUncompressedPC(const OFString& sopClass, DcmSCU& scu);

DcmDataset* GetDatasetPtr(const OFString& filePath);

bool ExecuteCompress(DcmDataset*& datasetPtr,CompressType type);

bool ExecuteCStore(DcmSCU& scu, const OFString& sopClass, const OFString& filePath, CompressType type);

#endif // !DICOM_SCU

