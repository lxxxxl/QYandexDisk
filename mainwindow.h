#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>

#include "qyandexdisk.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    void appendText(QString text);

public slots:
    void slotRemoved(bool status);
    void slotCreated(bool status);
    void slotUploaded(bool status);
    void slotDownloaded(QByteArray data);
    void slotCapacity(QYandexDisk::CapacityInfo *info);
    void slotSize(qint64 size);
    void slotList(QList<QYandexDisk::FileInfo*> list);
    void slotError();


    void clickCapacity();
    void clickSize();
    void clickMkdir();
    void clickRemove();
    void clickUpload();
    void clickDownload();
    void clickList();


private:
    QLabel *label;
    QYandexDisk *disk;

};

#endif // MAINWINDOW_H
