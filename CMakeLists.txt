#
# Configure what to build
#
set(BUILD_WEB_APP 0)   # the web application
set(BUILD_TESTS 0)     # unit tests
set(BUILD_IMPORTER 1)  # an importer for the old database content

#
# Basic settings
#
project(RankingApp)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)

#
# My SqliteOverlay lib
#
set(SQLITE_OVERLAY_INCLUDE_DIR "../SqliteOverlay")
set(SQLITE_OVERLAY_LIB "SqliteOverlay")
if (CMAKE_BUILD_TYPE MATCHES Debug)
    set(SQLITE_OVERLAY_LIB_DIR "../../SqliteOverlay/debug")
else (CMAKE_BUILD_TYPE MATCHES Debug)
    set(SQLITE_OVERLAY_LIB_DIR "../../SqliteOverlay/release")
endif (CMAKE_BUILD_TYPE MATCHES Debug)


#
# Find Boost and GTest
#
find_package(GTest)
find_package(Boost COMPONENTS filesystem system log log_setup)
add_definitions(-DBOOST_LOG_DYN_LINK=1)  # linking fails if this is not set

#
# The WebToolkit (Wt)
#
set(WT_LIB "wt")
set(WT_CONNECTOR "wthttp")

#
# My lib of sloppyly implemented helper functions
#
set(Sloppy_HOME_DIR "../libSloppy")
set(Sloppy_INCLUDE_DIR ${Sloppy_HOME_DIR})
if (CMAKE_BUILD_TYPE MATCHES Debug)
  find_library(Sloppy_LIB NAME Sloppy HINTS "${Sloppy_HOME_DIR}/debug")
else (CMAKE_BUILD_TYPE MATCHES Debug)
  find_library(Sloppy_LIB NAME Sloppy HINTS "${Sloppy_HOME_DIR}/release")
endif (CMAKE_BUILD_TYPE MATCHES Debug)
include_directories(${Sloppy_INCLUDE_DIR})

#
# The web application
#
set(APP_SOURCES
    app/RankingApp.cpp
    main.cpp
    app/urls.h
    app/LazyContentLoader.cpp
    app/AllPlayersWidget.cpp
)

#
# The data import tool
#
set(IMPORTER_SOURCES
    importer/importer.cpp
)

#
# The UI-independent functions / logic
#
set(LIB_SOURCES
    RankingDataDefs.h
    RankingDb.cpp
    RankingSystem.cpp
    RankingErrCodes.h
    ConvenienceFuncs.h
    ConvenienceFuncs.cpp
    PlayerMngr.cpp
    Player.cpp
    ValidityPeriod.cpp
    ScoreQueue.cpp
    Score.cpp
    MatchMngr.cpp
    Match.cpp
)

#
# Main Targets
#
include_directories("." ${SQLITE_OVERLAY_INCLUDE_DIR} "./gsl")
link_directories("." ${SQLITE_OVERLAY_LIB_DIR})
add_library(${PROJECT_NAME}_Logic STATIC ${LIB_SOURCES})
set_property(TARGET ${PROJECT_NAME}_Logic PROPERTY CXX_STANDARD 14)
set_property(TARGET ${PROJECT_NAME}_Logic PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(${PROJECT_NAME}_Logic ${SQLITE_OVERLAY_LIB})

if (BUILD_WEB_APP)
    add_executable(${PROJECT_NAME} ${APP_SOURCES})
    set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
    set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
    target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_Logic ${WT_LIB} ${WT_CONNECTOR} ${Sloppy_LIB})
endif (BUILD_WEB_APP)

if (BUILD_IMPORTER AND Boost_FOUND)
    add_executable(${PROJECT_NAME}_importer ${IMPORTER_SOURCES})
    set_property(TARGET ${PROJECT_NAME}_importer PROPERTY CXX_STANDARD 14)
    set_property(TARGET ${PROJECT_NAME}_importer PROPERTY CXX_STANDARD_REQUIRED ON)
    target_link_libraries(${PROJECT_NAME}_importer ${PROJECT_NAME}_Logic ${Boost_LIBRARIES} ${SQLITE_OVERLAY_LIB} ${Sloppy_LIB})
endif (BUILD_IMPORTER AND Boost_FOUND)

#
# Unit tests
#
set(LIB_SOURCES_TST
    tests/BasicTestClass.cpp
    tests/tstConvenienceFuncs.cpp
    tests/tstRankingSys.cpp
    tests/tstPlayerMngr.cpp
    tests/tstValidityPeriod.cpp
    tests/tstScoreQueue.cpp
    tests/tstMatchMngr.cpp
)

if (GTEST_FOUND AND Boost_FOUND AND BUILD_TESTS)
    include_directories("." ${GTEST_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
    link_directories(".")

    add_executable(${PROJECT_NAME}_Tests ${LIB_SOURCES} ${LIB_SOURCES_TST})
    set_property(TARGET ${PROJECT_NAME}_Tests PROPERTY CXX_STANDARD 11)
    set_property(TARGET ${PROJECT_NAME}_Tests PROPERTY CXX_STANDARD_REQUIRED ON)

    target_link_libraries(${PROJECT_NAME}_Tests ${PROJECT_NAME}_Logic ${GTEST_BOTH_LIBRARIES} ${Boost_LIBRARIES} ${Sloppy_LIB})
endif (GTEST_FOUND AND Boost_FOUND)
