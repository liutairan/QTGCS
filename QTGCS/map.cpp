#include "map.h"

Map::Map(double lat, double lon, int level, int width, int height, QObject *parent) :
    QObject(parent)
{
    _width = width;
    _height = height;
    _zoomlevel = level;
    _homeLat = lat;
    _homeLon = lon;
    _centerLat = lat;
    _centerLon = lon;

    _maptype = "hybrid";
    _originLat =  30.4081580;
    _originLon = -91.1795330;

    _iterationX = 0;
    _iterationY = 0;

    _dX = 0;
    _dY = 0;

    currentWorkingPath = "";
    cfgFilePath = "";
    //_cachepath = "";
    //_cachepath = currentWorkingPath + "/mapscache/";
    //cacheFolder = currentWorkingPath + "/mapscache/";
    //cacheIndex = currentWorkingPath + "/mapcache.txt";

    initPath();
    _init_tile_index();
    loadImage();
}

QByteArray readJsonFile(const QString &filename)
{
    QFile f(filename);
    if (!f.open(QFile::ReadOnly | QFile::Text)) {
        f.close();
        return QString().toUtf8();
    } else {
        QTextStream in(&f);
        QByteArray retValue = in.readAll().toUtf8();
        f.close();
        return retValue;
    }
}

void Map::initPath()
{
    //QString currentWorkingPath = QDir::currentPath();
    //qDebug() << currentWorkingPath;
    //qDebug() << currentWorkingPath.length();
    //qDebug() << currentWorkingPath[currentWorkingPath.length()-1];
    //QString cachePath = currentWorkingPath + "/mapscache/";
    //_cachepath = cachePath;
    currentWorkingPath = QCoreApplication::applicationDirPath();
    qDebug() << currentWorkingPath;
    cfgFilePath = currentWorkingPath + "/config.json";
    qDebug() << cfgFilePath;
    QByteArray val;
    val = readJsonFile(cfgFilePath);
    QJsonDocument doc;
    doc = QJsonDocument::fromJson(val);
    QJsonObject obj = doc.object();
    mapKey = obj["Map Key"].toString();
    resourcePath = obj["Resource"].toString();

    cacheFolder = resourcePath + "/mapscache/";
    cacheIndex = resourcePath + "/mapcache.txt";
    if(QDir(cacheFolder).exists())
    {
        ;
    }
    else
    {
        QDir().mkdir(cacheFolder);
    }
}

void Map::move(int dx, int dy)
{
    GeoStep tempStep = _local_tile_step();
    double local_dx_to_lon = dx * tempStep._lonStep/(1.0*_TILESIZE);
    double local_dy_to_lat = dy * tempStep._latStep/(1.0*_TILESIZE);
    _centerLat = _centerLat + local_dy_to_lat;
    _centerLon = _centerLon - local_dx_to_lon;
    loadImage();
}

void Map::zoom(int dlevel)
{
    _zoomlevel = _zoomlevel + dlevel;
    if (_zoomlevel > 21)
    {
        _zoomlevel = 21;
    }
    else if (_zoomlevel < 9)
    {
        _zoomlevel = 9;
    }
    loadImage();
}

void Map::loadImage()
{
    QStringList filters;
    filters << "*.jpg";

    //QFileInfoList fileInfoList = QDir(_cachepath).entryInfoList(filters, QDir::Files|QDir::NoDotAndDotDot);
    QFileInfoList fileInfoList = QDir(cacheFolder).entryInfoList(filters, QDir::Files|QDir::NoDotAndDotDot);
    for (int i = 0; i < fileInfoList.size(); i++)
    {
        QFileInfo fileInfo = fileInfoList.at(i);
        //qDebug() << fileInfo.fileName();
    }

    try
    {
        StitchTileInfo tempTileInfo = localLoadImage();
        if (tempTileInfo.missingTiles.isEmpty())
        {
            //qDebug() << "Exist";
            tempLogMessage.id = "Map";
            tempLogMessage.message = "Tile exist.";
            emit logMessageRequest(tempLogMessage);
        }
        else if (tempTileInfo.missingTiles.length() > 0)
        {
            //qDebug() << "Not exist";
            tempLogMessage.id = "Map";
            tempLogMessage.message = "Tile not exist, downloading.";
            emit logMessageRequest(tempLogMessage);
            webLoadImage(tempTileInfo);
            localLoadImage();
        }
        else
        {
            qDebug() << "Error: Unknown state";
        }
    }
    catch (...)
    {}
}

