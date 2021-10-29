#include <iostream>
#include <cassert>

#include <boost/filesystem.hpp>

#include "dbConstant.h"
#include "dicomDB.h"

using namespace mysqlx;
using namespace boost::filesystem;

int main(int argc, const char* argv[])
{
	// Connect to MySQL Server and create table
	Session session(host, port, username, password);
	Table seriesIndexTable = CreateTableFromSQL(session, schemaName, seriesIndexStr, createIndexTableSQL);
	assert(seriesIndexTable.existsInDatabase());

	path pPath(dataPath);
	SeriesIndexInfo indexInfo;
	SeriesInstanceInfo instanceInfo;

	directory_iterator endIter;
	bool isFirstFile = true;
	std::string dcmPath;
	std::string seriesName;

	for (directory_iterator iter(pPath); iter != endIter; iter++)
	{
		isFirstFile = true;
		path seriesPath(iter->path().string());
		std::cout << "Dealing with Series [" << iter->path().filename().string() << "]" << std::endl;
		seriesName = iter->path().filename().string();
		Table seriesInstanceTable = CreateTableFromSQL(session, schemaName, seriesName, createInstanceTableSQL);
		assert(seriesInstanceTable.existsInDatabase());

		for (directory_iterator seriesIter(seriesPath); seriesIter != endIter; seriesIter++)
		{
			dcmPath = seriesIter->path().string();
			// First file: add series index
			if (isFirstFile)
			{
				GetSeriesIndexInfo(dcmPath, indexInfo);
				seriesIndexTable
					.insert("patientName", "patientID", "studyID", "studyInstanceUID",
					        "seriesDescription", "seriesInstanceUID", "seriesNumber")
					.values(indexInfo.patientName, indexInfo.patientID, indexInfo.studyID, indexInfo.studyInstanceUID,
					        indexInfo.seriesDescription, indexInfo.seriesInstanceUID, indexInfo.seriesNumber)
					.execute();
				isFirstFile = false;
				std::cout << "Series [" << iter->path().filename().string() << "] Index Inserted." << std::endl;
			}
			// All file: add instance in series
			GetSeriesInstanceInfo(dcmPath, instanceInfo);
			seriesInstanceTable
				.insert("SOPInstanceUID", "imagePositionPatient", "imageOrientationPatient", "localFilePath")
				.values(instanceInfo.SOPInstanceUID, instanceInfo.imagePositionPatient,
				        instanceInfo.imageOrientationPatient, dcmPath)
				.execute();
			std::cout << "Instance [" << seriesIter->path().filename().string() << "] Info Inserted." << std::endl;
		}
	}
	return 0;
}