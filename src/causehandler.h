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

    Q_INVOKABLE void forceRefresh( void );
    Q_INVOKABLE QString getCode( QString id );
    Q_INVOKABLE QString getName( QString id );
    Q_INVOKABLE QString getPassengerTerm(QString id , QString lang);
    Q_INVOKABLE QString getDescription( QString id );

private:

    enum FileType {
        CauseCode,
        DetailedCode,
        thirdCode
    };

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

    const QUrl causeCodeUrl = QUrl::fromUserInput("https://rata.digitraffic.fi/api/v1/metadata/cause-category-codes");
    const QUrl detailedCauseCodeUrl = QUrl::fromUserInput("https://rata.digitraffic.fi/api/v1/metadata/detailed-cause-category-codes");
    const QUrl thirdCauseCodeUrl = QUrl::fromUserInput("https://rata.digitraffic.fi/api/v1/metadata/third-cause-category-codes");
    const QString causeCodeName = "cause-code.json";
    const QString detailedCauseName = "detailed-code.json";
    const QString thirdCauseCodeName = "third-code.json";

    // Functions
    void parseData( void );
    void appendCause( Cause* t, QString p, QString v );
    void getCauseData(bool forced = false);
    bool readFromWeb( FileType filetype );
    bool countBrackets();
    bool readFromFile(FileType filetype );
    bool writeToFile( FileType file );
    void storeValue( QString p, QString v);

signals:

public slots:

};

#endif // CAUSEHANDLER_H