StitchTileInfo Map::localLoadImage()
{
    StitchTileInfo tempTileInfo = _findLocalImage();
    if(tempTileInfo.missingTiles.length() > 0)
    {
        return tempTileInfo;
    }
    else if (tempTileInfo.localTiles.length() == 9)
    {
        ImageWithBorder tempImageAndBorder = _stitchImages(tempTileInfo);
        QPixmap tempImage = _cropImage(tempImageAndBorder);
        retImage = tempImage;
        return tempTileInfo;
    }
    return tempTileInfo;
}

void Map::webLoadImage(StitchTileInfo tileInfo)
{
    TileIndex tempTileIndex;
    foreach (tempTileIndex, tileInfo.missingTiles) {
        _downloadTile(tempTileIndex);
    }
}

void Map::_downloadTile(TileIndex tempTileIndex)
{
    int zoomlevel = tempTileIndex.zoomlevel;
    int x = tempTileIndex.x;
    int y = tempTileIndex.y;
    double lat = geoDictList.at(zoomlevel-9).latList.at(y);
    double lon = geoDictList.at(zoomlevel-9).lonList.at(x);
    double lat_rounded = _round_to(lat, 6);
    double lon_rounded = _round_to(lon, 6);
    QString strLat = QString::number(lat_rounded, 'f', 6);
    QString strLon = QString::number(lon_rounded, 'f', 6);
    QString strZoomLevel = QString::number(zoomlevel, 10);
    QString strX = QString::number(x, 10);
    QString strY = QString::number(y, 10);

    _grab_tile(lat_rounded, lon_rounded, zoomlevel, _maptype);
    QString outputStr = strLat+" "+strLon+" "+strZoomLevel+" "+_maptype+" "+strX+" "+strY + "\n";

    QFile file(cacheIndex);
    if (!file.open(QIODevice::Append | QIODevice::Text))
    {;}

    QTextStream out(&file);
    out << outputStr;
    file.close();
}

// Download map images from google static map with the api.
void Map::_grab_tile(double lat, double lon, int zoom, QString maptype)
{
    QString strLat = QString::number(lat, 'f', 6);
    QString strLon = QString::number(lon, 'f', 6);
    QString strZoomLevel = QString::number(zoom, 10);

    QString urlstr = QString("http://maps.googleapis.com/maps/api/staticmap?center=%1,%2&zoom=%3&maptype=hybrid&size=640x640&format=jpg&key="+mapKey).arg(strLat, strLon, strZoomLevel);

    QNetworkAccessManager manager;
    QNetworkReply *response = manager.get(QNetworkRequest(QUrl(urlstr)));
    QEventLoop event;
    QObject::connect(response,SIGNAL(finished()),&event,SLOT(quit()));
    event.exec();
    QByteArray tempBytes = response->readAll();
    QPixmap tempImage;
    tempImage.loadFromData(tempBytes);
    QString filepath = QString(cacheFolder + "%1_%2_%3_hybrid_640_640.jpg").arg(strLat, strLon, strZoomLevel);
    tempImage.save(filepath);
}

