import QtQuick 2.0
import Sailfish.Silica 1.0

CoverBackground {
    Column {
        id: label
        anchors.centerIn: parent
        Label {
            text: qsTr("Train Ready:")
            color: Theme.highlightColor
        }
        Label {
            horizontalAlignment: Text.AlignRight
            text: jna.getTrainReadyAccepted ? "True" : "False"
        }
        Label {
            horizontalAlignment: Text.AlignLeft
            color: Theme.highlightColor
            text: qsTr("Source:")
        }
        Label {
            horizontalAlignment: Text.AlignRight
            text: jna.getTrainReadySource
        }
        Label {
            color: Theme.highlightColor
            text: qsTr("Timestamp:")
        }
        Label {
            text: jna.getTrainReadyTime
            horizontalAlignment: Text.AlignRight
        }
    }

    CoverActionList {
        id: coverAction

        CoverAction {
            iconSource: "image://theme/icon-cover-next"
        }

        CoverAction {
            iconSource: "image://theme/icon-cover-pause"
        }
    }
}


