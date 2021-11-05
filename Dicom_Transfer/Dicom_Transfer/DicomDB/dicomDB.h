#ifndef DICOMDB_H
#define DICOMDB_H

#include <string>
#include <mysqlx/xdevapi.h>

using namespace mysqlx;

// DB info
namespace mydb {
	static std::string host = "localhost";
	const int port = 33060;
	static std::string username = "pyb0924";
	static std::string password = "peter2000";
	static string schemaName = "dicom";
	static string tableName = "test_dicom_db";
};


//SQL string
static string createTableSQL = "CREATE TABLE %s (\
SOPInstanceUID VARCHAR(64) NOT NULL UNIQUE,\
patientName VARCHAR(64) NOT NULL,\
patientID VARCHAR(64) NOT NULL,\
studyID VARCHAR(16) NOT NULL,\
studyInstanceUID VARCHAR(64) NOT NULL,\
seriesDescription VARCHAR(64) NOT NULL,\
seriesInstanceUID VARCHAR(64) NOT NULL,\
seriesNumber SMALLINT NOT NULL,\
PRIMARY KEY(SOPInstanceUID))";

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