#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstdinc.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/dcmnet/cond.h"
#include "dcmtk/ofstd/ofdatime.h"
#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmnet/dcasccfg.h"
#include "dcmtk/dcmnet/dcasccff.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmsr/dsrdoc.h"
#include "dcmtk/dcmdata/dcmetinf.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcostrmz.h"
#include <dcmtk/dcmjpeg/djencode.h>
#include <dcmtk/dcmjpeg/djrplol.h>

#include "../DicomJPEG/dicomJPEG.h"
#include "../DicomDB/dicomDB.h"

#include "MySCP.h"

static void StoreSCPCallback(void* callbackData, T_DIMSE_StoreProgress* progress, T_DIMSE_C_StoreRQ* req,
	char* imageFileName, DcmDataset** imageDataSet, T_DIMSE_C_StoreRSP* rsp, DcmDataset** statusDetail) {
	DIC_UI sopClass;
	DIC_UI sopInstance;
	if (progress->state == DIMSE_StoreEnd) {
		*statusDetail = nullptr;
		StoreCallbackData* cbdata = OFstatic_cast(StoreCallbackData*, callbackData);

		if (cbdata->useDB)
		{
			Session session(mydb::host, mydb::port, mydb::username, mydb::password);
			Table dcmTable = CreateTableFromSQL(session, mydb::schemaName, "test_MySCP", createTableSQL);
			assert(dcmTable.existsInDatabase());
			InsertDcmToDB(*(cbdata->dcmff), dcmTable);
			std::cout << "insert DICOM index to DB success!" << std::endl;
		}
		
		if (rsp->DimseStatus == STATUS_Success) {
			if (!DU_findSOPClassAndInstanceInDataSet(*imageDataSet,
				sopClass, sizeof(sopClass),
				sopInstance, sizeof(sopInstance))) {
				rsp->DimseStatus = STATUS_STORE_Error_CannotUnderstand;
			}
			else if (strcmp(sopClass, req->AffectedSOPClassUID) != 0) {
				rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
			}
			else if (strcmp(sopInstance, req->AffectedSOPInstanceUID) != 0) {
				rsp->DimseStatus = STATUS_STORE_Error_DataSetDoesNotMatchSOPClass;
			}
		}

		if (cbdata->writeTransferSyntax != EXS_Unknown)
		{
			DJEncoderRegistration::registerCodecs(); // register JPEG codecs

			DcmDataset* dataset = cbdata->dcmff->getDataset();
			DJ_RPLossless params; // codec parameters, we use the defaults

			// this causes the lossless JPEG version of the dataset to be created
			if (dataset->chooseRepresentation(cbdata->writeTransferSyntax, &params).good() &&
				dataset->canWriteXfer(cbdata->writeTransferSyntax))
			{
				std::cout << "compress DICOM file: Success" << std::endl;
			}
			DJEncoderRegistration::cleanup();
		}
		cbdata->dcmff->saveFile(cbdata->saveFileName, cbdata->writeTransferSyntax);
		std::cout << "DICOM file ["<<cbdata->saveFileName.c_str()<< "] saved!" << std::endl;
	}
}

MySCP::MySCP()
{
}

MySCP::MySCP(int port, std::string aeTitle, eCompressType type, std::string name,bool useDB)
{
	m_port = port;
	m_aeTitle = aeTitle;
	m_useDB = useDB;
	setWriteTransferSyntax(type);
	setSaveFileName(name.c_str());
}

void MySCP::setWriteTransferSyntax(eCompressType compressType)
{
	switch (compressType)
	{
	case eCompressType::NO_COMPRESSION:
		writeTransferSyntax = EXS_Unknown;
		break;
	case eCompressType::JPEG_COMPRESSION:
		writeTransferSyntax = EXS_JPEGProcess14SV1;
		break;
	default:
		break;
	}
}
void MySCP::setPort(int port)
{
	m_port = port;
}
void MySCP::setAETitle(std::string aeTitle)
{
	m_aeTitle = aeTitle;
}

void MySCP::setSaveFileName(const char* name)
{
	saveFileName = OFString(name);
}

void MySCP::setUseDB(bool useDB)
{
	m_useDB = useDB;
}

