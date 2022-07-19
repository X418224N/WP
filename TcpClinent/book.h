#ifndef BOOK_H
#define BOOK_H

#include <QWidget>
#include<QPushButton>
#include<QListWidget>
#include<QHBoxLayout>
#include<QVBoxLayout>
#include"protocol.h"
#include<QTimer>
class book : public QWidget
{
    Q_OBJECT
public:
    explicit book(QWidget *parent = nullptr);
    void updateFileList(const PDU *pdu);
    void clearEnterDir();
    QString enterDir();
    void setDownloadStatus(bool status);
    bool getDownLoadStatus();
    QString getSaveFilePath();//活动私用的保存的路径
    QString getShareFileName();

    qint64 m_iTotal;//总文件大小
    qint64 m_iRecved;//已收到多少
public slots:
    void createDir();
    void flushFile();
    void delDir();
    void reNameFile();
    void enterDir(const QModelIndex &index);
    void returnPre();
    void delRegFile();
    void uploadFile();
    void uploadFileDate();
    void downloadFile();
    void shareFile();
    void moveFile();
    void selectDestDir();
signals:
private:
    QListWidget *m_pBookListW;
    QPushButton *m_pReturnPB;
    QPushButton *m_pCreateDirPB;
    QPushButton *m_pDelPB;
    QPushButton *m_pRenamePB;
    QPushButton *m_pFlushFilePB;
    QPushButton *m_pUploadPB;
    QPushButton *m_pDownLoadPB;
    QPushButton *m_pDelFilePB;
    QPushButton *m_pShareeFilePB;
    QPushButton *m_pMoveFilePB;
    QPushButton *m_pSelectDirPB;
    QString m_strEnterDir;
    QString m_strUploadFilePath;//用来保存打开的路径
    QTimer *m_pTimer;
    QString m_strSaveFilePath;
    bool m_bDownload;
    QString m_strshareFileName;
    QString m_strMoveFileName;
    QString m_strMoveFilePath;
    QString m_strDestDir;

};

#endif // BOOK_H
