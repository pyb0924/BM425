#include <cassert>

#include <boost/format.hpp>
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
	
	DcmElement* elementBuffer = nullptr;
	OFString stringBuffer;
	Sint32 shortBuffer;
	if (datasetPtr->findAndGetElement(DCM_PatientName, elementBuffer).good())
	{
		assert(elementBuffer->getOFStringArray(stringBuffer).good());
		seriesIndexInfo.patientName = std::string(stringBuffer.c_str());
	}
	if (datasetPtr->findAndGetElement(DCM_PatientID, elementBuffer).good())
	{
		assert(elementBuffer->getOFStringArray(stringBuffer).good());
		seriesIndexInfo.patientID = std::string(stringBuffer.c_str());
	}
	if (datasetPtr->findAndGetElement(DCM_StudyID, elementBuffer).good())
	{
		assert(elementBuffer->getOFStringArray(stringBuffer).good());
		seriesIndexInfo.studyID = std::string(stringBuffer.c_str());
	}
	if (datasetPtr->findAndGetElement(DCM_StudyInstanceUID, elementBuffer).good())
	{
		assert(elementBuffer->getOFStringArray(stringBuffer).good());
		seriesIndexInfo.studyInstanceUID = std::string(stringBuffer.c_str());
	}
	if (datasetPtr->findAndGetElement(DCM_SeriesDescription, elementBuffer).good())
	{
		assert(elementBuffer->getOFStringArray(stringBuffer).good());
		seriesIndexInfo.seriesDescription = std::string(stringBuffer.c_str());
	}
	if (datasetPtr->findAndGetElement(DCM_SeriesInstanceUID, elementBuffer).good())
	{
		assert(elementBuffer->getOFStringArray(stringBuffer).good());
		seriesIndexInfo.seriesInstanceUID = std::string(stringBuffer.c_str());
	}
	if (datasetPtr->findAndGetElement(DCM_SeriesNumber, elementBuffer).good())
	{
		assert(elementBuffer->getSint32(shortBuffer).good());
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

	DcmElement* elementBuffer;
	OFString stringBuffer;
	if (datasetPtr->findAndGetElement(DCM_SOPInstanceUID, elementBuffer).good())
	{
		assert(elementBuffer->getOFStringArray(stringBuffer).good());
		seriesInstanceInfo.SOPInstanceUID = std::string(stringBuffer.c_str());
	}
	if (datasetPtr->findAndGetElement(DCM_ImagePositionPatient, elementBuffer).good())
	{
		assert(elementBuffer->getOFStringArray(stringBuffer).good());
		seriesInstanceInfo.imagePositionPatient = std::string(stringBuffer.c_str());
	}
	if (datasetPtr->findAndGetElement(DCM_ImageOrientationPatient, elementBuffer).good())
	{
		assert(elementBuffer->getOFStringArray(stringBuffer).good());
		seriesInstanceInfo.imageOrientationPatient = std::string(stringBuffer.c_str());
	}
}


Table CreateTableFromSQL(Session& session, const string& schemaName, const string& tableName, string createSQL)
{
	session.sql(string("USE ") + schemaName).execute();
	session.sql(string("DROP TABLE IF EXISTS ") + tableName).execute();
	string createSQLWithName = boost::str(boost::format(createSQL) % tableName);
	session.sql(createSQLWithName).execute();
	return session.getSchema(schemaName).getTable(tableName);
}