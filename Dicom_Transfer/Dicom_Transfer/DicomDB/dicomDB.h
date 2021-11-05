#ifndef DICOMDB_H
#define DICOMDB_H

#include <string>
#include <mysqlx/xdevapi.h>

using namespace mysqlx;

struct DcmIndexInfo
{
	std::string SOPInstanceUID;
	std::string patientName;
	std::string patientID;
	std::string studyID;
	std::string studyInstanceUID;
	std::string seriesDescription;
	std::string seriesInstanceUID;
	short seriesNumber = 0;
};

Table CreateTableFromSQL(Session& session, const string& schemaName, const string& tableName, string createSQL);

void GetDcmIndexInfo(DcmFileFormat& fileformat, DcmIndexInfo& dcmIndexInfo);

void InsertDcmToDB(DcmFileFormat& fileformat, Table& table);
#endif