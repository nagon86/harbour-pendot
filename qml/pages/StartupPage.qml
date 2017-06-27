import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.pendot.junat 1.0
import harbour.pendot.timetablemodel 1.0

Page {

    property int myModelPointers

    Component.onCompleted: {
        startupPoller.start()
        console.log("Loading Startup page...")
    }

    Timer {
        id: startupPoller
        interval: 2000
        repeat: true
        onTriggered: {
            myModelPointers = myModel.getPointers()
            if ( myModelPointers === 3 ) {
                pageStack.replace(Qt.resolvedUrl("MainPage.qml"))
                startupPoller.stop()
            }
            else {
                // Station model
                console.log("Pointers: " + myModelPointers)
                switch(myModelPointers) {
                case 0:
                    myModel.setPointer(jna.getPointer())
                    myModel.setStationPointer(stn.getStationPointer())
                    console.log("Connecting ALL")
                    break
                case 1:
                    myModel.setPointer(jna.getPointer())
                    console.log("Connecting jna" + jna.getPointer())
                    break
                case 2:
                    myModel.setStationPointer(stn.getStationPointer())
                    console.log("Connecting stn")
                    break
                }


//                if ( myModelPointers === 0 ) {
//                    myModel.setPointer(jna.getPointer())
//                    myModel.setStationPointer(stn.getStationPointer())
//                }
//                // Station model
//                if ( myModelPointers === 1 ) {
//                    myModel.setPointer(jna.getPointer())
//                }
//                // Juna model
//                if ( myModelPointers === 2 ) {
//                    myModel.setStationPointer(stn.getStationPointer())
//                }
            }
        }
    }

    Label {
        anchors.bottom: busyIndicator.top
        anchors.horizontalCenter: busyIndicator.horizontalCenter
        anchors.bottomMargin: Theme.paddingLarge
        text: "Starting Up..."
        font.pixelSize: Theme.fontSizeLarge
    }

    BusyIndicator {
        id: busyIndicator
        size: BusyIndicatorSize.Large
        anchors.centerIn: parent
        running: true
    }
}
