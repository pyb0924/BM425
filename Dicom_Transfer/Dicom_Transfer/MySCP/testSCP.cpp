#include "MySCP.h"
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char const* argv[])
{
	int port;
	std::string aetName;
	std::string sDicomPath;
	std::string outputFileName;
	int iCompressOption = 0;

	po::options_description cmd("MySCP");
	cmd.add_options()
		("port,p",po::value<int>(&port)->default_value(104),"SCP port")
		("name,n",po::value<std::string>(&aetName)->default_value("STORESCP"),"SCP AET title")
		("db,d","if SCP use database")
		("output,o",po::value<std::string>(&outputFileName)->default_value("jpeg_test.dcm"),"output file name")
		("compress,c", "if compress to JPEG")
		("help,h", "show help message");

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, cmd), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		std::cout << cmd << std::endl;
		return 1;
	}

	bool useDB = vm.count("db") ? true : false;

	eCompressType type;
	if (vm.count("compress"))
	{
		type = eCompressType::JPEG_COMPRESSION;
	}
	else
	{
		type = eCompressType::NO_COMPRESSION;
	}
	
	MySCP scp(port, aetName, type, outputFileName, useDB);
	scp.run();
	return 0;
}