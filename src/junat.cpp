#include "junat.h"
#include <QString>
#include <QFile>
#include <QDateTime>
#include <QNetworkAccessManager>
#include <QNetworkReply>

//using namespace std;

Junat::Junat(QObject *parent) :
    QObject(parent),
    trainNr("0"),
    //departureDate(""),
    operatorUICCode(""),
    operatorShortCode("NULL"),
    trainType("NULL"),
    timeTableType(""),
    trainCategory(""),
    commuterLineID(""),
    runningCurrently("false"),
    cancelled("false"),
    version("0"),
    n_reply(NULL)
{

    // Create class for network manager
    n_manager = new QNetworkAccessManager(this);
    n_request = QNetworkRequest();

    timetableAcceptanceDate = QDateTime::currentDateTime();
    lastRefreshTime = QDateTime::currentDateTime();
    departureDate = QDateTime::currentDateTime();
    s_trainNr = "";

    // Connect network manager to parseJSON
    //connect(n_manager, SIGNAL(finished(QNetworkReply*)),
    //        this, SLOT(parseJSON(QNetworkReply*)));
}

Junat::~Junat() {
    timeTableRows.clear();
    delete n_manager;
    n_manager = NULL;
}

void Junat::initData(void) {
    trainNr = "";
    departureDate = QDateTime();
    operatorUICCode = "";
    operatorShortCode = "";
    trainType = "";
    timeTableType = "";
    trainCategory = "";
    commuterLineID = "";
    runningCurrently = "";
    cancelled = "";
    version = "";
    timetableAcceptanceDate = QDateTime();
    timeTableRows.clear();
    filteredTimeTable.clear();
    n_error = NetError();
}

void Junat::getJSON() {
    if ( n_reply == NULL ) {
#ifdef QT_QML_DEBUG
        qDebug() << "Initiating network request: " << QDateTime::currentDateTime().toString("HH:mm:ss");
#endif
        n_request.setUrl(currentUrl);
        n_reply = n_manager->get(n_request);
        connect( n_reply, SIGNAL(readyRead()), this, SLOT(parseJSON()) );
        connect( n_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(netError(QNetworkReply::NetworkError)) );
    }
}

void Junat::refresData(void) {
#ifdef QT_QML_DEBUG
    qDebug() << "Refreshing JSON";
#endif
    getJSON();
}

void Junat::netError( QNetworkReply::NetworkError nErr ) {
#ifdef QT_QML_DEBUG
    qDebug() << QDateTime::currentDateTime().toString("HH:mm:ss") << " ...Network Error Handler... ";
#endif
    qDebug() << "Network error: " << nErr;
    n_error.body = nErr;
    n_error.summary = "Network Error";
    n_error.previewBody = nErr;
    n_error.previewSummary = "Network Error";
    emit networkErrorNotification();
}

//void Junat::parseJSON(QNetworkReply* nReply) {
void Junat::parseJSON() {

    if ( n_reply->error() != QNetworkReply::NoError ) {
#ifdef QT_QML_DEBUG
        qDebug() << "Network error, waiting for next refresh cycle";
#endif
        return;
    }

    // Read data to ByteArray
    QByteArray data = n_reply->readAll();
    n_reply->deleteLater();
    disconnect(n_reply, SIGNAL(readyRead()));
    disconnect(n_reply, SIGNAL(error(QNetworkReply::NetworkError)));
    n_reply = NULL;

    QString tmp(data);
    int bCount = 0;
#ifdef QT_QML_DEBUG
    qDebug() << "Reading URL data...";
#endif

    for ( int i = 0; i < tmp.length(); i++ ) {
        if (tmp.at(i) == '{' || tmp.at(i) == '[') {
            bCount++;
        }
        else if (tmp.at(i) == '}' || tmp.at(i) == ']') {
            bCount--;
        }
    }

    if ( bCount != 0 ) {
#ifdef QT_QML_DEBUG
        qDebug() << "Invalid data: Bracket miss match. " << tmp.length();
        n_error.body = "Waitin for next Refresh cycle";
        n_error.summary = "Received invalid data";
        n_error.previewBody = "Waitin for next Refresh cycle";
        n_error.previewSummary = "Received invalid data";
        emit networkErrorNotification();
#endif
        return;
    }
    else {
#ifdef QT_QML_DEBUG
        qDebug() << "Data length: " << tmp.length();
#endif
    }
    if ( tmp == JSONData ) {
        // Data has not changed
    }
    else {
        JSONData = tmp;

        if (JSONData.length() > 2) {
            // Valid Response
#ifdef QT_QML_DEBUG
            qDebug() << "Clearing data structures.";
#endif
            initData();
#ifdef QT_QML_DEBUG
            qDebug() << "Starting parser.";
#endif
            if (parseData()) {

            }
        }
        // Do stuff
    }
    fixCauseCodes();
    filterTimeTable();
    emit TimeTableChanged();
#ifdef QT_QML_DEBUG
    qDebug() << "emit refreshGui " << QDateTime::currentDateTime().toString("HH:mm:ss");
#endif
    lastRefreshTime = QDateTime::currentDateTime();
    emit refreshGui();
}

