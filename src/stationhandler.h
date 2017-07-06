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

    Q_INVOKABLE QString getStationName( QString shortStationCode );
    Q_INVOKABLE StationHandler* getStationPointer(void);
    Q_INVOKABLE QString getStationCount( void ) const;
    Q_INVOKABLE void forceRefresh( void );

private:

    // https://rata.digitraffic.fi/api/v1/metadata/stations
    struct Station {
        bool passengerTraffic; // false,
        QString type; // "STATION",
        QString stationName; // "Ahonpää",
        QString stationShortCode; // "AHO",
        QString stationUICCode; // 1343,
        QString countryCode;// "FI",
        double longitude; // = 25.01206612315972,
        double latitude; // = 64.55181651445501
    };

    bool classUpdating;
    const QUrl stationURL = QUrl::fromUserInput("https://rata.digitraffic.fi/api/v1/metadata/stations");
    const QString STATION_FILENAME = "stationlist.json";
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