GeoStep Map::_tile_step(double lat, double lon, int zoomlevel)
{
    double _pixrad = _EARTHPIX / M_PI;
    double latitude = lat;
    double longitude = lon;
    double lonpix = _EARTHPIX + longitude * qDegreesToRadians(_pixrad);
    double sinlat = qSin(qDegreesToRadians(latitude));
    double latpix = _EARTHPIX - _pixrad * qLn((1 + sinlat)/(1 - sinlat)) /2.0;

    double _latStep = qFabs(_pix_to_lat(0,latpix,3,zoomlevel) - _pix_to_lat(2,latpix,3,zoomlevel))/2.0;
    double _lonStep = qFabs(_pix_to_lon(0,lonpix,3,zoomlevel) - _pix_to_lon(2,lonpix,3,zoomlevel))/2.0;

    GeoStep tempStep;
    tempStep._latStep = _latStep;
    tempStep._lonStep = _lonStep;
    return tempStep;
}

GeoStep Map::_local_tile_step()
{
    double latitude = _centerLat;
    double longitude = _centerLon;
    int zoomlevel = _zoomlevel;
    GeoStep tempStep = _tile_step(latitude, longitude, zoomlevel);
    return tempStep;
}

long Map::_pixels_to_degrees(long pixels, int zoom)
{
    if (zoom <= 21)
    {
        return pixels * (1 << (21 - zoom));
    }
    else if (zoom == 22)
    {
        return (pixels >> 1);
    }

}

double Map::_pix_to_lat(int k, double latpix, int ntiles, int zoom)
{
    double _pixrad = _EARTHPIX / M_PI;
    int _tile_index = k - int(ntiles/2);
    long temp_pixels1 = _pixels_to_degrees(_tile_index * _TILESIZE, zoom);

    double temp_pixels2 = latpix + temp_pixels1 - _EARTHPIX;
    double temp_degrees1 = temp_pixels2/_pixrad;

    return qRadiansToDegrees(M_PI/2.0 - 2.0*qAtan(qExp(temp_degrees1)));
    //return qRadiansToDegrees(M_PI/2.0 - 2.0*qAtan(qExp(((latpix + _pixels_to_degrees((k-int(ntiles/2))*_TILESIZE, zoom)) - _EARTHPIX) / _pixrad)));
}

double Map::_pix_to_lon(int j, double lonpix, int ntiles, int zoom)
{
    double _pixrad = _EARTHPIX / M_PI;
    int _tile_index = j - int(ntiles/2);
    long temp_pixels1 = _pixels_to_degrees(_tile_index * _TILESIZE, zoom);
    double temp_pixels2 = lonpix + temp_pixels1 - _EARTHPIX;
    double temp_degrees1 = temp_pixels2/_pixrad;
    return qRadiansToDegrees(temp_degrees1);
}

double Map::_round_to(double value, int digits)
{
    return double( int(value * qPow(10,digits))/(1.0*qPow(10,digits)) );
}

