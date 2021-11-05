#include <iostream>
#include <cassert>

#include <boost/filesystem.hpp>
#include <dcmtk/dcmdata/dctk.h>
#include "dbConstant.h"
#include "dicomDB.h"

using namespace mysqlx;
namespace fs = boost::filesystem;

int main(int argc, const char* argv[])
{
	// Connect to MySQL Server and create table
	Session session(mydb::host, mydb::port, mydb::username, mydb::password);
	Table dcmTable = CreateTableFromSQL(session, mydb::schemaName, mydb::tableName, createTableSQL);
	assert(dcmTable.existsInDatabase());

	fs::path pPath("E:\\BM425\\data");

	fs::recursive_directory_iterator endIter;
	std::string dcmPath;

	DcmFileFormat fileformat;
	OFFilename dcmFileName;
	OFCondition status;

	for (fs::recursive_directory_iterator iter(pPath); iter != endIter; iter++)
	{
		dcmPath = iter->path().string();
		if (fs::is_directory(dcmPath))
		{
			continue;
		}
		dcmFileName = OFFilename(dcmPath.c_str());
		status = fileformat.loadFile(dcmFileName);
		if (status.bad())
		{
			throw "Dicom File Not Found!";
		}

		InsertDcmToDB(fileformat, dcmTable);
		std::cout << "Instance [" << iter->path().filename().string() << "] Info Inserted." << std::endl;
	}
	return 0;
}