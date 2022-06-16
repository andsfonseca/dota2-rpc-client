cmake_minimum_required(VERSION 3.5)
project(dota2-rpc-client CXX)

#[[Includes]]
include(CheckIncludeFileCXX)
include(FetchContent)

find_package(Git REQUIRED)

# ## OS Requirements
# ## - OS must contain an identifier corresponding to your current operating system: Windows, Macos, Linux.
if(NOT OS) # If OS is not defined, it tries to find it.
    if(WIN32)
        set(OS Windows)
    elseif(APPLE)
        set(OS Macos)
    elseif(UNIX)
        set(OS Linux)
    else()
        message(FATAL_ERROR "Cannot detect operating system")
    endif()

    message(STATUS "OS was not defined, automatically set to ${OS}")
elseif(NOT OS STREQUAL "Windows" AND NOT OS STREQUAL "Macos" AND NOT OS STREQUAL "Linux") # OS can only have following values: Windows, Macos, Linux.
    message(FATAL_ERROR "OS has wrong value. Was: ${OS}, must be: Windows|Macos|Linux")
endif()

# ## ARCH Requirements
# ## - ARCH must contain the processor architecture you want to use to build the SDK, possible values: x86_64, x86 (Window only).
if(NOT ARCH)
    message("ARCH not defined, automatically set to x86_64")
    set(ARCH x86_64)
endif()

if(ARCH STREQUAL "x86" AND NOT OS STREQUAL "Windows") # ARCH can be x86 only on Windows
    message(FATAL_ERROR "x86 is only possible on Windows")
endif()

if(ARCH STREQUAL "x86" OR ARCH STREQUAL "x86_64")
    message(STATUS "ARCH is ${ARCH}")
else()
    message(FATAL_ERROR "ARCH has wrong value. Was: ${ARCH}, must be: x86_64|x86")
endif()

#[[CMAKE CXX]]
check_include_file_cxx(any HAS_ANY)
check_include_file_cxx(string_view HAS_STRING_VIEW)
check_include_file_cxx(coroutine HAS_COROUTINE)

if(NOT "${CMAKE_CXX_STANDARD}" STREQUAL "")
# Do nothing
elseif(HAS_ANY AND HAS_STRING_VIEW AND HAS_COROUTINE)
    set(CMAKE_CXX_STANDARD 20)
elseif(HAS_ANY AND HAS_STRING_VIEW)
    set(CMAKE_CXX_STANDARD 17)
else()
    set(CMAKE_CXX_STANDARD 14)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

file(GLOB SRC_List
    "main.cc"
    "${THIRD_PARTY_BASE_DIR}/discord-sdk-src/cpp/*.cpp"
)

add_executable(${PROJECT_NAME} ${SRC_List})

set(DISCORD_GAME_SDK_PATH ${THIRD_PARTY_BASE_DIR}/discord-sdk-src)

if(OS STREQUAL "Windows")
    if(ARCH STREQUAL "x86_64")
        set(CMAKE_DISCORD_GAME_SDK_LIB_FOLDER ${DISCORD_GAME_SDK_PATH}/lib/x86_64)
        set(CMAKE_DISCORD_GAME_SDK_LIB_PATH ${CMAKE_DISCORD_GAME_SDK_LIB_FOLDER}/discord_game_sdk.dll.lib)
    else()
        set(CMAKE_DISCORD_GAME_SDK_LIB_FOLDER ${DISCORD_GAME_SDK_PATH}/lib/x86)
        set(CMAKE_DISCORD_GAME_SDK_LIB_PATH ${CMAKE_DISCORD_GAME_SDK_LIB_FOLDER}/discord_game_sdk.dll.lib)
    endif()
elseif(OS STREQUAL "Linux")
    set(CMAKE_DISCORD_GAME_SDK_LIB_FOLDER ${DISCORD_GAME_SDK_PATH}/lib/x86_64)
    set(CMAKE_DISCORD_GAME_SDK_LIB_PATH ${CMAKE_DISCORD_GAME_SDK_LIB_FOLDER}/libdiscord_game_sdk.so)
elseif(OS STREQUAL "Macos")
    set(CMAKE_DISCORD_GAME_SDK_LIB_FOLDER ${DISCORD_GAME_SDK_PATH}/lib/x86_64)
    set(CMAKE_DISCORD_GAME_SDK_LIB_PATH ${CMAKE_DISCORD_GAME_SDK_LIB_FOLDER}/discord_game_sdk.dylib)
endif()

if(OS STREQUAL "Linux")
    execute_process(COMMAND mv discord_game_sdk.so libdiscord_game_sdk.so
            WORKING_DIRECTORY ${CMAKE_DISCORD_GAME_SDK_LIB_FOLDER}
            OUTPUT_VARIABLE AUX)
endif()

target_link_directories(${PROJECT_NAME} PRIVATE "${CMAKE_DISCORD_GAME_SDK_LIB_FOLDER}")
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_DISCORD_GAME_SDK_LIB_PATH})

