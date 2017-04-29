#include "stationhandler.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>

StationHandler::StationHandler(QObject *parent):
    QObject(parent)
{
    // Create class for network manager
    n_manager = new QNetworkAccessManager(this);

    // Connect network manager to parseJSON
    connect(n_manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(parseData(QNetworkReply*)));
    getData();
}

void StationHandler::getData( void ) {
    n_manager->get(QNetworkRequest(stationURL));
}

StationHandler::~StationHandler() {
    _stationList.clear();
    delete n_manager;
    n_manager = NULL;
}


void StationHandler::parseData(QNetworkReply* nReply) {

    // Read data to ByteArray
    QByteArray data = nReply->readAll();
#ifdef QT_QML_DEBUG
    qDebug() << "Reading Station URL data...";
#endif

    QString param;
    QString value;
    bool isValue = false;
    Station tmp;

    jsonStationsData = data;

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
