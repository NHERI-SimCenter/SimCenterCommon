#-------------------------------------------------
#
# Project created by fmk
#
#-------------------------------------------------

#message($$_PRO_FILE_PWD_)
#message($$PWD)

INCLUDEPATH += $$PWD/EVENTS/earthquake
INCLUDEPATH += $$PWD/EVENTS/earthquake/StochasticMotionInput/include

#INCLUDEPATH += "../SimCenterCommon/Workflow"

SOURCES += $$PWD/UQ/DakotaResults.cpp \
    $$PWD/EVENTS/earthquake/EarthquakeEventSelection.cpp \
    $$PWD/EVENTS/earthquake/ExistingPEER_Events.cpp \
    $$PWD/EVENTS/earthquake/SHAMotionWidget.cpp \
    $$PWD/EVENTS/earthquake/StochasticMotionInput/src/StochasticMotionInputWidget.cpp \
    $$PWD/EVENTS/earthquake/StochasticMotionInput/src/StochasticModelWidget.cpp \
    $$PWD/EVENTS/earthquake/StochasticMotionInput/src/VlachosEtAlModel.cpp \
    $$PWD/EVENTS/earthquake/StochasticMotionInput/src/DabaghiDerKiureghianPulse.cpp \
    $$PWD/EVENTS/earthquake/peerNGA/PeerLoginDialog.cpp \
    $$PWD/EVENTS/earthquake/peerNGA/PeerNgaRecordsWidget.cpp \
    $$PWD/EVENTS/earthquake/peerNGA/RecordSelectionPlot.cpp \
    $$PWD/EVENTS/earthquake/peerNGA/PeerNgaWest2Client.cpp \
    $$PWD/EDP/EDP_EarthquakeSelection.cpp \
    $$PWD/EDP/StandardEarthquakeEDP.cpp

HEADERS += $$PWD/UQ/DakotaResults.h \
    $$PWD/EVENTS/earthquake/EarthquakeEventSelection.h \
    $$PWD/EVENTS/earthquake/ExistingPEER_Events.h \
    $$PWD/EVENTS/earthquake/SHAMotionWidget.h \
    $$PWD/EVENTS/earthquake/StochasticMotionInput/include/StochasticMotionInputWidget.h \
    $$PWD/EVENTS/earthquake/StochasticMotionInput/include/StochasticModelWidget.h \
    $$PWD/EVENTS/earthquake/StochasticMotionInput/include/VlachosEtAlModel.h \
    $$PWD/EVENTS/earthquake/StochasticMotionInput/include/DabaghiDerKiureghianPulse.h \
    $$PWD/EVENTS/earthquake/peerNGA/PeerLoginDialog.h \
    $$PWD/EVENTS/earthquake/peerNGA/PeerNgaRecordsWidget.h \
    $$PWD/EVENTS/earthquake/peerNGA/RecordSelectionPlot.h \
    $$PWD/EVENTS/earthquake/peerNGA/PeerNgaWest2Client.h \
    $$PWD/EDP/EDP_EarthquakeSelection.h \
    $$PWD/EDP/StandardEarthquakeEDP.h

