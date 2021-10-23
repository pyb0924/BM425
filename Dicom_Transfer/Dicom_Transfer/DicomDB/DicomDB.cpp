#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>

#include "dicomDB.h"

void GetSeriesIndexInfo(std::string dcmPath, SeriesIndexInfo& seriesIndexInfo)
{
	DcmFileFormat fileformat;
	OFFilename dcmFileName = OFFilename(dcmPath.c_str());
	OFCondition status = fileformat.loadFile(dcmFileName);
	if (status.bad())
	{
		throw "Dicom File Not Found!";
	}
	DcmDataset* datasetPtr = fileformat.getDataset();

	OFString stringBuffer;
	Sint32 shortBuffer;
	if (datasetPtr->findAndGetOFString(DCM_PatientName, stringBuffer).good())
	{
		seriesIndexInfo.patientName = std::string(stringBuffer.c_str());
	}
	if (datasetPtr->findAndGetOFString(DCM_PatientID, stringBuffer).good())
	{
		seriesIndexInfo.patientID = std::string(stringBuffer.c_str());
	}
	if (datasetPtr->findAndGetOFString(DCM_StudyID, stringBuffer).good())
	{
		seriesIndexInfo.studyID = std::string(stringBuffer.c_str());
	}
	if (datasetPtr->findAndGetOFString(DCM_StudyInstanceUID, stringBuffer).good())
	{
		seriesIndexInfo.studyInstanceUID = std::string(stringBuffer.c_str());
	}
	if (datasetPtr->findAndGetOFString(DCM_SeriesDescription, stringBuffer).good())
	{
		seriesIndexInfo.seriesDescription = std::string(stringBuffer.c_str());
	}
	if (datasetPtr->findAndGetOFString(DCM_SeriesInstanceUID, stringBuffer).good())
	{
		seriesIndexInfo.seriesInstanceUID = std::string(stringBuffer.c_str());
	}
	if (datasetPtr->findAndGetSint32(DCM_SeriesNumber, shortBuffer).good())
	{
		seriesIndexInfo.seriesNumber = shortBuffer;
	}
}

void GetSeriesInstanceInfo(std::string dcmPath, SeriesInstanceInfo& seriesInstanceInfo)
{
	DcmFileFormat fileformat;
	OFFilename dcmFileName = OFFilename(dcmPath.c_str());
	OFCondition status = fileformat.loadFile(dcmFileName);
	if (status.bad())
	{
		throw "Dicom File Not Found!";
	}
	DcmDataset* datasetPtr = fileformat.getDataset();

	OFString stringBuffer;
	if (datasetPtr->findAndGetOFString(DCM_SOPInstanceUID, stringBuffer).good())
	{
		seriesInstanceInfo.SOPInstanceUID = std::string(stringBuffer.c_str());
	}
	if (datasetPtr->findAndGetOFString(DCM_ImagePositionPatient, stringBuffer).good())
	{
		seriesInstanceInfo.imagePositionPatient = std::string(stringBuffer.c_str());
	}
	if (datasetPtr->findAndGetOFString(DCM_ImageOrientationPatient, stringBuffer).good())
	{
		seriesInstanceInfo.imageOrientationPatient = std::string(stringBuffer.c_str());
	}
}


Table CreateTableFromSQL(Session& session, const string& schemaName,const string& tableName, string& createSQL)
{
	string quotedName = string("`")+ schemaName + string("`.`") + tableName + string("`");
	session.sql(string("DROP TABLE IF EXISTS ") + quotedName).execute();
	createSQL.insert(13, tableName);
	//TODO correct SQL batch
	session.sql(createSQL).execute();
	return session.getSchema(schemaName).getTable(tableName);
}