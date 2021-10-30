/* Yandex Disk client library
*/

#ifndef QYANDEXDISK_H
#define QYANDEXDISK_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>



class QYandexDisk : public QObject
{
    Q_OBJECT
public:
    explicit QYandexDisk(QString token, QObject *parent = nullptr);
    // download file from disk
    void download(QString filename);
    // upload file to disk
    void upload(QString filename, QByteArray data);
    // remove file or directory from disk
    void remove(QString path);
    // create new directory
    void mkdir(QString path);
    // list files in directory
    void list(QString path);
    // get file size
    void size(QString path);
    // get free space count on Disk
    void capacity();

    struct CapacityInfo{
        qint64 trashSize;
        qint64 totalSpace;
        qint64 usedSpace;
    };


public slots:
    void readyRemove();
    void readyMkdir();
    void readyCapacity();
    void readyUploadPhase1();
    void readyUploadPhase2();
    void readyDownloadPhase1();
    void readyDownloadPhase2();

signals:
    void signalRemoved(bool status);
    void signalCreated(bool status);
    void signalCapacity(CapacityInfo *info);
    void signalUploaded(bool status);
    void signalDownloaded(QByteArray data);
    void signalError();

private:
    // fills m_request with provided Disk API method and it's arg
    void fillRequest(QString method, QString arg);

    QString token;
    QNetworkAccessManager *m_networkAccessManager;
    QNetworkRequest m_request;
    QNetworkReply *m_reply;

    QString baseUrl = "https://cloud-api.yandex.net/v1/disk/";


    enum HttpResponseCode{
                HTTP_OK = 200,
                HTTP_CREATED = 201,
                HTTP_ACCEPTED = 202,
                HTTP_NO_CONTENT = 204,
                HTTP_MOVED_PERMANENTLY = 301,
                HTTP_MOVED_TEMPORARILY = 302
    };
};

#endif // QYANDEXDISK_H