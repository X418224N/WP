#ifndef SHAREFILE_H
#define SHAREFILE_H

#include <QWidget>
#include<QPushButton>
#include<QHBoxLayout>
#include<QVBoxLayout>
#include<QButtonGroup>
#include<QScrollArea>
#include<QCheckBox>
#include<QListWidget>
#include"tcpclinent.h"
class shareFile : public QWidget
{
    Q_OBJECT
public:
    explicit shareFile(QWidget *parent = nullptr);
    static shareFile &getInstance();
    void test();
    void updateFriend(QListWidget *pFriendList);
public slots:
    void cancelSelect();
    void selectAll();
    void okshare();
    void cancelshare();
signals:

private:
    QPushButton *m_pSelectAllPB;
    QPushButton *m_pCancelSelectPB;
    QPushButton *m_pOKPB;
    QPushButton *m_pCancelPB;
    QScrollArea *m_pSA;//展示区域
    QWidget *m_pFriendW;
    QVBoxLayout *m_pFriendWVBL;
    QButtonGroup *m_pButtonGroup;
};

#endif // SHAREFILE_H
