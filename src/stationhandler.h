#ifndef STATIONHANDLER_H
#define STATIONHANDLER_H

#include <QObject>
#include <QString>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QMultiMap>
#include <QMap>
#include <QFile>

class StationHandler : public QObject {
    Q_OBJECT
public:
    explicit StationHandler(QObject *parent = 0);
    ~StationHandler();

    // ** Getter functions for qml **
    // Gets station name that is matching with the short code
    // If value can't be found, return station short code
    Q_INVOKABLE QString getStationName( QString shortStationCode );

    // Gets station UIC code that is matching with the short code
    // If value can't be found, return station short code
    Q_INVOKABLE QString getStationUICCode( QString shortStationCode );

    // Gets station country that is matching with the short code
    // If value can't be found, return station short code
    Q_INVOKABLE QString getStationCountry( QString shortStationCode );

    // Gets station longitude that is matching with the short code
    // If value can't be found, return station short code
    Q_INVOKABLE QString getStationLongitude( QString shortStationCode );

    // Gets station latitude that is matching with the short code
    // If value can't be found, return station short code
    Q_INVOKABLE QString getStationLatitude( QString shortStationCode );

    // Function to help linking c++ classes together via qml
    Q_INVOKABLE StationHandler* getStationPointer(void);

    // Get station list length
    // Result converted to QString for ease of printing
    Q_INVOKABLE QString getStationCount( void ) const;

    // Force refresh that can be invoked from qml
    Q_INVOKABLE void forceRefresh( void );

private:

    // https://rata.digitraffic.fi/api/v1/metadata/stations
    struct Station {
        bool passengerTraffic;
        QString type;
        QString stationName;
        QString stationShortCode;
        QString stationUICCode;
        QString countryCode;
        double longitude;
        double latitude;
    };

    bool classUpdating;
    const QUrl stationURL = QUrl::fromUserInput("https://rata.digitraffic.fi/api/v1/metadata/stations");
    const QString STATION_FILENAME = "stationlist.json";
    const QString STORAGE_FOLDER = ".local/share/harbour-pendot";
    QString jsonStationsData;
    QMap<QString,Station> _stationList;
    QNetworkAccessManager* n_manager;

    // Functions
    void getData(bool forced = false);
    bool readFromFile(QFile* file);
    void parseData( void );
    void addStation(Station* s, QString p, QString v);

private slots:
    void readNetworkReply(QNetworkReply* nReply);
};

#endif // STATIONHANDLER_H
