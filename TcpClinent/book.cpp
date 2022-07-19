#include "book.h"
#include"protocol.h"
#include"tcpclinent.h"
#include<QInputDialog>
#include<QMessageBox>
#include<QFileDialog>
#include"opewidget.h"
#include"sharefile.h"
book::book(QWidget *parent) : QWidget(parent)
{
    m_strEnterDir.clear();
    m_bDownload=false;
    m_pTimer =new QTimer;
    m_pBookListW=new QListWidget;
    m_pReturnPB=new QPushButton("返回");
    m_pCreateDirPB=new QPushButton("创建文件夹");
    m_pDelPB=new QPushButton("删除文件夹");
    m_pRenamePB=new QPushButton("重命名文件夹");
    m_pFlushFilePB=new QPushButton("刷新文件夹");

    QVBoxLayout *pDirVBl=new QVBoxLayout;
    pDirVBl->addWidget(m_pReturnPB);
    pDirVBl->addWidget(m_pCreateDirPB);
    pDirVBl->addWidget(m_pDelPB);
    pDirVBl->addWidget(m_pRenamePB);
    pDirVBl->addWidget(m_pFlushFilePB);

    m_pUploadPB=new QPushButton("上传文件");
    m_pDownLoadPB=new QPushButton("下载文件");
    m_pDelFilePB=new QPushButton("删除文件");
    m_pShareeFilePB=new QPushButton("共享文件");
    m_pMoveFilePB=new QPushButton("移动文件");
     m_pSelectDirPB=new QPushButton("目标目录");
     m_pSelectDirPB->setEnabled(false);

    QVBoxLayout *pFileVBL=new QVBoxLayout;

    pFileVBL->addWidget(m_pUploadPB);
    pFileVBL->addWidget(m_pDownLoadPB);
    pFileVBL->addWidget(m_pDelFilePB);
    pFileVBL->addWidget(m_pShareeFilePB);
    pFileVBL->addWidget(m_pMoveFilePB);
    pFileVBL->addWidget(m_pSelectDirPB);
    QHBoxLayout *pMain=new QHBoxLayout;
    pMain->addWidget(m_pBookListW);
    pMain->addLayout(pDirVBl);
    pMain->addLayout(pFileVBL);
    setLayout(pMain);

    connect(m_pCreateDirPB,&QPushButton::clicked,this,&book::createDir);
    connect(m_pFlushFilePB,&QPushButton::clicked,this,&book::flushFile);
    connect(m_pDelPB,&QPushButton::clicked,this,&book::delDir);
    connect(m_pRenamePB,&QPushButton::clicked,this,&book::reNameFile);
    connect(m_pBookListW,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(enterDir(QModelIndex)));
    connect(m_pReturnPB,&QPushButton::clicked,this,&book::returnPre);
    connect(m_pDelFilePB,&QPushButton::clicked,this,&book::delRegFile);
    connect(m_pUploadPB,&QPushButton::clicked,this,&book::uploadFile);
    connect(m_pTimer,&QTimer::timeout,this,&book::uploadFileDate);
    connect(m_pDownLoadPB,&QPushButton::clicked,this,&book::downloadFile);
    connect(m_pShareeFilePB,&QPushButton::clicked,this,&book::shareFile);
    connect(m_pMoveFilePB,&QPushButton::clicked,this,&book::moveFile);
    connect(m_pSelectDirPB,&QPushButton::clicked,this,&book::selectDestDir);


}
void book::updateFileList(const PDU *pdu)
{
    if(NULL==pdu)
    {
        return;
    }
    QListWidgetItem *pItemTmp=NULL;
    int row=m_pBookListW->count();
    while(m_pBookListW->count()>0)
    {
       pItemTmp= m_pBookListW->item(row-1);
       m_pBookListW->removeItemWidget(pItemTmp);
       delete pItemTmp;
       row=row-1;
    }
    FileInfo *pFileInfo =NULL;
    int iCount=pdu->uiMsgLen/sizeof(FileInfo);
    for(int i=0;i<iCount;i++)
    {
        pFileInfo=(FileInfo*)(pdu->caMsg)+i;
        qDebug()<<pFileInfo->caFileName<<pFileInfo->iFileType;
        QListWidgetItem *pItem=new QListWidgetItem;
        if(0==pFileInfo->iFileType)
        {
            pItem->setIcon(QIcon(QPixmap(":/map/dir.jpg")));
        }
        else if(1==pFileInfo->iFileType)
        {
            pItem->setIcon(QIcon(QPixmap(":/map/reg.jpg")));
        }
        pItem->setText(pFileInfo->caFileName);
        m_pBookListW->addItem(pItem);
    }
}

void book::clearEnterDir()
{
    m_strEnterDir.clear();
}

QString book::enterDir()
{
    return m_strEnterDir;
}

void book::setDownloadStatus(bool status)
{
    m_bDownload=status;

}

bool book::getDownLoadStatus()
{
    return m_bDownload;
}

