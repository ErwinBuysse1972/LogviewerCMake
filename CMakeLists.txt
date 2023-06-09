cmake_minimum_required(VERSION 3.5)

project(LogViewerCMake VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/Logic)

set(PROJECT_SOURCES
    Logic/cconversion.cpp
    Logic/cfunctracer.cpp
    Logic/clogfile.cpp
    Logic/ctracer.cpp
    Logic/cscopedtimer.cpp
    Logic/cconfigsettings.cpp
    Logic/clogentry.cpp
    logviewer.cpp
    logviewer.h
    inc/Logic/clogfile.h
    inc/Logic/cconversion.h
    inc/Logic/cconfigsettings.h
    inc/Logic/cfunctracer.h
    inc/Logic/cscopedtimer.h
    inc/Logic/ctracer.h
    inc/Logic/IDataObject.h
    inc/Logic/clogentry.h
    main.cpp
    QCheckableComboBox.h
    QLogFileWidget.cpp
    QLogFileWidget.h
    searchform.cpp
    searchform.h
    searchform.ui
    LogViewer_en_BE.ts
    logentryview.cpp
    logentryview.h
    logentryview.ui
    qfilelogfilemodel.cpp
    qfilelogfilemodel.h
    qcashfilemodel.cpp
    qcashfilemodel.h
    qfiletableview.cpp
    qfiletableview.h
    richtextdelegate.cpp
    richtextdelegate.h
)
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(LogViewer
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET LogViewer APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(LogViewer SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(LogViewer
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(LogViewer PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(LogViewer PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER www.wellnesssweetbonihu.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS LogViewer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(LogViewer)
endif()
