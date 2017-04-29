# NOTICE:
#
# Application name defined in TARGET has a corresponding QML filename.
# If name defined in TARGET is changed, the following needs to be done
# to match new name:
#   - corresponding QML filename must be changed
#   - desktop icon filename must be changed
#   - desktop filename must be changed
#   - icon definition filename in desktop file must be changed
#   - translation filenames have to be changed

# The name of your application
TARGET = Pendot

CONFIG += sailfishapp

SOURCES += src/Pendot.cpp \
    src/junat.cpp \
    src/timetablemodel.cpp \
    src/stationhandler.cpp

OTHER_FILES += qml/Pendot.qml \
    qml/cover/CoverPage.qml \
    qml/pages/FirstPage.qml \
    qml/pages/SecondPage.qml \
    rpm/Pendot.changes.in \
    rpm/Pendot.spec \
    rpm/Pendot.yaml \
    translations/*.ts \
    Pendot.desktop

SAILFISHAPP_ICONS = 86x86 108x108 128x128 256x256

# to disable building translations every time, comment out the
# following CONFIG line
CONFIG += sailfishapp_i18n

# German translation is enabled as an example. If you aren't
# planning to localize your app, remember to comment out the
# following TRANSLATIONS line. And also do not forget to
# modify the localized app name in the the .desktop file.
TRANSLATIONS += translations/Pendot-de.ts

HEADERS += \
    src/junat.h \
    src/timetablemodel.h \
    src/stationhandler.h

DISTFILES += \
    qml/pages/MainPage.qml

