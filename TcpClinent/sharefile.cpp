#include "sharefile.h"
#include"opewidget.h"
shareFile::shareFile(QWidget *parent) : QWidget(parent)
{
    m_pSelectAllPB=new QPushButton("全选");
    m_pCancelSelectPB=new QPushButton("取消选择");
    m_pOKPB=new QPushButton("确定");
    m_pCancelPB=new QPushButton("取消");
    m_pSA=new QScrollArea;//展示区域
    m_pFriendW=new QWidget;
    m_pFriendWVBL=new QVBoxLayout(m_pFriendW);
    m_pButtonGroup=new QButtonGroup(m_pFriendW);//管理选项
    m_pButtonGroup->setExclusive(false);//设置复选框多个
    QHBoxLayout *pTopHBL=new QHBoxLayout;
    pTopHBL->addWidget(m_pSelectAllPB);
    pTopHBL->addWidget(m_pCancelSelectPB);
    pTopHBL->addStretch();
    QHBoxLayout *pDOWNHBL=new QHBoxLayout;
    pDOWNHBL->addWidget(m_pOKPB);
    pDOWNHBL->addWidget(m_pCancelPB);
    QVBoxLayout *pMainVbl=new QVBoxLayout;
    pMainVbl->addLayout(pTopHBL);
    pMainVbl->addWidget(m_pSA);
    pMainVbl->addLayout(pDOWNHBL);
    setLayout(pMainVbl);

    connect(m_pCancelSelectPB,&QPushButton::clicked,this,&shareFile::cancelSelect);
    connect(m_pSelectAllPB,&QPushButton::clicked,this,&shareFile::selectAll);
    connect(m_pOKPB,&QPushButton::clicked,this,&shareFile::okshare);
    connect(m_pCancelPB,&QPushButton::clicked,this,&shareFile::cancelshare);

}

shareFile &shareFile::getInstance()
{
    static shareFile instance;
    return instance;
}

void shareFile::test()
{
    QHBoxLayout *p=new QHBoxLayout(m_pFriendW);
    QCheckBox *pCB=NULL;
    for(int i=0;i<15;i++)
    {
        pCB=new QCheckBox("jack");
        p->addWidget(pCB);
        m_pButtonGroup->addButton(pCB);
    }
    m_pSA->setWidget(m_pFriendW);
}

void shareFile::updateFriend(QListWidget *pFriendList)
{
    if(NULL==pFriendList)
    {
        return;
    }
    QAbstractButton *tmp=NULL;
    QList<QAbstractButton *> preFriendList= m_pButtonGroup->buttons();
    for(int i=0;i<preFriendList.size();i++)
    {
        tmp=preFriendList[i];
        m_pFriendWVBL->removeWidget(tmp);
        m_pButtonGroup->removeButton(tmp);
        preFriendList.removeOne(tmp);
        delete tmp;
        tmp =NULL;
    }

    QCheckBox *pCB=NULL;
    for(int i=0;i<pFriendList->count();i++)
    {
        pCB=new QCheckBox(pFriendList->item(i)->text());
        m_pFriendWVBL->addWidget(pCB);
        m_pButtonGroup->addButton(pCB);
    }
    m_pSA->setWidget(m_pFriendW);

}

void shareFile::cancelSelect()
{
   QList<QAbstractButton *> cbList= m_pButtonGroup->buttons();//通过buttongroup获取所有checkBOX
   for(int i=0;i<cbList.size();i++)
   {
       if(cbList[i]->isChecked())
       {
           cbList[i]->setChecked(false);
       }
   }
}

void shareFile::selectAll()
{
    QList<QAbstractButton *> cbList= m_pButtonGroup->buttons();//通过buttongroup获取所有checkBOX
    for(int i=0;i<cbList.size();i++)
    {
        if(!cbList[i]->isChecked())
        {
            cbList[i]->setChecked(true);
        }
    }
}

void shareFile::okshare()
{
    QString strName=TcpClinent::getInstance().loginName();//获得发送者
    QString strCurPath=TcpClinent::getInstance().curPath();//获得当前路径
    QString strShareFileName=OpeWidget::getInstance().getBook()->getShareFileName();
    QString strPath=strCurPath+"/"+strShareFileName;//当前路径和要发送的文件
    //计算选择了多少人
    QList<QAbstractButton *> cbList= m_pButtonGroup->buttons();//通过butto合成路径ngroup获取所有checkBOX
    int num=0;
    for(int i=0;i<cbList.size();i++)
    {
        if(cbList[i]->isChecked())
        {
            num++;
        }
    }
    PDU *pdu=mkPDU(32*num+strPath.size()+1);
    pdu->uiMsgType=ENUM_MSG_TYPE_SHARE_FILE_REQUEST;
    sprintf(pdu->caDate,"%s %d",strName.toStdString().c_str(),num);//分享者和多少人
    int j=0;
    for(int i=0;i<cbList.size();i++)
    {
        if(cbList[i]->isChecked())
        {
            memcpy((char*)(pdu->caMsg)+j*32,cbList[i]->text().toStdString().c_str(),cbList[i]->text().size());//将每个人拷贝进去
            j++;
        }
    }
    memcpy((char*)(pdu->caMsg)+num*32,strPath.toStdString().c_str(),strPath.size());//拷贝路径
    TcpClinent::getInstance().getTcpSocket().write((char*)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;

}

void shareFile::cancelshare()
{
    hide();
}