void Map::_init_tile_index()
{
    double _pixrad = _EARTHPIX / M_PI;
    for (int i = 0; i < 14; i++)
    {
        int tempLevel = i + 9;
        LevelDict tempLevelDict;
        tempLevelDict.level = i + 9;
        int iter_steps = (1 << i);

        QSet<double> tempLatSet;
        QSet<double> tempLonSet;
        double latitude = _centerLat;
        double longitude = _centerLon;
        tempLatSet << latitude;
        tempLonSet << longitude;
        for (int j = 0; j< iter_steps; j++)
        {
            double sinlat = qSin(qDegreesToRadians(latitude));
            double latpix = _EARTHPIX - _pixrad *qLn((1+sinlat)/(1-sinlat))/2.0;
            double newlat = _pix_to_lat(0, latpix, 3, int(tempLevel));
            latitude = newlat;
            tempLatSet << latitude;

            double lonpix = _EARTHPIX + longitude * qDegreesToRadians(_pixrad);
            double newlon = _pix_to_lon(0, lonpix, 3, int(tempLevel));
            longitude = newlon;
            tempLonSet << longitude;
        }
        latitude = _centerLat;
        longitude = _centerLon;
        for (int j = 0; j< iter_steps; j++)
        {
            double sinlat = qSin(qDegreesToRadians(latitude));
            double latpix = _EARTHPIX - _pixrad *qLn((1+sinlat)/(1-sinlat))/2.0;
            double newlat = _pix_to_lat(2, latpix, 3, int(tempLevel));
            latitude = newlat;
            tempLatSet << latitude;

            double lonpix = _EARTHPIX + longitude * qDegreesToRadians(_pixrad);
            double newlon = _pix_to_lon(2, lonpix, 3, int(tempLevel));
            longitude = newlon;
            tempLonSet << longitude;
        }
        QList<double> tempLatList = QList<double>::fromSet(tempLatSet);
        qSort(tempLatList);
        tempLevelDict.latList = tempLatList;
        QList<double> tempLonList = QList<double>::fromSet(tempLonSet);
        qSort(tempLonList);
        tempLevelDict.lonList = tempLonList;

        /*
        if (i == 2)
        {
            double temp;
            foreach (temp, tempLonList) {
                qDebug() << temp;

            }
            double temp2;
            foreach (temp2, tempLatList) {
                qDebug() << temp2;

            }
        }
        */
        //latlonDict << tempLevelDict;
        geoDictList << tempLevelDict;
    }
}

TileIndex Map::_pos_to_tile_index(double lat, double lon, int zoomlevel)
{
    int dictIndex = zoomlevel - 9;
    double temp = lat;
    int latIndex = 0;
    for (int i = 0; i<geoDictList.at(dictIndex).latList.length(); i++)
    {
        if(geoDictList.at(dictIndex).latList.at(i) >= temp)
        {
            //qDebug() << geoDictList.at(0).latList.at(i);
            if (i == 0)
            {
                latIndex = 0;
                break;
            }
            else if (qAbs(temp-geoDictList.at(dictIndex).latList.at(i)) <= qAbs(temp-geoDictList.at(dictIndex).latList.at(i-1)))
            {
                latIndex = i;
                break;
            }
            else if (qAbs(temp-geoDictList.at(dictIndex).latList.at(i)) >= qAbs(temp-geoDictList.at(dictIndex).latList.at(i-1)))
            {
                latIndex = i-1;
                break;
            }
            else if (i == geoDictList.at(dictIndex).latList.length()-1)
            {
                latIndex = geoDictList.at(dictIndex).latList.length()-1;
                break;
            }
        }
        else if (i == geoDictList.at(dictIndex).latList.length()-1)
        {
            latIndex = geoDictList.at(dictIndex).latList.length()-1;
            break;
        }
    }
    //qDebug() << latIndex;
    double temp2 = lon;
    int lonIndex = 0;
    for (int i = 0; i<geoDictList.at(dictIndex).lonList.length(); i++)
    {
        if(geoDictList.at(dictIndex).lonList.at(i) >= temp2)
        {
            //qDebug() << geoDictList.at(0).lonList.at(i);
            if (i == 0)
            {
                lonIndex = 0;
                break;
            }
            else if (qAbs(temp2-geoDictList.at(dictIndex).lonList.at(i)) <= qAbs(temp2-geoDictList.at(dictIndex).lonList.at(i-1)))
            {
                lonIndex = i;
                break;
            }
            else if (qAbs(temp2-geoDictList.at(dictIndex).lonList.at(i)) >= qAbs(temp2-geoDictList.at(dictIndex).lonList.at(i-1)))
            {
                lonIndex = i-1;
                break;
            }
            else if (i == geoDictList.at(dictIndex).lonList.length()-1)
            {
                lonIndex = geoDictList.at(dictIndex).lonList.length()-1;
                break;
            }
        }
        else if (i == geoDictList.at(dictIndex).lonList.length()-1)
        {
            lonIndex = geoDictList.at(dictIndex).lonList.length()-1;
            break;
        }
    }
    //qDebug() << lonIndex;
    TileIndex tempTileIndex;
    tempTileIndex.x = lonIndex;
    tempTileIndex.y = latIndex;
    tempTileIndex.zoomlevel = zoomlevel;
    //qDebug() << latIndex << lonIndex << qSetRealNumberPrecision( 10 )<<geoDictList.at(dictIndex).latList.at(latIndex) << geoDictList.at(dictIndex).lonList.at(lonIndex);
    return tempTileIndex;
}

