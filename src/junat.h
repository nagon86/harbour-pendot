#ifndef JUNAT_H
#define JUNAT_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QDateTime>
#include <QUrl>
#include <QNetworkAccessManager>

class Junat : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString trainNr READ getTrainNr WRITE setTrainNr NOTIFY refreshGui)
    Q_PROPERTY(QString getOperatorCode READ getOperatorCode NOTIFY refreshGui)
    Q_PROPERTY(QString getTrainType READ getTrainType NOTIFY refreshGui)
    Q_PROPERTY(QString getDepartureDate READ getDepartureDate NOTIFY refreshGui)
    //Q_PROPERTY(QStringList* getStationList READ getStationList NOTIFY refreshGui)


public:
    struct cause {
        QString categoryCodeId;
        QString categoryCode;
        QString detailedCategoryCodeId;
        QString detailedCategoryCode;
    };

    struct timeTableRow {
        bool trainStopping;
        QString stationShortCode;
        QString stationUICCode;
        QString countryCode; // FI or RU
        QString type; //ARRIVAL or DEPARTURE --> enum?
        bool commercialStop;
        QString commercialTrack;
        bool cancelled;
        QDateTime scheduledTime;
        QDateTime liveEstimateTime;
        QDateTime actualTime;
        int differenceInMinutes;
        cause causes;
        //---------------
        //: Constructor :
        //---------------
        timeTableRow() : trainStopping(false), commercialStop(false), cancelled(false), differenceInMinutes(0), causes(cause()) {
        }
    };

    explicit Junat(QObject *parent = 0);
    ~Junat();
    //QString get (QString url);
    Q_INVOKABLE Junat* getPointer(void);
    Q_INVOKABLE void refresData(void);
    QString getTrainNr() const;
    QString getOperatorCode() const;
    QString getTrainType() const;
    QString getDepartureDate() const;
    const timeTableRow* getTimeTableRow(int index) const;
    int getTimeTableCount(void ) const;
    void setTrainNr(QString nr);
    QString s_trainNr;

private:
    QString trainNr;
    QDateTime departureDate;
    QString operatorUICCode;
    QString operatorShortCode;
    QString trainType;
    QString trainCategory;
    QString commuterLineID;
    QString runningCurrently;
    QString cancelled;
    QString version;
    QDateTime timetableAcceptanceDate;
    //std::vector<timeTableRow> timeTableRows;
    QVector<timeTableRow> timeTableRows;

    QVector<timeTableRow*> filteredTimeTable;

    QString JSONData;
    QNetworkAccessManager* n_manager;

    QUrl currentUrl;

    // Functions
    void initData(void);
    void setDebugData(void);
    void buildUrl(void);
    void networkError(void);
    int parseData(int index = 0, QString prevParam = "");
    //bool storeData(QString p, QString v, QString pp = "");
    bool storeData(QString p, QString v, QString pp = "", timeTableRow* t = 0);
    //bool parseMetaData();
    void addMetaData(QString p, QString v);
    //bool parseTimeTableRows(int startIndex );
    void addTimetableParam(timeTableRow* tmp, const QString param, const QString value);
    //int parseCause(timeTableRow* t, int index);
    void addCauseCode(timeTableRow* t, const QString p, const QString v);
    QString isoDateWithMsToISODate(QString s);

    void filterTimeTable(QString type = "ARRIVAL");

signals:
    void refreshGui();
    void NumberChanged();
    void TimeTableChanged();
    void JSONChanged();

public slots:
    void refreshJunat();

private slots:
    //void getJSON(QUrl url);
    void getJSON();
    void parseJSON(QNetworkReply* nReply);
};

#endif // JUNAT_H