int Junat::parseData() {

    int index = 0;
    int bOpen = 0;
    int timeTableLevel = 0;
    bool isValue = false;
    QString param;
    QString value;
    QVector<QString> paramStack;
    timeTableRow tmp;

    do {
        if ( JSONData.at(index) == '[' ) {
            paramStack.push_back(param);
            bOpen++;
            if ( param == "timeTableRows" ) {
                timeTableLevel = bOpen;
            }
            param.clear();
        }
        else if ( JSONData.at(index) == ']') {
            bOpen--;
            if ( paramStack.at(paramStack.length()-1) =="timeTableRows") {
                timeTableRows.append(tmp);
                tmp = timeTableRow();
            }
            paramStack.pop_back();
        }
        else if ( JSONData.at(index) == '{' ) {
            if ( param.size() != 0 ) {
                paramStack.push_back(param);
            }
            else {
                paramStack.push_back(paramStack.at(paramStack.length()-1));
            }
            isValue = false;
            param.clear();
            bOpen++;
        }
        else if ( JSONData.at(index) == '}' ) {
            storeData(param, value, paramStack.at(paramStack.length()-1), &tmp);
            isValue = false;
            paramStack.pop_back();
            bOpen--;
            param.clear();
            value.clear();
        }
        else if ( !isValue && JSONData.at(index) == ':' ) {
            isValue = true;
        }
        else if ( JSONData.at(index) == ',' ) {
            storeData(param, value, paramStack.at(paramStack.length()-1), &tmp);
            if ( paramStack.at(paramStack.length()-1) == "timeTableRows" && bOpen == timeTableLevel ) {
                if ( tmp.causes.hasCause ) {
                    qDebug() << "HAS CAUSE!";
                }
                timeTableRows.append(tmp);
                tmp = timeTableRow();
            }
            isValue = false;
            param.clear();
            value.clear();
        }
        else if ( JSONData.at(index) == '"' ) {
            //Skip
        }
        else {
            if ( isValue ) {
                value.append(JSONData.at(index));
            }
            else {
                param.append(JSONData.at(index));
            }
        }
    index++;
    } while (bOpen > 0);
    return index;
}

bool Junat::storeData(QString p, QString v, QString pp, timeTableRow* t) {
    if ( !pp.isEmpty() ) {
        if ( pp == "timeTableRows") {
            addTimetableParam(t, p, v);
        }
        else if ( pp == "causes") {
            addCauseCode(t, p, v);
        }
        else if ( pp == "trainReady") {
            addMetaData(p, v);
        }
    }
    else {
        addMetaData(p, v);
    }

    return true;
}

