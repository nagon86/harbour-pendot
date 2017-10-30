import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.pendot.junat 1.0
import harbour.pendot.timetablemodel 1.0
//import QtSensors 5.3



Page {
    id: page

    property bool metadataVisible: true
    property bool metadataLock: false

    Component.onCompleted: {
        dTimer.start()
    }

    Timer {
        id: dTimer
        interval: 200
        repeat: true
        running: false
        onTriggered: {
            if ( pageStack.busy ) {                
                //console.log("Page stack busy")
            }
            else {
                //console.log("Pushing new page")
                dTimer.stop()
                pageStack.push(Qt.resolvedUrl("SearchPage.qml"))
            }
        }
    }

    // To enable PullDownMenu, place our content in a SilicaFlickable
    SilicaFlickable {
        anchors.fill: parent

        // PullDownMenu and PushUpMenu must be declared in SilicaFlickable, SilicaListView or SilicaGridView
        PullDownMenu {
            MenuItem {
                text: qsTr("Test GoTo")
                onClicked: {
                    if ( myListView.currentIndex == 20 ) {
                        myListView.currentIndex = 50
                    }
                    else {
                        myListView.currentIndex = 10
                    }
                }
            }
            MenuItem {
                text: qsTr(metadataVisible ? "Metadata Lock: Off" : "Metadata Lock: On")
                onClicked: {
                    metadataLock = !metadataLock
                }
            }
            MenuItem {
                text: qsTr(metadataVisible ? "Hide Metadata" : "Show Metadata")
                onClicked: {
                    if ( metadataLock == false ) {
                        metadataLock = true
                    }
                    metadataVisible = !metadataVisible
                }
            }

            MenuItem {
                text: qsTr("Manual Refresh")
                onClicked: {
                    refreshTimer.start()
                    jna.refreshJunat()
                }
            }

            MenuItem {
                text: qsTr("Search")
                onClicked: pageStack.push(Qt.resolvedUrl("SearchPage.qml"))
            }
        }

        // Tell SilicaFlickable the height of its content.
        //contentHeight: column.height

        // Place our content in a Column.  The PageHeader is always placed at the top
        // of the page, followed by our content.
            PageHeader {
                anchors.top: parent.top
                id: pageHeader
                title: jna.getTrainType + jna.trainNr
            }

            Row {
                id: refreshInfo
                anchors.horizontalCenter: parent.horizontalCenter
                x: Theme.paddingLarge
                y: Theme.paddingMedium
                Label {
                    color: Theme.highlightColor
                    text: qsTr("Updated:")
                    font.pixelSize: Theme.fontSizeTiny
                }
                Label {
                    text: jna.getLastRefreshTime
                    font.pixelSize: Theme.fontSizeTiny
                }
            }

            Grid {
                anchors.top: pageHeader.bottom
                id: dataGrid
                columns: 2
                columnSpacing: (page.width - Theme.paddingLarge) / 3
                rowSpacing: Theme.paddingSmall / 2
                x: Theme.paddingLarge
                visible: metadataVisible

                Label {
                    color: Theme.highlightColor
                    text: qsTr("Date:")
                }
                Label { text: jna.getDepartureDate }
                Label {
                    color: Theme.highlightColor
                    text: qsTr("Operator:")
                }
                Label { text: jna.getOperatorCode }
                Label {
                    color: Theme.highlightColor
                    text: qsTr("Train Ready:")
                }
                Label { text: jna.getTrainReadyAccepted ? "True" : "False" }
                Label {
                    color: Theme.highlightColor
                    text: qsTr("   Source:")
                }
                Label { text: jna.getTrainReadySource }
                Label {
                    color: Theme.highlightColor
                    text: qsTr("   Timestamp:")
                }
                Label { text: jna.getTrainReadyTime }
            }

            Label {
                anchors.top: dataGrid.bottom
                id: textTimeTable
                x: Theme.paddingLarge
                visible: metadataVisible
                text: qsTr("Timetable:")
                color: Theme.highlightColor
            }

            SilicaListView {
                anchors.top: metadataVisible ? textTimeTable.bottom : pageHeader.bottom
                anchors.topMargin: metadataVisible ? Theme.paddingLarge : 0
                width: parent.width
                height: metadataVisible ? parent.height - pageHeader.height - dataGrid.height- textTimeTable.height : parent.height - pageHeader.height

                // this is needed so that the list elements
                // do not overlap with other controls in column
                clip: true

                id: myListView
                model: myModel

                VerticalScrollDecorator {}

                delegate: ListItem {
                    id: timeTableItem
                    width: parent.width
                    contentHeight: timeMain.height + timeSecondary.height + Theme.paddingSmall

                    //menu: ContextMenu

                    // Background rectangle
                    Rectangle {
                        id: itemRect

                        width: parent.width
                        height: parent.height

                        color: Theme.primaryColor
                        opacity: !(index & 1) ? 0.05 : 0.00
                        //opacity: 0.05
                        //visible: !(index & 1)

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                pageStack.push(Qt.resolvedUrl("StationDetails.qml"), { absoluteIndex: absoluteIndex })
                            }
                        }
                    }

                    Label {
                        id: ifStop
                        x: Theme.paddingLarge
                        y: Theme.paddingMedium
                        anchors.rightMargin: Theme.paddingMedium
                        horizontalAlignment: Label.AlignHCenter

                        width: Theme.paddingSmall * 4
                        height: Theme.paddingSmall * 6

                        text: fillerChar
                        color: actualTime != "" ? Theme.highlightColor : Theme.primaryColor //"white"
                        font.pixelSize: Theme.fontSizeLarge
                        font.bold: true
                    }
                    Label {
                        id: timeMain
                        x: Theme.paddingLarge
                        y: Theme.paddingMedium

                        anchors.left: ifStop.right
                        anchors.leftMargin: Theme.paddingMedium
                        anchors.rightMargin: Theme.paddingMedium
                        horizontalAlignment: Label.AlignHCenter
                        verticalAlignment: Label.AlignHCenter

                        width: Theme.paddingSmall * 15
                        height: Theme.paddingSmall * 6

                        text: differenceInMin == 0 ? scheduledTime : actualTime != "" ? actualTime : estimateTime
                        font.pixelSize: Theme.fontSizeLarge
                    }
                    Label {
                        id: timeSecondary
                        x: Theme.paddingLarge

                        anchors.top: differenceInMin == 0 ? parent.top : timeMain.bottom
                        anchors.left: ifStop.right
                        horizontalAlignment: Label.AlignHCenter
                        verticalAlignment: Label.AlignHCenter

                        width: timeMain.width
                        height: timeMain.height
                        text: differenceInMin == 0 ? "" : scheduledTime
                        font.pixelSize: differenceInMin == 0 ? Theme.fontSizeLarge/2 : Theme.fontSizeLarge / 2
                        font.strikeout: true
                        visible: true
                    }
                    Label {
                        id: station
                        x: Theme.paddingLarge
                        y: Theme.paddingMedium

                        anchors.left: timeMain.right
                        anchors.leftMargin: Theme.paddingMedium
                        anchors.rightMargin: Theme.paddingMedium
                        horizontalAlignment: Label.AlignLeft

                        width: Theme.paddingSmall * 25
                        height: Theme.paddingSmall * 6

                        text: stationName
                        font.pixelSize: Theme.fontSizeLarge
                    }
                    Label {
                        id: difference
                        y: Theme.paddingMedium

                        //anchors.verticalCenter: parent.verticalCenter
                        anchors.leftMargin: Theme.paddingMedium
                        anchors.rightMargin: Theme.paddingMedium

                        width: parent.width - Theme.paddingLarge
                        height: Theme.paddingSmall * 6

                        horizontalAlignment: Text.AlignRight
                        text: differenceInMin
                        color: hasCause ? Theme.highlightColor : Theme.primaryColor
                        font.pixelSize: Theme.fontSizeLarge
                    }
                }

                onMovementEnded: {
                    if ( !metadataLock ) {
                        metadataVisible = (myListView.contentY > 80 ? false : true)
                    }
                }

                Connections {
                    target: myListView.model
                    onDataChanged: {
                        myListView.update()
                    }
                }


            }
    }
}


