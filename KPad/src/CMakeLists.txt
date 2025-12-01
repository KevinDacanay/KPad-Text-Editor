cmake_minimum_required(VERSION 3.16)
project(KPad VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# --- Windows app icon resource (.rc) ---
if(WIN32)
    set(APP_ICON_RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/app_icon.rc")
endif()

# ============================================
# KPAD RICH TEXT EDITOR (Main Application)
# ============================================
set(KPAD_SOURCES
    main.cpp
    kpad.cpp
    kpad.h
    kpad.ui
    kpad_file.cpp
    kpad_edit.cpp
    kpad_view.cpp
    kpad_statusbar.cpp
    kpad_keyevents.cpp
    kpad_misc.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(kpad
        MANUAL_FINALIZATION
        ${KPAD_SOURCES}
        ${APP_ICON_RESOURCE}
    )
else()
    if(ANDROID)
        add_library(kpad SHARED ${KPAD_SOURCES})
    else()
        add_executable(kpad
            ${KPAD_SOURCES}
            ${APP_ICON_RESOURCE}
        )
    endif()
endif()

target_link_libraries(kpad PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(kpad PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# ============================================
# INSTALLATION
# ============================================
include(GNUInstallDirs)

install(TARGETS kpad
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Finalize executables for Qt6
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(kpad)
endif()
