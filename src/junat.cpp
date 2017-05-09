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
    trainCategory(""),
    commuterLineID(""),
    runningCurrently("false"),
    cancelled("false"),
    version("0")
{

    // Create class for network manager
    n_manager = new QNetworkAccessManager(this);

    timetableAcceptanceDate = QDateTime::fromString("1970-01-01", "yyyy-MMM-dd");
    s_trainNr = "";

    // Connect network manager to parseJSON
    connect(n_manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(parseJSON(QNetworkReply*)));
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
    trainCategory = "";
    commuterLineID = "";
    runningCurrently = "";
    cancelled = "";
    version = "";
    timetableAcceptanceDate = QDateTime();
    timeTableRows.clear();
    filteredTimeTable.clear();
}

void Junat::getJSON() {
    n_manager->get(QNetworkRequest(currentUrl));
}

void Junat::refresData(void) {
#ifdef QT_QML_DEBUG
    qDebug() << "Refreshing JSON";
#endif
    getJSON();
}

void Junat::parseJSON(QNetworkReply* nReply) {

    if ( nReply->error() != QNetworkReply::NoError ) {
#ifdef QT_QML_DEBUG
        qDebug() << "Network error, waiting for next refresh cycle";
#endif
        return;
    }

    // Read data to ByteArray
    QByteArray data = nReply->readAll();
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
        qDebug() << "Invalid data: Bracket miss match.";
#endif
        return;
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

    filterTimeTable();
    emit TimeTableChanged();
#ifdef QT_QML_DEBUG
    qDebug() << "emit refreshGui";
#endif
    emit refreshGui();
}

int Junat::parseData(int index, QString prevParam) {

    int bOpen = 0;
    bool isValue = false;
    QString param;
    QString value;
    timeTableRow tmp;

    if ( JSONData.at(index) == '[' ) {
        bOpen++;
        index++;
    }
    else if ( prevParam == "trainReady") {
        index++;
    }
    else {
        // Fail?
    }
    do {
        if ( JSONData.at(index) == '[') {
            index = parseData(index, param);
            param.clear();
        }
        else if ( JSONData.at(index) == ']') {
            bOpen--;
            return index;
        }
        else if ( JSONData.at(index) == '{' ) {
            bOpen++;
        }
        else if ( JSONData.at(index) == '}' ) {
            storeData(param, value, prevParam, &tmp);
            isValue = false;
            bOpen--;
            if ( prevParam == "timeTableRows" && bOpen == 1 ) {
                timeTableRows.append(tmp);
                tmp = timeTableRow();
            }
            param.clear();
            value.clear();
        }
        else if ( !isValue && JSONData.at(index) == ':' ) {
            if ( param == "trainReady") {
                index = parseData(index, param)-1;
                param.clear();
            }
            else {
                isValue = true;
            }
        }
        else if ( JSONData.at(index) == ',' ) {
            storeData(param, value, prevParam, &tmp);
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
            qDebug() << "Train ready information. Discarding: " + p + ", " + v;
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
    }
    else if ( p == "categoryCode" ) {
        t->causes.categoryCode = v;
    }
    else if ( p == "detailedCategoryCodeId" ) {
        t->causes.detailedCategoryCodeId = v;
    }
    else if ( p == "detailedCategoryCode" ) {
        t->causes.detailedCategoryCode = v;
    }
    else {
#ifdef QT_QML_DEBUG
        qDebug() << "addCauseCode: Unused param and value: " + p + " " + v;
#endif
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
        currentUrl = QUrl(QString("http://rata.digitraffic.fi/api/v1/live-trains/") + s_trainNr);
    }
#ifdef QT_QML_DEBUG
    qDebug() << currentUrl;
#endif
    getJSON();
}

// Getters
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

// Setters
void Junat::setTrainNr(QString nr) {
    if ( s_trainNr == nr ) {
#ifdef QT_QML_DEBUG
        qDebug() << "TrainNr not changed. Discarding.";
#endif
    }
    else {
        s_trainNr = nr;
        buildUrl();
        emit NumberChanged();
    }
}

int Junat::getTimeTableCount(void ) const {
#ifdef QT_QML_DEBUG
    qDebug() << "last timetable: " << filteredTimeTable.length() << " station: " << filteredTimeTable.at(filteredTimeTable.length()-1)->stationShortCode;
#endif
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
