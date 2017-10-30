import QtQuick 2.0
import Sailfish.Silica 1.0
import harbour.pendot.junat 1.0
import harbour.pendot.stationhandler 1.0
import harbour.pendot.causehandler 1.0

Page {

    id: stationDetailsPage

    property int absoluteIndex: 0

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: detailsColumn.height + detailPageHeader.height

        VerticalScrollDecorator {}

        clip: true

        PageHeader {
            id: detailPageHeader
            title: qsTr("Details")
        }

        Column {
            id: detailsColumn
            anchors.top: detailPageHeader.bottom

            SectionHeader {
                width: stationDetailsPage.width - Theme.paddingLarge * 2
                text: "Station Information"
                font.pixelSize: Theme.fontSizeMedium
                verticalAlignment: Text.AlignHCenter
            }
            // Station information
            Grid {
                columns: 2
                //columnSpacing: (stationDetailsPage.width - Theme.paddingLarge) / 3
                rowSpacing: Theme.paddingSmall / 2
                x: Theme.paddingLarge
                Label {
                    color: Theme.highlightColor
                    text: qsTr("Name: ")
                    font.pixelSize: Theme.fontSizeMedium
                    verticalAlignment: Text.AlignHCenter
                }
                Label {
                    text: stn.getStationName(jna.getStationName(absoluteIndex))
                    font.pixelSize: Theme.fontSizeMedium
                    verticalAlignment: Text.AlignHCenter
                }
                Label {
                    color: Theme.highlightColor
                    text: qsTr("Short code: ")
                    font.pixelSize: Theme.fontSizeMedium
                    verticalAlignment: Text.AlignHCenter
                }
                Label {
                    text: jna.getStationName(absoluteIndex)
                    font.pixelSize: Theme.fontSizeMedium
                    verticalAlignment: Text.AlignHCenter
                }
                Label {
                    color: Theme.highlightColor
                    text: qsTr("UIC Code: ")
                    font.pixelSize: Theme.fontSizeMedium
                    verticalAlignment: Text.AlignHCenter
                }
                Label {
                    text: stn.getStationUICCode(jna.getStationName(absoluteIndex))
                    font.pixelSize: Theme.fontSizeMedium
                    verticalAlignment: Text.AlignHCenter
                }
                Label {
                    color: Theme.highlightColor
                    text: qsTr("Country: ")
                    font.pixelSize: Theme.fontSizeMedium
                    verticalAlignment: Text.AlignHCenter
                }
                Label {
                    text: stn.getStationCountry(jna.getStationName(absoluteIndex))
                    font.pixelSize: Theme.fontSizeMedium
                    verticalAlignment: Text.AlignHCenter
                }
                Label {
                    color: Theme.highlightColor
                    text: qsTr("Longitude: ")
                    font.pixelSize: Theme.fontSizeMedium
                    verticalAlignment: Text.AlignHCenter
                }
                Label {
                    text: stn.getStationLongitude(jna.getStationName(absoluteIndex))
                    font.pixelSize: Theme.fontSizeMedium
                    verticalAlignment: Text.AlignHCenter
                }
                Label {
                    color: Theme.highlightColor
                    text: qsTr("Latitude: ")
                    font.pixelSize: Theme.fontSizeMedium
                    verticalAlignment: Text.AlignHCenter
                }
                Label {
                    text: stn.getStationLatitude(jna.getStationName(absoluteIndex))
                    font.pixelSize: Theme.fontSizeMedium
                    verticalAlignment: Text.AlignHCenter
                }
            }


            SectionHeader {
                width: stationDetailsPage.width - Theme.paddingLarge * 2
                text: "Cause Codes"
            }
            Label {
                x: Theme.paddingLarge
                color: Theme.highlightColor
                text: qsTr("No Causes")
                font.pixelSize: Theme.fontSizeMedium
                verticalAlignment: Text.AlignHCenter
                visible: !jna.stopHasCause(absoluteIndex)
            }

            /***************************
         *     Cause Category      *
         ***************************/

            Column {
                // Cause Code
                x: Theme.paddingLarge
                visible: jna.stopHasCause(absoluteIndex)
                Label {
                    //x: Theme.paddingLarge
                    color: Theme.highlightColor
                    text: qsTr("Cause Codes")
                    visible: jna.getStopCauseCode(absoluteIndex) != "NULL"
                    font.pixelSize: Theme.fontSizeMedium
                    verticalAlignment: Text.AlignHCenter
                }
                Grid {
                    columns: 2
                    //columnSpacing: (stationDetailsPage.width - Theme.paddingLarge) / 3
                    rowSpacing: Theme.paddingSmall / 2
                    visible: jna.getStopCauseCode(absoluteIndex) != "NULL"
                    x: Theme.paddingLarge// * 2
                    Label {
                        color: Theme.highlightColor
                        text: qsTr("Code: ")
                        font.pixelSize: Theme.fontSizeMedium
                        verticalAlignment: Text.AlignHCenter
                    }
                    Label {
                        text: jna.getStopCauseCode(absoluteIndex)
                        wrapMode: Text.Wrap
                        font.pixelSize: Theme.fontSizeMedium
                        verticalAlignment: Text.AlignHCenter
                    }
                    Label {
                        color: Theme.highlightColor
                        text: qsTr("Name: ")
                        font.pixelSize: Theme.fontSizeMedium
                        verticalAlignment: Text.AlignHCenter
                    }
                    Label {
                        color: Theme.primaryColor
                        text: causeHandler.getName(jna.getStopCauseCode(absoluteIndex))
                        wrapMode: Text.Wrap
                        font.pixelSize: Theme.fontSizeMedium
                        verticalAlignment: Text.AlignHCenter
                    }
                }

                Label {
                    color: Theme.highlightColor
                    //x: Theme.paddingLarge
                    text: qsTr("Passenger Term: ")
                    font.pixelSize: Theme.fontSizeMedium
                    verticalAlignment: Text.AlignHCenter
                    visible: causeHandler.getPassengerTerm(jna.getStopCauseCode(absoluteIndex), "fi") != "NULL"
                }
                Grid {
                    columns: 2
                    //columnSpacing: (stationDetailsPage.width - Theme.paddingLarge) / 3
                    rowSpacing: Theme.paddingSmall / 2
                    visible: causeHandler.getPassengerTerm(jna.getStopCauseCode(absoluteIndex), "fi") != "NULL"
                    x: Theme.paddingLarge// * 2
                    Label {
                        color: Theme.highlightColor
                        text: qsTr("Fi: ")
                        font.pixelSize: Theme.fontSizeMedium
                        verticalAlignment: Text.AlignHCenter
                    }
                    Label {
                        text: causeHandler.getPassengerTerm(jna.getStopCauseCode(absoluteIndex), "fi")
                        font.pixelSize: Theme.fontSizeMedium
                        verticalAlignment: Text.AlignHCenter
                    }
                    Label {
                        color: Theme.highlightColor
                        text: qsTr("En: ")
                        font.pixelSize: Theme.fontSizeMedium
                        verticalAlignment: Text.AlignHCenter
                    }
                    Label {
                        text: causeHandler.getPassengerTerm(jna.getStopCauseCode(absoluteIndex), "en")
                        font.pixelSize: Theme.fontSizeMedium
                        verticalAlignment: Text.AlignHCenter
                    }
                    Label {
                        color: Theme.highlightColor
                        text: qsTr("Se: ")
                        font.pixelSize: Theme.fontSizeMedium
                        verticalAlignment: Text.AlignHCenter
                    }
                    Label {
                        text: causeHandler.getPassengerTerm(jna.getStopCauseCode(absoluteIndex), "sv")
                        font.pixelSize: Theme.fontSizeMedium
                        verticalAlignment: Text.AlignHCenter
                    }
                }

            /***************************
             * Detailed Cause Category *
             ***************************/

                // Additional spacing
                Rectangle {
                    height: Theme.paddingLarge
                    width: parent.width
                    color: Theme.primaryColor
                    opacity: 0.01
                }

                Label {
                    //x: Theme.paddingLarge
                    color: Theme.highlightColor
                    text: qsTr("Detailed Cause Code")
                    visible: jna.getStopDetailedCauseCode(absoluteIndex) != "NULL"
                    font.pixelSize: Theme.fontSizeMedium
                    verticalAlignment: Text.AlignHCenter
                }
                Grid {
                    columns: 2
                    //columnSpacing: (stationDetailsPage.width - Theme.paddingLarge) / 3
                    rowSpacing: Theme.paddingSmall / 2
                    visible: jna.getStopDetailedCauseCode(absoluteIndex) != "NULL"
                    x: Theme.paddingLarge// * 2
                    Label {
                        color: Theme.highlightColor
                        text: qsTr("Code: ")
                        font.pixelSize: Theme.fontSizeMedium
                        verticalAlignment: Text.AlignHCenter
                    }
                    Text {
                        width: stationDetailsPage.width - detailedName.width - Theme.paddingLarge * 2
                        color: Theme.primaryColor
                        wrapMode: Text.Wrap
                        text: jna.getStopDetailedCauseCode(absoluteIndex)
                        font.pixelSize: Theme.fontSizeMedium
                        verticalAlignment: Text.AlignHCenter
                    }
                    Label {
                        id: detailedName
                        color: Theme.highlightColor
                        text: qsTr("Name: ")
                        font.pixelSize: Theme.fontSizeMedium
                        verticalAlignment: Text.AlignHCenter
                    }
                    Text {
                        width: stationDetailsPage.width - detailedName.width - Theme.paddingLarge * 2
                        color: Theme.primaryColor
                        wrapMode: Text.Wrap
                        text: causeHandler.getName(jna.getStopDetailedCauseCode(absoluteIndex))
                        font.pixelSize: Theme.fontSizeMedium
                        verticalAlignment: Text.AlignHCenter
                    }
                }

                Label {
                    //x: Theme.paddingLarge
                    color: Theme.highlightColor
                    text: qsTr("Passenger Term: ")
                    visible: causeHandler.getPassengerTerm(jna.getStopDetailedCauseCode(absoluteIndex), "fi") != "NULL"
                    font.pixelSize: Theme.fontSizeMedium
                    verticalAlignment: Text.AlignHCenter
                }
                Grid {
                    columns: 2
                    //columnSpacing: (stationDetailsPage.width - Theme.paddingLarge) / 3
                    rowSpacing: Theme.paddingSmall / 2
                    visible: causeHandler.getPassengerTerm(jna.getStopDetailedCauseCode(absoluteIndex), "fi") != "NULL"
                    x: Theme.paddingLarge// * 2
                    Label {
                        color: Theme.highlightColor
                        text: qsTr("Fi: ")
                        font.pixelSize: Theme.fontSizeMedium
                        verticalAlignment: Text.AlignHCenter
                    }
                    Text {
                        width: stationDetailsPage.width - detailedName.width - Theme.paddingLarge * 2
                        color: Theme.primaryColor
                        wrapMode: Text.Wrap
                        text: causeHandler.getPassengerTerm(jna.getStopDetailedCauseCode(absoluteIndex), "fi")
                        font.pixelSize: Theme.fontSizeMedium
                        verticalAlignment: Text.AlignHCenter
                    }
                    Label {
                        color: Theme.highlightColor
                        text: qsTr("En: ")
                        font.pixelSize: Theme.fontSizeMedium
                        verticalAlignment: Text.AlignHCenter
                    }
                    Text {
                        width: stationDetailsPage.width - detailedName.width - Theme.paddingLarge * 2
                        color: Theme.primaryColor
                        wrapMode: Text.Wrap
                        text: causeHandler.getPassengerTerm(jna.getStopDetailedCauseCode(absoluteIndex), "en")
                        font.pixelSize: Theme.fontSizeMedium
                        verticalAlignment: Text.AlignHCenter
                    }
                    Label {
                        color: Theme.highlightColor
                        text: qsTr("Se: ")
                        font.pixelSize: Theme.fontSizeMedium
                        verticalAlignment: Text.AlignHCenter
                    }
                    Text {
                        width: stationDetailsPage.width - detailedName.width - Theme.paddingLarge * 2
                        color: Theme.primaryColor
                        wrapMode: Text.Wrap
                        text: causeHandler.getPassengerTerm(jna.getStopDetailedCauseCode(absoluteIndex), "sv")
                        font.pixelSize: Theme.fontSizeMedium
                        verticalAlignment: Text.AlignHCenter
                    }
                }

                /***************************
             * Third Cause Category *
             ***************************/

                // Additional spacing
                Rectangle {
                    height: Theme.paddingLarge
                    opacity: 0
                }

                Label {
                    //x: Theme.paddingLarge
                    color: Theme.highlightColor
                    text: qsTr("Third Cause Code")
                    visible: jna.getStopThirdCauseCode(absoluteIndex) != "NULL"
                    font.pixelSize: Theme.fontSizeMedium
                    verticalAlignment: Text.AlignHCenter
                }

                Grid {
                    columns: 2
                    width: parent.width
                    //columnSpacing: (stationDetailsPage.width - Theme.paddingLarge) / 3
                    rowSpacing: Theme.paddingSmall / 2
                    visible: jna.getStopThirdCauseCode(absoluteIndex) != "NULL"
                    x: Theme.paddingLarge// * 2
                    Label {
                        color: Theme.highlightColor
                        text: qsTr("Code: ")
                        font.pixelSize: Theme.fontSizeMedium
                        verticalAlignment: Text.AlignHCenter
                    }
                    Text {
                        width: stationDetailsPage.width - thirdName.width - Theme.paddingLarge * 2
                        color: Theme.primaryColor
                        wrapMode: Text.Wrap
                        text: jna.getStopThirdCauseCode(absoluteIndex)
                        font.pixelSize: Theme.fontSizeMedium
                        verticalAlignment: Text.AlignHCenter
                    }
                    Label {
                        id: thirdName
                        color: Theme.highlightColor
                        text: qsTr("Name: ")
                        font.pixelSize: Theme.fontSizeMedium
                        verticalAlignment: Text.AlignHCenter
                    }
                    Text {
                        width: stationDetailsPage.width - thirdName.width - Theme.paddingLarge * 2
                        color: Theme.primaryColor
                        wrapMode: Text.Wrap
                        text: causeHandler.getName(jna.getStopThirdCauseCode(absoluteIndex))
                        font.pixelSize: Theme.fontSizeMedium
                        verticalAlignment: Text.AlignHCenter
                    }
//                    Label {
//                        id: thirdDescr
//                        color: Theme.highlightColor
//                        text: qsTr("Description: ")
//                        visible: causeHandler.getDescription(jna.getStopThirdCauseCode(absoluteIndex)) != "NULL"
//                    }
//                    Text {
//                        width: stationDetailsPage.width - thirdDescr.width - Theme.paddingLarge * 2
//                        color: Theme.primaryColor
//                        wrapMode: Text.Wrap
//                        text: causeHandler.getDescription(jna.getStopThirdCauseCode(absoluteIndex))
//                        visible: causeHandler.getDescription(jna.getStopThirdCauseCode(absoluteIndex)) != "NULL"
//                    }
                }

                Label {
                    id: thirdDescr
                    x: Theme.paddingLarge
                    color: Theme.highlightColor
                    text: qsTr("Description: ")
                    visible: (jna.getStopThirdCauseCode(absoluteIndex) != "NULL") && (causeHandler.getDescription(jna.getStopThirdCauseCode(absoluteIndex)) != "NULL")
                    font.pixelSize: Theme.fontSizeMedium
                    verticalAlignment: Text.AlignHCenter
                }
                Text {
                    x: Theme.paddingLarge * 2
                    width: parent.width - Theme.paddingLarge * 3
                    color: Theme.primaryColor
                    wrapMode: Text.Wrap
                    text: causeHandler.getDescription(jna.getStopThirdCauseCode(absoluteIndex))
                    visible: (jna.getStopThirdCauseCode(absoluteIndex) != "NULL") && (causeHandler.getDescription(jna.getStopThirdCauseCode(absoluteIndex)) != "NULL")
                    font.pixelSize: Theme.fontSizeMedium
                    verticalAlignment: Text.AlignHCenter
                }

                Label {
                    //x: Theme.paddingLarge
                    color: Theme.highlightColor
                    text: qsTr("Passenger Term: ")
                    visible: causeHandler.getPassengerTerm(jna.getStopThirdCauseCode(absoluteIndex), "fi") != "NULL"
                    font.pixelSize: Theme.fontSizeMedium
                    verticalAlignment: Text.AlignHCenter
                }
                Grid {
                    columns: 2
                    //columnSpacing: (stationDetailsPage.width - Theme.paddingLarge) / 3
                    rowSpacing: Theme.paddingSmall / 2
                    visible: causeHandler.getPassengerTerm(jna.getStopThirdCauseCode(absoluteIndex), "fi") != "NULL"
                    x: Theme.paddingLarge// * 2
                    Label {
                        color: Theme.highlightColor
                        text: qsTr("Fi: ")
                        font.pixelSize: Theme.fontSizeMedium
                        verticalAlignment: Text.AlignHCenter
                    }
                    Text {
                        width: stationDetailsPage.width - thirdName.width - Theme.paddingLarge * 2
                        color: Theme.primaryColor
                        wrapMode: Text.Wrap
                        text: causeHandler.getPassengerTerm(jna.getStopThirdCauseCode(absoluteIndex), "fi")
                        font.pixelSize: Theme.fontSizeMedium
                        verticalAlignment: Text.AlignHCenter
                    }
                    Label {
                        color: Theme.highlightColor
                        text: qsTr("En: ")
                        font.pixelSize: Theme.fontSizeMedium
                        verticalAlignment: Text.AlignHCenter
                    }
                    Text {
                        width: stationDetailsPage.width - thirdName.width - Theme.paddingLarge * 2
                        color: Theme.primaryColor
                        wrapMode: Text.Wrap
                        text: causeHandler.getPassengerTerm(jna.getStopThirdCauseCode(absoluteIndex), "en")
                        font.pixelSize: Theme.fontSizeMedium
                        verticalAlignment: Text.AlignHCenter
                    }
                    Label {
                        color: Theme.highlightColor
                        text: qsTr("Se: ")
                        font.pixelSize: Theme.fontSizeMedium
                        verticalAlignment: Text.AlignHCenter
                    }
                    Text {
                        width: stationDetailsPage.width - thirdName.width - Theme.paddingLarge * 2
                        color: Theme.primaryColor
                        wrapMode: Text.Wrap
                        text: causeHandler.getPassengerTerm(jna.getStopThirdCauseCode(absoluteIndex), "sv")
                        font.pixelSize: Theme.fontSizeMedium
                        verticalAlignment: Text.AlignHCenter
                    }
                }

            }
        }
    }
}