void Junat::addMetaData(QString p, QString v) {
    if (p.isEmpty() && v.isEmpty()) {
        return;
    }
    if ( p == "trainNumber" ) {
        trainNr = v;
    }
    else if ( p == "departureDate" ) {
        departureDate = QDateTime::fromString(v, Qt::ISODate);
    }
    else if ( p == "operatorUICCode" ) {
        operatorUICCode = v;
    }
    else if ( p == "operatorShortCode" ) {
        operatorShortCode = v;
    }
    else if ( p == "trainType" ) {
        trainType = v;
    }
    else if ( p == "timeTableType" ) {
        timeTableType = v;
    }
    else if ( p == "trainCategory" ) {
        trainCategory = v;
    }
    else if ( p == "commuterLineID" ) {
        commuterLineID = v;
    }
    else if ( p == "runningCurrently" ) {
        runningCurrently = v;
    }
    else if ( p == "cancelled" ) {
        cancelled = v;
    }
    else if ( p == "version" ) {
        version = v;
    }
    else if ( p == "timetableAcceptanceDate") {
        timetableAcceptanceDate = QDateTime::fromString(v, Qt::ISODate);
    }
    else if ( p == "source") {
        trainReady.source = v;
    }
    else if ( p == "accepted") {
        if ( v == "true" ) {
            trainReady.accepted = true;
        }
        else {
            trainReady.accepted = false;
        }
    }
    else if ( p == "timestamp") {
        trainReady.timeStamp = QDateTime::fromString(v, Qt::ISODate);
    }
    else if ( p == "timetableType" ) {
        timeTableType = v;
    }
    else {
#ifdef QT_QML_DEBUG
        if (!p.isEmpty() || !v.isEmpty()) {
            qDebug() << "Unused param and value: " + p + ", " + v;
        }
#endif
    }
}

void Junat::addTimetableParam(timeTableRow* tmp, const QString param, const QString value) {
        if (param.isEmpty() && value.isEmpty()) {
            return;
        }

        if (param == "trainStopping") {
            if ( value.toLower() == "true") {
                tmp->trainStopping = true;
            }
            else {
                tmp->trainStopping = false;
            }
        }
        else if ( param == "stationShortCode" ) {
            tmp->stationShortCode = value;
        }
        else if ( param == "stationUICCode" ) {
            tmp->stationUICCode = value;
        }
        else if ( param == "countryCode" ) {
            tmp->countryCode = value;
        }
        else if ( param == "type" ) {
            tmp->type = value;
        }
        else if ( param == "commercialStop" ) {
            if ( value.toLower() == "true") {
                tmp->commercialStop = true;
            }
            else {
                tmp->commercialStop = false;
            }
        }
        else if ( param == "commercialTrack" ) {
            tmp->commercialTrack = value;
        }
        else if ( param == "cancelled" ) {
            if ( value.toLower() == "true") {
                tmp->cancelled = true;
            }
            else {
                tmp->cancelled = false;
            }
        }
        else if ( param == "scheduledTime" ) {
            tmp->scheduledTime = QDateTime::fromString(value, Qt::ISODate);
        }
        else if ( param == "liveEstimateTime" ) {
            tmp->liveEstimateTime = QDateTime::fromString(value, Qt::ISODate);
        }
        else if ( param == "actualTime" ) {
            tmp->actualTime = QDateTime::fromString(value, Qt::ISODate);;
        }
        else if ( param == "differenceInMinutes" ) {
            tmp->differenceInMinutes = value.toInt();
        }
        else if ( param == "estimateSource" ) {
            tmp->estimateSource = value;
        }
        else {
#ifdef QT_QML_DEBUG
            qDebug() << "addTimetableParam: Unused param and value: " + param + " " + value;
#endif
        }
}

void Junat::addCauseCode(timeTableRow* t, const QString p, const QString v) {
    if (p.isEmpty() && v.isEmpty()) {
        return;
    }

    if ( p == "categoryCodeId" ) {
        t->causes.categoryCodeId = v;
        t->causes.hasCause = true;
    }
    else if ( p == "categoryCode" ) {
        t->causes.categoryCode = v;
        t->causes.hasCause = true;
    }
    else if ( p == "detailedCategoryCodeId" ) {
        t->causes.detailedCategoryCodeId = v;
        t->causes.hasCause = true;
    }
    else if ( p == "detailedCategoryCode" ) {
        t->causes.detailedCategoryCode = v;
        t->causes.hasCause = true;
    }
    else if ( p == "thirdCategoryCode" ) {
        t->causes.thirdCategoryCode = v;
        t->causes.hasCause = true;
    }
    else if ( p == "thirdCategoryCodeId" ) {
        t->causes.thirdCategoryCodeId = v;
        t->causes.hasCause = true;
    }
    else {
#ifdef QT_QML_DEBUG
        qDebug() << "addCauseCode: Unused param and value: " + p + " " + v;
#endif
    }
}

