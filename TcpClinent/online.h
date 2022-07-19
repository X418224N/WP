#ifndef ONLINE_H
#define ONLINE_H

#include <QWidget>
#include"protocol.h"

namespace Ui {
class online;
}

class online : public QWidget
{
    Q_OBJECT

public:
    explicit online(QWidget *parent = nullptr);
    ~online();
    void showUsr(PDU *pdu);
Ui::online *ui;
private slots:
void on_addfriendPB_clicked();

private:
//    Ui::online *ui;
};

#endif // ONLINE_H
