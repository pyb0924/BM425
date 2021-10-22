/*
*  Author:  Qin Yimin
*  Date: 2019-12-13
*  Purpose: Test Useage of the DcmSCU class
*/

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */ 
#include "dcmtk/dcmnet/diutil.h"
#include "dcmtk/dcmnet/scu.h" 
#include "dcmtk/dcmnet/scp.h"
#include "dcmtk/dcmnet/dstorscp.h"
#include <Windows.h> // ���ڴ���storeSCP�߳̽�������

#define OFFIS_CONSOLE_APPLICATION "testscu" 

static OFLogger echoscuLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);
static char rcsid[] = "$dcmtk:  v"  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

/* ��PACS����������Ҫ���õĲ��� */
// our application entity title used for calling the peer machine	����ʹ�õ�AET
#define APPLICATIONTITLE     "ACME1" 

// host name of the peer machine	SCP��������
#define PEERHOSTNAME         "localhost" 

// TCP/IP port to connect to the peer machine	SCP���ŵĶ˿ںţ�����ʱ��ο�PACS�����ļ�
#define PEERPORT 5678

// application entity title of the peer machine		SCPʹ�õ�AET������ʱ��ο�PACS�����ļ�
#define PEERAPPLICATIONTITLE "CONQUESTSRV1" 

// C-STORE ��������Ҫ���͵��ļ���
# define STOREFILENAME "E:\\DCMTK\\CT5.dcm"

// C-STORE �� C-GET �������������ݵĶ˿ں�
#define STORESCPPORT 1235

// ���ظ���scu��������������ģ�Presentation Context��ID
DWORD WINAPI storeScpThread(LPVOID lpParameter) {
	DcmStorageSCP* pSCP = (DcmStorageSCP*)lpParameter;
	pSCP->listen(); // ����storeSCP����C-MOVE����
	return 0L;
}

static Uint8 findUncompressedPC(const OFString& sopClass,
	DcmSCU& scu)
{
	Uint8 pc;
	pc = scu.findPresentationContextID(sopClass, UID_LittleEndianExplicitTransferSyntax);
	if (pc == 0)
		pc = scu.findPresentationContextID(sopClass, UID_BigEndianExplicitTransferSyntax);
	if (pc == 0)
		pc = scu.findPresentationContextID(sopClass, UID_LittleEndianImplicitTransferSyntax);
	return pc;
}

int main(int argc, char* argv[])
{
	/* ���ý���������Ҫ�Ĳ��� */
	/* Setup DICOM connection parameters */
	OFLog::configure(OFLogger::DEBUG_LOG_LEVEL); // ʹ��DEBUGģʽ����־��������ӽ�������ϸ��Ϣ
	DcmSCU scu;	// ����һ��DcmSCU����
	// set AE titles 
	scu.setAETitle(APPLICATIONTITLE); // ����ʹ�õ�AET����Ҫ����PACS�����ļ�����
	scu.setPeerHostName(PEERHOSTNAME); // SCP��������������Ǳ�������Ϊ localhost
	scu.setPeerPort(PEERPORT); // SCP�Ķ˿ں�
	scu.setPeerAETitle(PEERAPPLICATIONTITLE); // SCP��AET����Ҫ����PACS�����ļ�����
	/* ���ý���������Ҫ�Ĳ��� */

	// Use presentation context for FIND/MOVE in study root, propose all uncompressed transfer syntaxes 
	// �ṩ��SCPѡ��Ĵ������壨Transfer syntax���б�
	OFList<OFString> ts;
	ts.push_back(UID_LittleEndianExplicitTransferSyntax);
	ts.push_back(UID_BigEndianExplicitTransferSyntax);
	ts.push_back(UID_LittleEndianImplicitTransferSyntax);

	// ���ݳ������壨abstract syntax���ʹ��������б�������������
	scu.addPresentationContext(UID_VerificationSOPClass, ts); // C-ECHO������abstract syntaxΪ"VerificationSOPClass"
	scu.addPresentationContext(UID_CTImageStorage, ts); // C-STORE�������������Ŀ���Ǵ洢CTͼ��

	/* ��PACSϵͳ�������� */
	/* Initialize network */
	OFCondition result = scu.initNetwork();
	if (result.bad()) // �����ʼ��ʧ��
	{
		DCMNET_ERROR("Unable to set up the network: " << result.text());
		return 1;
	}
	/* Negotiate Association */
	result = scu.negotiateAssociation();
	if (result.bad()) // �޷���������
	{
		DCMNET_ERROR("Unable to negotiate association: " << result.text());
		return 1;
	}
	/* ��PACSϵͳ�������� */

	/* Let's look whether the server is listening:
	Assemble and send C-ECHO request
	*/
	result = scu.sendECHORequest(0);
	if (result.bad()) // C-ECHO����ʧ�ܣ�Ӧ���SCP��hostname��port����
	{
		DCMNET_ERROR("Could not process C-ECHO with the server: " << result.text());
		return 1;
	}


	/* C-STORE ���� */
	/* Assemble and send C-STORE request */
	T_ASC_PresentationContextID presID = findUncompressedPC(UID_CTImageStorage, scu);
	if (presID == 0)
	{
		DCMNET_ERROR("There is no uncompressed presentation context for C-STORE");
		return 1;
	}
	Uint16 rspStatusCode;
	result = scu.sendSTORERequest(presID, STOREFILENAME, NULL, rspStatusCode);
	if (result.bad()) {
		DCMNET_ERROR("C-STORE Operation failed.");
	}
	else {
		DCMNET_INFO("C-STORE Operation completed successfully.");
	}

	/* Release association */
	scu.closeAssociation(DCMSCU_RELEASE_ASSOCIATION);

	return 0;
}
