#ifndef STORE_SCP_H
#define STORE_SCP_H

#include <dcmtk/config/osconfig.h>
#include <dcmtk/ofstd/ofcond.h>
#include <dcmtk/dcmnet/assoc.h>
#include <dcmtk/dcmnet/dimse.h>
#include <dcmtk/dcmnet/dcasccfg.h>
#include <dcmtk/dcmdata/dcfilefo.h>

enum class eCompressType
{
	NO_COMPRESSION = 0,
	JPEG_COMPRESSION
};

struct StoreCallbackData
{
	DcmFileFormat* dcmff;
	T_ASC_Association* assoc;
	E_TransferSyntax writeTransferSyntax = EXS_Unknown;
	OFString saveFileName;
	bool useDB;
};

static void StoreSCPCallback(void* callbackData, T_DIMSE_StoreProgress* progress, T_DIMSE_C_StoreRQ* req,
	char* imageFileName, DcmDataset** imageDataSet, T_DIMSE_C_StoreRSP* rsp, DcmDataset** statusDetail);

class MySCP
{
public:
	MySCP();
	MySCP(int port, std::string aeTitle, eCompressType type, std::string name,bool useDB);
	void setPort(int port);
	void setWriteTransferSyntax(eCompressType type);
	void setAETitle(std::string aeTitle);
	void setSaveFileName(const char* name);
	void setUseDB(bool useDB);
	void run();

private:
	OFCondition AcceptAssociation(T_ASC_Network* net, DcmAssociationConfiguration& asccfg);
	OFCondition ProcessCommands(T_ASC_Association* assoc);
	OFCondition EchoSCP(T_ASC_Association* assoc, T_DIMSE_Message* msg, T_ASC_PresentationContextID presID);
	OFCondition StoreSCP(T_ASC_Association* assoc, T_DIMSE_Message* msg, T_ASC_PresentationContextID presID);
private:
	int m_port = 104;
	std::string m_aeTitle = "STORESCP";
	E_TransferSyntax writeTransferSyntax = EXS_Unknown;
	OFString saveFileName = "test_jpeg.dcm";
	bool m_useDB= false;
};
#endif // !STORE_SCP_H
