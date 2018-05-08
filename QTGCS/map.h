#ifndef MAP_H
#define MAP_H

#include <QObject>
#include <QString>
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <QDir>
#include <QtMath>
#include <QSet>
#include <QUrl>
#include <QUrlQuery>
#include <QtNetwork>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSsl>
#include <QSslConfiguration>
#include <QSslCertificate>
#include <QByteArray>

#include "logmessage.h"

#define _EARTHPIX  268435456
// Number of pixels in half the earth's circumference at zoom = 21
#define _DEGREE_PRECISION = 6
// Number of decimal places for rounding coordinates
#define _TILESIZE  640
// Larget tile we can grab without paying
#define _GRABRATE  10
// Fastest rate at which we can download tiles without paying

//#define _pixrad  _EARTHPIX / M_PI

struct PixelPosition
{
    int x;
    int y;
};

struct GPSCoordinate
{
    double lat;
    double lon;
};

struct TileIndex
{
    int x;
    int y;
    int zoomlevel;
};

struct StitchTileInfo
{
    QList<TileIndex> localTiles;
    QList<TileIndex> missingTiles;
};

struct CacheAreaInfo
{
    double lat;
    double lon;
    int zoomlevel;
    QString type;
    int indexX;
    int indexY;
};

struct GeoStep
{
    double _latStep;
    double _lonStep;
};

struct AutoZoomGeoMapInfo
{
    double _center_lat;
    double _center_lon;
    int _zoomlevel;
};

struct LevelDict
{
    int level;
    QList<double> latList;
    QList<double> lonList;
};

struct GeoDict
{
    QList<LevelDict> geoDictList;
};

struct ImageWithBorder
{
    QPixmap image;
    double max_lat;
    double min_lat;
    double max_lon;
    double min_lon;
};

class Map : public QObject
{
    Q_OBJECT
    int _width;
    int _height;

    QString _maptype;
    double _originLat;
    double _originLon;
    double _homeLat;
    double _homeLon;
    double _centerLat;
    double _centerLon;
public:
    Map(double, double, int, int, int, QObject *parent = 0);

    int _zoomlevel;

    int _iterationX;
    int _iterationY;

    int _dX;
    int _dY;

    QString currentWorkingPath;
    QString cfgFilePath;
    //QString _cachepath;
    QString resourcePath;
    QString cacheFolder;
    QString cacheIndex;
    QString mapKey;
    QPixmap retImage;

    //GeoDict latlonDict;
    QList<LevelDict> geoDictList;

    void initPath();

    void initLoad();
    void move(int,int);
    void zoom(int);
    void _reload(AutoZoomGeoMapInfo);
    void return_origin();
    PixelPosition GPStoImagePos(GPSCoordinate);
    GPSCoordinate PostoGPS(PixelPosition);

    void loadImage();
    void _init_tile_index();
    TileIndex _pos_to_tile_index(double, double, int);

    long _pixels_to_degrees(long,int);
    double _pix_to_lat(int, double, int, int);
    double _pix_to_lon(int, double, int, int);
    double _round_to(double,int);

    GeoStep _local_tile_step();
    GeoStep _tile_step(double,double,int);

    AutoZoomGeoMapInfo _find_zoomlevel(double,double,double,double);


    void _grab_tile(double, double, int, QString);

    StitchTileInfo localLoadImage();
    StitchTileInfo _findLocalImage();
    StitchTileInfo _findImages(QList<CacheAreaInfo>);
    ImageWithBorder _stitchImages(StitchTileInfo);
    QPixmap _cropImage(ImageWithBorder);
    void webLoadImage(StitchTileInfo);
    void _downloadTile(TileIndex);

    void _getBigImage();
    void _new_image();
    void _groupLists();
    QPixmap _stitchMaps();
    AutoZoomGeoMapInfo _requiredMap();

    //LogMessage tempLogMessage;
signals:
    void logMessageRequest(LogMessage);
};

#endif // MAP_H
