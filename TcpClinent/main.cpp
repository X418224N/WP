#include "tcpclinent.h"
#include"opewidget.h"
#include <QApplication>
#include"online.h"
#include"friend.h"
#include<QMessageBox>
#include"sharefile.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    TcpClinent w;
//    w.show();
//    OpeWidget w;
//    w.show();
//    online w;
//    w.show();
//    Friend w;
//    w.show();
    TcpClinent::getInstance().show();
//shareFile w;
//w.show();
    return a.exec();
}
