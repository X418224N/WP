#include "mytcpsocket.h"
#include<QDebug>
#include"mytcpserver.h"
#include<QDir>
#include<QFile>

MyTcpSocket::MyTcpSocket()
{
//    connect(this,SIGNAL(readyRead()),this,SLOT(recvMsg()));
    connect(this,&MyTcpSocket::readyRead,this,&MyTcpSocket::recvMsg);
    connect(this,&MyTcpSocket::disconnected,this,&MyTcpSocket::clientOffline);
    m_bUploadt=false;
    m_pTimer=new QTimer;
    connect(m_pTimer,&QTimer::timeout,this,&MyTcpSocket::sendFileToClinent);
}

QString MyTcpSocket::getName()
{
    return m_strName;
}

void MyTcpSocket::copyDir(QString strSrcDir, QString strDestDir)
{
    QDir dir;
    dir.mkdir(strDestDir);
    dir.setPath(strSrcDir);
    QFileInfoList fileInfoList=dir.entryInfoList();
    QString srcTmp;
    QString destTmp;
    for(int i=0;i<fileInfoList.size();i++)
    {
        if(fileInfoList[i].isFile())//文件
        {
            srcTmp=strSrcDir+'/'+fileInfoList[i].fileName();
            destTmp=strDestDir+'/'+fileInfoList[i].fileName();
            QFile::copy(srcTmp,destTmp);
        }
        else if(fileInfoList[i].isDir())
        {
            if(QString(".")==fileInfoList[i].fileName()||QString("..")==fileInfoList[i].fileName())
            {
                continue;
            }
            srcTmp=strSrcDir+'/'+fileInfoList[i].fileName();
            destTmp=strDestDir+'/'+fileInfoList[i].fileName();
            copyDir(srcTmp,destTmp);//递归拷贝
        }
    }
}

