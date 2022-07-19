#include "online.h"
#include "ui_online.h"
#include<QDebug>
#include"tcpclinent.h"
online::online(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::online)
{
    ui->setupUi(this);
}

online::~online()
{
    delete ui;
}

void online::showUsr(PDU *pdu)
{
    if(NULL==pdu)
    {
        return;
    }
    ui->listWidget->clear();
    uint uiSize=pdu->uiMsgLen/32;
    char caTmp[32];
    for(uint i=0;i<uiSize;i++)
    {
        memcpy(caTmp,(char*)(pdu->caMsg)+i*32,32);
        ui->listWidget->addItem(caTmp);
    }

}

void online::on_addfriendPB_clicked()
{
    QListWidgetItem*pitem= ui->listWidget->currentItem();//当前的行
    QString strPerUsrName=pitem->text();
   QString strLoginName= TcpClinent::getInstance().loginName();//获得登录的名字
   PDU*pdu=mkPDU(0);
   pdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;
   mempcpy(pdu->caDate,strPerUsrName.toStdString().c_str(),strPerUsrName.size());//放入添加的名字
   mempcpy(pdu->caDate+32,strLoginName.toStdString().c_str(),strLoginName.size());//放入自己的名字
   TcpClinent::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);//发送数据
   free(pdu);
   pdu=NULL;
}