void MySCP::run()
{
	T_ASC_Network* net;
	DcmAssociationConfiguration asccfg;
	OFString temp_str;
	int port = 104;
	OFCondition cond = ASC_initializeNetwork(NET_ACCEPTOR, port, 30, &net);
	if (cond.bad()) {
		DimseCondition::dump(temp_str, cond);
	}

	while (cond.good()) {
		cond = AcceptAssociation(net, asccfg);
	}

	// release network
	if (cond.good()) {
		cond = ASC_dropNetwork(&net);
	}
	if (cond.bad()) {
		DimseCondition::dump(temp_str, cond);
		std::cout << temp_str.c_str() << std::endl;
	}
}

OFCondition MySCP::ProcessCommands(T_ASC_Association* assoc)
{
	OFCondition cond = EC_Normal;
	T_DIMSE_Message msg;
	T_ASC_PresentationContextID presID = 0;
	DcmDataset* statusDetail = NULL;

	// start a loop to be able to receive more than one DIMSE command
	while (cond == EC_Normal || cond == DIMSE_NODATAAVAILABLE || cond == DIMSE_OUTOFRESOURCES)
	{
		// receive a DIMSE command over the network
		cond = DIMSE_receiveCommand(assoc, DIMSE_BLOCKING, 0, &presID, &msg, &statusDetail);
		if (cond == EC_Normal)
		{
			// storescp can only process a C-ECHO-RQ and a C-STORE-RQ
			switch (msg.CommandField)
			{
			case DIMSE_C_ECHO_RQ:
				// process C-ECHO-Request
				cond = EchoSCP(assoc, &msg, presID);
				break;
			case DIMSE_C_STORE_RQ:
				// process C-STORE-Request
				cond = StoreSCP(assoc, &msg, presID);
				break;
			default:
				// we cannot handle this kind of message
				cond = DIMSE_BADCOMMANDTYPE;
				break;
			}
		}
	}
	return cond;
}

OFCondition MySCP::EchoSCP(T_ASC_Association* assoc, T_DIMSE_Message* msg, T_ASC_PresentationContextID presID)
{
	OFString temp_str;
	OFCondition cond = DIMSE_sendEchoResponse(assoc, presID, &msg->msg.CEchoRQ, STATUS_Success, nullptr);
	if (cond.bad()) {
		DimseCondition::dump(temp_str, cond);
		std::cout << "Echo SCP failed: " << temp_str.c_str() << std::endl;
	}
	else {
		std::cout << "Echo SCP success!" << std::endl;
	}
	return cond;
}

OFCondition MySCP::StoreSCP(T_ASC_Association* assoc, T_DIMSE_Message* msg, T_ASC_PresentationContextID presID)
{
	OFCondition cond = EC_Normal;
	T_DIMSE_C_StoreRQ* req;
	req = &msg->msg.CStoreRQ;
	StoreCallbackData callbackData;
	DcmFileFormat dcmff;

	callbackData.assoc = assoc;
	callbackData.dcmff = &dcmff;
	callbackData.saveFileName = saveFileName;
	callbackData.writeTransferSyntax = writeTransferSyntax;
	callbackData.useDB = m_useDB;

	const char* aet = nullptr;
	const char* aec = nullptr;

	if (assoc && assoc->params) {
		aet = assoc->params->DULparams.callingAPTitle;
		aec = assoc->params->DULparams.calledAPTitle;
		if (aet) {
			dcmff.getMetaInfo()->putAndInsertString(DCM_SourceApplicationEntityTitle, aet);
		}
	}

	if (std::string(aec) != m_aeTitle) {
		std::cout << "aet title check error: " << aet << std::string(aec) << m_aeTitle;
	}
	else
	{
		DcmDataset* dset = dcmff.getDataset();
		cond = DIMSE_storeProvider(assoc, presID, req, nullptr, OFTrue, &dset,
			StoreSCPCallback, &callbackData, DIMSE_BLOCKING, 0);

		if (cond.bad()) {
			OFString temp_str;
			DimseCondition::dump(temp_str, cond);
			std::cout << "Store SCP failed!" << std::endl;
		}
		else {
			std::cout << "Store SCP success!" << std::endl;
		}
	}

	return cond;
}