QString book::getSaveFilePath()
{
    return m_strSaveFilePath;
}
void book::createDir()
{
    QString strNewDir=QInputDialog::getText(this,"新建文件夹","新文件夹名字");
    if(!strNewDir.isEmpty())
    {
        if(strNewDir.size()>32)
        {
            QMessageBox::warning(this,"新建文件夹","新文件夹名字不能超过32个字符");
        }
        else
        {
        QString strName=TcpClinent::getInstance().loginName();//接收登陆者信息
        QString strCurPath=TcpClinent::getInstance().curPath();
        PDU *pdu=mkPDU(strCurPath.size()+1);
        pdu->uiMsgType=ENUM_MSG_TYPE_CREATE_DIR_REQUEST;
        strncpy(pdu->caDate,strName.toStdString().c_str(),strName.size());//将名字拷贝进caDate中
        strncpy(pdu->caDate+32,strNewDir.toStdString().c_str(),strNewDir.size());
        memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
        TcpClinent::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);//将消息包发送到服务器
        free(pdu);
        pdu=NULL;
        }
    }
    else
    {
        QMessageBox::warning(this,"新建文件夹","新文件夹名字不能为空");
    }
}

void book::flushFile()
{
    QString strCurPath=TcpClinent::getInstance().curPath();
    PDU *pdu=mkPDU(strCurPath.size()+1);
    pdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
    strncpy((char*)(pdu->caMsg),strCurPath.toStdString().c_str(),strCurPath.size());
    TcpClinent::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

void book::delDir()
{
    QString strCurPath=TcpClinent::getInstance().curPath();
    QListWidgetItem *pItem=m_pBookListW->currentItem();//选中的文件
    if(NULL==pItem)
    {
        QMessageBox::warning(this,"删除文件","请选中要删除的文件");
    }
    else
    {
        QString strDelName=pItem->text();
        PDU *pdu=mkPDU(strCurPath.size()+1);
        pdu->uiMsgType=ENUM_MSG_TYPE_DEL_DIR_REQUEST;
        strncpy(pdu->caDate,strDelName.toStdString().c_str(),strDelName.size());
        memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
        TcpClinent::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
}

void book::reNameFile()
{
    QString strCurPath=TcpClinent::getInstance().curPath(); //要修改的信息
    QListWidgetItem *pItem=m_pBookListW->currentItem();//选中的文件
    if(NULL==pItem)
    {
        QMessageBox::warning(this,"重命名","请选中你要修改名字的文件夹");
    }
    else
    {
        QString strOldName=pItem->text();
        QString strNewName=QInputDialog::getText(this,"重命名","请输入新的文件名");
        if(!strNewName.isEmpty())
        {
            PDU *pdu=mkPDU(strCurPath.size()+1);
            pdu->uiMsgType=ENUM_MSG_TYPE_RENAME_FILE_REQUEST;
            strncpy(pdu->caDate,strOldName.toStdString().c_str(),strOldName.size());
            strncpy(pdu->caDate+32,strNewName.toStdString().c_str(),strNewName.size());
            memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
            TcpClinent::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
            free(pdu);
            pdu=NULL;
        }
        else
        {
            QMessageBox::warning(this,"重命名","请不要为空");
        }
    }
}

void book::enterDir(const QModelIndex &index)
{
    QString strDirName=index.data().toString();//目录名字
    m_strEnterDir=strDirName;
    QString strCurPath=TcpClinent::getInstance().curPath();//当前所在的目录
    PDU *pdu=mkPDU(strCurPath.size()+1);
    pdu->uiMsgType=ENUM_MSG_TYPE_ENTER_DIR_REQUEST;
    strncpy(pdu->caDate,strDirName.toStdString().c_str(),strDirName.size());
    memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
    TcpClinent::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

void book::returnPre()
{
    QString strCurpath=TcpClinent::getInstance().curPath();
    QString strRootPath="./"+TcpClinent::getInstance().loginName();
    if(strCurpath==strRootPath)
    {
        QMessageBox::warning(this,"返回","返回失败，已经在最开始的目录中");
    }
    else
    {
    int index=strCurpath.lastIndexOf('/');
    strCurpath.remove(index,strCurpath.size()-index);
    TcpClinent::getInstance().setCurPath(strCurpath);
    clearEnterDir();
    flushFile();
    }
}

void book::delRegFile()
{
    QString strCurPath=TcpClinent::getInstance().curPath();
    QListWidgetItem *pItem=m_pBookListW->currentItem();//选中的文件
    if(NULL==pItem)
    {
        QMessageBox::warning(this,"删除文件","请选中要删除的文件");
    }
    else
    {
        QString strDelName=pItem->text();
        PDU *pdu=mkPDU(strCurPath.size()+1);
        pdu->uiMsgType=ENUM_MSG_TYPE_DEL_REG_REQUEST;
        strncpy(pdu->caDate,strDelName.toStdString().c_str(),strDelName.size());
        memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
        TcpClinent::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
}

void book::uploadFile()
{
    QString strCurPath=TcpClinent::getInstance().curPath();
    m_strUploadFilePath=QFileDialog::getOpenFileName();//返回选取文件上传的路径的名字
    if(!m_strUploadFilePath.isEmpty())
    {
        int index=m_strUploadFilePath.lastIndexOf('/');//从后往前找
        QString strFileName=m_strUploadFilePath.right(m_strUploadFilePath.size()-index-1);
        QFile file(m_strUploadFilePath);
        qint64 fileSize=file.size();//活动文件大小
        PDU *pdu=mkPDU(strCurPath.size()+1);
        pdu->uiMsgType=ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;
        memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());
        sprintf(pdu->caDate,"%s %lld",strFileName.toStdString().c_str(),fileSize);
        TcpClinent::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
        m_pTimer->start(1000);
    }
    else
    {
        QMessageBox::warning(this,"上传文件","请选中你要上传的文件");
    }
}

void book::uploadFileDate()//分开请求和内容
{
    m_pTimer->stop();
    QFile file(m_strUploadFilePath);
    if(!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this,"上传文件","打开文件失败");
        return;
    }
       char *pBuffer=new char[4096];
        qint64 ret=0;
        while(true)
        {
            ret=file.read(pBuffer,4096);
            if(ret>0&&ret<=4096)
            {
                TcpClinent::getInstance().getTcpSocket().write(pBuffer,ret);
            }
            else if(0==ret)
            {
                break;
            }
            else
            {
                QMessageBox::warning(this,"上传文件","上传文件失败：读取文件失败");
                break;
            }
        }
        file.close();
        delete []pBuffer;
        pBuffer=NULL;
}

void book::downloadFile()
{
//服务器发送下载请求,服务器把文件大小发送给客户端
    QListWidgetItem *pItem=m_pBookListW->currentItem();//选中的文件
    if(NULL==pItem)
    {
        QMessageBox::warning(this,"下载文件","请选中要下载的文件");
    }
    else
    {
        QString strSaveFilePath=QFileDialog::getSaveFileName();//弹出窗口选择要保存的路径
        if(strSaveFilePath.isEmpty())
        {
            QMessageBox::warning(this,"下载文件","选择的路径不能为空");
            m_strSaveFilePath.clear();
        }
        else
        {
            m_strSaveFilePath=strSaveFilePath;
        }
        QString strCurPath=TcpClinent::getInstance().curPath();
        PDU *pdu=mkPDU(strCurPath.size()+1);
        pdu->uiMsgType=ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST;
        QString strFileName=pItem->text();
        strcpy(pdu->caDate,strFileName.toStdString().c_str());//拷贝选中内容
        memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.size());//拷贝路径
        TcpClinent::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);//发送给服务器

    }
}

