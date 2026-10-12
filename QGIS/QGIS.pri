QT += svg printsupport

#Change these to the appropriate folders
mac {

DEFINES += Q_WS_MAC HAVE_STATIC_PROVIDERS #WITH_QTWEBKIT

PATH_TO_QGIS_DEPS=$$PWD/mac/qgis-deps-0.9/stage

PATH_TO_QGIS_SRC=$$PWD/mac/src
PATH_TO_INSTALL=$$PWD/mac/Install


} else:linux {

DEFINES += HAVE_STATIC_PROVIDERS #WITH_QTWEBKIT

INCLUDEPATH += /usr/include/Qca-qt5/QtCrypto

PATH_TO_QGIS_DEPS=$$PWD/linux/deps
PATH_TO_QGIS_SRC=$$PWD/linux/src
PATH_TO_INSTALL=$$PWD/linux/install


}
else {

DEFINES += Q_WS_WIN APP_STATIC_DEFINE HAVE_STATIC_PROVIDERS QTSIGNAL_STATIC_DEFINE QWT_DLL # CORE_STATIC_DEFINE  GUI_STATIC_DEFINE ANALYSIS_STATIC_DEFINE NATIVE_STATIC_DEFINE _3D_STATIC_DEFINE

# Defines needed to compile with MSVC
DEFINES += NOMINMAX _IMAGEHLP_SOURCE_ _USE_MATH_DEFINES

PATH_TO_QGIS_DEPS=$$PWD\win\OSGeo4W
PATH_TO_QGIS_SRC=$$PWD\win\src
PATH_TO_INSTALL=$$PWD\win\Install

}


# Do not change
PATH_TO_QGIS_EXT=$$PWD/external

# Need this to prevent a compile error
DEFINES += TEST_DATA_DIR=\\\"$$PWD/tests/testdata\\\"

INCLUDEPATH += \
            $$PWD \

include(QGISSrc.pri)

SOURCES += \
        $$PWD/QgisApp.cpp \
        $$PWD/QGISVisualizationWidget.cpp \
        $$PWD/VisualizationWidget.cpp \
        $$PWD/GISLegendView.cpp \
        $$PWD/PopUpWidget.cpp \
        $$PWD/SimCenterMapcanvasWidget.cpp \
        $$PWD/GISSelectable.cpp \


HEADERS += \
        $$PWD/QgisApp.h \
        $$PWD/QGISVisualizationWidget.h \
        $$PWD/VisualizationWidget.h \
        $$PWD/GISLegendView.h \
        $$PWD/PopUpWidget.h \
        $$PWD/SimCenterMapcanvasWidget.h \
        $$PWD/GISSelectable.h \


