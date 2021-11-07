#include <cassert>

#include <boost/format.hpp>
#include <dcmtk/dcmdata/dctk.h>

#include "dicomDB.h"

Table CreateTableFromSQL(Session& session, const string& schemaName, const string& tableName, string createSQL)
{
	session.sql(string("USE ") + schemaName).execute();
	session.sql(string("DROP TABLE IF EXISTS ") + tableName).execute();
	string createSQLWithName = boost::str(boost::format(createSQL) % tableName);
	session.sql(createSQLWithName).execute();
	return session.getSchema(schemaName).getTable(tableName);
}

void GetDcmIndexInfo(DcmFileFormat& fileformat, DcmIndexInfo& dcmIndexInfo)
{
	DcmDataset* datasetPtr = fileformat.getDataset();
	
	DcmElement* elementBuffer = nullptr;
	OFString stringBuffer;
	Sint32 shortBuffer;
	if (datasetPtr->findAndGetElement(DCM_SOPInstanceUID, elementBuffer).good())
	{
		assert(elementBuffer->getOFStringArray(stringBuffer).good());
		dcmIndexInfo.SOPInstanceUID = std::string(stringBuffer.c_str());
	}
	if (datasetPtr->findAndGetElement(DCM_PatientName, elementBuffer).good())
	{
		assert(elementBuffer->getOFStringArray(stringBuffer).good());
		dcmIndexInfo.patientName = std::string(stringBuffer.c_str());
	}
	if (datasetPtr->findAndGetElement(DCM_PatientID, elementBuffer).good())
	{
		assert(elementBuffer->getOFStringArray(stringBuffer).good());
		dcmIndexInfo.patientID = std::string(stringBuffer.c_str());
	}
	if (datasetPtr->findAndGetElement(DCM_StudyID, elementBuffer).good())
	{
		assert(elementBuffer->getOFStringArray(stringBuffer).good());
		dcmIndexInfo.studyID = std::string(stringBuffer.c_str());
	}
	if (datasetPtr->findAndGetElement(DCM_StudyInstanceUID, elementBuffer).good())
	{
		assert(elementBuffer->getOFStringArray(stringBuffer).good());
		dcmIndexInfo.studyInstanceUID = std::string(stringBuffer.c_str());
	}
	if (datasetPtr->findAndGetElement(DCM_SeriesDescription, elementBuffer).good())
	{
		assert(elementBuffer->getOFStringArray(stringBuffer).good());
		dcmIndexInfo.seriesDescription = std::string(stringBuffer.c_str());
	}
	if (datasetPtr->findAndGetElement(DCM_SeriesInstanceUID, elementBuffer).good())
	{
		assert(elementBuffer->getOFStringArray(stringBuffer).good());
		dcmIndexInfo.seriesInstanceUID = std::string(stringBuffer.c_str());
	}
	if (datasetPtr->findAndGetElement(DCM_SeriesNumber, elementBuffer).good())
	{
		assert(elementBuffer->getSint32(shortBuffer).good());
		dcmIndexInfo.seriesNumber = shortBuffer;
	}
}

void InsertDcmToDB(DcmFileFormat& fileformat, Table& table)
{
	DcmIndexInfo indexInfo;
	GetDcmIndexInfo(fileformat, indexInfo);
	RowResult result=table.select("SOPInstanceUID").execute();
	Row row;
	bool flag = false;
	while ((row = result.fetchOne()))
	{
		if (std::string(row[0]) == indexInfo.SOPInstanceUID)
		{
			flag = true;
			break;
		}
	}
	if (flag)
	{
		std::cout << "SOPInstance Existed in DB!" << std::endl;
		return;
	}

	table.insert("SOPInstanceUID", "patientName", "patientID", "studyID", "studyInstanceUID", 
		"seriesDescription", "seriesInstanceUID", "seriesNumber")
		.values(indexInfo.SOPInstanceUID,indexInfo.patientName, indexInfo.patientID, indexInfo.studyID,
			indexInfo.studyInstanceUID, indexInfo.seriesDescription, indexInfo.seriesInstanceUID, indexInfo.seriesNumber)
		.execute();
	std::cout << "insert DICOM index to DB success!" << std::endl;
}