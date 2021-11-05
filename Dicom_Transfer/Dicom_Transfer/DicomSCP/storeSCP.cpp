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
#include "storeSCP.h"

#include "../DicomJPEG/dicomJPEG.h"
#include "../DicomDB/dicomDB.h"
struct StoreCallbackData {
    DcmFileFormat* dcmff;
    T_ASC_Association* assoc;
    std::string patientInfo;
    std::string study;
};
struct StoreCallbackData
{
    char* imageFileName;
    DcmFileFormat* dcmff;
    T_ASC_Association* assoc;
};

struct LocalSettings {
    int port;
    std::string aeTitle;
};

static void StoreSCPCallback(
    void* callbackData,
    T_DIMSE_StoreProgress* progress,
    T_DIMSE_C_StoreRQ* req,
    char* /*imageFileName*/, DcmDataset** imageDataSet,
    T_DIMSE_C_StoreRSP* rsp,
    DcmDataset** statusDetail) {
    DIC_UI sopClass;
    DIC_UI sopInstance;
    if (progress->state == DIMSE_StoreEnd) {
        *statusDetail = nullptr;
        StoreCallbackData* cbdata = OFstatic_cast(StoreCallbackData*, callbackData);
        InsertImageToDB(cbdata->dcmff, cbdata->study, cbdata->patientInfo);
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
    }
}

static void InsertImageToDB(DcmFileFormat* ff, std::string study, std::string patientInfo)
{

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
    //-------------------------------�󶨶˿��ṩscp����--------------------------------------//
    while (cond.good()&& !abort_flag) {
        /* ���չ�����ȷ�ϻ�ܾ�����
         * ��������ϵ�õ����ϣ�
         * �ṩ��Ӧ�ķ��񣬲�������Ҫ����һ���������� */
        cond = AcceptAssociation(net, asccfg);
    }
    //--------------------------------���ٶ˿ڼ���-------------------------------------//
    /* �ͷ��ڴ� T_ASC_Network*.
     * �˵�����������õ�ASC_initializeNetwork���������Ӧ�� */
    if (cond.good()) {
        cond = ASC_dropNetwork(&net);
    }
    if (cond.bad()) {
        DimseCondition::dump(temp_str, cond);
        std::cout << temp_str.c_str() << std::endl;
    }
}

OFCondition MySCP::ProcessCommands(T_ASC_Association * assoc)
/*
 * This function receives DIMSE commands over the network connection
 * and handles these commands correspondingly. Note that in case of
 * storescp only C-ECHO-RQ and C-STORE-RQ commands can be processed.
 *
 * Parameters:
 *   assoc - [in] The association (network connection to another DICOM application).
 */
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
       
        // check what kind of error occurred. If no data was
        // received, check if certain other conditions are met
        
        // check if peer did release or abort, or if we have a valid message
        if (cond == EC_Normal)
        {
            // in case we received a valid message, process this command
            // note that storescp can only process a C-ECHO-RQ and a C-STORE-RQ
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
    // ��ʼ��һЩ����
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
        // ��C-STORE-RQ�����ʵ����Ϣ������ֲ�����
        req = &msg->msg.CStoreRQ;
        // ��ʼ��һЩ����
        StoreCallbackData callbackData;
        DcmFileFormat dcmff;
      
        callbackData.assoc = assoc;
        callbackData.dcmff = &dcmff;
        const char* aet = nullptr;
        const char* aec = nullptr;
        // �� SourceApplicationEntityTitle �洢�� metaheader ��
        if (assoc && assoc->params) {
            aet = assoc->params->DULparams.callingAPTitle;
            aec = assoc->params->DULparams.calledAPTitle;
            if (aet) {
                dcmff.getMetaInfo()->putAndInsertString(DCM_SourceApplicationEntityTitle, aet);
            }
        }
        LocalSettings settings;
        std::string aeTitle = settings.aeTitle;
        if (std::string(aec) != aeTitle) {
            std::cout << "����У��ʧ��" << aet << std::string(aec) << aeTitle;
        }
        else {
            // ����һ����ַ�����ڴ洢ͨ��������յ���Ϣ
            DcmDataset* dset = dcmff.getDataset();
            cond = DIMSE_storeProvider(assoc, presID, req, nullptr, OFTrue, &dset,
                StoreSCPCallback, &callbackData, DIMSE_BLOCKING, 0);
           
            // ������ִ�����ת����Ӧ����Ϣ����Ҫʱɾ������ļ�
            if (cond.bad()) {
                OFString temp_str;
                DimseCondition::dump(temp_str, cond);
                std::cout << "Store SCP failed"<<std::endl;
            }
            else {
                std::cout << "Store SCP success" << std::endl;
            }
        }
        // ���ط���ֵ
        return cond;
}
