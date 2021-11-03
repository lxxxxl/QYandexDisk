#include "qyandexdisk.h"

QYandexDisk::QYandexDisk(QString token, QObject *parent) : QObject(parent)
{
    this->token = token;

    m_networkAccessManager = new QNetworkAccessManager(this);
}


void QYandexDisk::download(QString filename)
{

}
// upload file to disk
void QYandexDisk::upload(QString filename, QByteArray data)
{

}
// remove file or directory from disk
void QYandexDisk::remove(QString path)
{
    QNetworkRequest *request = createRequest("resources", path);
    m_reply = this->m_networkAccessManager->deleteResource(*request);
    connect(m_reply, SIGNAL(readyRead()), this, SLOT(readyRemove()));
    delete request;
}
// create new directory
void QYandexDisk::mkdir(QString path)
{
    QNetworkRequest *request = createRequest("resources", path);
    m_reply = this->m_networkAccessManager->put(*request, QByteArray());
    connect(m_reply, SIGNAL(readyRead()), this, SLOT(readyMkdir()));
    delete request;

}
// list files in directory
void QYandexDisk::list(QString path)
{

}
// get file size
void QYandexDisk::size(QString path)
{

}
// get free space count on Disk
void QYandexDisk::capacity()
{
    QNetworkRequest *request = createRequest(QString(), QString());
    m_reply = this->m_networkAccessManager->get(*request);
    connect(m_reply, SIGNAL(readyRead()), this, SLOT(readyCapacity()));
    delete request;
}

QNetworkRequest *QYandexDisk::createRequest(QString method, QString arg)
{

    QString url = baseUrl + method;
    if (arg.length() > 0)
        url += "?path=" + arg;
    QNetworkRequest *request = new QNetworkRequest(QUrl(url));
    request->setRawHeader("Authorization", QString("OAuth " + this->token).toLocal8Bit());
    return request;
}

// slot for processing "capacity" API request
void QYandexDisk::readyCapacity()
{
    QVariant statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int status = statusCode.toInt();
    if (status != HTTP_OK){
        emit signalError();
        return;
    }

    QByteArray reply = m_reply->readAll();
    // parse JSON
    QJsonDocument replyDoc = QJsonDocument::fromJson(reply);
    QJsonObject rootObject = replyDoc.object();

    CapacityInfo info;
    info.totalSpace = rootObject.value("total_space").toDouble(0);
    info.trashSize = rootObject.value("trash_size").toDouble(0);
    info.usedSpace = rootObject.value("used_space").toDouble(0);
    m_reply->deleteLater();
    emit signalCapacity(&info);

}

// slot for processing "delete" API request
void QYandexDisk::readyRemove()
{
    QVariant statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int status = statusCode.toInt();
    m_reply->deleteLater();
    emit signalRemoved((status == HTTP_NO_CONTENT) || (status == HTTP_ACCEPTED));
}

// slot for processing "create-folder" API request
void QYandexDisk::readyMkdir()
{
    QVariant statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int status = statusCode.toInt();
    m_reply->deleteLater();
    emit signalCreated(status == HTTP_CREATED);
}
