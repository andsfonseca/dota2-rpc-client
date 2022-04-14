cmake_minimum_required(VERSION 3.5)
project(dota2-rpc-client CXX)

include(CheckIncludeFileCXX)
include(FetchContent)

find_package(Git REQUIRED)

check_include_file_cxx(any HAS_ANY)
check_include_file_cxx(string_view HAS_STRING_VIEW)
check_include_file_cxx(coroutine HAS_COROUTINE)
if (NOT "${CMAKE_CXX_STANDARD}" STREQUAL "")
    # Do nothing
elseif (HAS_ANY AND HAS_STRING_VIEW AND HAS_COROUTINE)
    set(CMAKE_CXX_STANDARD 20)
elseif (HAS_ANY AND HAS_STRING_VIEW)
    set(CMAKE_CXX_STANDARD 17)
else ()
    set(CMAKE_CXX_STANDARD 14)
endif ()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(${PROJECT_NAME} main.cc)

#[[External Libraries]]
set(THIRD_PARTY_BASE_DIR ${PROJECT_SOURCE_DIR}/third_party)
set(FETCHCONTENT_BASE_DIR ${THIRD_PARTY_BASE_DIR} CACHE PATH "Missing description." FORCE)
set(CMAKE_TLS_VERIFY true)

#[[1. Discord Game SDK]]
set(DISCORD_SDK_URL "https://dl-game-sdk.discordapp.net/2.5.6/discord_game_sdk.zip")

if(NOT EXISTS ${THIRD_PARTY_BASE_DIR}/discord-sdk-src)

    FetchContent_Declare(
        discord-sdk
    URL ${DISCORD_SDK_URL}
    )

    FetchContent_MakeAvailable(discord-sdk)

endif()

#[[2. Drogon]]

set(DROGON_GIT "https://github.com/drogonframework/drogon.git")
set(DROGON_GIT_TAG "v1.7.5")

if(NOT EXISTS ${THIRD_PARTY_BASE_DIR}/drogon)

    execute_process(COMMAND ${GIT_EXECUTABLE} clone -b ${DROGON_GIT_TAG} ${DROGON_GIT}
    WORKING_DIRECTORY ${THIRD_PARTY_BASE_DIR}
    COMMAND_ERROR_IS_FATAL ANY)

    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
    WORKING_DIRECTORY ${THIRD_PARTY_BASE_DIR}/drogon/
    COMMAND_ERROR_IS_FATAL ANY)

endif()

add_subdirectory(${THIRD_PARTY_BASE_DIR}/drogon) 
target_link_libraries(${PROJECT_NAME} PRIVATE drogon)


if (CMAKE_CXX_STANDARD LESS 17)
    # With C++14, use boost to support any, string_view and filesystem
    message(STATUS "use c++14")
    find_package(Boost 1.61.0 REQUIRED)
    target_link_libraries(${PROJECT_NAME} PUBLIC Boost::boost)
elseif (CMAKE_CXX_STANDARD LESS 20)
    message(STATUS "use c++17")
else ()
    message(STATUS "use c++20")
endif ()

aux_source_directory(controllers CTL_SRC)
aux_source_directory(filters FILTER_SRC)
aux_source_directory(plugins PLUGIN_SRC)
aux_source_directory(models MODEL_SRC)

drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
                    ${CMAKE_CURRENT_BINARY_DIR})
# use the following line to create views with namespaces.
# drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
#                     ${CMAKE_CURRENT_BINARY_DIR} TRUE)

target_include_directories(${PROJECT_NAME}
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
                                   ${CMAKE_CURRENT_SOURCE_DIR}/models)
target_sources(${PROJECT_NAME}
               PRIVATE
               ${SRC_DIR}
               ${CTL_SRC}
               ${FILTER_SRC}
               ${PLUGIN_SRC}
               ${MODEL_SRC})

# ##############################################################################
# uncomment the following line for dynamically loading views 
# set_property(TARGET ${PROJECT_NAME} PROPERTY ENABLE_EXPORTS ON)

# ##############################################################################

