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

    /*Junat {
        id: jna
        Component.onCompleted: {
            myModel.setPointer(jna.getPointer())
        }
    }
    TimeTableModel {
        id : myModel
    }*/

    // To enable PullDownMenu, place our content in a SilicaFlickable
    SilicaFlickable {
        anchors.fill: parent

        // PullDownMenu and PushUpMenu must be declared in SilicaFlickable, SilicaListView or SilicaGridView
        PullDownMenu {
            MenuItem {
                text: qsTr("Show Page 2")
                onClicked: pageStack.push(Qt.resolvedUrl("SecondPage.qml"))
            }
        }

        // Tell SilicaFlickable the height of its content.
        contentHeight: column.height

        // Place our content in a Column.  The PageHeader is always placed at the top
        // of the page, followed by our content.
        Column {
            id: column

            width: page.width
            spacing: Theme.paddingLarge
            PageHeader {
                title: jna.getTrainType + jna.trainNr
            }
            Grid {
                id: testGrid
                columns: 2
                columnSpacing: (page.width - 100) / 2
                rowSpacing: Theme.paddingSmall
                x: Theme.paddingLarge

                Label {
                    color: Theme.secondaryHighlightColor
                    text: qsTr("Date:")
                }
                Label {text: jna.getDepartureDate }
                Label {
                    color: Theme.secondaryHighlightColor
                    text: qsTr("Operator:")
                }
                Label {text: jna.getOperatorCode }
            }
            Label {
                x: Theme.paddingLarge
                text: qsTr("Timetable:")
                color: Theme.secondaryHighlightColor
                font.pixelSize: 0
            }
            TextField {
                id: testField
                width: 160
                placeholderText: qsTr("Text box")
                onClicked: {
                    testField.text = "";
                }
                onFocusChanged: {
                    jna.trainNr = testField.text
                    refreshTimer.start()
                }
            }

            SilicaListView {
                width: parent.width
                height: parent.height

                id: listView
                //x: Theme.paddingLarge
                model: myModel

                VerticalScrollDecorator {}

                delegate: ListItem {
                    id: timeTableItem
                    width: parent.width
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
                        width: Theme.paddingSmall * 5
                        anchors.verticalCenter: parent.verticalCenter
                        horizontalAlignment: Label.AlignHCenter
                        text: fillerChar
                        color: actualTime != "" ? Theme.highlightColor : Theme.primaryColor //"white"
                        font.pixelSize: Theme.fontSizeLarge
                        font.bold: true
                    }
                    Label {
                        anchors.left: ifStop.right
                        id: timeMain
                        x: Theme.paddingLarge
                        horizontalAlignment: Label.AlignHCenter
                        width: Theme.paddingSmall * 15
                        anchors.verticalCenter: differenceInMin == 0 ? parent.verticalCenter : parent.Top
                        //text: scheduledTime
                        text: differenceInMin == 0 ? scheduledTime : actualTime != "" ? actualTime : estimateTime
                        //font.pixelSize: differenceInMin == 0 ? Theme.fontSizeLarge : Theme.fontSizeLarge / 4 * 3
                        font.pixelSize: Theme.fontSizeLarge
                    }
                    Label {
                        anchors.top: timeMain.bottom
                        anchors.left: ifStop.right
                        id: timeSecondary
                        x: Theme.paddingLarge
                        horizontalAlignment: Label.AlignHCenter
                        width: timeMain.width
                        //font.pixelSize: Theme.fontSizeSmall / 4
                        //text: actualTime != "" ? actualTime : estimateTime
                        text: differenceInMin == 0 ? "" : scheduledTime
                        font.pixelSize: differenceInMin == 0 ? Theme.fontSizeLarge : Theme.fontSizeLarge / 2
                        font.strikeout: true
                        visible: differenceInMin == 0 ? false : true
                    }

                    Label {
                        anchors.left: timeMain.right
                        id: station
                        x: Theme.paddingLarge
                        horizontalAlignment: Label.AlignLeft
                        anchors.verticalCenter: parent.verticalCenter
                        width: Theme.paddingSmall * 25
                        text: stationName
                        font.pixelSize: Theme.fontSizeLarge
                    }
                    Label {
                        //anchors.left: station.right
                        //anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        id: difference
                        width: parent.width - Theme.paddingLarge
                        //width: Theme.paddingSmall * 8
                        //x: Theme.paddingLarge
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
        }
    }
}


