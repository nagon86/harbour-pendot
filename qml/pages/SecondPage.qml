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

Dialog {
    id: settingsDialog

    property int timerInterval
    property int tmp

    onAccepted: {
        console.log("Pages: " + pageStack.depth)
        if (tmp >= 1 && tmp < 100000) {
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
        width: parent.width

        DialogHeader {
            id: dialogHeader
            title: qsTr("Search Page")
        }

        TextField {
            id: trainNrField
            width: parent.width
            placeholderText: "Insert Train Number"
            label: "Train Number"
            onFocusChanged:  {
                if (tmp = trainNrField.text*1) {
                    tmp = trainNrField.text*1
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
            maximumValue: 60
            value: 5
            stepSize: 1
            valueText: value + " min"
            label: "Refresh interval"
        }
    }
}





