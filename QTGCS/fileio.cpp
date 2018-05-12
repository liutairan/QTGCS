#include "fileio.h"


QString readFile(const QString &fileName)
{
    if (fileName.isEmpty())
    {
        return {};
    }
    QFileInfo info(fileName);
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //qDebug() << "Reading file" << info.absoluteFilePath();
        QTextStream in(&file);
        QString contents = in.readAll();
        if (file.error() != QFile::NoError || in.status() != QTextStream::Ok)
        {
            qDebug() << "Errors:" << in.status() << "/" << file.errorString();
        }
        return contents;
    }
    qDebug() << "Cannot open file" << info.absoluteFilePath();
    return {};
}

void writeFile(const QString &fileName, QString outString)
{
    QFile f(fileName);
    if (!f.open(QFile::WriteOnly | QFile::Text))
    {
        f.close();
    }
    else
    {
        QTextStream out(&f);
        out << outString;
        f.close();
    }
}

QByteArray readJsonFile(const QString &fileName)
{
    return readFile(fileName).toUtf8();
}

void writeJsonFile(const QString &fileName, QString outString)
{
    writeFile(fileName, outString);
}

QByteArray readMapIndexFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ;
    }
    QTextStream in(&file);
    return in.readAll().toUtf8();
//    QString line = in.readLine();
//    while (!line.isNull())
//    {
//        CacheAreaInfo tempCacheAreaInfo;
//        tempCacheAreaInfo.lat = line.split(" ").at(0).toDouble();
//        tempCacheAreaInfo.lon = line.split(" ").at(1).toDouble();
//        tempCacheAreaInfo.zoomlevel = line.split(" ").at(2).toInt();
//        tempCacheAreaInfo.type = line.split(" ").at(3);
//        tempCacheAreaInfo.indexX = line.split(" ").at(4).toInt();
//        tempCacheAreaInfo.indexY = line.split(" ").at(5).toInt();
//        localAreas.append(tempCacheAreaInfo);
//        // read next line
//        line = in.readLine();
//    }
//    file.close();
}

void writeMapIndexFile(const QString &fileName, QString outString)
{}

QStringList readXBeeAddrFile(const QString &fileName)
{
    QString inString = readFile(fileName);
    QStringList outStringList = inString.split("\n");
    outStringList.removeAll("");
    return outStringList;
}

void writeXBeeAddrFile(const QString &fileName, QString outString)
{}

QByteArray readLogFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ;
    }
    QTextStream in(&file);
    return in.readAll().toUtf8();
}

void writeLogFile(const QString &fileName, QString outString)
{}
