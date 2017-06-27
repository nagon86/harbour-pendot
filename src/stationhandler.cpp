#include "stationhandler.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>

StationHandler::StationHandler(QObject *parent):
    QObject(parent)
{
    // Create class for network manager
    n_manager = new QNetworkAccessManager(this);

    // Connect network manager to parseJSON
    connect(n_manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(readNetworkReply(QNetworkReply*)));
    getData();
}

void StationHandler::getData( bool forced ) {
    QFile file(STATION_FILENAME);
    if ( !forced && readFromFile(&file) ) {
        qDebug() << "ReadCompleted";
    }
    else {
        n_manager->get(QNetworkRequest(stationURL));
    }
}

StationHandler::~StationHandler() {
    _stationList.clear();
    delete n_manager;
    n_manager = NULL;
}

bool StationHandler::readFromFile(QFile* file) {
    int bOpen = 0;
    if ( !file->open(QIODevice::ReadOnly /*| QIODevice::Text*/) ) {
        //Failed to open
#ifdef QT_QML_DEBUG
        qDebug() << "Failed to open file: " << file->fileName();
#endif
        return false;
    }
    else {
#ifdef QT_QML_DEBUG
        qDebug() << "Reading from file: " << file->fileName();
#endif
        jsonStationsData = file->readAll();
        file->close();
        if ( jsonStationsData.length() > 0 && jsonStationsData.at(0) == '[' ) {
            for ( int i = 0; i < jsonStationsData.length(); i++ ) {
                if ( jsonStationsData.at(i) == '[' || jsonStationsData.at(i) == '{' ) {
                    bOpen++;
                }
                else if ( jsonStationsData.at(i) == ']' || jsonStationsData.at(i) == '}') {
                    bOpen--;
                }
            }
            if ( bOpen != 0 ) {
                return false;
            }
        }
        else {
            return false;
        }
#ifdef QT_QML_DEBUG
        qDebug() << "Red: " << jsonStationsData.length() << " bytes.";
#endif
        parseData();
    }
    return true;
}

void StationHandler::forceRefresh( void ) {
    _stationList.clear();
    getData(true);
}

void StationHandler::readNetworkReply(QNetworkReply* nReply) {
    int bOpen = 0;
    // Read data to ByteArray
    QByteArray data = nReply->readAll();
    nReply->deleteLater();
#ifdef QT_QML_DEBUG
    qDebug() << "Reading Station URL data...";
#endif

    for ( int i = 0; i < data.length(); i++ ) {
        if ( data.at(i) == '[' || data.at(i) == '{' ) {
            bOpen++;
        }
        else if ( data.at(i) == ']' || data.at(i) == '}') {
            bOpen--;
        }
    }
#ifdef QT_QML_DEBUG
    qDebug() << "Brackets: " << bOpen << " Length: " << data.length();
#endif
    if ( bOpen == 0 ) {
        jsonStationsData = data;
        QFile file(STATION_FILENAME);
        if ( !file.open(QIODevice::WriteOnly | QIODevice::Truncate /*| QIODevice::Text*/) ) {
            return;
        }
        QTextStream in(&file);
        in << data;
        file.close();
        parseData();
    }
}


void StationHandler::parseData(void) {

    QString param;
    QString value;
    bool isValue = false;
    Station tmp;

#ifdef QT_QML_DEBUG
    qDebug() << "Starting Station Parser...";
#endif

    for ( int i = 0; i < jsonStationsData.length(); i++ ) {
        if ( jsonStationsData.at(i) == '[' || jsonStationsData.at(i) == ']' ||
             jsonStationsData.at(i) == '{' || jsonStationsData.at(i) == '"' ) {
            // Skip
        }
        else if ( jsonStationsData.at(i) == '}' ) {
            isValue = false;
            addStation(&tmp, param, value);
            _stationList.insert(tmp.stationShortCode, tmp);
            tmp = Station();
            param.clear();
            value.clear();
            // Add to map
        }
        else if ( jsonStationsData.at(i) == ':' ) {
            if ( !isValue ) {
                isValue = true;
            }
        }
        else if ( jsonStationsData.at(i) == ',' ) {
            isValue = false;
            addStation(&tmp, param, value);
            param.clear();
            value.clear();
        }
        else {
            if ( isValue ) {
                value.append(jsonStationsData.at(i));
            }
            else {
                param.append(jsonStationsData.at(i));
            }
        }
    }
#ifdef QT_QML_DEBUG
    qDebug() << "Map Count " << _stationList.count();
#endif
}

void StationHandler::addStation(Station* s, QString p, QString v) {
    if ( p.isEmpty() && v.isEmpty() ) {
        // Nothing to do
        return;
    }
    if (p == "passengerTraffic" ) {
        if ( v == "true" ) {
            s->passengerTraffic = true;
        }
        else {
            s->passengerTraffic = false;
        }
    }
    else if (p == "type" ) {
        s->type = v;
    }
    else if (p == "stationName" ) {
        s->stationName = v;
    }
    else if (p == "stationShortCode" ) {
        s->stationShortCode = v;
    }
    else if (p == "stationUICCode" ) {
        s->stationUICCode = v;
    }
    else if (p == "countryCode" ) {
        s->countryCode = v;
    }
    else if (p == "longitude" ) {
        s->longitude = v.toDouble();
    }
    else if (p == "latitude" ) {
        s->latitude = v.toDouble();
    }
    else {
#ifdef QT_QML_DEBUG
        qDebug() << "addStation: Unused param and value: " + p + " " + v;
#endif
    }
}

QString StationHandler::getStationName( QString shortStationCode ) {
    QMap<QString,Station>::const_iterator iter = _stationList.find(shortStationCode);
    if ( iter == _stationList.end() ) {
        return shortStationCode;
    }
    return iter->stationName;
}

QString StationHandler::getStationCount( void ) const {
    return static_cast<QString>(_stationList.count());
}

StationHandler* StationHandler::getStationPointer(void) {
    return this;
}
