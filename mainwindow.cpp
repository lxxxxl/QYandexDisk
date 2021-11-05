#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    label = new QLabel();
    disk = new QYandexDisk("TOKEN");

    connect(disk, SIGNAL(signalRemoved(bool)), this, SLOT(slotRemoved(bool)));
    connect(disk, SIGNAL(signalCreated(bool)), this, SLOT(slotCreated(bool)));
    connect(disk, SIGNAL(signalUploaded(bool)), this, SLOT(slotUploaded(bool)));
    connect(disk, SIGNAL(signalDownloaded(QByteArray)), this, SLOT(slotDownloaded(QByteArray)));
    connect(disk, SIGNAL(signalCapacity(QYandexDisk::CapacityInfo*)), this, SLOT(slotCapacity(QYandexDisk::CapacityInfo*)));
    connect(disk, SIGNAL(signalSize(qint64)), this, SLOT(slotSize(qint64)));
    connect(disk, SIGNAL(signalList(QList<QYandexDisk::FileInfo*>)), this, SLOT(slotList(QList<QYandexDisk::FileInfo*>)));
    connect(disk, SIGNAL(signalError()), this, SLOT(slotError()));

    QVBoxLayout *layout = new QVBoxLayout(this);

    QPushButton *buttonCapacity = new QPushButton("Capatity");
    connect(buttonCapacity, SIGNAL(clicked()), this, SLOT(clickCapacity()));
    layout->addWidget(buttonCapacity);

    QPushButton *buttonList = new QPushButton("List");
    connect(buttonList, SIGNAL(clicked()), this, SLOT(clickList()));
    layout->addWidget(buttonList);

    QPushButton *buttonSize = new QPushButton("Size");
    connect(buttonSize, SIGNAL(clicked()), this, SLOT(clickSize()));
    layout->addWidget(buttonSize);

    QPushButton *buttonMkdir = new QPushButton("Mkdir");
    connect(buttonMkdir, SIGNAL(clicked()), this, SLOT(clickMkdir()));
    layout->addWidget(buttonMkdir);

    QPushButton *buttonRemove = new QPushButton("Remove");
    connect(buttonRemove, SIGNAL(clicked()), this, SLOT(clickRemove()));
    layout->addWidget(buttonRemove);

    QPushButton *buttonUpload = new QPushButton("Upload");
    connect(buttonUpload, SIGNAL(clicked()), this, SLOT(clickUpload()));
    layout->addWidget(buttonUpload);

    QPushButton *buttonDownload = new QPushButton("Download");
    connect(buttonDownload, SIGNAL(clicked()), this, SLOT(clickDownload()));
    layout->addWidget(buttonDownload);

    layout->addWidget(label);
    label->setText("aaa");

    QWidget *widget = new QWidget();
    widget->setLayout(layout);
    setCentralWidget(widget);
}

void MainWindow::slotRemoved(bool status)
{
    QString text = QString("removed: %1").arg(status);
    label->setText(text);
}

void MainWindow::slotCreated(bool status)
{
    QString text = QString("created: %1").arg(status);
    label->setText(text);
}

void MainWindow::slotCapacity(QYandexDisk::CapacityInfo *info)
{
    qint64 freeSpace = info->totalSpace - info->usedSpace;
    QString text = QString("Free space: %1MB").arg(freeSpace / 1024 / 1024);
    label->setText(text);
}

void MainWindow::slotUploaded(bool status)
{
    QString text = QString("uploaded: %1").arg(status);
    label->setText(text);
}

void MainWindow::slotSize(qint64 size)
{
    QString text = QString("size: %1 bytes").arg(size);
    label->setText(text);
}

void MainWindow::slotDownloaded(QByteArray data)
{
    QString text = QString(data);
    label->setText(text);
}

void MainWindow::slotList(QList<QYandexDisk::FileInfo*> list)
{
    QString text = QString("files: %1").arg(list.size());
    label->setText(text);
    for (QYandexDisk::FileInfo* f: list)
        delete f;
}

void MainWindow::slotError()
{
    label->setText("Error occured");
}

void MainWindow::clickCapacity()
{
    label->setText("capacity()");
    disk->capacity();
}

void MainWindow::clickList()
{
    label->setText("lsit()");
    disk->list("/test");
}

void MainWindow::clickSize()
{
    label->setText("size()");
    disk->size("/test.txt");
}

void MainWindow::clickMkdir()
{
    label->setText("mkdir()");
    disk->mkdir("/test");
}

void MainWindow::clickRemove()
{
    label->setText("remove()");
    disk->remove("/test.txt");
}

void MainWindow::clickUpload()
{
    label->setText("upload()");
    disk->upload("/test.txt", QString("Hello World!").toLocal8Bit());
}

void MainWindow::clickDownload()
{
    label->setText("download()");
    disk->download("/test.txt");
}