if(OS STREQUAL "Windows")
    # Move DLL File Automatically While Building
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD # Adds a post-build event to MyTest
        COMMAND ${CMAKE_COMMAND} -E copy_if_different # which executes "cmake - E copy_if_different..."
        "${CMAKE_DISCORD_GAME_SDK_LIB_FOLDER}/discord_game_sdk.dll" # <--this is in-file
        $<TARGET_FILE_DIR:${PROJECT_NAME}>) # <--this is out-file path
endif()

#[[2. Drogon]]
set(DROGON_GIT "https://github.com/drogonframework/drogon.git")
set(DROGON_GIT_TAG "v1.7.5")

if(NOT EXISTS ${THIRD_PARTY_BASE_DIR}/drogon)
    execute_process(COMMAND ${GIT_EXECUTABLE} clone -b ${DROGON_GIT_TAG} ${DROGON_GIT}
        WORKING_DIRECTORY ${THIRD_PARTY_BASE_DIR}
        OUTPUT_VARIABLE AUX)

    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
        WORKING_DIRECTORY ${THIRD_PARTY_BASE_DIR}/drogon/
        OUTPUT_VARIABLE AUX)
endif()

set(BUILD_ORM FALSE CACHE INTERNAL "")
set(BUILD_EXAMPLES FALSE CACHE INTERNAL "")
set(BUILD_CTL FALSE CACHE INTERNAL "")

add_subdirectory(${THIRD_PARTY_BASE_DIR}/drogon)
target_link_libraries(${PROJECT_NAME} PRIVATE drogon)

#[[3. SrvLib.git]]
set(SRVLIB_GIT "https://github.com/andsfonseca/SrvLib.git")

if(NOT EXISTS ${THIRD_PARTY_BASE_DIR}/SrvLib)
    execute_process(COMMAND ${GIT_EXECUTABLE} clone ${SRVLIB_GIT}
        WORKING_DIRECTORY ${THIRD_PARTY_BASE_DIR}
        OUTPUT_VARIABLE AUX)
endif()

add_subdirectory(${THIRD_PARTY_BASE_DIR}/SrvLib)

target_link_libraries(${PROJECT_NAME} PRIVATE srvlib)

#[[4.Native file dialog]]
set(NFD_GIT "https://github.com/btzy/nativefiledialog-extended.git")
set(NFD_GIT_HASH 28ade5a)

if(NOT EXISTS ${THIRD_PARTY_BASE_DIR}/nativefiledialog-extended)
    execute_process(COMMAND ${GIT_EXECUTABLE} clone ${NFD_GIT}
        WORKING_DIRECTORY ${THIRD_PARTY_BASE_DIR}
        OUTPUT_VARIABLE AUX)

    execute_process(COMMAND ${GIT_EXECUTABLE} checkout ${NFD_GIT_HASH}
        WORKING_DIRECTORY ${THIRD_PARTY_BASE_DIR}/nativefiledialog-extended
        OUTPUT_VARIABLE AUX)
endif()

add_subdirectory(${THIRD_PARTY_BASE_DIR}/nativefiledialog-extended)
target_link_libraries(${PROJECT_NAME} PRIVATE nfd)

# Source Files

# Source
set(CMAKE_INCLUDE_PATH ${CMAKE_CURRENT_LIST_DIR}/include)
add_subdirectory(src)

# add_executable(${PROJECT_NAME} main.cc ${DISCORD_SRC_LIST})
if(CMAKE_CXX_STANDARD LESS 17)
    # With C++14, use boost to support any, string_view and filesystem
    message(STATUS "use c++14")
    find_package(Boost 1.61.0 REQUIRED)
    target_link_libraries(${PROJECT_NAME} PUBLIC Boost::boost)
elseif(CMAKE_CXX_STANDARD LESS 20)
    message(STATUS "use c++17")
else()
    message(STATUS "use c++20")
endif()

aux_source_directory(controllers CTL_SRC)
aux_source_directory(filters FILTER_SRC)
aux_source_directory(plugins PLUGIN_SRC)
aux_source_directory(models MODEL_SRC)

drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
    ${CMAKE_CURRENT_BINARY_DIR})

# use the following line to create views with namespaces.
# drogon_create_views(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/views
# ${CMAKE_CURRENT_BINARY_DIR} TRUE)
target_include_directories(${PROJECT_NAME}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_INCLUDE_PATH}
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

# Additional Files
if(MSVC)
    # Config Files
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/src/templates/config" "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>"
    )

    # Language Files
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/lang" "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/lang"
    )

    # Template Files
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/src/templates/templates" "${CMAKE_CURRENT_BINARY_DIR}/$<CONFIG>/templates"
    )
else()
    # Config Files
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/src/templates/config" "${CMAKE_CURRENT_BINARY_DIR}"
    )

    # Language Files
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/lang" "${CMAKE_CURRENT_BINARY_DIR}/lang"
    )

    # Template Files
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/src/templates/templates" "${CMAKE_CURRENT_BINARY_DIR}/templates"
    )
endif()
