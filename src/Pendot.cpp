/*
  Copyright (C) 2013 Jolla Ltd.
  Contact: Thomas Perl <thomas.perl@jollamobile.com>
  All rights reserved.

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Jolla Ltd nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifdef QT_QML_DEBUG
#include <QtQuick>
#endif

#include <QtQml>
#include <QtGlobal>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <sailfishapp.h>
#include "junat.h"
#include "timetablemodel.h"
#include "logwriter.h"

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
        //fprintf(stderr, "%s Debug: %s\n", timestamp.constData(), localMsg.constData());
        log.write(message);
        stream.reset();
        break;
    case QtInfoMsg:
        fprintf(stderr, "%s Info: %s (%s:%u, %s)\n", timestamp.constData(), localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
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

int main(int argc, char *argv[])
{
    // SailfishApp::main() will display "qml/template.qml", if you need more
    // control over initialization, you can use:
    //
    //   - SailfishApp::application(int, char *[]) to get the QGuiApplication *
    //   - SailfishApp::createView() to get a new QQuickView * instance
    //   - SailfishApp::pathTo(QString) to get a QUrl to a resource file
    //
    // To display the view, call "show()" (will show fullscreen on device).

    qInstallMessageHandler(myMessageOutput);

    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));
    app->setApplicationName("pendot");

    qmlRegisterType<Junat>("harbour.pendot.junat",1,0,"Junat");
    qmlRegisterType<TimeTableModel>("harbour.pendot.timetablemodel",1,0,"TimeTableModel");
    qmlRegisterType<StationHandler>("harbour.pendot.stationhandler",1,0,"StationHandler");

    QScopedPointer<QQuickView> view(SailfishApp::createView());
    view->setSource(SailfishApp::pathTo("qml/Pendot.qml"));
    view->showFullScreen();

    return app->exec();
    //return SailfishApp::main(argc, argv);
}