StitchTileInfo Map::_findImages(QList<CacheAreaInfo> areaList)
{
    double lat = _centerLat;
    double lon = _centerLon;
    int zoom = _zoomlevel;

    TileIndex tempIndex = _pos_to_tile_index(lat,lon,zoom);
    int x_ind = tempIndex.x;
    int y_ind = tempIndex.y;
    //qDebug() << zoom<<x_ind<<y_ind;
    QList<TileIndex> waitingTiles;
    TileIndex temp1,temp2,temp3,temp4,temp5,temp6,temp7,temp8,temp9;
    temp1.x= x_ind-1; temp1.y=y_ind+1;temp1.zoomlevel = zoom;
    temp2.x= x_ind; temp2.y=y_ind+1;temp2.zoomlevel = zoom;
    temp3.x= x_ind+1; temp3.y=y_ind+1;temp3.zoomlevel = zoom;
    temp4.x= x_ind-1; temp4.y=y_ind;temp4.zoomlevel = zoom;
    temp5.x= x_ind; temp5.y=y_ind;temp5.zoomlevel = zoom;
    temp6.x= x_ind+1; temp6.y=y_ind;temp6.zoomlevel = zoom;
    temp7.x= x_ind-1; temp7.y=y_ind-1;temp7.zoomlevel = zoom;
    temp8.x= x_ind; temp8.y=y_ind-1;temp8.zoomlevel = zoom;
    temp9.x= x_ind+1; temp9.y=y_ind-1;temp9.zoomlevel = zoom;
    waitingTiles << temp1<<temp2<<temp3<<temp4<<temp5<<temp6<<temp7<<temp8<<temp9;
    StitchTileInfo retTileInfo;
    //qDebug() << areaList.length();
    if (areaList.length() == 0)
    {
        retTileInfo.missingTiles = waitingTiles;
    }
    else
    {
        for (int i=0; i<9; i++)
        {
            TileIndex tocheck = waitingTiles[i];
            //qDebug() << tocheck.zoomlevel << tocheck.x << tocheck.y;
            int res = 0;
            CacheAreaInfo tempCacheArea;
            foreach (tempCacheArea,areaList)
            {
                //qDebug() << tocheck.zoomlevel << tempCacheArea.zoomlevel;
                if ((tempCacheArea.zoomlevel == tocheck.zoomlevel) && (tempCacheArea.indexX == tocheck.x) && (tempCacheArea.indexY == tocheck.y))
                {
                    res = 1;
                    break;
                }
            }
            //qDebug() << res;
            if (res == 0)
            {
                retTileInfo.missingTiles << tocheck;
            }
            else
            {
                retTileInfo.localTiles << tocheck;
            }
        }
    }
    return retTileInfo;
}

StitchTileInfo Map::_findLocalImage()
{
    QList<CacheAreaInfo> localAreas;
    try
    {
        QFile file(cacheIndex);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            ;
        }
        QTextStream in(&file);
        QString line = in.readLine();
        while (!line.isNull())
        {
            CacheAreaInfo tempCacheAreaInfo;
            tempCacheAreaInfo.lat = line.split(" ").at(0).toDouble();
            tempCacheAreaInfo.lon = line.split(" ").at(1).toDouble();
            tempCacheAreaInfo.zoomlevel = line.split(" ").at(2).toInt();
            tempCacheAreaInfo.type = line.split(" ").at(3);
            tempCacheAreaInfo.indexX = line.split(" ").at(4).toInt();
            tempCacheAreaInfo.indexY = line.split(" ").at(5).toInt();
            localAreas.append(tempCacheAreaInfo);
            // read next line
            line = in.readLine();
        }
        file.close();
    }
    catch (...)
    {
        qDebug() << "No local image";
    }
    StitchTileInfo tempStitchTileInfo;
    tempStitchTileInfo = _findImages(localAreas);
    return tempStitchTileInfo;
}