void Junat::fixCauseCodes(void) {
    if ( timeTableRows.length() <= 2 ) {
        return;
    }
    for ( int i = 1; i < timeTableRows.length(); i++ ) {
        if ( timeTableRows.at(i).stationShortCode == timeTableRows.at(i-1).stationShortCode ) {
            if ( timeTableRows.at(i).causes.hasCause && !timeTableRows.at(i-1).causes.hasCause ) {
                timeTableRows[i-1].causes = timeTableRows[i].causes;
            }
            else if ( !timeTableRows.at(i).causes.hasCause && timeTableRows.at(i-1).causes.hasCause ) {
                timeTableRows[i].causes = timeTableRows[i-1].causes;
            }
            else {
                // Nothing to do
            }
        }
    }
}

void Junat::refreshJunat() {
#ifdef QT_QML_DEBUG
    qDebug() << "Refreshing JSON";
#endif
    getJSON();
}

void Junat::buildUrl(void) {
    if (s_trainNr.toInt()) {
        currentUrl = QUrl(QString("https://rata.digitraffic.fi/api/v1/live-trains/") + s_trainNr);
    }
#ifdef QT_QML_DEBUG
    qDebug() << currentUrl;
#endif
    getJSON();
}

// -----------
// | Getters |
// -----------

QString Junat::getTrainNr() const {
    return s_trainNr;
}

QString Junat::getOperatorCode() const {
    return operatorShortCode.toUpper();
}
QString Junat::getTrainType() const {
    return trainType;
}

QString Junat::getDepartureDate() const {
    return departureDate.toLocalTime().toString("dd-MMM-yyyy");
}

QString Junat::getLastRefreshTime() const {
    return lastRefreshTime.toLocalTime().toString("HH:mm:ss");
}

Junat* Junat::getPointer(void) {
    return this;
}

const Junat::timeTableRow *Junat::getTimeTableRow(int index) const {
    if ( index < 0 || index >= filteredTimeTable.length() ) {
#ifdef QT_QML_DEBUG
        qDebug() << "index errooor";
#endif
        return NULL;
    }

    //return &timeTableRows.at(index);
    return filteredTimeTable.at(index);
}

QString Junat::getErrSummary(void) {
    return n_error.summary;
}

QString Junat::getErrBody(void) {
    return n_error.body;
}

QString Junat::getErrPrevSummary(void) {
    return n_error.previewSummary;
}

QString Junat::getErrPrevBody(void) {
    return n_error.previewBody;
}

// Setters
void Junat::setTrainNr(QString nr) {
    if ( s_trainNr == nr ) {
#ifdef QT_QML_DEBUG
        qDebug() << "TrainNr not changed.";
#endif
        refreshJunat();
    }
    else {
        s_trainNr = nr;
        buildUrl();
        emit NumberChanged();
    }
}

int Junat::getTimeTableCount(void ) const {
    //return timeTableRows.length();
    return filteredTimeTable.length();
}

void Junat::filterTimeTable(QString type) {
    if ( timeTableRows.length() <= 0 ) {
        return;
    }

    filteredTimeTable.clear();
    filteredTimeTable.push_back( &timeTableRows[0] );

    for ( int i = 1; i < timeTableRows.length()-1; i++ ) {
        if ( timeTableRows.at(i).type == type ) {
            filteredTimeTable.push_back( &timeTableRows[i] );
        }
    }

    filteredTimeTable.push_back( &timeTableRows[timeTableRows.length()-1] );
}

QString Junat::getTrainReadySource() const {
    return trainReady.source;
}

QString Junat::getTrainReadyAccepted() const {
    if ( trainReady.accepted ) {
        return "TRUE";
    }
    else {
        return "FALSE";
    }
}

QString Junat::getTrainReadyTime() const {
    return trainReady.timeStamp.toLocalTime().toString("HH:mm:ss");
}
