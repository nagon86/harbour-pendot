#ifndef LOGWRITER_H
#define LOGWRITER_H

#include <QTextStream>
#include <QFile>

// simple class to handle logging

class LogWriter
{
public:
    LogWriter();
    ~LogWriter();
    void write(QString text) const;
private:
    QFile* file;
    const QString LOG_FILENAME = "Pendot-Log.txt";
    const QString STORAGE_FOLDER = ".local/share/harbour-pendot";
};

#endif // LOGWRITER_H
