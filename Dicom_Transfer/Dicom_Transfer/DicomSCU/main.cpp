#include <cassert>
#include <Windows.h>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <dcmtk/config/osconfig.h>
#include <dcmtk/dcmnet/diutil.h>
#include <dcmtk/dcmnet/scu.h> 

#include "DicomSCU.h"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

int main(int argc, char* argv[])
{
	CompressType eCompressType;
	int iCompressOption = 0;
	po::options_description cmd(OFFIS_CONSOLE_APPLICATION);
	std::string sDicomPath;
	cmd.add_options()
		("path,p", po::value<std::string>(&sDicomPath)->required(), "DICOM file path (file or directory)")
		("compress,c", po::value<int>(&iCompressOption)->default_value(0), "DICOM compress level")
		("help,h", "show help message");
	po::parse_command_line(argc, argv, cmd);

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, cmd), vm);
	po::notify(vm);

	eCompressType = static_cast<CompressType>(iCompressOption);

	OFLog::configure(OFLogger::DEBUG_LOG_LEVEL);
	DcmSCU scu;
	// set AET
	scu.setAETitle(APPLICATIONTITLE);
	// set peer SCP
	scu.setPeerHostName(PEERHOSTNAME);
	scu.setPeerPort(PEERPORT);
	scu.setPeerAETitle(PEERAPPLICATIONTITLE);

	// transfer syntaxes list
	OFList<OFString> ts;
	ts.push_back(UID_LittleEndianExplicitTransferSyntax);
	ts.push_back(UID_BigEndianExplicitTransferSyntax);
	ts.push_back(UID_LittleEndianImplicitTransferSyntax);
	scu.addPresentationContext(UID_VerificationSOPClass, ts); // C-ECHO: VerificationSOPClass
	scu.addPresentationContext(UID_MRImageStorage, ts); // C-STORE: VTImageStorage

	assert(scu.initNetwork().good());
	assert(scu.negotiateAssociation().good());

	// C-ECHO: test connection
	assert(scu.sendECHORequest(0).good());

	// C-STORE
	fs::path pDicomPath(sDicomPath);
	OFString file;
	if (fs::is_directory(pDicomPath))
	{
		fs::recursive_directory_iterator endIter;
		for (fs::recursive_directory_iterator iter(pDicomPath); iter != endIter; iter++)
		{
			file = iter->path().string().c_str();
			assert(ExecuteCStore(scu, UID_MRImageStorage, file, eCompressType));
		}
	}
	else {
		file = sDicomPath.c_str();
		assert(ExecuteCStore(scu, UID_MRImageStorage, file, eCompressType));
	}

	// close connection
	scu.closeAssociation(DCMSCU_RELEASE_ASSOCIATION);

	return 0;
}
