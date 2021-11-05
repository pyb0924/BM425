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

Uint8 findUncompressedPC(const OFString& sopClass, DcmSCU& scu);

bool ExecuteCStore(DcmSCU& scu, const OFString& sopClass, const OFString& filePath);

#endif // !DICOM_SCU

