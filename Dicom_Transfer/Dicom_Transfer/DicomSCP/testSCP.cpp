#include "MySCP.h"
#include <boost/program_options.hpp>

namespace po = boost::program_options;

int main(int argc, char* argv)
{
	MySCP scp(104, "STORESCP", eCompressType::JPEG_COMPRESSION, "jpeg_test.dcm", true);
	scp.run();
	return 0;
}