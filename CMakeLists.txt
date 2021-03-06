cmake_minimum_required(VERSION 3.10.2)

if (WIN32)
    project(Network_Chess_Client LANGUAGES CXX)
    project(Network_Chess_Server LANGUAGES CXX)
    project(Network_Chess_Test LANGUAGES CXX)
elseif(UNIX)
    project(Network_Chess_Client)
    project(Network_Chess_Server)
    project(Network_Chess_Test)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_CONFIGURATION_TYPES "Release;RelWithDebInfo" CACHE STRING "" FORCE)

#======================= INCLUSION OF Qt =======================#
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
SET(CMAKE_AUTORCC ON)
set(CMAKE_PREFIX_PATH $ENV{QTDIR})
find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Test REQUIRED)


#=================== INCLUSION OF Project Files ====================#
set(CLIENT_DIR "${CMAKE_SOURCE_DIR}/Client")
set(SERVER_DIR "${CMAKE_SOURCE_DIR}/Server")
set(RESOURCES_DIR "${CMAKE_SOURCE_DIR}/Resources")
set(TEST_DIR "${CMAKE_SOURCE_DIR}/Test")

include_directories(${CLIENT_DIR})
include_directories(${SERVER_DIR})
include_directories(${RESOURCES_DIR})

file(GLOB SOURCES
    Client/frontend/ChessMainWindow.cpp
    Client/frontend/ChessMainWindow.ui
    Client/frontend/LocalChessWidget.cpp
    Client/frontend/LocalChessWidget.ui
    Client/frontend/OnlineChessWidget.cpp
    Client/frontend/OnlineChessWidget.ui
    Client/frontend/ChessTableWidget.cpp
    Client/frontend/ChessTableWidget.ui
    Client/frontend/SwitchPawnDialog.cpp
    Client/frontend/SwitchPawnDialog.ui
    Client/frontend/HomePageWidget.cpp
    Client/frontend/HomePageWidget.ui
    Client/frontend/LoginWidget.cpp
    Client/frontend/LoginWidget.ui
    Client/frontend/utilities/HoverEventFilter.cpp
    Client/backend/ChessAPIService.cpp
    Client/common/ChessField.h
    Client/backend/ChessModel.cpp
    Client/backend/IChessModel.h
    Server/ChessServer.cpp
    Server/DatabaseHandler.cpp
    Resources/resources.qrc
    )

set(ignoreMe "${QT_QMAKE_EXECUTABLE}")

#=================== SETUP EXECTUABLE ====================#
# Add the forms directory to the AUTOUIC search paths
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_AUTOUIC_SEARCH_PATHS} ${CLIENT_DIR}/frontend)

# Add the executable
if (WIN32)
    add_executable(Network_Chess_Client WIN32 ${CLIENT_DIR}/main.cpp ${SOURCES})
    add_executable(Network_Chess_Server WIN32 ${SERVER_DIR}/main.cpp ${SOURCES})
    add_executable(Network_Chess_Test WIN32 ${TEST_DIR}/ChessModelTest.cpp ${SOURCES})
elseif(UNIX)
    add_executable(Network_Chess_Client ${CLIENT_DIR}/main.cpp ${SOURCES})
    add_executable(Network_Chess_Server ${SERVER_DIR}/main.cpp ${SOURCES})
    add_executable(Network_Chess_Test ${TEST_DIR}/ChessModelTest.cpp ${SOURCES})
endif()

# Add the target includes for Network_Chess
target_include_directories(Network_Chess_Client PUBLIC ${CLIENT_DIR})
target_include_directories(Network_Chess_Server PUBLIC ${SERVER_DIR})
target_include_directories(Network_Chess_Client PUBLIC ${RESOURCES_DIR})
target_include_directories(Network_Chess_Test PUBLIC ${TEST_DIR})

#===================== LINKING LIBRARIES =======================#
target_link_libraries(Network_Chess_Client Qt5::Widgets Qt5::Network Qt5::Sql)
target_link_libraries(Network_Chess_Server Qt5::Widgets Qt5::Network Qt5::Sql)
target_link_libraries(Network_Chess_Test Qt5::Widgets Qt5::Network Qt5::Sql Qt5::Test)

#===================== INSTALLING =======================#
install(TARGETS Network_Chess_Client DESTINATION bin COMPONENT binaries)
install(TARGETS Network_Chess_Server DESTINATION bin COMPONENT binaries)
