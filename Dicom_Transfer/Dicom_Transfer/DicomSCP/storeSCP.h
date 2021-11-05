#ifndef STORE_SCP_H
#define STORE_SCP_H

#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/dcmnet/assoc.h"
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmnet/dimse.h"
#include <dcmtk/dcmnet/dcasccfg.h>

class MySCP
{
public:
	void setAbort(bool flag);
	void run();
private:
    OFCondition AcceptAssociation(T_ASC_Network* net,
        DcmAssociationConfiguration& asccfg);
    OFCondition ProcessCommands(T_ASC_Association* assoc);
    OFCondition EchoSCP(T_ASC_Association* assoc,
        T_DIMSE_Message* msg, T_ASC_PresentationContextID presID);
    OFCondition StoreSCP(T_ASC_Association* assoc,
        T_DIMSE_Message* msg, T_ASC_PresentationContextID presID);
private:
    bool abort_flag;
};
#endif // !STORE_SCP_H
