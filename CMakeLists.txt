#
# Configure what to build
#
set(BUILD_WEB_APP 0)   # the web application
set(BUILD_TESTS 1)     # unit tests

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
set(SQLITE_OVERLAY_LIB_DIR "../SqliteOverlay-build")
set(SQLITE_OVERLAY_LIB "SqliteOverlay")

#
# The WebToolkit (Wt)
#
set(WT_LIB "wt")
set(WT_CONNECTOR "wthttp")

#
# The web application
#
set(APP_SOURCES
    app/RankingApp.cpp
    main.cpp
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
)

#
# Main Targets
#
include_directories("." ${SQLITE_OVERLAY_INCLUDE_DIR})
link_directories("." ${SQLITE_OVERLAY_LIB_DIR})
add_library(${PROJECT_NAME}_Logic STATIC ${LIB_SOURCES})
set_property(TARGET ${PROJECT_NAME}_Logic PROPERTY CXX_STANDARD 11)
set_property(TARGET ${PROJECT_NAME}_Logic PROPERTY CXX_STANDARD_REQUIRED ON)
target_link_libraries(${PROJECT_NAME}_Logic ${SQLITE_OVERLAY_LIB})

if (BUILD_WEB_APP)
    add_executable(${PROJECT_NAME} ${APP_SOURCES})
    set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
    set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
    target_link_libraries(${PROJECT_NAME} ${PROJECT_NAME}_Logic ${WT_LIB} ${WT_CONNECTOR})
endif (BUILD_WEB_APP)

#
# Unit tests
#
set(LIB_SOURCES_TST
    tests/BasicTestClass.cpp
    tests/tstConvenienceFuncs.cpp
    tests/tstRankingSys.cpp
    tests/tstPlayerMngr.cpp
    tests/tstValidityPeriod.cpp
)

find_package(GTest)
find_package(Boost COMPONENTS filesystem system)

if (GTEST_FOUND AND Boost_FOUND AND BUILD_TESTS)
    include_directories("." ${GTEST_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
    link_directories(".")

    add_executable(${PROJECT_NAME}_Tests ${LIB_SOURCES} ${LIB_SOURCES_TST})
    set_property(TARGET ${PROJECT_NAME}_Tests PROPERTY CXX_STANDARD 11)
    set_property(TARGET ${PROJECT_NAME}_Tests PROPERTY CXX_STANDARD_REQUIRED ON)

    target_link_libraries(${PROJECT_NAME}_Tests ${PROJECT_NAME}_Logic ${GTEST_BOTH_LIBRARIES} ${Boost_LIBRARIES})
endif (GTEST_FOUND AND Boost_FOUND)
