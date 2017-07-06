#include "logwriter.h"
#include <QStandardPaths>
#include <QtGlobal>
#include <QFileInfo>
#include <QDebug>

LogWriter::LogWriter() {
    file = new QFile(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/" + LOG_FILENAME);
    //file = new QFile(QStandardPaths::DocumentsLocation + "\\" + LOG_FILENAME);
    file->open(QIODevice::WriteOnly | QIODevice::Append);
    QFileInfo fInfo(*file);
    qWarning() << "Opening file: " << fInfo.absoluteFilePath().toLocal8Bit();
}

LogWriter::~LogWriter() {
    file->close();
    file->deleteLater();
    file = NULL;
}

void LogWriter::write(QString text) const {
    //QByteArray tmp = text->toLocal8Bit();
    //file->write(tmp.constData());
    if ( !file->exists() ) {
        qWarning("Logfile not existing.");
    }
    file->write(text.toLocal8Bit());
}
