cmake_minimum_required(VERSION 3.16)

project(Habit_Tracker VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Add habitClass.cpp to your sources here so it's always compiled
set(PROJECT_SOURCES
    main.cpp
    habbit_tracker.cpp
    habbit_tracker.h
    habbit_tracker.ui
    habitClass.cpp
    habitClass.h

)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(Habit_Tracker
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(Habit_Tracker SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(Habit_Tracker
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(Habit_Tracker PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# iOS/macOS bundle stuff omitted for brevity; keep as you had

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(Habit_Tracker)
endif()
