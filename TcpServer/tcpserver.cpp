#include "tcpserver.h"
#include "ui_tcpserver.h"
#include"mytcpserver.h"
#include<QByteArray>
#include<QDebug>
#include<QMessageBox>
#include<QHostAddress>
#include<QFile>
TcpServer::TcpServer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpServer)
{
    ui->setupUi(this);
    loadConfig();
    MyTcpServer::getInstance().listen(QHostAddress(m_strIP),m_usPort);
}

TcpServer::~TcpServer()
{
    delete ui;
}

void TcpServer::loadConfig()
{
    QFile file(":/client.config");
    if(file.open(QIODevice::ReadOnly))
    {
        //用baData读取文件中所有信息
        QByteArray baData=file.readAll();
        //将QByteArray转换成字符串
        QString strData=QString(baData);//baData.toStdString(),c_str();
        file.close();
        //用空格替换\r\n
        strData.replace("\r\n"," ");
        //用字符串列表切割空格
        QStringList strList= strData.split(" ");
       m_strIP=strList.at(0);
       m_usPort=strList.at(1).toUShort();
    }
    else
    {
        QMessageBox::critical(this,"open config","open config failed");
    }
}

