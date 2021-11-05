#include "qyandexdisk.h"

QYandexDisk::QYandexDisk(QString token, QObject *parent) : QObject(parent)
{
    this->token = token;

    m_networkAccessManager = new QNetworkAccessManager(this);
    // allow processing HTTP redirects
    m_networkAccessManager->setRedirectPolicy(QNetworkRequest::NoLessSafeRedirectPolicy);
}


void QYandexDisk::download(QString filename)
{
    QNetworkRequest *request = createRequest("resources/download", filename);
    m_reply = this->m_networkAccessManager->get(*request);
    connect(m_reply, SIGNAL(finished()), this, SLOT(readyDownloadPhase1()));
}
// upload file to disk
void QYandexDisk::upload(QString filename, QByteArray data)
{
    QNetworkRequest *request = createRequest("resources/upload", filename);
    m_reply = this->m_networkAccessManager->get(*request);
    m_reply->setProperty("data", data);
    connect(m_reply, SIGNAL(finished()), this, SLOT(readyUploadPhase1()));
}
// remove file or directory from disk
void QYandexDisk::remove(QString path)
{
    QNetworkRequest *request = createRequest("resources", path);
    m_reply = this->m_networkAccessManager->deleteResource(*request);
    connect(m_reply, SIGNAL(finished()), this, SLOT(readyRemove()));
    delete request;
}
// create new directory
void QYandexDisk::mkdir(QString path)
{
    QNetworkRequest *request = createRequest("resources", path);
    m_reply = this->m_networkAccessManager->put(*request, QByteArray());
    connect(m_reply, SIGNAL(finished()), this, SLOT(readyMkdir()));
    delete request;

}
// list files in directory
void QYandexDisk::list(QString path)
{
    QNetworkRequest *request = createRequest("resources", path);
    m_reply = this->m_networkAccessManager->get(*request);
    connect(m_reply, SIGNAL(finished()), this, SLOT(readyList()));
}
// get file size
void QYandexDisk::size(QString path)
{
    QNetworkRequest *request = createRequest("resources", path);
    m_reply = this->m_networkAccessManager->get(*request);
    connect(m_reply, SIGNAL(readyRead()), this, SLOT(readySize()));
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

// slot for processing "upload" API request phase 1
void QYandexDisk::readyUploadPhase1()
{
    QVariant statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int status = statusCode.toInt();
    if (status != HTTP_OK){
        emit signalError();
        return;
    }

    QByteArray reply = m_reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(reply);
    QJsonObject obj = doc.object();

    QString href = obj.value("href").toString();
    QNetworkRequest *request = new QNetworkRequest(QUrl(href));
    QByteArray data = m_reply->property("data").toByteArray();
    m_reply = this->m_networkAccessManager->put(*request, data);
    connect(m_reply, SIGNAL(finished()), this, SLOT(readyUploadPhase2()));
}

// slot for processing "upload" API request phase 2
void QYandexDisk::readyUploadPhase2()
{
    QVariant statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int status = statusCode.toInt();
    emit signalUploaded(status == HTTP_CREATED);
}

// slot for processing "download" API request phase 1
void QYandexDisk::readyDownloadPhase1()
{
    QVariant statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int status = statusCode.toInt();
    if (status != HTTP_OK){
        emit signalError();
        return;
    }

    QByteArray reply = m_reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(reply);
    QJsonObject obj = doc.object();

    QString href = obj.value("href").toString();
    QNetworkRequest *request = new QNetworkRequest(QUrl(href));
    m_reply = this->m_networkAccessManager->get(*request);
    connect(m_reply, SIGNAL(finished()), this, SLOT(readyDownloadPhase2()));
}

// slot for processing "download" API request phase 2
void QYandexDisk::readyDownloadPhase2()
{
    QVariant statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int status = statusCode.toInt();
    if (status == HTTP_OK)
        emit signalDownloaded(m_reply->readAll());
    else
        emit signalError();
}

// slot for processing "size" API request
void QYandexDisk::readySize()
{
    QVariant statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int status = statusCode.toInt();
    if (status != HTTP_OK){
        emit signalError();
        return;
    }

    QByteArray reply = m_reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(reply);
    QJsonObject obj = doc.object();

    if (!obj.contains("size")){
        emit signalError();
        return;
    }

    qint64 size = obj.value("size").toDouble(0);
    emit signalSize(size);
}

// slot for processing "list" API request
void QYandexDisk::readyList()
{
    QVariant statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int status = statusCode.toInt();
    if (status != HTTP_OK){
        emit signalError();
        return;
    }

    QList<FileInfo*> list;

    QByteArray reply = m_reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(reply);
    QJsonObject obj = doc.object();

    QJsonObject _embedded = obj.value("_embedded").toObject();
    QJsonArray items = _embedded.value("items").toArray();

    for (const QJsonValue& item: qAsConst(items)){
        QJsonObject object = item.toObject();
        FileInfo *info = new FileInfo;
        QString type = object.value("type").toString();
        info->isDir = (type == "dir");
        if (info->isDir){
            info->md5 = QString();
            info->size = 0;
        }
        else{
            info->md5 = object.value("md5").toString();
            info->size = object.value("size").toDouble();
        }
        info->name = object.value("name").toString();
        info->path = object.value("path").toString();
        QString created = object.value("created").toString();
        info->created = QDateTime::fromString(created, Qt::ISODate);
        info->modified = QDateTime::fromString(object.value("modified").toString(), Qt::ISODate);

        list.append(info);
    }
    emit signalList(list);
}
