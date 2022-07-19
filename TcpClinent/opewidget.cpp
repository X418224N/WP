#include "opewidget.h"

OpeWidget::OpeWidget(QWidget *parent) : QWidget(parent)
{
    m_plistW=new QListWidget(this);
    m_plistW->addItem("好友");
    m_plistW->addItem("图书");
    m_pFriend=new Friend;
    m_pBook=new book;
    m_pSW=new QStackedWidget;
    m_pSW->addWidget(m_pFriend);
    m_pSW->addWidget(m_pBook);
    QHBoxLayout *pMain=new QHBoxLayout;
    pMain->addWidget(m_plistW);
    pMain->addWidget(m_pSW);
    setLayout(pMain);
    connect(m_plistW,SIGNAL(currentRowChanged(int)),m_pSW,SLOT(setCurrentIndex(int)));//实现好友和图书之间的切换
}

OpeWidget &OpeWidget::getInstance()
{
    static OpeWidget instance;
    return instance;
}

Friend *OpeWidget::getFriend()
{
    return m_pFriend;
}

book *OpeWidget::getBook()
{
    return m_pBook;
}
