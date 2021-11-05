/* Yandex Disk client library
 *
 * TODO:
 */

#ifndef QYANDEXDISK_H
#define QYANDEXDISK_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>



class QYandexDisk : public QObject
{
    Q_OBJECT
public:
    explicit QYandexDisk(QString token, QObject *parent = nullptr);
    // download file from disk
    // https://yandex.ru/dev/disk/api/reference/content.html
    void download(QString filename);
    // upload file to disk
    // https://yandex.ru/dev/disk/api/reference/upload.html
    void upload(QString filename, QByteArray data);
    // remove file or directory from disk
    // https://yandex.ru/dev/disk/api/reference/delete.html
    void remove(QString path);
    // create new directory
    // https://yandex.ru/dev/disk/api/reference/create-folder.html
    void mkdir(QString path);
    // list files in directory
    void list(QString path);
    // get file size
    // https://yandex.ru/dev/disk/api/reference/meta.html
    void size(QString path);
    // https://yandex.ru/dev/disk/api/reference/capacity.html
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
    void readySize();

signals:
    void signalRemoved(bool status);
    void signalCreated(bool status);
    void signalCapacity(QYandexDisk::CapacityInfo *info);
    void signalUploaded(bool status);
    void signalDownloaded(QByteArray data);
    void signalSize(qint64 size);
    void signalError();

private:
    // creates QNetworkRequest with provided Disk API method and it's arg
    QNetworkRequest* createRequest(QString method, QString arg);

    QString token;
    QNetworkAccessManager *m_networkAccessManager;
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
