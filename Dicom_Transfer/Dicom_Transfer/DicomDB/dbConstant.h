#ifndef DB_CONSTANT_H
#define DB_CONSTANT_H

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

#endif // !DB_CONSTANT_H

