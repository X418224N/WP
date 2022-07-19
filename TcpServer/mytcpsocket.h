#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H
#include"protocol.h"
#include<QTcpSocket>
#include"opedb.h"
#include<QFile>
#include<QTimer>
class MyTcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    MyTcpSocket();
    QString getName();
    void copyDir(QString strSrcDir,QString strDestDir);
signals:
    void offLine(MyTcpSocket *mysocket);
public slots:
    void recvMsg();//当socket有信号过来发出readyread信号，用recvMsg接受
    void clientOffline();//用来处理客户端下线信号
    void sendFileToClinent();
private:
        QString m_strName;
        QFile m_file;
        qint64 m_iTotal;
        qint64 m_iRecved;
        bool m_bUploadt;//上传文件状态
        QTimer *m_pTimer;
};

#endif // MYTCPSOCKET_H
