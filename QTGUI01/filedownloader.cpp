#include "filedownloader.h"

FileDownloader::FileDownloader(QObject *parent) :
    QObject(parent)
{
}

FileDownloader::~FileDownloader() { }

void FileDownloader::doDownload()
{
    manager = new QNetworkAccessManager(this);

    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(replyFinished(QNetworkReply*)));
    QString urlstr = "http://maps.googleapis.com/maps/api/staticmap?center=30.000000,-90.000000&zoom=19&maptype=hybrid&size=640x640&format=jpg&key=";
    QNetworkRequest req = QNetworkRequest(QUrl(urlstr));
    //req.setSslConfiguration(QSslConfiguration::defaultConfiguration());
    manager->get(req);
}

void FileDownloader::replyFinished (QNetworkReply *reply)
{
    if(reply->error())
    {
        qDebug() << "ERROR!";
        qDebug() << reply->errorString();
    }
    else
    {
        qDebug() << "Get here";
        qDebug() << reply->header(QNetworkRequest::ContentTypeHeader).toString();
        qDebug() << reply->header(QNetworkRequest::LastModifiedHeader).toDateTime().toString();;
        qDebug() << reply->header(QNetworkRequest::ContentLengthHeader).toULongLong();
        qDebug() << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        qDebug() << reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
        QByteArray tempData = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(tempData);
        if (doc.isNull())
        {
            qDebug() << "Null";
        }
        if (doc.isObject())
        {
            qDebug() << "Is object";
        }
        QJsonObject obj = doc.object();
        qDebug() << obj;
        QString currentWorkingPath = QDir::currentPath();
        QFile *file = new QFile(currentWorkingPath+"/downloaded.txt");
        if(file->open(QFile::Append))
        {
            file->write(reply->readAll());
            file->flush();
            file->close();
        }
        delete file;
    }

    reply->deleteLater();
}
