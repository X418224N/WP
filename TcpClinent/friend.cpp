#include "friend.h"
#include"protocol.h"
#include"tcpclinent.h"
#include<QInputDialog>
#include<QDebug>
#include"privatechat.h"
#include<QMessageBox>
Friend::Friend(QWidget *parent) : QWidget(parent)
{
    m_pShowMsgTE=new QTextEdit;//显示信息
    m_pFriendListWidget= new QListWidget;//好友列表
    m_PInputMsgLe=new QLineEdit;//输入框
    m_pDelFriendPB=new QPushButton("删除好友");//删除好友
    m_pFlushFriendPB=new QPushButton("刷新好友");//刷新好友
    m_pShowOnlineUsrPB=new QPushButton("显示在线用户");//显示在线用户
    m_pSearchUsrPB=new QPushButton("查找用户");//查找用户
    m_pMsgSendPB=new QPushButton("信息发送");//
    m_pPrivateChatPB=new QPushButton("私聊");//

    QVBoxLayout *pLeftPBVBL=new QVBoxLayout;//垂直布局
    pLeftPBVBL->addWidget(m_pDelFriendPB);
    pLeftPBVBL->addWidget(m_pFlushFriendPB);
    pLeftPBVBL->addWidget(m_pShowOnlineUsrPB);
    pLeftPBVBL->addWidget(m_pSearchUsrPB);
    pLeftPBVBL->addWidget(m_pPrivateChatPB);

    QHBoxLayout *pTopHBL=new QHBoxLayout;//水平布局
    pTopHBL->addWidget(m_pShowMsgTE);
    pTopHBL->addWidget(m_pFriendListWidget);
    pTopHBL->addLayout(pLeftPBVBL);

    QHBoxLayout *pMsgHBL=new QHBoxLayout;
    pMsgHBL->addWidget(m_PInputMsgLe);
    pMsgHBL->addWidget(m_pMsgSendPB);

    m_pOnline=new online;

    QVBoxLayout *pMain=new QVBoxLayout;
    pMain->addLayout(pTopHBL);
    pMain->addLayout(pMsgHBL);
    pMain->addWidget(m_pOnline);
    m_pOnline->hide();
    setLayout(pMain);

    connect(m_pShowOnlineUsrPB,&QPushButton::clicked,this,&Friend::showOnline);
    connect(m_pSearchUsrPB,&QPushButton::clicked,this,&Friend::searchUsr);
    connect(m_pFlushFriendPB,&QPushButton::clicked,this,&Friend::flushFriend);
    connect(m_pDelFriendPB,&QPushButton::clicked,this,&Friend::delFriend);
    connect(m_pPrivateChatPB,&QPushButton::clicked,this,&Friend::privateChat);
    connect(m_pMsgSendPB,&QPushButton::clicked,this,&Friend::groupChat);
}

void Friend::showAllOnlineUsr(PDU *pdu)
{
    if(NULL==pdu)
    {
        return;
    }
    m_pOnline->showUsr(pdu);
}

void Friend::updateFriendList(PDU *pdu)
{
    if(NULL==pdu)
    {
        return;
    }
    m_pFriendListWidget->clear();
    uint uiSize=pdu->uiMsgLen/32;
    char caName[32]={'\0'};
    for(uint i=0;i<uiSize;i++)
    {
        memcpy(caName,(char*)(pdu->caMsg)+i*32,32);
        m_pFriendListWidget->addItem(caName);
    }
}

void Friend::updateGroupMsg(PDU *pdu)
{
    QString strMsg=QString("%1 说: %2 ").arg(pdu->caDate).arg((char*)(pdu->caMsg));
    m_pShowMsgTE->append(strMsg);
}

QListWidget *Friend::getFriendList()
{
    return m_pFriendListWidget;
}

void Friend::showOnline()
{
    if(m_pOnline->isHidden())
    {
        m_pOnline->show();
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_ALL_ONLINE_REQUEST;
        TcpClinent::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
    }
    else
    {
        m_pOnline->close();

    }
}

void Friend::searchUsr()
{
    m_strSearchName= QInputDialog::getText(this,"搜索","用户名");
    if(!m_strSearchName.isEmpty())
    {
        qDebug()<<m_strSearchName;
        PDU *pdu=mkPDU(0);
        memcpy(pdu->caDate,m_strSearchName.toStdString().c_str(),m_strSearchName.size());
        pdu->uiMsgType=ENUM_MSG_TYPE_SEARCH_USR_REQUEST;
        TcpClinent::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
}

void Friend::flushFriend()//刷新好友请求
{

    QString strName=TcpClinent::getInstance().loginName();//接受登录名字
    PDU *pdu=mkPDU(0);
    pdu->uiMsgType= ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST;
    memcpy(pdu->caDate,strName.toStdString().c_str(),strName.size());
    TcpClinent::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

void Friend::delFriend()
{
    if(NULL!=m_pFriendListWidget->currentItem())
    {
        QString strFriendName= m_pFriendListWidget->currentItem()->text();
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST;
        QString strSelfName=TcpClinent::getInstance().loginName();
        memcpy(pdu->caDate,strSelfName.toStdString().c_str(),strSelfName.size());
        memcpy(pdu->caDate+32,strFriendName.toStdString().c_str(),strFriendName.size());
        TcpClinent::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;

    }
}

void Friend::privateChat()
{
    if(NULL!=m_pFriendListWidget->currentItem())
    {
    QString strChatName=m_pFriendListWidget->currentItem()->text();
    PrivateChat::getInstance().setChatName(strChatName);
    if(PrivateChat::getInstance().isHidden())
    {
        PrivateChat::getInstance().show();
    }
    }
    else
    {
        QMessageBox::warning(this,"私聊","请选择私聊的对象");
    }

}

void Friend::groupChat()
{
    QString strMsg=m_PInputMsgLe->text();
    if(!strMsg.isEmpty())
    {
        PDU *pdu=mkPDU(strMsg.size()+1);
        pdu->uiMsgType=ENUM_MSG_TYPE_GROUP_CHAT_REQUEST;
        QString strName=TcpClinent::getInstance().loginName();
        strncpy(pdu->caDate,strName.toStdString().c_str(),strName.size());
        strncpy((char*)(pdu->caMsg),strMsg.toStdString().c_str(),strMsg.size());
        TcpClinent::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    }
    else
    {
        QMessageBox::warning(this,"群聊","群聊信息不能为空");
    }
}
