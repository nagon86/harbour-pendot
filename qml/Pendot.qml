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

import QtQuick 2.0
import Sailfish.Silica 1.0
import org.nemomobile.notifications 1.0
import harbour.pendot.junat 1.0
import harbour.pendot.timetablemodel 1.0
import harbour.pendot.stationhandler 1.0

import "pages"

ApplicationWindow
{

    property bool jnaIsLoaded
    property bool stnIsLoaded

    Junat {
        id: jna
        Component.onCompleted: {
            myModel.setPointer(jna.getPointer())
            jnaIsLoaded
        }
    }
    StationHandler {
        id: stn
        Component.onCompleted: {
            myModel.setStationPointer(stn.getStationPointer())
            stnIsLoaded = true
        }
    }

    TimeTableModel {
        id : myModel
        Component.onCompleted: {
            myModel.setPointer(jna.getPointer())
            myModel.setStationPointer(stn.getStationPointer())
        }
    }

    Notification {
        id: notification
        category: "x-nemo.example"
        appName: "Pendot"
        appIcon: "/usr/share/example-app/icon-l-application"
        summary: "Notification summary"
        body: "Notification body"
        previewSummary: "Notification preview summary"
        previewBody: "Notification preview body"
        itemCount: 1
        //timestamp: "2013-02-20 18:21:00"
        remoteActions: [ {
                "name": "default",
                "displayName": "Do something",
                "icon": "icon-s-do-it",
                "service": "org.nemomobile.example",
                "path": "/example",
                "iface": "org.nemomobile.example",
                "method": "doSomething",
                "arguments": [ "argument", 1 ]
            },{
                "name": "ignore",
                "displayName": "Ignore the problem",
                "icon": "icon-s-ignore",
                "service": "org.nemomobile.example",
                "path": "/example",
                "iface": "org.nemomobile.example",
                "method": "ignore",
                "arguments": [ "argument", 1 ]
            } ]
        onClicked: console.log("Clicked")
        onClosed: console.log("Closed, reason: " + reason)
    }

    Connections {
        target: jna
        onNetworkErrorNotification: {
            notification.summary = jna.getErrSummary();
            notification.body = jna.getErrBody();
            notification.previewSummary = jna.getErrPrevSummary();
            notification.previewBody = jna.getErrPrevBody();
            notification.publish()
            console.log("onErrorNotification")
        }
        //onRefereshGui: {
        //    notification.close()
        //}
    }

    Timer {
        id: refreshTimer
        interval: 5*60*1000 // interval in ms (5min)
        running: false
        repeat: true
        onTriggered: {
            jna.refreshJunat()
            notification.close()
        }
    }

    //initialPage: Component { MainPage { } }
    initialPage: Component { StartupPage { } }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")
    allowedOrientations: defaultAllowedOrientations
}


