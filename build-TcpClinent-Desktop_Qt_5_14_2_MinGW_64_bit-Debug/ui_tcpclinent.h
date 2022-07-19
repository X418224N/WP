/********************************************************************************
** Form generated from reading UI file 'tcpclinent.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TCPCLINENT_H
#define UI_TCPCLINENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TcpClinent
{
public:
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout_3;
    QLabel *name_lab;
    QLabel *pwd_lab;
    QVBoxLayout *verticalLayout_2;
    QLineEdit *name_le;
    QLineEdit *pwd_le;
    QPushButton *login_pb;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *regist_pb;
    QSpacerItem *horizontalSpacer;
    QPushButton *cancel_pb;
    QSpacerItem *horizontalSpacer_3;

    void setupUi(QWidget *TcpClinent)
    {
        if (TcpClinent->objectName().isEmpty())
            TcpClinent->setObjectName(QString::fromUtf8("TcpClinent"));
        TcpClinent->resize(487, 333);
        horizontalLayout = new QHBoxLayout(TcpClinent);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        name_lab = new QLabel(TcpClinent);
        name_lab->setObjectName(QString::fromUtf8("name_lab"));
        QFont font;
        font.setFamily(QString::fromUtf8("\345\276\256\350\275\257\351\233\205\351\273\221"));
        font.setPointSize(16);
        name_lab->setFont(font);

        verticalLayout_3->addWidget(name_lab);

        pwd_lab = new QLabel(TcpClinent);
        pwd_lab->setObjectName(QString::fromUtf8("pwd_lab"));
        pwd_lab->setFont(font);
        pwd_lab->setLayoutDirection(Qt::LeftToRight);

        verticalLayout_3->addWidget(pwd_lab);


        horizontalLayout_3->addLayout(verticalLayout_3);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        name_le = new QLineEdit(TcpClinent);
        name_le->setObjectName(QString::fromUtf8("name_le"));

        verticalLayout_2->addWidget(name_le);

        pwd_le = new QLineEdit(TcpClinent);
        pwd_le->setObjectName(QString::fromUtf8("pwd_le"));
        pwd_le->setEchoMode(QLineEdit::Password);

        verticalLayout_2->addWidget(pwd_le);


        horizontalLayout_3->addLayout(verticalLayout_2);


        verticalLayout_4->addLayout(horizontalLayout_3);

        login_pb = new QPushButton(TcpClinent);
        login_pb->setObjectName(QString::fromUtf8("login_pb"));
        login_pb->setFont(font);

        verticalLayout_4->addWidget(login_pb);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);

        regist_pb = new QPushButton(TcpClinent);
        regist_pb->setObjectName(QString::fromUtf8("regist_pb"));

        horizontalLayout_2->addWidget(regist_pb);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        cancel_pb = new QPushButton(TcpClinent);
        cancel_pb->setObjectName(QString::fromUtf8("cancel_pb"));

        horizontalLayout_2->addWidget(cancel_pb);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_3);


        verticalLayout_4->addLayout(horizontalLayout_2);


        horizontalLayout->addLayout(verticalLayout_4);


        retranslateUi(TcpClinent);

        QMetaObject::connectSlotsByName(TcpClinent);
    } // setupUi

    void retranslateUi(QWidget *TcpClinent)
    {
        TcpClinent->setWindowTitle(QCoreApplication::translate("TcpClinent", "TcpClinent", nullptr));
        name_lab->setText(QCoreApplication::translate("TcpClinent", "\347\224\250\346\210\267\345\220\215:", nullptr));
        pwd_lab->setText(QCoreApplication::translate("TcpClinent", "\345\257\206\347\240\201:", nullptr));
        login_pb->setText(QCoreApplication::translate("TcpClinent", "\347\231\273\345\275\225", nullptr));
        regist_pb->setText(QCoreApplication::translate("TcpClinent", "\346\263\250\345\206\214", nullptr));
        cancel_pb->setText(QCoreApplication::translate("TcpClinent", "\346\263\250\351\224\200", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TcpClinent: public Ui_TcpClinent {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TCPCLINENT_H