ImageWithBorder Map::_stitchImages(StitchTileInfo tilesInfo)
{
    int ntiles = 3;
    int bigsize = ntiles * _TILESIZE;
    QPixmap bigimage(bigsize, bigsize);
    QPainter painter(&bigimage);
    double min_lat = _centerLat;
    double max_lat = _centerLat;
    double min_lon = _centerLon;
    double max_lon = _centerLon;
    for (int i =0; i<9; i++)
    {
        int row_num = int(i/3);
        int col_num = i%3;

        int x_ind = tilesInfo.localTiles.at(i).x;
        int y_ind = tilesInfo.localTiles.at(i).y;

        double lat = geoDictList.at(_zoomlevel-9).latList.at(y_ind);
        double lon = geoDictList.at(_zoomlevel-9).lonList.at(x_ind);
        double lat_rounded = _round_to(lat,6);
        double lon_rounded = _round_to(lon,6);
        //qDebug() << qSetRealNumberPrecision( 10 ) << x_ind << y_ind << lat << lon;
        if (lat_rounded > max_lat)
        {
            max_lat = lat_rounded;
        }
        if (lat_rounded < min_lat)
        {
            min_lat = lat_rounded;
        }
        if (lon_rounded > max_lon)
        {
            max_lon = lon_rounded;
        }
        if (lon_rounded < min_lon)
        {
            min_lon = lon_rounded;
        }
        QString strLat = QString::number(lat_rounded, 'f', 6);
        QString strLon = QString::number(lon_rounded, 'f', 6);
        QString strZoomLevel = QString::number(_zoomlevel, 10);
        QString strWidth = QString::number(_width, 10);
        QString strHeight = QString::number(_height, 10);
        QString tempName = strLat+"_"+strLon+"_"+strZoomLevel+"_"+_maptype+"_"+strWidth+"_"+strHeight+".jpg";

        QPixmap tile;
        tile.load(cacheFolder + tempName);
        painter.drawPixmap(col_num *_TILESIZE, row_num *_TILESIZE, tile);
    }
    ImageWithBorder retStruct;
    retStruct.image = bigimage;
    retStruct.min_lat = min_lat;
    retStruct.max_lat = max_lat;
    retStruct.min_lon = min_lon;
    retStruct.max_lon = max_lon;
    return retStruct;
}

QPixmap Map::_cropImage(ImageWithBorder imageAndBorder)
{
    double image_center_lat = (imageAndBorder.max_lat + imageAndBorder.min_lat)/2.0;
    double image_center_lon = (imageAndBorder.max_lon + imageAndBorder.min_lon)/2.0;
    double tile_length_lat = (imageAndBorder.max_lat - imageAndBorder.min_lat)/2.0;
    double tile_length_lon = (imageAndBorder.max_lon - imageAndBorder.min_lon)/2.0;
    int dx = int(640*(_centerLon - image_center_lon)/tile_length_lon);
    int dy = int(640*(_centerLat - image_center_lat)/tile_length_lat);
    int northwest_x = 640 + dx;
    int northwest_y = 640 - dy;
    QPixmap tempCropImage;
    tempCropImage = imageAndBorder.image.copy(northwest_x, northwest_y, 640, 640);
    //tempCropImage = imageAndBorder.image.copy(640,640,640,640);
    return tempCropImage;
}


