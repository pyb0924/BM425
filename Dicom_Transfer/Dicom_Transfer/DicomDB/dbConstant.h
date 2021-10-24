#ifndef DB_CONSTANT_H
#define DB_CONSTANT_H

#include <mysqlx/xdevapi.h>

using namespace mysqlx;

// login info
static std::string host = "localhost";
const int port = 33060;
static std::string username = "pyb0924";
static std::string password = "peter2000";

//DB info
static string schemaName = string("dicom");
static string seriesIndexStr = "series_index";
static std::string dataPath = "E:\\BM425\\data";

//SQL string
static string createIndexTableSQL = "CREATE TABLE (\
patientName VARCHAR(64) NOT NULL,\
patientID VARCHAR(64) NOT NULL,\
studyID VARCHAR(16) NOT NULL,\
studyInstanceUID VARCHAR(64) NOT NULL,\
seriesDescription VARCHAR(64) NOT NULL,\
seriesInstanceUID VARCHAR(64) NOT NULL UNIQUE,\
seriesNumber SMALLINT NOT NULL,\
PRIMARY KEY(seriesInstanceUID));";

static string createInstanceTableSQL = "CREATE TABLE (\
SOPInstanceUID VARCHAR(64) NOT NULL UNIQUE,\
imagePositionPatient VARCHAR(64) NOT NULL,\
imageOrientationPatient VARCHAR(16) NOT NULL,\
localFilePath VARCHAR(256) NOT NULL,\
PRIMARY KEY(SOPInstanceUID))";

#endif // !DB_CONSTANT_H

