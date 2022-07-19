#include "tcpclinent.h"
#include "ui_tcpclinent.h"
#include<QByteArray>
#include<QDebug>
#include<QMessageBox>
#include<QHostAddress>
#include"privatechat.h"
TcpClinent::TcpClinent(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TcpClinent)
{
    ui->setupUi(this);
    loadConfig();
    connect(&m_tcpsocket,&QTcpSocket::connected,this,&TcpClinent::showConnect);
    connect(&m_tcpsocket,&QTcpSocket::readyRead,this,&TcpClinent::recvMsg);//发出数据给客户端，客户端收到数据发出readyread信号，调用recvMsg去接收
//    connect(&m_tcpsocket,SIGNAL(connected()),this,SLOT(showConnect()));
    //连接服务器
    m_tcpsocket.connectToHost(QHostAddress(m_strIP),m_usPort);
    resize(500,250);
}

TcpClinent::~TcpClinent()
{
    delete ui;
}

void TcpClinent::loadConfig()
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

TcpClinent &TcpClinent:: getInstance()
{
    static TcpClinent instance;//活动界面对象的引用
    return instance;
}

QTcpSocket &TcpClinent::getTcpSocket()
{
    return m_tcpsocket;
}

QString TcpClinent::loginName()
{
    return m_strloginName;
}

QString TcpClinent::curPath()
{
    return m_strCurPath;
}

void TcpClinent::setCurPath(QString strCurPath)
{
    m_strCurPath=strCurPath;
}

void TcpClinent::showConnect()
{
    QMessageBox::information(this,"连接服务器","连接服务器成功");
}

