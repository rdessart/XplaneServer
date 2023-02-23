cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 20)
set(PROJECT_TYP)

project(XplaneServer)


SET(SRC 
    Datarefs/Dataref.cpp
    Datarefs/DatarefManager.cpp
    Datarefs/FFA320Dataref.cpp
    Network/NetworkUtils.cpp
    Network/UDPBeacon.cpp
    Network/UDPServer.cpp
    Tools/Logger.cpp
    Tools/Utils.cpp)

SET(HDR
    Datarefs/AbstractDataref.h
    Datarefs/Dataref.h
    Datarefs/DatarefManager.h
    Datarefs/FFA320Dataref.h
    Network/IServer.h
    Network/Message.h
    Network/NetworkUtils.h
    Network/UDPBeacon.h
    Network/UDPServer.h
    Tools/Logger.h
    Tools/Utils.h
    SharedValue.h)

SET(ALL_SRC ${SRC} ${HRD})
LIST(SORT ALL_SRC)

include_directories(XplaneServer PUBLIC $ENV{xplane_sdk}/CHeaders)

add_library(XplaneServer ${ALL_SRC})

set(CMAKE_C_FLAGS -g -W -Wall -Wextra -Werror -fvisibility=hidden)
set(CMAKE_CXX_FLAGS -g -W -Wall -Wextra -Werror -fvisibility=hidden)


find_package(nlohmann_json CONFIG REQUIRED)
add_definitions(-DXPLM200=1 -DXPLM300=1 -DXPLM301=1)
if(APPLE)
    message("APPLE")
    add_definitions(-DAPL=1)
    set(CMAKE_C_FLAGS)
    set(CMAKE_CXX_FLAGS)
    find_library(XPLM_LIBRARY XPLM "${xplane_sdk}/Libraries/Mac")
    target_link_libraries(XplaneServer $ENV{XPLM_LIBRARY} nlohmann_json::nlohmann_json)
    set(OUTPUT_FILE_NAME "mac.xpl")
    set_target_properties(XplaneServer PROPERTIES LINK_FLAGS -fPIC -fvisibility=hidden -bundle)
elseif(LINUX)
    message("LINUX")
    add_definitions(-DLIN=1)
    set(OUTPUT_FILE_NAME "lin.xpl")
endif()


set_target_properties(XplaneServer PROPERTIES PREFIX "")
set_target_properties(XplaneServer PROPERTIES OUTPUT_NAME ${OUTPUT_FILE_NAME})
set_target_properties(XplaneServer PROPERTIES SUFFIX "")