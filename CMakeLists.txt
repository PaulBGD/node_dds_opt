cmake_minimum_required(VERSION 3.16)
project(node_dds_opt)
cmake_policy(SET CMP0042 NEW)

add_definitions(-DNAPI_VERSION=3)
set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_JS_INC})
add_subdirectory("libs/DirectXTex")

add_library(node_dds_opt SHARED main.cpp textures.cpp textures.hpp ${CMAKE_JS_SRC})
set_target_properties(node_dds_opt PROPERTIES PREFIX "" SUFFIX ".node")
target_link_libraries(node_dds_opt PRIVATE ${CMAKE_JS_LIB} directxtex)

execute_process(COMMAND node -p "require('node-addon-api').include"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE NODE_ADDON_API_DIR
        )
string(REPLACE "\n" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
string(REPLACE "\"" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})
target_include_directories(node_dds_opt PRIVATE ${NODE_ADDON_API_DIR})
