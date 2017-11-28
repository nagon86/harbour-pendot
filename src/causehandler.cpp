#include "causehandler.h"
#include <QFile>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTextStream>
#include <QTextCodec>

// Default constructor
causehandler::causehandler(QObject *parent) : QObject(parent) {
    getCauseData();
}

// Default destructor
causehandler::~causehandler() {
    causeList.clear();
}

// Reads JSON data and stores it to QMap<QString,Cause> structure
void causehandler::parseData(void) {

    QString param;
    QString value;
    bool isValue = false;
    bool isPassengerTerm = false;
    bool isQuotation = false;
    Cause tmp;

    // For loop to go through JSON data one character at a time
    for ( int i = 0; i < jsonData.length(); i++ ) {
        if ( jsonData.at(i) == '[' || jsonData.at(i) == ']' ||
             jsonData.at(i) == '{' ) {
            // Skip
        }
        // Tracks if special characters (e.g. , and :) are within
        // the JSON value.
        else if ( jsonData.at(i) == '"' ) {
            isQuotation = !isQuotation;
        }
        // If end of element/entry Entry should be appended to the structure
        else if ( !isQuotation && jsonData.at(i) == '}' ) {
            // Passenger Term need to be handled separately
            if ( isPassengerTerm ) {
                // Store new value to tmp
                appendCause(&tmp, param, value);
                isPassengerTerm = false;
                isValue = false;
                param.clear();
                value.clear();
            }
            else {
                isValue = false;
                // Store new value to tmp
                appendCause(&tmp, param, value);
                // Insert struct to map
                causeList.insert(tmp.categoryCode, tmp);
                tmp = Cause();
                param.clear();
                value.clear();
            }
        }
        else if ( !isQuotation && jsonData.at(i) == ':' ) {
            if ( param == "passengerTerm" ) {
                isPassengerTerm = true;
                param.clear();
            }
            else if ( !isValue ) {
                isValue = true;
            }
        }
        else if ( !isQuotation && jsonData.at(i) == ',' ) {
            isValue = false;
            appendCause(&tmp, param, value);
            param.clear();
            value.clear();
        }
        else {
            if ( isValue ) {
                value.append(jsonData.at(i));
            }
            else {
                param.append(jsonData.at(i));
            }
        }
    }
    jsonData.clear();
}

// Helper function to store data to struct.
// Parameter t is the pointer to struct.
// Parameters p and v contains parameter name and parameter value respectively
void causehandler::appendCause( Cause *t, QString p, QString v ) {
    // Check what value is being stored and store value v to correct location
    if ( p == "id" ) {
        t->id = v;
    }
    else if ( p == "categoryCode" || p == "detailedCategoryCode" ||
              p == "thirdCategoryCode") {
        t->categoryCode = v;
    }
    else if ( p == "categoryName" || p == "detailedCategoryName" ||
              p == "thirdCategoryName" ) {
        t->categoryName = v;
    }
    else if ( p == "validFrom" ) {
        t->validFrom = v;
    }
    else if ( p == "fi" ) {
        t->passengerTerm.fi = v;
    }
    else if ( p == "en" ) {
        t->passengerTerm.en = v;
    }
    else if ( p == "sv" ) {
        t->passengerTerm.sv = v;
    }
    else if ( p == "description" ) {
        t->description = v;
    }
#ifdef QT_QML_DEBUG
    else {
        if (!p.isEmpty() || !v.isEmpty()) {
            qDebug() << "Unused param and value: " + p + ", " + v;
        }
    }
#endif
}

// Function to deside if JSON data should be read from local file
// or from URL.
// When forced set to true it will always read from URL
void causehandler::getCauseData(bool forced) {
    static bool hasCause = false;
    static bool hasDetailedCause = false;
    static bool hasThirdCause = false;

    // Allow forcefull get from the web
    if ( forced ) {
        causeList.clear();
        hasCause = false;
        hasDetailedCause = false;
        hasThirdCause = false;
    }
    else {
        // Try to read from file
        if ( !hasCause )  {
            hasCause = readFromFile(FileType::CauseCode);
        }
        if ( !hasDetailedCause ) {
            hasDetailedCause = readFromFile(FileType::DetailedCode);
        }
        if ( !hasThirdCause ) {
            hasThirdCause = readFromFile(FileType::thirdCode);
        }
    }

    // Request from the digitraffic.com
    if ( !hasCause &&  !readFromWeb(FileType::CauseCode)) {
        // Failed to get CauseCode
    }
    if ( !hasDetailedCause && !readFromWeb(FileType::DetailedCode) ) {
        // Failed to get DetailedCode
    }
    if ( !hasThirdCause && !readFromWeb(FileType::thirdCode) ) {
        // Failed to get ThirdCode
    }
}

// Reads JSON data from local file.
// Parameter filetype defines which "tier" of cause code will be requested
// as well as to what file the JSON reply will be read.
// Returns true if reading was successfull
bool causehandler::readFromFile( FileType filetype ) {
    QString filename;

    switch (filetype) {
    case FileType::CauseCode:
        filename = causeCodeName;
        break;
    case FileType::DetailedCode:
        filename = detailedCauseName;
        break;
    case FileType::thirdCode:
        filename = thirdCauseCodeName;
        break;
    }

    // Store filename to variable
    QFile file(STORAGE_FOLDER + "/" + filename);

    // Check if file exists
    if (!file.exists()) {
        return false;
    }

    // Try to open file for read only
    if ( !file.open(QIODevice::ReadOnly | QIODevice::Text) ) {
        return false;
    }

    // Read all data to variable
    jsonData = file.readAll();
    file.close();

    // Start crunching the data
    parseData();
    return true;
}

