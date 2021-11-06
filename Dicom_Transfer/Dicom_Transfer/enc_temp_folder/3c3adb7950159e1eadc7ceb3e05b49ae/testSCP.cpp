#include "MySCP.h"


int main(int argc, char* argv)
{
	MySCP scp(104, "STORESCP", eCompressType::JPEG_COMPRESSION, "jpeg_test.dcm",true);
	scp.run();
	return 0;
}