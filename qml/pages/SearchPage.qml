import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.pendot.junat 1.0
import harbour.pendot.stationhandler 1.0
import harbour.pendot.causehandler 1.0

Dialog {
    id: settingsDialog

    property int timerInterval
    property int tmp

    onAccepted: {
        if (tmp = trainNrField.text*1) {
            tmp = trainNrField.text*1
        }
        else {
            //Notify for non integer
            console.log("Not a number.")
            return
        }

        if (tmp >= 1 && tmp < 100000) {
            refreshTimer.stop()
            jna.trainNr = tmp.toString()
            refreshTimer.interval = refreshInterval.value * 60 * 1000
            refreshTimer.start() // Restarts the timer
        }
        else {
            console.log("Error with the train number, should not happen.")
        }
    }

    Column {
        id: optionsContainer
        anchors.fill: parent

        DialogHeader {
            id: dialogHeader
            title: qsTr("Search Page")
        }

        TextField {
            id: trainNrField
            width: parent.width
            placeholderText: "Insert Train Number"
            label: "Train Number"
            //canPaste: false
            inputMethodHints: Qt.ImhDigitsOnly
            onFocusChanged:  {
                if (tmp = trainNrField.text*1) {
                    tmp = trainNrField.text*1
                    trainNrField.text = tmp
                }
                else {
                    //trainNrField.text = "Please insert integer"
                    trainNrField.text = "49"
                }
            }
        }

        Slider {
            width: parent.width
            id: refreshInterval
            minimumValue: 1
            maximumValue: 15
            value: 5
            stepSize: 1
            valueText: value + " min"
            label: "Refresh interval"
        }

        SectionHeader {
            text: "Force Refresh"
        }

        Button {
            width: parent.width - Theme.paddingLarge*2
            x: Theme.paddingLarge
            id: refreshStations
            text: "Refresh Satations"
            onClicked: {
                stn.forceRefresh()
            }
        }

        Rectangle {
            width: parent.width
            height: Theme.paddingMedium
            opacity: 0.00
        }

        Button {
            width: parent.width - Theme.paddingLarge*2
            x: Theme.paddingLarge
            id: refreshCauses
            text: "Refresh Causes"
            onClicked: {
                causeHandler.forceRefresh()
            }
        }
    }
}





