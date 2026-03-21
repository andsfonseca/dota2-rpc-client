cmake_minimum_required(VERSION 3.26)

# [[Conan 2 Integration]]
if(NOT DEFINED CONAN_SETTINGS)
    set(CONAN_SETTINGS "compiler.cppstd=17" CACHE STRING "Conan settings")
endif()

set(CONAN_HOST_PROFILE "default" CACHE STRING "Conan host profile")
set(CONAN_BUILD_PROFILE "default" CACHE STRING "Conan build profile")

# Download conan_provider.cmake automatically
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan_provider.cmake")
    message(STATUS "Downloading conan_provider.cmake from https://github.com/conan-io/cmake-conan")
    file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/develop2/conan_provider.cmake"
                  "${CMAKE_BINARY_DIR}/conan_provider.cmake"
                  TLS_VERIFY ON)
endif()

set(CMAKE_PROJECT_TOP_LEVEL_INCLUDES "${CMAKE_BINARY_DIR}/conan_provider.cmake")

project(dota2-rpc-client CXX)

#[[Includes]]
include(CheckIncludeFileCXX)
include(FetchContent)

find_package(Git REQUIRED)

list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

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
check_include_file_cxx(coroutine HAS_COROUTINE)

if(NOT "${CMAKE_CXX_STANDARD}" STREQUAL "")
elseif(HAS_COROUTINE)
    set(CMAKE_CXX_STANDARD 20)
else()
    set(CMAKE_CXX_STANDARD 17)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#[[External Libraries]]

set(THIRD_PARTY_BASE_DIR ${PROJECT_SOURCE_DIR}/third_party)
set(FETCHCONTENT_BASE_DIR ${THIRD_PARTY_BASE_DIR} CACHE PATH "Missing description." FORCE)
set(CMAKE_TLS_VERIFY true)

if(OS STREQUAL "Linux")
    find_package(Threads REQUIRED)
    find_package(PkgConfig QUIET)
endif()

#[[1. Discord Game SDK]]
set(DISCORD_SDK_URL "https://dl-game-sdk.discordapp.net/3.2.1/discord_game_sdk.zip")

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
target_compile_options(${PROJECT_NAME} PRIVATE -include cstdint)

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
    # Rename discord_game_sdk.so to libdiscord_game_sdk.so if needed
    if(EXISTS "${CMAKE_DISCORD_GAME_SDK_LIB_FOLDER}/discord_game_sdk.so" AND NOT EXISTS "${CMAKE_DISCORD_GAME_SDK_LIB_FOLDER}/libdiscord_game_sdk.so")
        file(RENAME "${CMAKE_DISCORD_GAME_SDK_LIB_FOLDER}/discord_game_sdk.so" "${CMAKE_DISCORD_GAME_SDK_LIB_FOLDER}/libdiscord_game_sdk.so")
    endif()
endif()

target_link_directories(${PROJECT_NAME} PRIVATE "${CMAKE_DISCORD_GAME_SDK_LIB_FOLDER}")
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_DISCORD_GAME_SDK_LIB_PATH})

if(OS STREQUAL "Linux")
    target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
endif()

if(OS STREQUAL "Windows")
    # Move DLL File Automatically While Building
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD # Adds a post-build event to MyTest
        COMMAND ${CMAKE_COMMAND} -E copy_if_different # which executes "cmake - E copy_if_different..."
        "${CMAKE_DISCORD_GAME_SDK_LIB_FOLDER}/discord_game_sdk.dll" # <--this is in-file
        $<TARGET_FILE_DIR:${PROJECT_NAME}>) # <--this is out-file path
endif()

#[[2. Drogon]]
find_package(Drogon CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Drogon::Drogon)

#[[3. jsoncpp]]
find_package(JsonCpp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE JsonCpp::JsonCpp)

#[[4. SrvLib.git]]
add_subdirectory(${THIRD_PARTY_BASE_DIR}/SrvLib)
target_link_libraries(${PROJECT_NAME} PRIVATE srvlib)

#[[5.Native file dialog]]
add_subdirectory(${THIRD_PARTY_BASE_DIR}/nativefiledialog-extended)
target_link_libraries(${PROJECT_NAME} PRIVATE nfd)

# Source Files

# Source
set(CMAKE_INCLUDE_PATH ${CMAKE_CURRENT_LIST_DIR}/include)
add_subdirectory(src)

if(CMAKE_CXX_STANDARD EQUAL 20)
    message(STATUS "Building with C++20 (coroutine support detected)")
else()
    message(STATUS "Building with C++17")
endif()

aux_source_directory(controllers CTL_SRC)
aux_source_directory(filters FILTER_SRC)
aux_source_directory(plugins PLUGIN_SRC)
aux_source_directory(models MODEL_SRC)

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
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/src/templates/config" "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    )

    # Language Files
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/lang" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/lang"
    )

    # Template Files
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/src/templates/templates" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/templates"
    )

    # Windows Installation Scripts
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/scripts/install-windows.bat" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/"
    )
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/scripts/uninstall-windows.bat" "$<TARGET_FILE_DIR:${PROJECT_NAME}>/"
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

    # Linux Service Scripts
    if(OS STREQUAL "Linux")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/scripts/install-service.sh" "${CMAKE_CURRENT_BINARY_DIR}/"
        )
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/scripts/uninstall-service.sh" "${CMAKE_CURRENT_BINARY_DIR}/"
        )
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_SOURCE_DIR}/scripts/service/dota2-rpc-client.service" "${CMAKE_CURRENT_BINARY_DIR}/"
        )
    endif()
endif()
