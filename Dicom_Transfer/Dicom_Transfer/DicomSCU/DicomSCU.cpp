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
#include <Windows.h> // 用于创建storeSCP线程接收数据

#define OFFIS_CONSOLE_APPLICATION "testscu" 

static OFLogger echoscuLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);
static char rcsid[] = "$dcmtk:  v"  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

/* 与PACS建立连接需要设置的参数 */
// our application entity title used for calling the peer machine	本机使用的AET
#define APPLICATIONTITLE     "ACME1" 

// host name of the peer machine	SCP的主机名
#define PEERHOSTNAME         "localhost" 

// TCP/IP port to connect to the peer machine	SCP开放的端口号，设置时请参考PACS配置文件
#define PEERPORT 5678

// application entity title of the peer machine		SCP使用的AET，设置时请参考PACS配置文件
#define PEERAPPLICATIONTITLE "CONQUESTSRV1" 

// C-STORE 操作中想要发送的文件名
# define STOREFILENAME "E:\\DCMTK\\CT5.dcm"

// C-STORE 或 C-GET 中用来接收数据的端口号
#define STORESCPPORT 1235

// 返回给定scu对象的描述上下文（Presentation Context）ID
DWORD WINAPI storeScpThread(LPVOID lpParameter) {
	DcmStorageSCP* pSCP = (DcmStorageSCP*)lpParameter;
	pSCP->listen(); // 开启storeSCP监听C-MOVE请求
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
	/* 设置建立连接需要的参数 */
	/* Setup DICOM connection parameters */
	OFLog::configure(OFLogger::DEBUG_LOG_LEVEL); // 使用DEBUG模式的日志，输出连接建立的详细信息
	DcmSCU scu;	// 创建一个DcmSCU对象
	// set AE titles 
	scu.setAETitle(APPLICATIONTITLE); // 本机使用的AET，需要根据PACS配置文件设置
	scu.setPeerHostName(PEERHOSTNAME); // SCP的主机名，如果是本机设置为 localhost
	scu.setPeerPort(PEERPORT); // SCP的端口号
	scu.setPeerAETitle(PEERAPPLICATIONTITLE); // SCP的AET，需要根据PACS配置文件设置
	/* 设置建立连接需要的参数 */

	// Use presentation context for FIND/MOVE in study root, propose all uncompressed transfer syntaxes 
	// 提供供SCP选择的传输语义（Transfer syntax）列表
	OFList<OFString> ts;
	ts.push_back(UID_LittleEndianExplicitTransferSyntax);
	ts.push_back(UID_BigEndianExplicitTransferSyntax);
	ts.push_back(UID_LittleEndianImplicitTransferSyntax);

	// 根据抽象语义（abstract syntax）和传输语义列表构建描述上下文
	scu.addPresentationContext(UID_VerificationSOPClass, ts); // C-ECHO操作，abstract syntax为"VerificationSOPClass"
	scu.addPresentationContext(UID_CTImageStorage, ts); // C-STORE操作，本程序的目的是存储CT图像

	/* 与PACS系统建立连接 */
	/* Initialize network */
	OFCondition result = scu.initNetwork();
	if (result.bad()) // 网络初始化失败
	{
		DCMNET_ERROR("Unable to set up the network: " << result.text());
		return 1;
	}
	/* Negotiate Association */
	result = scu.negotiateAssociation();
	if (result.bad()) // 无法建立连接
	{
		DCMNET_ERROR("Unable to negotiate association: " << result.text());
		return 1;
	}
	/* 与PACS系统建立连接 */

	/* Let's look whether the server is listening:
	Assemble and send C-ECHO request
	*/
	result = scu.sendECHORequest(0);
	if (result.bad()) // C-ECHO操作失败，应检查SCP的hostname和port设置
	{
		DCMNET_ERROR("Could not process C-ECHO with the server: " << result.text());
		return 1;
	}


	/* C-STORE 操作 */
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
