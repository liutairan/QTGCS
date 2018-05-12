#ifndef FILEIO_H
#define FILEIO_H

#include <QObject>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QByteArray>
#include <QString>

    QString readFile(const QString &fileName);
    void writeFile(const QString &fileName, QString outString);
    QByteArray readJsonFile(const QString &fileName);
    void writeJsonFile(const QString &fileName, QString outString);
    QByteArray readMapIndexFile(const QString &fileName);
    void writeMapIndexFile(const QString &fileName, QString outString);
    QStringList readXBeeAddrFile(const QString &fileName);
    void writeXBeeAddrFile(const QString &fileName, QString outString);
    QByteArray readLogFile(const QString &fileName);
    void writeLogFile(const QString &fileName, QString outString);


#endif // FILEIO_H
