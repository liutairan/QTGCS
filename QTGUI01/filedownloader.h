#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include <QApplication>
#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSsl>
#include <QSslConfiguration>
#include <QSslCertificate>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>

class FileDownloader : public QObject
{
    Q_OBJECT
public:
    explicit FileDownloader(QObject *parent = 0);
    virtual ~FileDownloader();
    void doDownload();

signals:

public slots:
    void replyFinished (QNetworkReply *reply);

private:
   QNetworkAccessManager *manager;
};

#endif // FILEDOWNLOADER_H
