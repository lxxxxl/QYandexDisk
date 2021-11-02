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

}
// create new directory
void QYandexDisk::mkdir(QString path)
{

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
    fillRequest(QString(), QString());
    m_reply = this->m_networkAccessManager->get(m_request);
    connect(m_reply, SIGNAL(readyRead()), this, SLOT(readyCapacity()));
}

void QYandexDisk::fillRequest(QString method, QString arg)
{
    QString url = baseUrl + method;
    if (arg.length() > 0)
        url += "?path=" + arg;
    m_request.setUrl(QUrl(url));
    m_request.setRawHeader("Authorization", QString("OAuth " + this->token).toLocal8Bit());
}

// slot for processing "capacity" API request
void QYandexDisk::readyCapacity()
{
    QVariant statusCode = m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    int status = statusCode.toInt();
    if (status != 200){
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
    emit signalCapacity(&info);
}
