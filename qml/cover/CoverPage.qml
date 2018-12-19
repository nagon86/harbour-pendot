import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.pendot.junat 1.0
import harbour.pendot.stationhandler 1.0

CoverBackground {

    property int currentStationIndex: jna.getCurrentStation
    property int pageMode: 0

    Column {
        id: label

        anchors {
            fill: parent
            centerIn: parent
            margins: Theme.paddingMedium
        }

        Image {
            anchors.horizontalCenter: parent.horizontalCenter
            //horizontalAlignment: Image.AlignHCenter
            source: "../images/Pendot.png"
            sourceSize.height: 128
        }
        Rectangle {
            width: parent.width
            height: Theme.paddingSmall
            opacity: 0.00
        }
        Label {
            width: parent.width
            text: qsTr("Train Ready:")
            color: Theme.highlightColor
            visible: pageMode > 0 ? false : true
        }
        Label {
            width: parent.width
            horizontalAlignment: Text.AlignRight
            text: jna.getTrainReadyAccepted ? "True" : "False"
            visible: pageMode > 0 ? false : true
        }
        Label {
            width: parent.width
            horizontalAlignment: Text.AlignLeft
            color: Theme.highlightColor
            text: qsTr("Source:")
            visible: pageMode > 0 ? false : true
        }
        Label {
            width: parent.width
            horizontalAlignment: Text.AlignRight
            text: jna.getTrainReadySource
            visible: pageMode > 0 ? false : true
        }
        Label {
            width: parent.width
            color: Theme.highlightColor
            text: qsTr("Timestamp:")
            visible: pageMode > 0 ? false : true
        }
        Label {
            width: parent.width
            text: jna.getTrainReadyTime
            horizontalAlignment: Text.AlignRight
            visible: pageMode > 0 ? false : true
        }
        Label {
            width: parent.width
            horizontalAlignment: Label.AlignLeft
            color: Theme.highlightColor
            text: qsTr("Updated:")
            visible: pageMode > 0 ? true : false
        }
        Label {
            width: parent.width
            horizontalAlignment: Label.AlignRight
            text: jna.getLastRefreshTime
            clip: true
            visible: pageMode > 0 ? true : false
        }
        Label {
            width: parent.width
            horizontalAlignment: Label.AlignLeft
            color: Theme.highlightColor
            text: qsTr("Last Station:")
            visible: pageMode > 0 ? true : false
        }
        Label {
            width: parent.width
            horizontalAlignment: Label.AlignRight
            text: stn.getStationName(jna.getStationName(currentStationIndex))
            clip: true
            visible: pageMode > 0 ? true : false
        }
        Label {
            width: parent.width
            horizontalAlignment: Label.AlignLeft
            color: Theme.highlightColor
            font.pixelSize: Theme.fontSizeMedium
            text: qsTr("Diff in Minutes:")
            visible: pageMode > 0 ? true : false
        }
        Label {
            width: parent.width
            horizontalAlignment: Label.AlignRight
            text: jna.getStationMinutes(currentStationIndex)
            font.pixelSize: Theme.fontSizeMedium
            visible: pageMode > 0 ? true : false
        }
    }

    CoverActionList {
        id: coverAction

        CoverAction {
            iconSource: "image://theme/icon-cover-next"
            onTriggered: {
                pageMode > 0 ? pageMode = 0 : pageMode = 1
                //console.log("Next Button: " + pageMode)
            }
        }

        CoverAction {
            iconSource: "image://theme/icon-cover-refresh"
            onTriggered: jna.refreshJunat()
        }
    }
}