void MyTcpSocket::recvMsg()//收到数据
{
    if(!m_bUploadt)//不是上传文件状态
    {
    qDebug()<<this->bytesAvailable();
    uint uiPDULen=0;
    //收到数据
    this->read((char*)&uiPDULen,sizeof(uint));//获得信息总大小
    //收数据
    uint uiMsgLen=uiPDULen-sizeof(PDU);//计算实际消息长度
    PDU *pdu=mkPDU(uiMsgLen);//接收剩余数据
    this->read((char*)pdu+sizeof(uint),uiPDULen-sizeof(uint));

    switch (pdu->uiMsgType) //判断消息类型
    {
    case ENUM_MSG_TYPE_REGIST_REQUEST:
    {
        char caName[32]={'\0'};
        char caPwd[32]={'\0'};
        //获得用户名密码
        strncpy(caName,pdu->caDate,32);
        strncpy(caPwd,pdu->caDate+32,32);
        bool ret=OpeDB::getInstance().handleRegist(caName,caPwd);//出来请求
        PDU *respdu=mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_REGIST_RESPOND;
        if(ret)
        {
            strcpy(respdu->caDate,REGIST_OK);
            QDir dir;
            qDebug()<<"create dir:"<< dir.mkdir(QString("./%1").arg(caName));
        }
        else
        {
            strcpy(respdu->caDate,REGIST_FAILED);
        }
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;
        break;
    }
    case ENUM_MSG_TYPE_LOGIN_REQUEST:
    {
        char caName[32]={'\0'};
        char caPwd[32]={'\0'};
        //获得用户名密码
        strncpy(caName,pdu->caDate,32);
        strncpy(caPwd,pdu->caDate+32,32);
        bool ret =OpeDB::getInstance().handleLogin(caName,caPwd);
        PDU *respdu=mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_LOGIN_REQUEST;
        if(ret)
        {
            strcpy(respdu->caDate,LOGIN_OK);
            m_strName=caName;
        }
        else
        {
            strcpy(respdu->caDate,LOGIN_FAILED);
        }
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;
        break;
    }
    case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST:
    {
        QStringList ret=OpeDB::getInstance().handleAllOnline();
        uint uiMsgLen=ret.size()*32;//每个名字给32个字节空间
        PDU *respdu=mkPDU(uiMsgLen);
        respdu->uiMsgType=ENUM_MSG_TYPE_ALL_ONLINE_RESPOND;
        for(int i=0;i<ret.size();i++)
        {
            memcpy((char*)(respdu->caMsg)+i*32,ret.at(i).toStdString().c_str(),ret.at(i).size());
        }
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;
        break;
    }
    case ENUM_MSG_TYPE_SEARCH_USR_REQUEST:
    {
        int ret=OpeDB::getInstance().handleSearchUsr(pdu->caDate);
        PDU*respdu=mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_SEARCH_USR_RESPOND;
        if(-1==ret)
        {
            strcpy(respdu->caDate,SEARCH_USR_NO);
        }
        else if(1==ret)
        {
            strcpy(respdu->caDate,SEARCH_USR_ONLINE);
        }
        else if(0==ret)
        {
            strcpy(respdu->caDate,SEARCH_USR_OFFLINE);
        }
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:
    {
        char caPerName[32]={'\0'};
        char caName[32]={'\0'};
        //获得用户名密码
        strncpy(caPerName,pdu->caDate,32);
        strncpy(caName,pdu->caDate+32,32);
        int ret=OpeDB::getInstance().handleAddFriend(caPerName,caName);
        PDU *respdu=NULL;
        if(-1==ret)
        {
            respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caDate,UNKNOW_ERROR);
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;//发送给客户端
        }
        else if(0==ret)
        {
            respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caDate,EXISTED_FRIEND);
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
        }
        else if(1==ret)
        {
            MyTcpServer::getInstance().resend(caPerName,pdu);
        }
        else if(2==ret)
        {
            respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caDate,ADD_FRIEND_OFFLINE);
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
        }
        else if(3==ret)
        {
            respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caDate,ADD_FRIEND_NOEXIST);
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
        }
    break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_AGGREE:
    {
        char caPerName[32]={'\0'};
        char caName[32]={'\0'};
        strncpy(caPerName,pdu->caDate,32);
        strncpy(caName,pdu->caDate+32,32);
        OpeDB::getInstance().handleAgreeAddFriend(caPerName,caName);
        MyTcpServer::getInstance().resend(caName,pdu);
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:
        {
            char caName[32]={'\0'};
            strncpy(caName,pdu->caDate+32,32);
            MyTcpServer::getInstance().resend(caName,pdu);
            break;
        }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST:
    {
        char caName[32]={'\0'};
        strncpy(caName,pdu->caDate,32);
        QStringList ret= OpeDB::getInstance().handleFlushFriend(caName);
        uint uiMsgLen=ret.size()*32;
        PDU *respdu=mkPDU(uiMsgLen);
        respdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;

        for(int i=0;i<ret.size();i++)
        {
            memcpy((char*)(respdu->caMsg)+i*32,ret.at(i).toStdString().c_str(),ret.at(i).size());
        }
        write((char*)respdu,respdu->uiPDULen);//发送给客户端
        free(respdu);
        respdu=NULL;
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST:
    {
        char caSelfName[32]={'\0'};
        char caFriendName[32]={'\0'};
        //获得用户名密码
        strncpy(caSelfName,pdu->caDate,32);
        strncpy(caFriendName,pdu->caDate+32,32);
        OpeDB::getInstance().handleDeleteFriend(caSelfName,caFriendName);
        //给删除的人的提示
        MyTcpServer::getInstance().resend(caFriendName,pdu);
        //给发送方的回复
        PDU *respdu=mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;
        strcpy(respdu->caDate,DEL_FRIEND_OK);
        write((char*)respdu,respdu->uiPDULen);//发送给客户端
        free(respdu);
        respdu=NULL;

        break;
    }
    case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST:
    {
        char caPerName[32]={'\0'};
        memcpy(caPerName,pdu->caDate+32,32);//将名字拷出来
        qDebug()<<caPerName;
        MyTcpServer::getInstance().resend(caPerName,pdu);
        break;
    }
    case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST:
    {
        char caName[32]={'\0'};
        strncpy(caName,pdu->caDate,32);
        QStringList onlinefriend= OpeDB::getInstance().handleFlushFriend(caName);
        QString tmp;
        for(int i=0;i<onlinefriend.size();i++)
        {
            tmp=onlinefriend.at(i);
            MyTcpServer::getInstance().resend(tmp.toStdString().c_str(),pdu);
        }
        break;
    }
    case ENUM_MSG_TYPE_CREATE_DIR_REQUEST:
    {
    QDir dir;
    QString strCurPath=QString("%1").arg((char*)(pdu->caMsg));
    qDebug()<<strCurPath;
    bool ret=dir.exists(strCurPath);
    PDU *respdu=NULL;
    if(ret)//当前目录存在
    {
        char caNewDir[32]={'\0'};
        memcpy(caNewDir,pdu->caDate+32,32);
        QString strNewPath=strCurPath+"/"+caNewDir;
        qDebug()<<strNewPath;
        ret =dir.exists(strNewPath);
        qDebug()<<"-->"<<ret;
        if(ret)//创建的文件名已存在
        {
            respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
            strcpy(respdu->caDate,FILE_NAME_EXIST);
        }
        else//创建的文件名不存在
        {
           dir.mkdir(strNewPath);
           respdu=mkPDU(0);
           respdu->uiMsgType=ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
           strcpy(respdu->caDate,CREAT_DIR_OK);
        }
    }
    else//当前目录不存在
    {
        respdu=mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
        strcpy(respdu->caDate,DIR_NO_EXIST);
    }
    write((char*)respdu,respdu->uiPDULen);//将数据发往客户端
    free(respdu);
    respdu=NULL;
    break;
    }
    case ENUM_MSG_TYPE_FLUSH_FILE_REQUEST:
    {
        char *pCurPath=new char[pdu->uiMsgLen];
        memcpy(pCurPath,pdu->caMsg,pdu->uiMsgLen);
        QDir dir(pCurPath);
        QFileInfoList fileInfoList= dir.entryInfoList();
        int iFileCount=fileInfoList.size();
        PDU *respdu=mkPDU(sizeof(FileInfo)*iFileCount);
        respdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;
        FileInfo *pFileInfo=NULL;
        QString strFileNmae;
        for(int i=0;i<fileInfoList.size();i++)
        {
            pFileInfo=(FileInfo*)(respdu->caMsg)+i;
            strFileNmae=fileInfoList[i].fileName();//把文件名字拿出来
            memcpy(pFileInfo->caFileName,strFileNmae.toStdString().c_str(),strFileNmae.size());//将名字放进去
            if(fileInfoList[i].isDir())
            {
                pFileInfo->iFileType=0;
            }
            else if(fileInfoList[i].isFile())
            {
                pFileInfo->iFileType=1;
            }
            qDebug()<<fileInfoList[i].fileName()<<fileInfoList[i].size()<<" 文件夹："<<fileInfoList[i].isDir()<<"常规文件："<<fileInfoList[i].isFile();
        }
        write((char*)respdu,respdu->uiPDULen);//将数据发往客户端
        free(respdu);
        respdu=NULL;
        break;
    }
    case ENUM_MSG_TYPE_DEL_DIR_REQUEST:
    {
        char caName[32]={'\0'};
        strcpy(caName,pdu->caDate);
        char *pPath=new char[pdu->uiMsgLen];
        memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
        QString strPath=QString("%1/%2").arg(pPath).arg(caName);
        QFileInfo fileInfo(strPath);
        bool ret=false;
        if(fileInfo.isDir())
        {

            QDir dir;
            dir.setPath(strPath);
            ret=dir.removeRecursively();//删除路径

        }
        else if(fileInfo.isFile())//常规文件
        {
            ret=false;
        }
        PDU *respdu=NULL;
        if(ret)
        {
            respdu=mkPDU(strlen(DEL_DIR_OK)+1);
            respdu->uiMsgType=ENUM_MSG_TYPE_DEL_DIR_RESPOND;
            memcpy(respdu->caDate,DEL_DIR_OK,strlen(DEL_DIR_OK));
        }
        else
        {
            respdu=mkPDU(strlen(DEL_DIR_FAILURED)+1);
            respdu->uiMsgType=ENUM_MSG_TYPE_DEL_DIR_RESPOND;
            memcpy(respdu->caDate,DEL_DIR_FAILURED,strlen(DEL_DIR_FAILURED));
        }
        write((char*)respdu,respdu->uiPDULen);//将数据发往客户端
        free(respdu);
        respdu=NULL;
        break;
    }
    case ENUM_MSG_TYPE_RENAME_FILE_REQUEST:
    {
        char caOldname[32]={'\0'};
        char caNewname[32]={'\0'};
        strncpy(caOldname,pdu->caDate,32);
        strncpy(caNewname,pdu->caDate+32,32);
        char *pPath=new char[pdu->uiMsgLen];
        memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
        QString strOldPath=QString("%1/%2").arg(pPath).arg(caOldname);
        QString strnewPath=QString("%1/%2").arg(pPath).arg(caNewname);
        qDebug()<<strOldPath;
         qDebug()<<strnewPath;
        QDir dir;
        bool ret= dir.rename(strOldPath,strnewPath);
        PDU *respdu=mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_RENAME_FILE_RESPOND;
        if(ret)
        {
            strcpy(respdu->caDate,RENAME_FILE_OK);
        }
        else
        {
            strcpy(respdu->caDate,RENAME_FILE_FAILURED);
        }
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;
        break;
    }
    case ENUM_MSG_TYPE_ENTER_DIR_REQUEST:
    {
        char caEnterName[32]={'\0'};
        strncpy(caEnterName,pdu->caDate,32);
        char *pPath=new char[pdu->uiMsgLen];
        memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
        QString strPath=QString("%1/%2").arg(pPath).arg(caEnterName);
        qDebug()<<strPath;
        QFileInfo fileInfo(strPath);
        PDU *respdu=NULL;
        if(fileInfo.isDir())
        {
            QDir dir(strPath);
            QFileInfoList fileInfoList= dir.entryInfoList();//获得单前文件下面所有的文件信息
            int iFileCount=fileInfoList.size();
            respdu=mkPDU(sizeof(FileInfo)*iFileCount);//根据文件个数产生文件大小的空间
            respdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;
            FileInfo *pFileInfo=NULL;
            QString strFileNmae;
            for(int i=0;i<fileInfoList.size();i++)
            {
                pFileInfo=(FileInfo*)(respdu->caMsg)+i;
                strFileNmae=fileInfoList[i].fileName();//把文件名字拿出来
                memcpy(pFileInfo->caFileName,strFileNmae.toStdString().c_str(),strFileNmae.size());//将名字放进去
                if(fileInfoList[i].isDir())
                {
                    pFileInfo->iFileType=0;
                }
                else if(fileInfoList[i].isFile())
                {
                    pFileInfo->iFileType=1;
                }
//                qDebug()<<fileInfoList[i].fileName()<<fileInfoList[i].size()<<" 文件夹："<<fileInfoList[i].isDir()<<"常规文件："<<fileInfoList[i].isFile();
            }
            write((char*)respdu,respdu->uiPDULen);//将数据发往客户端
            free(respdu);
            respdu=NULL;
        }
        else if(fileInfo.isFile())
        {

            respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_ENTER_DIR_RESPOND;
            strcpy(respdu->caDate,ENTER_DIR_FAILURED);
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
        }
        break;
    }
    case ENUM_MSG_TYPE_DEL_REG_REQUEST:
    {
        char caName[32]={'\0'};
        strcpy(caName,pdu->caDate);//删除的文件名
        char *pPath=new char[pdu->uiMsgLen];
        memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
        QString strPath=QString("%1/%2").arg(pPath).arg(caName);
        QFileInfo fileInfo(strPath);
        bool ret=false;
        if(fileInfo.isDir())
        {
            ret=false;
        }
        else if(fileInfo.isFile())
        {
            qDebug()<<strPath<<"223";
         QDir dir;
         dir.remove(strPath);
         ret=true;
        }
            PDU *respdu=mkPDU(0);
        if(ret)
        {
            respdu->uiMsgType=ENUM_MSG_TYPE_DEL_REG_RESPOND;
            strcpy(respdu->caDate,DEL_FILE_OK);
        }
        else
        {
            respdu->uiMsgType=ENUM_MSG_TYPE_DEL_REG_RESPOND;
            strcpy(respdu->caDate,DEL_FILE_FAILURED);
        }
        write((char*)respdu,respdu->uiPDULen);//将数据发往客户端
        free(respdu);
        respdu=NULL;
        break;
    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST:
    {
        char caFlieName[32]={'\0'};
        qint64 fileSize=0;
        sscanf(pdu->caDate,"%s %lld",caFlieName,&fileSize);//提取文件名
        char *pPath=new char[pdu->uiMsgLen];
        memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
        QString strPath=QString("%1/%2").arg(pPath).arg(caFlieName);
        delete []pPath;
        pPath =NULL;
        m_file.setFileName(strPath);
        //以只写的方式打开文件，诺文件不存在，则会自动创建文件
        if(m_file.open(QIODevice::WriteOnly));
        {
            m_bUploadt=true;
            m_iTotal=fileSize;
            m_iRecved=0;
        }
        break;
    }
    case ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST:
    {
        char caFileName[32]={'\0'};
        strcpy(caFileName,pdu->caDate);//拷贝文件名字
        char *pPath=new char[pdu->uiMsgLen];
        memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
        QString strPath=QString("%1/%2").arg(pPath).arg(caFileName);
        delete []pPath;
        pPath=NULL;
        QFileInfo fileInfo(strPath);//获取文件大小
        qint64 fileSize=fileInfo.size();//获取文件大小
        PDU *respdu=mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND;
        sprintf(respdu->caDate,"%s %lld",caFileName,fileSize);
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;
        m_file.setFileName(strPath);//打开文件
        m_file.open(QIODevice::ReadOnly);
        m_pTimer->start(500);
        qDebug()<<"服务器1";
        break;
//        char caFileName[32] = {'\0'};
//                   strcpy(caFileName,pdu->caDate);
//                   char *pPath =new char[pdu->uiMsgLen];
//                   memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
//                   QString strPath =QString("%1/%2").arg(pPath).arg(caFileName);
//                   qDebug()<<strPath;
//                   delete []pPath;
//                   pPath = NULL;

//                   QFileInfo fileInfo(strPath);
//                   qint64 fileSize = fileInfo.size();
//                   PDU *respdu = mkPDU(0);
//                   respdu->uiMsgType = ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND;
//                   sprintf(respdu->caDate,"%s %lld", caFileName,fileSize);

//                   write((char*)respdu,respdu->uiPDULen);
//                   free(respdu);
//                   respdu = NULL;

//                   m_file.setFileName(strPath);
//                   m_file.open(QIODevice::ReadOnly);
//                   m_pTimer->start(500);

//                   break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_REQUEST:
    {
        char caSendName[32]={'\0'};
        int num=0;
        sscanf(pdu->caDate,"%s%d",caSendName,&num);
        int size=num*32;
        PDU *respdu=mkPDU(pdu->uiMsgLen-size);//获得接受者名字所占空间
        respdu->uiMsgType=ENUM_MSG_TYPE_SHARE_FILE_NOTE;
        strcpy(respdu->caDate,caSendName);//发送者名字
        memcpy(respdu->caMsg,(char*)(pdu->caMsg)+size,pdu->uiMsgLen-size);//发送者共享的文件
        char caRecvName[32]={'\0'};//接收者名字
        for(int i=0;i<num;i++)
        {
            memcpy(caRecvName,(char*)(pdu->caMsg)+i*32,32);
            MyTcpServer::getInstance().resend(caRecvName,respdu);
        }
        free(respdu);
        respdu=NULL;
        respdu=mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_SHARE_FILE_RESPOND;
        strcpy(respdu->caDate,"share file ok");//回复发送方信息
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND:
    {
        QString strRecvPath=QString("./%1").arg(pdu->caDate);//接收方目录
        QString strShareFilePath=QString ("%1").arg((char*)(pdu->caMsg));//分享者名字
        int index=strShareFilePath.lastIndexOf('/');
        QString strFileName=strShareFilePath.right(strShareFilePath.size()-index-1);
        strRecvPath=strRecvPath+'/'+strFileName;
        QFileInfo fileInfo(strShareFilePath);//共享目录
        if(fileInfo.isFile())
        {
            QFile::copy(strShareFilePath,strRecvPath);//常规文件拷贝
        }
        else if(fileInfo.isDir())
        {
        copyDir(strShareFilePath,strRecvPath);
        }
        break;
    }
    case ENUM_MSG_TYPE_MOVE_FILE_REQUEST:
    {
        char caFileName[32]={'\0'};
        int srcLen=0;
        int  destLen=0;
        sscanf(pdu->caDate,"%d%d%s",&srcLen,&destLen,caFileName);
        char *pSrcPath=new char[srcLen+1];
        char *pDestPath=new char[destLen+1+32];
        memset(pSrcPath,'\0',srcLen+1);
        memset(pDestPath,'\0',srcLen+1+32);
        memcpy(pSrcPath,pdu->caMsg,srcLen);
        memcpy(pDestPath,(char*)(pdu->caMsg)+(srcLen+1),destLen);
        PDU *respdu=mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_MOVE_FILE_RESPOND;
        QFileInfo fileInfo(pDestPath);
        if(fileInfo.isDir())
        {
            strcat(pDestPath,"/");
            strcat(pDestPath,caFileName);
           bool ret= QFile::rename(pSrcPath,pDestPath);
           if(ret)
           {
               strcpy(respdu->caDate,MOVE_FILE_OK);
           }
           else
           {
                strcpy(respdu->caDate,COMMON_ERR);
           }

        }
        else if(fileInfo.isFile())
        {
            strcpy(respdu->caDate,MOVE_FILE_FAILURED);
        }
        qDebug()<<"end";
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;
        break;
    }
    case ENUM_MSG_TYPE_REMAKE_REQUEST:
    {
        char caName[32]={'\0'};
        char caPwd[32]={'\0'};
        //获得用户名密码
        strncpy(caName,pdu->caDate,32);
        strncpy(caPwd,pdu->caDate+32,32);
        bool ret =OpeDB::getInstance().handleremake(caName,caPwd);
        PDU *respdu=mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_REMAKE_RESPOND;
        if(ret)
        {
            strcpy(respdu->caDate,REMAKE_OK);
        }
        else
        {
            strcpy(respdu->caDate,REMAKE_FAILURED);

        }
        write((char*)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;
        break;
    }
    default:
        break;
    }
    free(pdu);
    pdu=NULL;
    }
    else
    {
        PDU *respdu=mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;
        QByteArray buff= readAll();//返回一个字节数组 QByteArray
        m_file.write(buff);
        m_iRecved+=buff.size();
        if(m_iTotal==m_iRecved)//接收数据结束
        {
            m_file.close();
            m_bUploadt=false;
            strcpy(respdu->caDate,UPLOAD_FILE_OK);
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
        }
        else if(m_iTotal<m_iRecved&&m_iTotal>m_iRecved)//接收数据比总的大
        {
            m_file.close();
            m_bUploadt=false;
            strcpy(respdu->caDate,UPLOAD_FILE_FAILURED);
            write((char*)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
        }
    }
}

void MyTcpSocket::clientOffline()
{
    OpeDB::getInstance().handleOffline(m_strName.toStdString().c_str());//在数据库里设置为0
    emit offLine(this);
}

void MyTcpSocket::sendFileToClinent()
{
//    qDebug()<<"服务器2";
//    m_pTimer->stop();
//    char *pDate=new char[4096];
//    qint64 ret=0;
//    while(true)
//    {
//        ret =m_file.read(pDate,4096);
//        if(ret>0&&ret<=4096)
//        {
//            write(pDate,ret);
//            qDebug()<<"服务器服务器";
//        }
//        else if(0==ret)
//        {
//            m_file.close();
//            break;
//        }
//        else if(ret<0)//发送文件给客户端过程失败
//        {
//            qDebug()<<"服务器失败";
//            m_file.close();
//            break;
//        }
//    }
//    delete [] pDate;
//    pDate=NULL;
//    m_pTimer->stop();
        char *pData = new char[4096];
        qint64 ret = 0;
        while (true)//循环的发送给客户端
        {
            ret = m_file.read(pData,4096);
            if(ret > 0 && ret <= 4096)
            {
                write(pData , ret);
            }
            else if(0 == ret)
            {
                m_file.close();
                break;
            }
            else if(ret < 0)
            {
                qDebug() << "发送文件给客户端失败";
                m_file.close();
                break;
            }
        }
        delete []pData;
        pData = NULL;
}
