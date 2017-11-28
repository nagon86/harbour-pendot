#include "junat.h"
#include <QString>
#include <QFile>
#include <QDateTime>
#include <QNetworkAccessManager>
#include <QNetworkReply>

//using namespace std;

// Default Constructor
Junat::Junat(QObject *parent) :
    QObject(parent),
    trainNr("0"),
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

    // Initialize retry timer
    retryTimer = new QTimer(this);
    retryTimer->setSingleShot(true);
    connect(retryTimer, SIGNAL(timeout()), this, SLOT(retryGetUrl()));

    timetableAcceptanceDate = QDateTime::currentDateTime();
    lastRefreshTime = QDateTime::currentDateTime();
    departureDate = QDateTime::currentDateTime();
    s_trainNr = "";
}

// Destructor
Junat::~Junat() {
    timeTableRows.clear();
    delete n_manager;
    delete retryTimer;
    retryTimer = NULL;
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

// Initiate new network request
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

// qml invokable alternative for getJSon
void Junat::refresData(void) {
    getJSON();
}

// create notification popup for network errors
void Junat::netError( QNetworkReply::NetworkError nErr ) {
#ifdef QT_QML_DEBUG
    qDebug() << QDateTime::currentDateTime().toString("HH:mm:ss") << " ...Network Error Handler... ";
#endif
    qDebug() << "Network error: " << nErr;
    n_error.body = nErr;
    n_error.summary = "Network Error";
    n_error.previewBody = nErr;
    n_error.previewSummary = "Network Error";
    n_reply->deleteLater();
    disconnect(n_reply, SIGNAL(readyRead()));
    disconnect(n_reply, SIGNAL(error(QNetworkReply::NetworkError)));
    n_reply = NULL;
    //emit networkErrorNotification();
}

// Start retry timer with 3s timeout
void Junat::startRetryTimer(void) {
    retryTimer->start(3000); // Start timer with 3s timeout
}

// Function to check how many retries has been used.
// And ultimately give up trying if no valid data is received.
void Junat::retryGetUrl() {
#ifdef QT_QML_DEBUG
    qDebug() << "retryGetUrl: " << QDateTime::currentDateTime().toString("HH:mm:ss");
#endif
    if ( retryCount >= MAX_RETRY_COUNT ) {
        retryCount = 0;
        emit networkErrorNotification();
        return;
    }
    else {
        retryCount++;
        getJSON();
    }
}

// Function to parse data to internal data structures
void Junat::parseJSON() {

    if ( n_reply->error() != QNetworkReply::NoError ) {
        startRetryTimer();
        n_reply->deleteLater();
        disconnect(n_reply, SIGNAL(readyRead()));
        disconnect(n_reply, SIGNAL(error(QNetworkReply::NetworkError)));
        n_reply = NULL;
        return;
    }

    // Read data to ByteArray
    QString tmp = n_reply->readAll();
    n_reply->deleteLater();
    disconnect(n_reply, SIGNAL(readyRead()));
    disconnect(n_reply, SIGNAL(error(QNetworkReply::NetworkError)));
    n_reply = NULL;

    int bCount = 0;

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
        //emit networkErrorNotification();
#endif
        tmp.clear();
        startRetryTimer();
        return;
    }

    if ( tmp == JSONData ) {
        // Data has not changed
    }
    else {
        JSONData = tmp;

        if (JSONData.length() > 2) {
            // Valid Response
            initData();
            if (parseData() == 0) {
                // Everything handled correctly
            }
        }
    }
    fixCauseCodes();
    fixActualTimes();
    filterTimeTable();
    emit TimeTableChanged();
#ifdef QT_QML_DEBUG
    qDebug() << "emit refreshGui " << QDateTime::currentDateTime().toString("HH:mm:ss");
#endif
    // Update last refresh time
    lastRefreshTime = QDateTime::currentDateTime();
    retryCount = 0;
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

    // Go through all data stored in JSONData variable
    do {
        // Start of new JSON table. Add table name
        // to parameter stack to keep track on what's going on
        if ( JSONData.at(index) == '[' ) {
            paramStack.push_back(param);
            // Count brackets to find what level time table is located
            bOpen++;
            if ( param == "timeTableRows" ) {
                timeTableLevel = bOpen;
            }
            param.clear();
        }
        // end of JSON table
        else if ( JSONData.at(index) == ']') {
            // count brackets
            bOpen--;
            // If end of timeTableRows
            if ( paramStack.at(paramStack.length()-1) =="timeTableRows") {
                // Store absolute index
                tmp.absoluteIndex = timeTableRows.length();
                // Add current data from tmp to vector
                timeTableRows.append(tmp);
                // Reset tmp variable
                tmp = timeTableRow();
            }
            // Remove one from the parameter stack
            paramStack.pop_back();
        }
        // Start of JSON element
        else if ( JSONData.at(index) == '{' ) {
            // Store parameter value before clearing temporary variables
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
        // End of JSON element
        else if ( JSONData.at(index) == '}' ) {
            // Store data to temporary struct
            storeData(param, value, paramStack.at(paramStack.length()-1), &tmp);
            // reduce parameter stack
            paramStack.pop_back();
            bOpen--;
            // clear temporary varaibles
            isValue = false;
            param.clear();
            value.clear();
        }
        // If separator between parameter name and value
        else if ( !isValue && JSONData.at(index) == ':' ) {
            isValue = true;
        }
        else if ( JSONData.at(index) == ',' ) {
            storeData(param, value, paramStack.at(paramStack.length()-1), &tmp);
            // If working timeTableRows values
            if ( paramStack.at(paramStack.length()-1) == "timeTableRows" && bOpen == timeTableLevel ) {
                // add absolute index
                tmp.absoluteIndex = timeTableRows.length();
                // store recorded values to vector
                timeTableRows.append(tmp);
                // Clear old values
                tmp = timeTableRow();
            }
            // Reset values and parameters
            isValue = false;
            param.clear();
            value.clear();
        }
        else if ( JSONData.at(index) == '"' ) {
            //Skip
        }
        // append data to temporary variables
        // based on isValue boolean
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

    // Return current index at the end of execution
    return index;
}

// Call helper functions to store data to proper locations in struct t.
// pp = value from parameter stack
// p = parameter name
// v = value name
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

// Helper function to map parameter names to variable names
// and store values to proper locations and in proper format
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

// Helper function to map parameter names to variable names
// and store values to proper locations and in proper format.
// Used for timetable parameters
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

// Helper function to map parameter names to variable names
// and store values to proper locations and in proper format.
// Used for cause codes
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

// Helper function to copy cause codes from arrival to departure
// and vice versa. Copied only if one is empty.
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

// Helper function to fix situations where some stations
// doesn't have proper time measurement and they never get
// the "ActualTime" parameter.
// Starts from last row and after first "actual time" marks every
// station as passed regardless if the station has "actual time" or not
void Junat::fixActualTimes(void) {
    bool isActual = false;
    for ( int i = timeTableRows.length()-1; i >= 0; i-- ) {
        if ( !isActual ) {
            isActual = !timeTableRows.at(i).actualTime.isNull();
        }
        else {
            if ( timeTableRows.at(i).actualTime.isNull() ) {
                timeTableRows[i].actualTime = timeTableRows.at(i).liveEstimateTime;
            }
        }
    }
}

void Junat::refreshJunat() {
    getJSON();
}

// Make request URL based on stored train number
void Junat::buildUrl(void) {
    // Check if stored train number is valid integer
    if (s_trainNr.toInt()) {
        currentUrl = QUrl(QString("https://rata.digitraffic.fi/api/v1/live-trains/") + s_trainNr);
    }
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
        return NULL;
    }
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

// -----------
// | Getters |
// -----------

void Junat::setTrainNr(QString nr) {
    if ( s_trainNr == nr ) {
        refreshJunat();
    }
    else {
        s_trainNr = nr;
        buildUrl();
        emit NumberChanged();
    }
}

int Junat::getTimeTableCount(void ) const {
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

QString Junat::getStationName(int index) const {
    if ( index >= timeTableRows.length() || index < 0 ) {
        return "NULL";
    }
    return timeTableRows.at(index).stationShortCode;
}

int Junat::getStationMinutes(int index) const {
    if ( index >= timeTableRows.length() || index < 0 ) {
        return 0;
    }
    return timeTableRows.at(index).differenceInMinutes;
}

bool Junat::stopHasCause(int index) const {
    return timeTableRows.at(index).causes.hasCause;
}

QString Junat::getStopCauseCode(int index) const {
    if ( timeTableRows.at(index).causes.categoryCode.isEmpty() ) {
        return "NULL";
    }
    return timeTableRows.at(index).causes.categoryCode;
}

QString Junat::getStopDetailedCauseCode(int index) const {
    if ( timeTableRows.at(index).causes.detailedCategoryCode.isEmpty() ) {
        return "NULL";
    }
    return timeTableRows.at(index).causes.detailedCategoryCode;
}

QString Junat::getStopThirdCauseCode(int index) const {
    if ( timeTableRows.at(index).causes.thirdCategoryCode.isEmpty() ) {
        return "NULL";
    }
    return timeTableRows.at(index).causes.thirdCategoryCode;
}

int Junat::getCurrentStation() const {
    for ( int i = timeTableRows.length()-1; i > 0; i-- ) {
        if ( !timeTableRows.at(i).actualTime.isNull() ) {
            return i;
        }
    }
    return 0;
}
