/********************************************************************************
** Form generated from reading UI file 'online.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ONLINE_H
#define UI_ONLINE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_online
{
public:
    QHBoxLayout *horizontalLayout;
    QListWidget *listWidget;
    QVBoxLayout *verticalLayout;
    QSpacerItem *verticalSpacer_2;
    QPushButton *addfriendPB;
    QSpacerItem *verticalSpacer;

    void setupUi(QWidget *online)
    {
        if (online->objectName().isEmpty())
            online->setObjectName(QString::fromUtf8("online"));
        online->resize(516, 300);
        horizontalLayout = new QHBoxLayout(online);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        listWidget = new QListWidget(online);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));

        horizontalLayout->addWidget(listWidget);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalSpacer_2 = new QSpacerItem(20, 58, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer_2);

        addfriendPB = new QPushButton(online);
        addfriendPB->setObjectName(QString::fromUtf8("addfriendPB"));

        verticalLayout->addWidget(addfriendPB);

        verticalSpacer = new QSpacerItem(20, 98, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        horizontalLayout->addLayout(verticalLayout);


        retranslateUi(online);

        QMetaObject::connectSlotsByName(online);
    } // setupUi

    void retranslateUi(QWidget *online)
    {
        online->setWindowTitle(QCoreApplication::translate("online", "Form", nullptr));
        addfriendPB->setText(QCoreApplication::translate("online", "\346\267\273\345\212\240\345\245\275\345\217\213", nullptr));
    } // retranslateUi

};

namespace Ui {
    class online: public Ui_online {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ONLINE_H
