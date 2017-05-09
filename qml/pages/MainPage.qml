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
import harbour.pendot.junat 1.0
import harbour.pendot.timetablemodel 1.0



Page {
    id: page

    property bool metadataVisible: true

    Component.onCompleted: {
        dTimer.start()
    }

    Timer {
        id: dTimer
        interval: 1000
        repeat: false
        running: false
        onTriggered: pageStack.push(Qt.resolvedUrl("SecondPage.qml"))
    }

    // To enable PullDownMenu, place our content in a SilicaFlickable
    SilicaFlickable {
        anchors.fill: parent

        // PullDownMenu and PushUpMenu must be declared in SilicaFlickable, SilicaListView or SilicaGridView
        PullDownMenu {
            MenuItem {
                text: qsTr(metadataVisible ? "Hide Metadata" : "Show Metadata")
                onClicked: {
                    metadataVisible = !metadataVisible
                }
            }

            MenuItem {
                text: qsTr("Search")
                onClicked: pageStack.push(Qt.resolvedUrl("SecondPage.qml"))
            }
        }

        // Tell SilicaFlickable the height of its content.
        //contentHeight: column.height

        // Place our content in a Column.  The PageHeader is always placed at the top
        // of the page, followed by our content.
        /*Column {
            id: column

            width: page.width
            spacing: Theme.paddingLarge*/
            PageHeader {
                anchors.top: parent.top
                id: pageHeader
                title: jna.getTrainType + jna.trainNr
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
                    text: qsTr("Source:")
                }
                Label { text: jna.getTrainReadySource }
                Label {
                    color: Theme.highlightColor
                    text: qsTr("Timestamp:")
                }
                Label { text: jna.getTrainReadyTime }
            }

            Label {
                anchors.top: metadataVisible ? dataGrid.bottom : pageHeader.bottom
                id: textTimeTable
                x: Theme.paddingLarge
                anchors.topMargin: metadataVisible ? Theme.paddingMedium : 0
                text: qsTr("Timetable:")
                color: Theme.highlightColor
            }

            SilicaListView {
                anchors.top: textTimeTable.bottom
                width: parent.width
                height: metadataVisible ? parent.height - pageHeader.height - dataGrid.height- textTimeTable.height : parent.height - pageHeader.height - textTimeTable.height
                //height: Theme.paddingMedium *4 *2 * myModel.rowCount()

                // this is needed so that the list elements
                // do not overlap with other controls in column
                clip: true

                id: listView
                //x: Theme.paddingLarge
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
                        opacity: 0.05
                        visible: !(index & 1)
                    }

                    Label {
                        id: ifStop
                        x: Theme.paddingLarge
                        y: Theme.paddingMedium
                        //anchors.verticalCenter: parent.verticalCenter
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
                        //anchors.verticalCenter: differenceInMin == 0 ? parent.verticalCenter : parent.Top
                        horizontalAlignment: Label.AlignHCenter
                        verticalAlignment: Label.AlignHCenter

                        width: Theme.paddingSmall * 15
                        height: Theme.paddingSmall * 6

                        //text: scheduledTime
                        text: differenceInMin == 0 ? scheduledTime : actualTime != "" ? actualTime : estimateTime
                        //font.pixelSize: differenceInMin == 0 ? Theme.fontSizeLarge : Theme.fontSizeLarge / 4 * 3
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
                        //font.pixelSize: Theme.fontSizeSmall / 4
                        //text: actualTime != "" ? actualTime : estimateTime
                        text: differenceInMin == 0 ? "" : scheduledTime
                        font.pixelSize: differenceInMin == 0 ? Theme.fontSizeLarge/2 : Theme.fontSizeLarge / 2
                        font.strikeout: true
                        visible: true//differenceInMin == 0 ? false : true
                    }
                    Label {
                        id: station
                        x: Theme.paddingLarge
                        y: Theme.paddingMedium

                        anchors.left: timeMain.right
                        anchors.leftMargin: Theme.paddingMedium
                        anchors.rightMargin: Theme.paddingMedium
                        horizontalAlignment: Label.AlignLeft
                        //anchors.verticalCenter: parent.verticalCenter

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
                        font.pixelSize: Theme.fontSizeLarge
                    }

                    //text: fillerChar + " " + stationName + " " + scheduledTime + " " + differenceInMin
                }
                //Label { text: actualTime + " " + differenceInMin }

                Connections {
                    target: listView.model
                    onDataChanged: {
                        listView.update()
                        console.log("Data Changed");
                    }
                }


            }
        //}
    }
}


