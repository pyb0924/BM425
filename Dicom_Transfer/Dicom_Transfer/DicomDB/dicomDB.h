#ifndef DICOMDB_H
#define DICOMDB_H

#include <string>
#include <mysqlx/xdevapi.h>

using namespace mysqlx;

struct SeriesIndexInfo
{
	std::string patientName;
	std::string patientID;
	std::string studyID;
	std::string studyInstanceUID;
	std::string seriesDescription;
	std::string seriesInstanceUID;
	short seriesNumber = 0;
};

struct SeriesInstanceInfo
{
	std::string SOPInstanceUID;
	std::string imagePositionPatient;
	std::string imageOrientationPatient;
};

void GetSeriesIndexInfo(std::string dcmPath, SeriesIndexInfo& seriesIndexInfo);

void GetSeriesInstanceInfo(std::string dcmPath, SeriesInstanceInfo& seriesInstanceInfo);

Table CreateTableFromSQL(Session& session, const string& schemaName, const string& tableName, string& createSQL);

#endif