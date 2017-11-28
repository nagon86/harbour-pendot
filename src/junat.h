#ifndef JUNAT_H
#define JUNAT_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QDateTime>
#include <QTimer>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class Junat : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString trainNr READ getTrainNr WRITE setTrainNr NOTIFY refreshGui)
    Q_PROPERTY(QString getOperatorCode READ getOperatorCode NOTIFY refreshGui)
    Q_PROPERTY(QString getTrainType READ getTrainType NOTIFY refreshGui)
    Q_PROPERTY(QString getDepartureDate READ getDepartureDate NOTIFY refreshGui)
    Q_PROPERTY(QString getTrainReadySource READ getTrainReadySource NOTIFY refreshGui)
    Q_PROPERTY(QString getTrainReadyAccepted READ getTrainReadyAccepted NOTIFY refreshGui)
    Q_PROPERTY(QString getTrainReadyTime READ getTrainReadyTime NOTIFY refreshGui)
    Q_PROPERTY(QString getLastRefreshTime READ getLastRefreshTime NOTIFY refreshGui)
    Q_PROPERTY(int getCurrentStation READ getCurrentStation NOTIFY refreshGui)

public:
    struct cause {
        bool hasCause;
        QString categoryCodeId;
        QString categoryCode;
        QString detailedCategoryCodeId;
        QString detailedCategoryCode;
        QString thirdCategoryCode;
        QString thirdCategoryCodeId;

        //---------------
        //: Constructor :
        //---------------
        cause() :  hasCause(false), categoryCodeId(""), categoryCode(""), detailedCategoryCodeId(""), detailedCategoryCode(""), thirdCategoryCode(""), thirdCategoryCodeId("") {
        }
    };

    struct timeTableRow {
        bool trainStopping;
        QString stationShortCode;
        QString stationUICCode;
        QString countryCode; // FI or RU
        QString type;
        bool commercialStop;
        QString commercialTrack;
        bool cancelled;
        QDateTime scheduledTime;
        QDateTime liveEstimateTime;
        QString estimateSource;
        QDateTime actualTime;
        int differenceInMinutes;
        int absoluteIndex;
        cause causes;
        //---------------
        //: Constructor :
        //---------------
        timeTableRow() : trainStopping(false), commercialStop(false), cancelled(false), differenceInMinutes(0), causes(cause()) {
        }
    };

    struct trainReadying {
        QString source;
        bool accepted;
        QDateTime timeStamp;
        //---------------
        //: Constructor :
        //---------------
        trainReadying() : source("NA"), accepted(false) {}
    };

    explicit Junat(QObject *parent = 0);
    ~Junat();
    Q_INVOKABLE Junat* getPointer(void);
    Q_INVOKABLE void refresData(void);
    Q_INVOKABLE QString getErrSummary(void);
    Q_INVOKABLE QString getErrBody(void);
    Q_INVOKABLE QString getErrPrevSummary(void);
    Q_INVOKABLE QString getErrPrevBody(void);

    Q_INVOKABLE QString getStationName(int index) const;
    Q_INVOKABLE int getStationMinutes(int index) const;
    Q_INVOKABLE bool stopHasCause(int index) const;
    Q_INVOKABLE QString getStopCauseCode(int index) const;
    Q_INVOKABLE QString getStopDetailedCauseCode(int index) const;
    Q_INVOKABLE QString getStopThirdCauseCode(int index) const;

    QString getTrainNr() const;
    QString getOperatorCode() const;
    QString getTrainType() const;
    QString getDepartureDate() const;
    QString getTrainReadySource() const;
    QString getTrainReadyAccepted() const;
    QString getTrainReadyTime() const;
    QString getLastRefreshTime() const;
    int getCurrentStation() const;
    const timeTableRow* getTimeTableRow(int index) const;
    int getTimeTableCount(void ) const;
    void setTrainNr(QString nr);
    QString s_trainNr;

private:
    struct NetError {
        QString summary;
        QString body;
        QString previewSummary;
        QString previewBody;
        //---------------
        //: Constructor :
        //---------------
        NetError() : summary("Empty Summary"), body("Empty Body"),
            previewSummary("Empty Preview Summary"), previewBody("Empty Preview Body") {}
    };

    QString trainNr;
    QDateTime departureDate;
    QString operatorUICCode;
    QString operatorShortCode;
    QString trainType;
    QString timeTableType;
    QString trainCategory;
    QString commuterLineID;
    QString runningCurrently;
    QString cancelled;
    QString version;
    QDateTime timetableAcceptanceDate;
    QDateTime lastRefreshTime;
    trainReadying trainReady;
    QVector<timeTableRow> timeTableRows;

    QVector<timeTableRow*> filteredTimeTable;

    QString JSONData;
    QNetworkAccessManager* n_manager;
    QNetworkRequest n_request;
    QNetworkReply* n_reply;
    NetError n_error;

    QUrl currentUrl;

    QTimer* retryTimer;
    int retryCount;
    const int MAX_RETRY_COUNT = 3;

    // Functions
    void initData(void);
    void setDebugData(void);
    void buildUrl(void);
    void networkError(void);
    void startRetryTimer(void);
    int parseData();
    // cause codes are usually on DEPARTURE. Copy Causes from Departure to Arrival.
    void fixCauseCodes(void);
    // some stations never receives actual time. Workaround for this issue.
    void fixActualTimes(void);
    bool storeData(QString p, QString v, QString pp = "", timeTableRow* t = NULL);
    void addMetaData(QString p, QString v);
    void addTimetableParam(timeTableRow* tmp, const QString param, const QString value);
    void addCauseCode(timeTableRow* t, const QString p, const QString v);
    QString isoDateWithMsToISODate(QString s);

    void filterTimeTable(QString type = "ARRIVAL");

signals:
    void refreshGui();
    void NumberChanged();
    void TimeTableChanged();
    void JSONChanged();
    void networkErrorNotification();

public slots:
    void refreshJunat();

private slots:
    void getJSON();
    void netError( QNetworkReply::NetworkError nErr );
    void retryGetUrl();
    void parseJSON();
};

#endif // JUNAT_H
