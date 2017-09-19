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
        contentHeight: detailsColumn.height

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
                }
                Label {
                    text: stn.getStationName(jna.getStationName(absoluteIndex))
                }
                Label {
                    color: Theme.highlightColor
                    text: qsTr("Short code: ")
                }
                Label {
                    text: jna.getStationName(absoluteIndex)
                }
                Label {
                    color: Theme.highlightColor
                    text: qsTr("UIC Code: ")
                }
                Label {
                    text: stn.getStationUICCode(jna.getStationName(absoluteIndex))
                }
                Label {
                    color: Theme.highlightColor
                    text: qsTr("Country: ")
                }
                Label {
                    text: stn.getStationCountry(jna.getStationName(absoluteIndex))
                }
                Label {
                    color: Theme.highlightColor
                    text: qsTr("Longitude: ")
                }
                Label {
                    text: stn.getStationLongitude(jna.getStationName(absoluteIndex))
                }
                Label {
                    color: Theme.highlightColor
                    text: qsTr("Latitude: ")
                }
                Label {
                    text: stn.getStationLatitude(jna.getStationName(absoluteIndex))
                }
            }


            SectionHeader {
                width: stationDetailsPage.width - Theme.paddingLarge * 2
                text: "Cause Codes"
            }
            Label {
                color: Theme.highlightColor
                text: qsTr("No Causes")
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
                }
                Grid {
                    columns: 2
                    //columnSpacing: (stationDetailsPage.width - Theme.paddingLarge) / 3
                    rowSpacing: Theme.paddingSmall / 2
                    visible: jna.getStopCauseCode(absoluteIndex) != "NULL"
                    x: Theme.paddingLarge// * 2
                    Label {
                        color: Theme.highlightColor
                        text: qsTr("Category Code: ")
                    }
                    Label {
                        text: jna.getStopCauseCode(absoluteIndex)
                    }
                    Label {
                        color: Theme.highlightColor
                        text: qsTr("Category Name: ")
                    }
                    Label {
                        text: causeHandler.getName(jna.getStopCauseCode(absoluteIndex))
                    }
                }

                Label {
                    color: Theme.highlightColor
                    //x: Theme.paddingLarge
                    text: qsTr("Passenger Term: ")
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
                    }
                    Label {
                        text: causeHandler.getPassengerTerm(jna.getStopCauseCode(absoluteIndex), "fi")
                    }
                    Label {
                        color: Theme.highlightColor
                        text: qsTr("En: ")
                    }
                    Label {
                        text: causeHandler.getPassengerTerm(jna.getStopCauseCode(absoluteIndex), "en")
                    }
                    Label {
                        color: Theme.highlightColor
                        text: qsTr("Se: ")
                    }
                    Label {
                        text: causeHandler.getPassengerTerm(jna.getStopCauseCode(absoluteIndex), "sv")
                    }
                }

                /***************************
             * Detailed Cause Category *
             ***************************/

                Label {
                    //x: Theme.paddingLarge
                    color: Theme.highlightColor
                    text: qsTr("Detailed Cause Code")
                    visible: jna.getStopDetailedCauseCode(absoluteIndex) != "NULL"
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
                    }
                    Text {
                        width: stationDetailsPage.width - detailedName.width - Theme.paddingLarge * 2
                        color: Theme.primaryColor
                        wrapMode: Text.Wrap
                        text: jna.getStopDetailedCauseCode(absoluteIndex)
                    }
                    Label {
                        id: detailedName
                        color: Theme.highlightColor
                        text: qsTr("Name: ")
                    }
                    Text {
                        width: stationDetailsPage.width - detailedName.width - Theme.paddingLarge * 2
                        color: Theme.primaryColor
                        wrapMode: Text.Wrap
                        text: causeHandler.getName(jna.getStopDetailedCauseCode(absoluteIndex))
                    }
                }

                Label {
                    //x: Theme.paddingLarge
                    color: Theme.highlightColor
                    text: qsTr("Passenger Term: ")
                    visible: causeHandler.getPassengerTerm(jna.getStopDetailedCauseCode(absoluteIndex), "fi") != "NULL"
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
                    }
                    Text {
                        width: stationDetailsPage.width - detailedName.width - Theme.paddingLarge * 2
                        color: Theme.primaryColor
                        wrapMode: Text.Wrap
                        text: causeHandler.getPassengerTerm(jna.getStopDetailedCauseCode(absoluteIndex), "fi")
                    }
                    Label {
                        color: Theme.highlightColor
                        text: qsTr("En: ")
                    }
                    Text {
                        width: stationDetailsPage.width - detailedName.width - Theme.paddingLarge * 2
                        color: Theme.primaryColor
                        wrapMode: Text.Wrap
                        text: causeHandler.getPassengerTerm(jna.getStopDetailedCauseCode(absoluteIndex), "en")
                    }
                    Label {
                        color: Theme.highlightColor
                        text: qsTr("Se: ")
                    }
                    Text {
                        width: stationDetailsPage.width - detailedName.width - Theme.paddingLarge * 2
                        color: Theme.primaryColor
                        wrapMode: Text.Wrap
                        text: causeHandler.getPassengerTerm(jna.getStopDetailedCauseCode(absoluteIndex), "sv")
                    }
                }

                /***************************
             * Detailed Cause Category *
             ***************************/

                Label {
                    //x: Theme.paddingLarge
                    color: Theme.highlightColor
                    text: qsTr("Third Cause Code")
                    visible: jna.getStopThirdCauseCode(absoluteIndex) != "NULL"
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
                    }
                    Text {
                        width: stationDetailsPage.width - thirdName.width - Theme.paddingLarge * 2
                        color: Theme.primaryColor
                        wrapMode: Text.Wrap
                        text: jna.getStopThirdCauseCode(absoluteIndex)
                    }
                    Label {
                        id: thirdName
                        color: Theme.highlightColor
                        text: qsTr("Name: ")
                    }
                    Text {
                        width: stationDetailsPage.width - thirdName.width - Theme.paddingLarge * 2
                        color: Theme.primaryColor
                        wrapMode: Text.Wrap
                        text: causeHandler.getName(jna.getStopThirdCauseCode(absoluteIndex))
                    }
                }

                Label {
                    //x: Theme.paddingLarge
                    color: Theme.highlightColor
                    text: qsTr("Passenger Term: ")
                    visible: causeHandler.getPassengerTerm(jna.getStopThirdCauseCode(absoluteIndex), "fi") != "NULL"
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
                    }
                    Text {
                        width: stationDetailsPage.width - thirdName.width - Theme.paddingLarge * 2
                        color: Theme.primaryColor
                        wrapMode: Text.Wrap
                        text: causeHandler.getPassengerTerm(jna.getStopThirdCauseCode(absoluteIndex), "fi")
                    }
                    Label {
                        color: Theme.highlightColor
                        text: qsTr("En: ")
                    }
                    Text {
                        width: stationDetailsPage.width - thirdName.width - Theme.paddingLarge * 2
                        color: Theme.primaryColor
                        wrapMode: Text.Wrap
                        text: causeHandler.getPassengerTerm(jna.getStopThirdCauseCode(absoluteIndex), "en")
                    }
                    Label {
                        color: Theme.highlightColor
                        text: qsTr("Se: ")
                    }
                    Text {
                        width: stationDetailsPage.width - thirdName.width - Theme.paddingLarge * 2
                        color: Theme.primaryColor
                        wrapMode: Text.Wrap
                        text: causeHandler.getPassengerTerm(jna.getStopThirdCauseCode(absoluteIndex), "sv")
                    }
                }

            }


            Label {
                text: "absoluteIndex"
            }
        }
    }
}
