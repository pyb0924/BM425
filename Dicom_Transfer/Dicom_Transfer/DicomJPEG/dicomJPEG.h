#ifndef DICOM_JPEG_H
#define DICOM_JPEG_H

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmdata/dcfilefo.h>

bool ExecuteCompress(DcmFileFormat& fileformat, E_TransferSyntax type);

#endif // !DICOM_JPEG_H

