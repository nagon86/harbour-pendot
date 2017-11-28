import QtQuick 2.0
import Sailfish.Silica 1.0
import org.nemomobile.notifications 1.0
import harbour.pendot.junat 1.0
import harbour.pendot.timetablemodel 1.0
import harbour.pendot.stationhandler 1.0
import harbour.pendot.causehandler 1.0

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

    CauseHandler {
        id : causeHandler
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
        /*onClosing: {
            notification.close()
        }*/
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

    initialPage: Component { MainPage { } }
    cover: Qt.resolvedUrl("cover/CoverPage.qml")
    allowedOrientations: defaultAllowedOrientations
}


