#ifndef LOGWRITER_H
#define LOGWRITER_H

#include <QTextStream>
#include <QFile>

class LogWriter
{
public:
    LogWriter();
    ~LogWriter();
    void write(QString text) const;
private:
    QFile* file;
    const QString LOG_FILENAME = "Pendot-Log.txt";
};

#endif // LOGWRITER_H
