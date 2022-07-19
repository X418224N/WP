#ifndef TCPCLINENT_H
#define TCPCLINENT_H

#include <QWidget>
#include<QFile>
#include<QTcpSocket>
#include"protocol.h"
#include"opewidget.h"
QT_BEGIN_NAMESPACE
namespace Ui { class TcpClinent; }
QT_END_NAMESPACE

class TcpClinent : public QWidget
{
    Q_OBJECT

public:
    TcpClinent(QWidget *parent = nullptr);
    ~TcpClinent();
    void loadConfig();
    static TcpClinent &getInstance();
    QTcpSocket &getTcpSocket();
    QString loginName();
    QString curPath();//返回当前路径
    void setCurPath(QString strCurPath);
public slots:
    void showConnect();
    void recvMsg();
private slots:
//    void on_send_pb_clicked();

    void on_login_pb_clicked();

    void on_regist_pb_clicked();

    void on_cancel_pb_clicked();

private:
    Ui::TcpClinent *ui;
    QString m_strIP;
    quint16 m_usPort;
    //连接服务器，和服务器进行数据交互
    QTcpSocket m_tcpsocket;
    QString m_strloginName;//保存自己的用户名
    QString m_strCurPath;//保存当前所在的目录
    QFile m_file;
};
#endif // TCPCLINENT_H