void TcpClinent::recvMsg()//接受数据
{
    if(!OpeWidget::getInstance().getBook()->getDownLoadStatus())
    {
    qDebug()<<m_tcpsocket.bytesAvailable();
    uint uiPDULen=0;
    //收到数据
    m_tcpsocket.read((char*)&uiPDULen,sizeof(uint));//获得信息总大小
    uint uiMsgLen=uiPDULen-sizeof(PDU);//计算实际消息长度
    PDU *pdu=mkPDU(uiMsgLen);//接收剩余数据
    m_tcpsocket.read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));
    switch (pdu->uiMsgType) //判断消息类型
    {
    case ENUM_MSG_TYPE_REGIST_RESPOND:
    {
        if(0==strcmp(pdu->caDate,REGIST_OK))
        {
            QMessageBox::information(this,"注册",REGIST_OK);
        }
        else if(0==strcmp(pdu->caDate,REGIST_FAILED))
        {
            QMessageBox::warning(this,"注册失败",REGIST_FAILED);
        }
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_REQUEST:
    {
        if(0==strcmp(pdu->caDate,LOGIN_OK))
        {
            m_strCurPath=QString("./%1").arg(m_strloginName);//获取登录名
            QMessageBox::information(this,"登录",LOGIN_OK);
            OpeWidget::getInstance().show();
            this->hide();
        }
        else /*if(0==strcmp(pdu->caDate,LOGIN_FAILED))*/
        {
            QMessageBox::warning(this,"登录失败",LOGIN_FAILED);
        }
        break;
    }
    case ENUM_MSG_TYPE_ALL_ONLINE_RESPOND:
    {
        OpeWidget::getInstance().getFriend()->showAllOnlineUsr(pdu);
        break;
    }
    case ENUM_MSG_TYPE_SEARCH_USR_RESPOND:
    {

        if(0==strcmp(SEARCH_USR_NO,pdu->caDate))
        {
          QMessageBox::information(this,"搜索",QString("%1:not exist").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
        }
        else if(0==strcmp(SEARCH_USR_ONLINE,pdu->caDate))
        {
            QMessageBox::information(this,"搜索",QString("%1:Online").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
        }
        else if(0==strcmp(SEARCH_USR_OFFLINE,pdu->caDate))
        {
            QMessageBox::information(this,"搜索",QString("%1:offline").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));
        }
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
    {

        char caPerName[32]={'\0'};
        strncpy(caPerName,pdu->caDate+32,32);
        int ret=QMessageBox::information(this,"添加好友",QString("%1 want to add you as friend?").arg(caPerName)
                                 ,QMessageBox::Yes,QMessageBox::No);
        PDU*respdu=mkPDU(0);
        memcpy(respdu->caDate,pdu->caDate,64);
        if(QMessageBox::Yes==ret)
        {
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_AGGREE;
        }
        else
        {
             respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_REFUSE;
        }
        m_tcpsocket.write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND:
    {
        QMessageBox::information(this,"添加好友",pdu->caDate);

    }
    case ENUM_MSG_TYPE_ADD_FRIEND_AGGREE:
    {
        char caPerName[32]={'\0'};
        memcpy(caPerName,pdu->caDate,32);
        QMessageBox::information(this,"添加好友",QString("已成功添加%1为好友").arg(caPerName));
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:
    {
        char caPerName[32]={'\0'};
        memcpy(caPerName,pdu->caDate,32);
        QMessageBox::information(this,"添加好友",QString("已成功拒绝%1为好友").arg(caPerName));
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND:
    {
        OpeWidget::getInstance().getFriend()->updateFriendList(pdu);

        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:
    {
        char caName[32]={'\0'};
        memcpy(caName,pdu->caDate,32);
        QMessageBox::information(this,"删除好友",QString("%1 已将你移除好友").arg(caName));

        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND:
    {
        QMessageBox::information(this,"删除好友","删除好友成功");
        break;
    }
    case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:
    {
        if(PrivateChat::getInstance().isHidden())
        {
            PrivateChat::getInstance().show();
        }
            char caSendName[32]={'\0'};
            memcpy(caSendName,pdu->caDate,32);
            QString strSendName=caSendName;
            PrivateChat::getInstance().setChatName(strSendName);
            PrivateChat::getInstance().updateMsg(pdu);
              break;
      }
     case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST:
    {
        OpeWidget::getInstance().getFriend()->updateGroupMsg(pdu);
        break;
    }
    case ENUM_MSG_TYPE_CREATE_DIR_RESPOND:
    {
        QMessageBox::information(this,"创建文件",pdu->caDate);
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FILE_RESPOND:
    {
        OpeWidget::getInstance().getBook()->updateFileList(pdu);
        QString strEnterDir=OpeWidget::getInstance().getBook()->enterDir();
        if(!strEnterDir.isEmpty())
        {
            m_strCurPath=m_strCurPath+"/"+strEnterDir;
            qDebug()<<"enter dir"<<m_strCurPath;
        }
        break;
    }
    case ENUM_MSG_TYPE_DEL_DIR_RESPOND:
    {
        QMessageBox::information(this,"删除文件夹",pdu->caDate);
        break;
    }
    case ENUM_MSG_TYPE_RENAME_FILE_RESPOND:
    {
        QMessageBox::information(this,"重命名",pdu->caDate  );
        break;
    }
    case ENUM_MSG_TYPE_ENTER_DIR_RESPOND:
    {
        OpeWidget::getInstance().getBook()->clearEnterDir();
        QMessageBox::information(this,"进入文件夹",pdu->caDate  );
        break;
    }
    case ENUM_MSG_TYPE_DEL_REG_RESPOND:
    {
        QMessageBox::information(this,"删除文件",pdu->caDate);
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND:
    {
        QMessageBox::information(this,"上传文件",pdu->caDate);
        break;
    }
    case ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND:
    {
        qDebug()<<"下载文件:"<<pdu->caDate;
        char caFileName[32]={'\0'};
        sscanf(pdu->caDate,"%s %lld",caFileName,&(OpeWidget::getInstance().getBook()->m_iTotal));
        if(strlen(caFileName)>0&&OpeWidget::getInstance().getBook()->m_iTotal>0)
        {
            OpeWidget::getInstance().getBook()->setDownloadStatus(true);//处于下载状态
            //客户端收到文件大小后打开
            m_file.setFileName(OpeWidget::getInstance().getBook()->getSaveFilePath());//活动要保存的路径
            if(!m_file.open(QIODevice::WriteOnly))
            {
                QMessageBox::warning(this,"下载文件","活动保存路径失败");
            }
        }
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_RESPOND:
    {
        QMessageBox::information(this,"共享文件",pdu->caDate);
     break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_NOTE://接受者
    {
        char *pPath=new char[pdu->uiMsgLen];
        memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
        char *pos=strrchr(pPath,'/');//从后面找一个字符
        if(NULL!=pos)
        {
            pos++;
            QString strNote=QString("%1 share file->%2\n Do you accept").arg(pdu->caDate).arg(pos);
            int ret=QMessageBox::question(this,"共享文件",QString("%1向你发送了文件，是否接受").arg(pdu->caDate));
            if(QMessageBox::Yes==ret)
            {
                PDU *respdu=mkPDU(pdu->uiMsgLen);
                respdu->uiMsgType=ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND;
                memcpy(respdu->caMsg,pdu->caMsg,pdu->uiMsgLen);
                QString strName=TcpClinent::getInstance().loginName();//你是谁
                strcpy(respdu->caDate,strName.toStdString().c_str());
                m_tcpsocket.write((char*)respdu,respdu->uiPDULen);
                this->close();
            }

        }

        break;
    }
    case ENUM_MSG_TYPE_MOVE_FILE_RESPOND:
        {
            QMessageBox::information(this,"移动文件",pdu->caDate);
        }
    case ENUM_MSG_TYPE_REMAKE_RESPOND:
    {
        if(0==strcmp(pdu->caDate,REMAKE_OK))
        {
            QMessageBox::information(this,"注销成功",REMAKE_OK);
        }
        else if(0==strcmp(pdu->caDate,REMAKE_FAILURED))
        {
            QMessageBox::warning(this,"注销失败",REMAKE_FAILURED);
        }
        break;
    }
    default:
        break;
    }
    free(pdu);
    pdu=NULL;
//    qDebug()<<caName<<caPwd<<pdu->uiMsgType;
    }
    else//接收数据
    {
       QByteArray buffer= m_tcpsocket.readAll();
       m_file.write(buffer);//把收到的数据保存在文件里 ,就是下载了
       book *pBook=OpeWidget::getInstance().getBook();
       pBook->m_iRecved+=buffer.size();
       if(pBook->m_iTotal==pBook->m_iRecved)//接受数据完成
       {
           m_file.close();
           pBook->m_iTotal=0;
           pBook->m_iRecved=0;
           pBook->setDownloadStatus(false);
           QMessageBox::information(this,"下载文件","下载文件成功");
       }
       else if(pBook->m_iTotal<pBook->m_iRecved)
       {
           m_file.close();
           pBook->m_iTotal=0;
           pBook->m_iRecved=0;
           pBook->setDownloadStatus(false);
           qDebug()<<"="<<pBook->m_iTotal<<pBook->m_iRecved;
           QMessageBox::critical(this,"下载文件","下载文件失败1");
       }
    }
}


//void TcpClinent::on_send_pb_clicked()
//{
//    QString strMsg=ui->lineEdit->text();
//    if(!strMsg.isEmpty())
//    {
//        PDU *pdu=mkPDU(strMsg.size()+1);//申请数据空间
//        pdu->uiMsgType=8888;
//        memcpy(pdu->caMsg,strMsg.toStdString().c_str(),strMsg.size());//把发送的数据拷贝到空间中
//        qDebug()<<(char*)(pdu->caMsg);
//        m_tcpsocket.write((char*)pdu,pdu->uiPDULen);
//        free(pdu);
//        pdu-=NULL;
//    }
//    else
//    {
//        QMessageBox::warning(this,"信息发送","发送的信息不能为空");
//    }
//}
void TcpClinent::on_login_pb_clicked()
{
    QString strName=ui->name_le->text();
    QString strPwd=ui->pwd_le->text();
    if(!strName.isEmpty()&&!strPwd.isEmpty())
    {
        m_strloginName=strName;
        PDU *pdu=mkPDU(0);//申请数据空间
        pdu->uiMsgType=ENUM_MSG_TYPE_LOGIN_REQUEST;
        strncpy(pdu->caDate,strName.toStdString().c_str(),32);
        strncpy(pdu->caDate+32,strPwd.toStdString().c_str(),32);
        m_tcpsocket.write((char*)pdu,pdu->uiPDULen);//把数据发送出去
        free(pdu);
        pdu=NULL;
    }
    else
    {
        QMessageBox::critical(this,"登录","登录失败\n登录的用户名或密码不能为空");
    }
}
void TcpClinent::on_regist_pb_clicked()
{
    QString strName=ui->name_le->text();
    QString strPwd=ui->pwd_le->text();
    if(!strName.isEmpty()&&!strPwd.isEmpty())
    {
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_REGIST_REQUEST;
        strncpy(pdu->caDate,strName.toStdString().c_str(),32);
        strncpy(pdu->caDate+32,strPwd.toStdString().c_str(),32);
        m_tcpsocket.write((char*)pdu,pdu->uiPDULen);//把数据发送出去
        free(pdu);
        pdu=NULL;
    }
    else
    {
        QMessageBox::critical(this,"注册","注册失败\n注册的用户名或密码不能为空");
    }
}
void TcpClinent::on_cancel_pb_clicked()
{
    QString strName=ui->name_le->text();
    QString strPwd=ui->pwd_le->text();
    if(!strName.isEmpty()&&!strPwd.isEmpty())
    {
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_REMAKE_REQUEST;
        strncpy(pdu->caDate,strName.toStdString().c_str(),32);
        strncpy(pdu->caDate+32,strPwd.toStdString().c_str(),32);
        m_tcpsocket.write((char*)pdu,pdu->uiPDULen);//把数据发送出去
        free(pdu);
        pdu=NULL;
    }
    else
    {
        QMessageBox::critical(this,"注销","注销失败\n注销的用户名或密码不能为空");
    }
}
