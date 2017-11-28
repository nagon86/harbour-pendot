#ifndef CAUSEHANDLER_H
#define CAUSEHANDLER_H

#include <QObject>
#include <QUrl>
#include <QString>
#include <QMap>

class causehandler : public QObject
{
    Q_OBJECT
public:
    explicit causehandler(QObject *parent = 0);
    ~causehandler();

    // Force refreshing of local data
    // (mainly for QML use)
    Q_INVOKABLE void forceRefresh( void );

    // Gets cause code matching with string id
    // Return "Not found" if id could not be found from local data
    Q_INVOKABLE QString getCode( QString id );

    // Gets cause name matching with string id
    // Return "Not found" if id could not be found from local data
    Q_INVOKABLE QString getName( QString id );

    // Get passenger term matching with string id.
    // Possible language options are "fi", "sv" and "en"
    // Return "NULL" if id could not be found from local data
    // Return "Unknown language" if lang is not valid
    Q_INVOKABLE QString getPassengerTerm(QString id , QString lang);

    // Get cause description matching with string id.
    // Return "Not found" if id could not be found from local data
    // Return "NULL" if id has no description
    Q_INVOKABLE QString getDescription( QString id );

private:

    enum FileType {
        CauseCode,
        DetailedCode,
        thirdCode
    };

    // Struct for storing Passenger Term
    struct Term {
        QString fi;
        QString en;
        QString sv;
    };

    struct Cause {
        QString id;
        QString categoryCode;
        QString categoryName;
        QString validFrom;
        QString description;
        Term passengerTerm;
    };

    QMap<QString,Cause> causeList;
    QString networkReply;
    QString jsonData;

    // Static URLs for cause code queries from digitraffic
    const QUrl causeCodeUrl = QUrl::fromUserInput("https://rata.digitraffic.fi/api/v1/metadata/cause-category-codes");
    const QUrl detailedCauseCodeUrl = QUrl::fromUserInput("https://rata.digitraffic.fi/api/v1/metadata/detailed-cause-category-codes");
    const QUrl thirdCauseCodeUrl = QUrl::fromUserInput("https://rata.digitraffic.fi/api/v1/metadata/third-cause-category-codes");
    // Filename of local data
    const QString causeCodeName = "cause-code.json";
    const QString detailedCauseName = "detailed-code.json";
    const QString thirdCauseCodeName = "third-code.json";
    const QString STORAGE_FOLDER = ".local/share/harbour-pendot";

    // *************
    // * Functions *
    // *************

    // Reads JSON data and stores it to QMap<QString,Cause> structure
    void parseData( void );

    // Helper function to store data to struct.
    // Parameter t is the pointer to struct.
    // Parameters p and v contains parameter name and parameter value respectively
    void appendCause( Cause* t, QString p, QString v );

    // Function to deside if JSON data should be read from local file
    // or from URL.
    // When forced set to true it will always read from URL
    void getCauseData(bool forced = false);

    // Requests JSON data from digitraffic URL.
    // Parameter filetype defines which "tier" of cause code will be requested
    // as well as to what file the JSON reply will be stored.
    // Returns true if request was successfull
    bool readFromWeb( FileType filetype );

    // Helper function to determine if all network data is received
    // This is done by counting +1 for '{' '[' characters and -1 for ']' '}' characters of JSON data
    // Returns true if end count is 0 (aka JSON assued to be valid)
    bool countBrackets();

    // Reads JSON data from local file.
    // Parameter filetype defines which "tier" of cause code will be requested
    // as well as to what file the JSON reply will be read.
    // Returns true if reading was successfull
    bool readFromFile(FileType filetype );

    // Writes JSON data in memory to a file.
    // Parameter filetype defines which "tier" of cause code will be requested
    // Returns true if writing was successfull
    bool writeToFile( FileType file );

signals:

public slots:

};

#endif // CAUSEHANDLER_H