void book::shareFile()
{
    QListWidgetItem *pItem=m_pBookListW->currentItem();//选中的文件
    if(NULL==pItem)
    {
        QMessageBox::warning(this,"分享文件","请选中要分享的文件");
        return;
    }
    else
    {
        m_strshareFileName=pItem->text();
    }
       Friend *pFriend =OpeWidget::getInstance().getFriend();//获得friend好友界面列表
       QListWidget *pFriendList=pFriend->getFriendList();
       shareFile::getInstance().updateFriend(pFriendList);//把在线好友列表更新到窗口上
       if(shareFile::getInstance().isHidden())
       {
           shareFile::getInstance().show();
       }

}

void book::moveFile()
{
    QListWidgetItem *pCurItem=m_pBookListW->currentItem();
    if(NULL!= pCurItem)
    {
        m_strMoveFileName=pCurItem->text();
        QString strCutPath =TcpClinent::getInstance().curPath();
        m_strMoveFilePath=strCutPath+'/'+m_strMoveFileName;
        m_pSelectDirPB->setEnabled(true);
;    }
    else
    {
        QMessageBox::warning(this,"移动文件","请选择要移动的文件");
    }
}

void book::selectDestDir()
{
    QListWidgetItem *pCurItem=m_pBookListW->currentItem();
    if(NULL!= pCurItem)
    {
        QString strDestDir=pCurItem->text();
        QString strCutPath =TcpClinent::getInstance().curPath();
        m_strDestDir=strCutPath+'/'+strDestDir;
        int srcLen=m_strMoveFilePath.size();
        int destLen=m_strDestDir.size();
        PDU *pdu=mkPDU(srcLen+destLen+2);
        pdu->uiMsgType=ENUM_MSG_TYPE_MOVE_FILE_REQUEST;
        sprintf(pdu->caDate,"%d %d %s",srcLen,destLen,m_strMoveFileName.toStdString().c_str());//两个路径大小和文件名拷贝
        memcpy(pdu->caMsg,m_strMoveFilePath.toStdString().c_str(),srcLen);
        memcpy((char*)(pdu->caMsg)+(srcLen+1),m_strDestDir.toStdString().c_str(),destLen);
        TcpClinent::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }
    else
    {
        QMessageBox::warning(this,"移动文件","请选择要移动的文件");
    }
    m_pSelectDirPB->setEnabled(false);

}

QString book::getShareFileName()
{
 return   m_strshareFileName;
}