void Map::_reload(AutoZoomGeoMapInfo mapInfo)
{
    _centerLat = mapInfo._center_lat;
    _centerLon = mapInfo._center_lon;
    _zoomlevel = mapInfo._zoomlevel;
    loadImage();
}

AutoZoomGeoMapInfo Map::_find_zoomlevel(double min_lat, double max_lat, double min_lon, double max_lon)
{
    double mid_lat = 0.5*(min_lat + max_lat);
    double mid_lon = 0.5*(min_lon + max_lon);
    double len_lat = max_lat - min_lat;
    double len_lon = max_lon - min_lon;

    QList<int> _zoom_level;
    _zoom_level <<21<<20<<19<<18<<17<<16<<15<<14<<13<<12<<11<<10<<9;
    QList<double> _lat_steps, _lon_steps;
    int level;
    foreach (level, _zoom_level)
    {
        GeoStep tempGeoStep = _tile_step(mid_lat, mid_lon, level);
        _lat_steps.append(tempGeoStep._latStep);
        _lon_steps.append(tempGeoStep._lonStep);
    }

    int lat_level_ind = 0;
    while (len_lat >= _lat_steps.at(lat_level_ind))
    {
        lat_level_ind = lat_level_ind + 1;
    }

    int lon_level_ind = 0;
    while (len_lon >= _lon_steps.at(lon_level_ind))
    {
        lon_level_ind = lon_level_ind + 1;
    }

    int ret_zoomlevel = qMin(_zoom_level.at(lat_level_ind), _zoom_level.at(lon_level_ind));

    AutoZoomGeoMapInfo tempMapInfo;
    tempMapInfo._center_lat = mid_lat;
    tempMapInfo._center_lon = mid_lon;
    tempMapInfo._zoomlevel = ret_zoomlevel;
    return tempMapInfo;
}


void Map::return_origin()
{
    _centerLat = _originLat;
    _centerLon = _originLon;
    loadImage();
}

// This function is used to translate GPS coordinates to
//    relative pixel point information on the local map
//    image.
//    Then the information can be used to plot GPS coordinates
//    on the map or plot missions loaded from files on the map.
PixelPosition Map::GPStoImagePos(GPSCoordinate co)
{
    GeoStep tempStep = _local_tile_step();
    double temp_point_x = ((co.lon-_centerLon)*(1.0*_TILESIZE)/tempStep._lonStep + 320);
    int point_x = int(temp_point_x);
    double temp_point_y = (320 - (co.lat-_centerLat)*(1.0*_TILESIZE)/tempStep._latStep);
    int point_y = int(temp_point_y);

    if ( (temp_point_x - int(temp_point_x)) >= 0.5)
    {
        point_x = int(temp_point_x + 1);
    }
    if ( (temp_point_y - int(temp_point_y)) >= 0.5)
    {
        point_y = int(temp_point_y + 1);
    }

    PixelPosition tempPo;
    tempPo.x = point_x;
    tempPo.y = point_y;
    return tempPo;
}

// This function is used to translate Pixel coordinates
//    on the map to real GPS coordinates.
//    For example, when click on the map, we get the
//    coordinate on the map (pixel), then from that we
//    can get the GPS coordinate which we really want.
//    Then this GPS coordinate can be used as mission
//    for Waypoints or other purposes.
GPSCoordinate Map::PostoGPS(PixelPosition po)
{
    GeoStep tempStep = _local_tile_step();
    double local_x_to_lon = (po.x-320)/(_TILESIZE*1.0) * tempStep._lonStep;
    double local_y_to_lat = (po.y-320)/(_TILESIZE*1.0) * tempStep._latStep;
    double point_lon = _centerLon + local_x_to_lon;
    double point_lat = _centerLat - local_y_to_lat;
    GPSCoordinate tempCo;
    tempCo.lat = point_lat;
    tempCo.lon = point_lon;
    return tempCo;
}

