#include"protocol.h"//协议
PDU *mkPDU(uint uiMsgLen)
{
    uint uiPDULen=sizeof (PDU)+uiMsgLen;
    PDU *pdu=(PDU*)malloc(uiPDULen);
    if(NULL==pdu)
    {
        exit(EXIT_FAILURE);
    }
    memset(pdu,0,uiPDULen);//清空
    pdu->uiPDULen=uiPDULen;
    pdu->uiMsgLen=uiMsgLen;
    return pdu;
}//协议动态空间申请
