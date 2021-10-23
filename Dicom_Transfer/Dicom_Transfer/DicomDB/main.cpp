#include <iostream>
#include <fstream>
#include <assert.h>

#include <boost/filesystem.hpp>
#include <mysqlx/xdevapi.h>

#include <dcmtk/dcmdata/dcfilefo.h>
#include <dcmtk/dcmdata/dcdeftag.h>

#include "dicomDB.h"

using namespace ::mysqlx;
using namespace boost::filesystem;

string createIndexTableSQL = "CREATE TABLE (\
patientName VARCHAR(64) NOT NULL,\
patientID VARCHAR(64) NOT NULL,\
studyID VARCHAR(16) NOT NULL,\
studyInstanceUID VARCHAR(64) NOT NULL,\
seriesDescription VARCHAR(64) NOT NULL,\
seriesInstanceUID VARCHAR(64) NOT NULL UNIQUE,\
seriesNumber SMALLINT NOT NULL,\
PRIMARY KEY(seriesInstanceUID));";

string createInstanceTableSQL = "CREATE TABLE (\
SOPInstanceUID VARCHAR(64) NOT NULL UNIQUE,\
imagePosition VARCHAR(16) NOT NULL,\
PRIMARY KEY(seriesInstanceUID))";

string seriesIndexStr = "seriesIndex";
string schemaName = string("test");
std::string sPath = "E:\\BM425\\data";

int main(int argc, const char* argv[])
{
	// Connect to MySQL Server and create table
	
	//string url = "mysqlx://localhost:33060/test?user=pyb0924&password=peter2000";
	//Session session(url);
	Session session("localhost", 33060, "pyb0924", "peter2000");
	session.createSchema(schemaName, true);
	Table seriesIndexTable = CreateTableFromSQL(session, schemaName, seriesIndexStr,createIndexTableSQL);
	assert(seriesIndexTable.existsInDatabase());
	
	path pPath(sPath);
	directory_iterator endIter;
	std::string dcmPath;
	SeriesIndexInfo indexInfo;
	SeriesInstanceInfo instanceInfo;
	bool isFirstFile = true;
	std::string seriesName;

	for (directory_iterator iter(pPath); iter != endIter; iter++)
	{
		isFirstFile = true;
		path seriesPath(iter->path().string());
		std::cout << "Dealing with Series [" << iter->path().filename().string() << "]"<<std::endl;
		seriesName = iter->path().filename().string();
		Table seriesInstanceTable = CreateTableFromSQL(session, schemaName,seriesName, createInstanceTableSQL);
		assert(seriesInstanceTable.existsInDatabase());

		for (directory_iterator seriesIter(seriesPath); seriesIter != endIter; seriesIter++)
		{
			dcmPath = seriesIter->path().string();
			// First file: add record to seriesIndex table
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
			// All: add record to seriesInstance table
			GetSeriesInstanceInfo(dcmPath, instanceInfo);
			seriesInstanceTable.insert("SOPInstanceUID", "imagePositionPatient", "imageOrientationPatient")
				.values(instanceInfo.SOPInstanceUID, instanceInfo.imagePositionPatient, instanceInfo.imageOrientationPatient)
				.execute();
			std::cout << "Instance [" << seriesIter->path().filename().string() << "] Info Inserted." << std::endl;
		}
	}
	return 0;
}