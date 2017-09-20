#include "causehandler.h"
#include <QFile>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QTextStream>

causehandler::causehandler(QObject *parent) : QObject(parent) {
    getCauseData();
}

causehandler::~causehandler() {
    causeList.clear();
}


void causehandler::parseData(void) {

    QString param;
    QString value;
    bool isValue = false;
    bool isPassengerTerm = false;
    bool isQuotation = false;
    Cause tmp;

    for ( int i = 0; i < jsonData.length(); i++ ) {
        if ( jsonData.at(i) == '[' || jsonData.at(i) == ']' ||
             jsonData.at(i) == '{' ) {
            // Skip
        }
        else if ( jsonData.at(i) == '"' ) {
            isQuotation = !isQuotation;
        }
        else if ( !isQuotation && jsonData.at(i) == '}' ) {
            if ( isPassengerTerm ) {
                appendCause(&tmp, param, value);
                isPassengerTerm = false;
                isValue = false;
                param.clear();
                value.clear();
            }
            else {
                isValue = false;
                appendCause(&tmp, param, value);
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

void causehandler::appendCause( Cause *t, QString p, QString v ) {
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

    QFile file(filename);
    if (!file.exists()) {
        return false;
    }
    if ( file.open(QIODevice::ReadOnly | QIODevice::Text) ) {
        return false;
    }
    jsonData = file.readAll();
    file.close();
    parseData();
    return true;
}

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

    QFile file(filename);
    if ( file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text) ) {
        return false;
    }

    QTextStream in(&file);
    in.setAutoDetectUnicode(true);
    in << jsonData;
    file.flush();
    file.close();
    return true;
}

bool causehandler::readFromWeb( FileType filetype ) {
    QEventLoop loop;
    QUrl fileURL;
    QNetworkAccessManager n_manager;
    QNetworkRequest n_request;

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

    n_request.setUrl(fileURL);
    QNetworkReply* n_reply = n_manager.get(n_request);
    connect(n_reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    jsonData = n_reply->readAll();
    disconnect( n_reply, SIGNAL(readyRead()) );
    n_reply->deleteLater();
    n_reply = NULL;

    if ( countBrackets() ) {
        writeToFile(filetype);
        parseData();
        return true;
    }
    else {
        return false;
    }
}

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

void causehandler::forceRefresh( void ) {
    getCauseData(true);
}

QString causehandler::getCode( QString id ) {
    QMap<QString,Cause>::const_iterator iter = causeList.find(id);
    if ( iter == causeList.end() ) {
        return "Not Found";
    }
    return iter->categoryCode;
}

QString causehandler::getName( QString id ) {
    QMap<QString,Cause>::const_iterator iter = causeList.find(id);
    if ( iter == causeList.end() ) {
        return "Not Found";
    }
    return iter->categoryName;
}

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