// Writes JSON data in memory to a file.
// Parameter filetype defines which "tier" of cause code will be requested
// Returns true if writing was successfull
bool causehandler::writeToFile( FileType filetype ) {
    QString filename;

    switch (filetype) {
    case FileType::CauseCode:
        filename = causeCodeName;
        break;
    case FileType::DetailedCode:
        filename = detailedCauseName;
        break;
    case FileType::thirdCode:
        filename = thirdCauseCodeName;
        break;
    }

    // Store file to variable
    QFile file(STORAGE_FOLDER + "/" + filename);
    // Try opening the file.
    // QIODevice::Truncate shrinks the file size to contain only newly written data
    if ( !file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text) ) {
        return false;
    }

    // Open QTextStream to help writing the data
    QTextStream in(&file);

    // Force text stream to utilize UTF-8 encoding
    in.setCodec(QTextCodec::codecForName("UTF-8"));
    in.setAutoDetectUnicode(true);

    // Write data to the file
    in << jsonData;
    file.flush();

    // Close file
    file.close();
    return true;
}

// Requests JSON data from digitraffic URL.
// Parameter filetype defines which "tier" of cause code will be requested
// as well as to what file the JSON reply will be stored.
// Returns true if request was successfull
bool causehandler::readFromWeb( FileType filetype ) {
    QEventLoop loop;
    QUrl fileURL;
    QNetworkAccessManager n_manager;
    QNetworkRequest n_request;

    // Select used URL based on filetype
    switch (filetype) {
    case FileType::CauseCode:
        fileURL = causeCodeUrl;
        break;
    case FileType::DetailedCode:
        fileURL = detailedCauseCodeUrl;
        break;
    case FileType::thirdCode:
        fileURL = thirdCauseCodeUrl;
        break;
    }

    // Initiate network request
    n_request.setUrl(fileURL);
    QNetworkReply* n_reply = n_manager.get(n_request);

    // Connect network reply to loop exit slot
    connect(n_reply, SIGNAL(finished()), &loop, SLOT(quit()));
    // Start waiting for network reply
    loop.exec();

    // Read network reply to variable
    jsonData = n_reply->readAll();
    // remove network reply connection
    disconnect( n_reply, SIGNAL(readyRead()) );
    // delete old network reply data
    n_reply->deleteLater();
    n_reply = NULL;

    // Verify JSON data
    if ( countBrackets() ) {
        writeToFile(filetype);
        parseData();
        return true;
    }
    else {
        return false;
    }
}

// Helper function to determine if all network data is received
// This is done by counting +1 for '{' '[' characters and -1 for ']' '}' characters of JSON data
// Returns true if end count is 0 (aka JSON assued to be valid)
bool causehandler::countBrackets() {
    int bCount = 0;
    if ( jsonData.length() > 0 ) {
        for ( int i = 0; i < jsonData.length(); i++ ) {
            if ( jsonData.at(i) == '{' || jsonData.at(i) == '[' ) {
                bCount++;
            }
            else if ( jsonData.at(i) == '}' || jsonData.at(i) == ']' ) {
                bCount--;
            }
        }
    }
    else {
        return false;
    }
    return bCount == 0;
}

// Force refreshing of local data
// (mainly for QML use)
void causehandler::forceRefresh( void ) {
    getCauseData(true);
}

// Gets cause code matching with string id
// Return "Not found" if id could not be found from local data
QString causehandler::getCode( QString id ) {
    QMap<QString,Cause>::const_iterator iter = causeList.find(id);
    if ( iter == causeList.end() ) {
        return "Not Found";
    }
    return iter->categoryCode;
}

// Gets cause name matching with string id
// Return "Not found" if id could not be found from local data
QString causehandler::getName( QString id ) {
    QMap<QString,Cause>::const_iterator iter = causeList.find(id);
    if ( iter == causeList.end() ) {
        return "Not Found";
    }
    return iter->categoryName;
}

// Get passenger term matching with string id.
// Possible language options are "fi", "sv" and "en"
// Return "NULL" if id could not be found from local data
// Return "Unknown language" if lang is not valid
QString causehandler::getPassengerTerm( QString id, QString lang ) {
    QMap<QString,Cause>::const_iterator iter = causeList.find(id);
    if ( iter == causeList.end() ) {
        return "NULL";
    }
    if ( iter->passengerTerm.fi.isEmpty() || iter->passengerTerm.en.isEmpty() ||
         iter->passengerTerm.sv.isEmpty() ) {
        return "NULL";
    }
    if ( lang == "fi" ) {
        return iter->passengerTerm.fi;
    }
    else if ( lang == "en" ) {
        return iter->passengerTerm.en;
    }
    else if ( lang == "sv" ) {
        return iter->passengerTerm.sv;
    }
    else {
        return "Unknown language";
    }
}

// Get cause description matching with string id.
// Return "Not found" if id could not be found from local data
// Return "NULL" if id has no description
QString causehandler::getDescription( QString id ) {
    QMap<QString,Cause>::const_iterator iter = causeList.find(id);
    if ( iter == causeList.end() ) {
        return "Not Found";
    }
    if ( iter->description.isEmpty() ) {
        return "NULL";
    }

    return iter->description;
}