OFCondition MySCP::AcceptAssociation(
	T_ASC_Network* net, DcmAssociationConfiguration& asccfg) {

	// init network
	char buf[BUFSIZ];
	T_ASC_Association* assoc;
	OFCondition cond;
	OFString temp_str;
	const char* knownAbstractSyntaxes[] = {
		UID_VerificationSOPClass
	};
	const char* transferSyntaxes[] = {
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr
	};
	int numTransferSyntaxes = 0;

	cond = ASC_receiveAssociation(net, &assoc, ASC_DEFAULTMAXPDU);
	// deal with error
	if (cond.bad()) {
		DimseCondition::dump(temp_str, cond);
		std::cout << "接收关联失败: " << temp_str.c_str() << std::endl;
	}
	if (gLocalByteOrder == EBO_LittleEndian) { /* defined in dcxfer.h */
		transferSyntaxes[0] = UID_LittleEndianExplicitTransferSyntax;
		transferSyntaxes[1] = UID_BigEndianExplicitTransferSyntax;
	}
	else {
		transferSyntaxes[0] = UID_BigEndianExplicitTransferSyntax;
		transferSyntaxes[1] = UID_LittleEndianExplicitTransferSyntax;
	}
	transferSyntaxes[2] = UID_LittleEndianImplicitTransferSyntax;
	numTransferSyntaxes = 3;

	if (cond.good()) {
		cond = ASC_acceptContextsWithPreferredTransferSyntaxes(
			assoc->params, knownAbstractSyntaxes,
			DIM_OF(knownAbstractSyntaxes), transferSyntaxes, numTransferSyntaxes);
		if (cond.bad()) {
			DimseCondition::dump(temp_str, cond);
			std::cout << temp_str.c_str() << std::endl;
		}
	}

	if (cond.good()) {
		cond = ASC_acceptContextsWithPreferredTransferSyntaxes(
			assoc->params, dcmAllStorageSOPClassUIDs,
			numberOfDcmAllStorageSOPClassUIDs,
			transferSyntaxes, numTransferSyntaxes);
		if (cond.bad()) {
			DimseCondition::dump(temp_str, cond);
			std::cout << temp_str.c_str() << std::endl;
		}
	}

	// check aet title
	ASC_setAPTitles(assoc->params, nullptr, nullptr, m_aeTitle.c_str());

	if (cond.good()) {
		cond = ASC_getApplicationContextName(assoc->params, buf, sizeof(buf));
		if ((cond.bad()) || strcmp(buf, UID_StandardApplicationContext) != 0) {
			T_ASC_RejectParameters rej = {
				ASC_RESULT_REJECTEDPERMANENT,
				ASC_SOURCE_SERVICEUSER,
				ASC_REASON_SU_APPCONTEXTNAMENOTSUPPORTED
			};
			DimseCondition::dump(temp_str, cond);
			std::cout << "assocation failed: context name error! " << buf << std::endl;
			cond = ASC_rejectAssociation(assoc, &rej);
			if (cond.bad()) {
				DimseCondition::dump(temp_str, cond);
				std::cout << temp_str.c_str() << std::endl;
			}
		}
		else {
			cond = ASC_acknowledgeAssociation(assoc);
			if (cond.bad()) {
				DimseCondition::dump(temp_str, cond);
				std::cout << temp_str.c_str() << std::endl;
			}
		}
	}

	if (cond.good()) {

		DIC_AE callingTitle;
		DIC_AE calledTitle;
		ASC_getAPTitles(assoc->params, callingTitle, sizeof(callingTitle),
			calledTitle, sizeof(calledTitle), nullptr, 0).good();

		cond = ProcessCommands(assoc);
		if (cond == DUL_PEERREQUESTEDRELEASE) {
			cond = ASC_acknowledgeRelease(assoc);
		}
		else {
			DimseCondition::dump(temp_str, cond);
			std::cout << "DIMSE failed（abort association）:" << temp_str.c_str() << std::endl;

			cond = ASC_abortAssociation(assoc);
		}
	}

	// clean up
	cond = ASC_dropSCPAssociation(assoc);
	if (cond.bad()) {
		DimseCondition::dump(temp_str, cond);
		std::cout << temp_str.c_str() << std::endl;
	}
	cond = ASC_destroyAssociation(&assoc);
	if (cond.bad()) {
		DimseCondition::dump(temp_str, cond);
		std::cout << temp_str.c_str() << std::endl;
	}
	return cond;
}
