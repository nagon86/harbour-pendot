#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <QtQml>
#include <QtGlobal>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QDir>
#include <sailfishapp.h>
#include "junat.h"
#include "timetablemodel.h"
#include "logwriter.h"
#include "causehandler.h"
#include "stationhandler.h"

#ifdef QT_QML_DEBUG
// Function to allow logging to a file
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    static LogWriter log;
    QString message;
    QTextStream stream(&message);
    QByteArray localMsg = msg.toLocal8Bit();
    QByteArray timestamp = QString(QDateTime::currentDateTime().toString("HH:mm:ss")).toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        stream << timestamp.constData() << " Debug: " << localMsg.constData() << " " << context.file << " " << context.line << " " << context.function << endl;
        fprintf(stderr, "%s Debug: %s\n", timestamp.constData(), localMsg.constData());
        log.write(message);
        stream.reset();
        break;
    case QtInfoMsg:
        fprintf(stderr, "%s Info: %s (%s:%u, %s)\n", timestamp.constData(), localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        //fprintf(stderr, "%s Warning: %s (%s:%u, %s)\n", timestamp.constData(), localMsg.constData(), context.file, context.line, context.function);
        fprintf(stderr, "%s Warning: %s (%s:%u, %s)\n", timestamp.constData(), localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "%s Critical: %s (%s:%u, %s)\n", timestamp.constData(), localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "%s Fatal: %s (%s:%u, %s)\n", timestamp.constData(), localMsg.constData(), context.file, context.line, context.function);
        abort();
    }
}
#endif

const QString STORAGE_FOLDER = ".local/share/harbour-pendot";

int main(int argc, char *argv[])
{
#ifdef QT_QML_DEBUG
    qInstallMessageHandler(myMessageOutput);
#endif

    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    app->setApplicationName("pendot");

    // Check if data folder exist.
    QDir dir(STORAGE_FOLDER);
    if ( !dir.exists() ) {
        dir.mkpath(".");
    }

    // Register classes in a way that qml knows how to utilize them
    qmlRegisterType<Junat>("harbour.pendot.junat",1,0,"Junat");
    qmlRegisterType<TimeTableModel>("harbour.pendot.timetablemodel",1,0,"TimeTableModel");
    qmlRegisterType<StationHandler>("harbour.pendot.stationhandler",1,0,"StationHandler");
    qmlRegisterType<causehandler>("harbour.pendot.causehandler",1,0,"CauseHandler");

    QScopedPointer<QQuickView> view(SailfishApp::createView());
    view->setSource(SailfishApp::pathTo("qml/Pendot.qml"));
    view->showFullScreen();

    return app->exec();
}
