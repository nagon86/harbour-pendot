#include "stationhandler.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTextCodec>
#include <QFile>

// Default constructor
StationHandler::StationHandler(QObject *parent):
    QObject(parent)
{
    // Create class for network manager
    n_manager = new QNetworkAccessManager(this);

    classUpdating = false;

    // Connect network manager to parseJSON
    connect(n_manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(readNetworkReply(QNetworkReply*)));
}

// Try to read sation information from local file.
// If it fails request from network.
void StationHandler::getData( bool forced ) {
    if ( classUpdating ) {
        qWarning() << "StationHandler still updating data";
        return;
    }
    classUpdating = true;
    QFile file(STORAGE_FOLDER + "/" + STATION_FILENAME);
    if ( !forced && readFromFile(&file) ) {
        classUpdating = false;
    }
    else {
        n_manager->get(QNetworkRequest(stationURL));
    }
}

// Destructor for class
StationHandler::~StationHandler() {
    _stationList.clear();
    delete n_manager;
    n_manager = NULL;
}

// Attempts to read data from parameter file
// If successful returns true
bool StationHandler::readFromFile(QFile* file) {
    int bOpen = 0;
    if ( !file->open(QIODevice::ReadOnly | QIODevice::Text) ) {
        //Failed to open
#ifdef QT_QML_DEBUG
        qDebug() << "Failed to open file: " << file->fileName();
#endif
        return false;
    }
    else {
#ifdef QT_QML_DEBUG
        qDebug() << "Opened file: " << file->fileName();
#endif
        jsonStationsData = file->readAll();
        file->close();
        // Count brackets to verify that all data is read
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
        parseData();
    }
    return true;
}

// force reading from the network
void StationHandler::forceRefresh( void ) {
    _stationList.clear();
    getData(true);
}

// Read data from network reply
void StationHandler::readNetworkReply(QNetworkReply* nReply) {
    int bOpen = 0;
    QString data = nReply->readAll();
    nReply->deleteLater();

    // Count brackets to ferify that all data received
    for ( int i = 0; i < data.length(); i++ ) {
        if ( data.at(i) == '[' || data.at(i) == '{' ) {
            bOpen++;
        }
        else if ( data.at(i) == ']' || data.at(i) == '}') {
            bOpen--;
        }
    }

    if ( bOpen == 0 ) {
        jsonStationsData = data;
        // Write data to a file
        QFile file(STORAGE_FOLDER + "/" + STATION_FILENAME);
        if ( !file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text) ) {
            return;
        }
        QTextStream in(&file);
        in.setCodec(QTextCodec::codecForName("UTF-8"));;
        in << data;
        file.flush();
        file.close();
        parseData();
        classUpdating = false;
    }
}

// Function to crunch down station information
void StationHandler::parseData(void) {

    QString param;
    QString value;
    bool isValue = false;
    Station tmp;

    // Loop through entire data set
    for ( int i = 0; i < jsonStationsData.length(); i++ ) {
        if ( jsonStationsData.at(i) == '[' || jsonStationsData.at(i) == ']' ||
             jsonStationsData.at(i) == '{' || jsonStationsData.at(i) == '"' ) {
            // Skip
        }

        // End of JSON "element"
        // Add data to station list
        else if ( jsonStationsData.at(i) == '}' ) {
            isValue = false;
            addStation(&tmp, param, value);
            _stationList.insert(tmp.stationShortCode, tmp);
            tmp = Station();
            param.clear();
            value.clear();
        }

        // Delimiter between parameter name and value
        // aka end of parameter name
        else if ( jsonStationsData.at(i) == ':' ) {
            if ( !isValue ) {
                isValue = true;
            }
        }

        // End of value
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
}

// Helper function to add parameter values to correct spot in the struct
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

// Gets station name that is matching with the short code
// If value can't be found, return station short code
QString StationHandler::getStationName( QString shortStationCode ) {
    QMap<QString,Station>::const_iterator iter = _stationList.find(shortStationCode);
    if ( iter == _stationList.end() ) {
        return shortStationCode;
    }
    return iter->stationName;
}

// Gets station UIC code that is matching with the short code
// If value can't be found, return station short code
QString StationHandler::getStationUICCode( QString shortStationCode ) {
    QMap<QString,Station>::const_iterator iter = _stationList.find(shortStationCode);
    if ( iter == _stationList.end() ) {
        return shortStationCode;
    }
    return iter->stationUICCode;
}

// Gets station country that is matching with the short code
// If value can't be found, return station short code
QString StationHandler::getStationCountry( QString shortStationCode ) {
    QMap<QString,Station>::const_iterator iter = _stationList.find(shortStationCode);
    if ( iter == _stationList.end() ) {
        return shortStationCode;
    }
    return iter->countryCode;
}

// Gets station longitude that is matching with the short code
// If value can't be found, return station short code
QString StationHandler::getStationLongitude( QString shortStationCode ) {
    QMap<QString,Station>::const_iterator iter = _stationList.find(shortStationCode);
    if ( iter == _stationList.end() ) {
        return shortStationCode;
    }
    return QString::number(iter->longitude);
}

// Gets station latitude that is matching with the short code
// If value can't be found, return station short code
QString StationHandler::getStationLatitude( QString shortStationCode ) {
    QMap<QString,Station>::const_iterator iter = _stationList.find(shortStationCode);
    if ( iter == _stationList.end() ) {
        return shortStationCode;
    }
    return QString::number(iter->latitude);
}

// Get station list length
// Result converted to QString for ease of printing
QString StationHandler::getStationCount( void ) const {
    return static_cast<QString>(_stationList.count());
}

// Function to help linking c++ classes together via qml
StationHandler* StationHandler::getStationPointer(void) {
    if ( _stationList.count() <= 0 ) {
        getData();
    }
    return this;
}
