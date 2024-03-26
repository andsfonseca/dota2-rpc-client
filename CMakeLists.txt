cmake_minimum_required(VERSION 3.17)

# Download conan-cmake automatically, you can also just copy the conan.cmake file
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                TLS_VERIFY ON)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

# No reason to set CMAKE_CONFIGURATION_TYPES if it's not a multiconfig generator
get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)

# Prepare conan before project inicialization
if(isMultiConfig)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE) 
    foreach(TYPE ${CMAKE_CONFIGURATION_TYPES})
        conan_cmake_install(PATH_OR_REFERENCE ${CMAKE_SOURCE_DIR}/conanfile.txt
                            BUILD missing
                            SETTINGS build_type=${TYPE})
    endforeach()
else()
    if(NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
    endif()
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release")
    conan_cmake_install(PATH_OR_REFERENCE ${CMAKE_SOURCE_DIR}/conanfile.txt
                        BUILD missing
                        SETTINGS build_type=${CMAKE_BUILD_TYPE})
endif()

# Enable cmake_find_package and cmake_find_package_multi inside binary folder
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_BINARY_DIR})
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR})

# Corrections to no-config/multi-config
if(isMultiConfig)
    # For multi-config builds (e.g. msvc)
    foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
        string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_CURRENT_BINARY_DIR}/${OUTPUTCONFIG})
    endforeach( OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES )

else()
    if (WIN32)
        # Generic, no-config when windows (e.g. with mingw)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
    else()
        # Generic, no-config when linux (e.g. with mingw)
        set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
    endif()
endif()

project(dota2-rpc-client CXX)

#[[Includes]]
include(CheckIncludeFileCXX)
include(FetchContent)

find_package(Git REQUIRED)

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

file(GLOB SRC_List
    "main.cc"
)
add_executable(${PROJECT_NAME} ${SRC_List})

find_package(discord REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE discord::discord)

if(WIN32)
    # Move DLL File Automatically While Building
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD # Adds a post-build event to MyTest
        COMMAND ${CMAKE_COMMAND} -E copy_if_different # which executes "cmake - E copy_if_different..."
        "${CMAKE_CURRENT_BINARY_DIR}/lib/discord_game_sdk.dll" 
        $<TARGET_FILE_DIR:${PROJECT_NAME}>) 
endif()

#[[2. Drogon]]
find_package(Drogon CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Drogon::Drogon)

#[[3. jsoncpp]]
find_package(jsoncpp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE jsoncpp::jsoncpp)

#[[4. SrvLib.git]]
set(SRVLIB_GIT "https://github.com/andsfonseca/SrvLib.git")

if(NOT EXISTS ${THIRD_PARTY_BASE_DIR}/SrvLib)
    execute_process(COMMAND ${GIT_EXECUTABLE} clone ${SRVLIB_GIT}
        WORKING_DIRECTORY ${THIRD_PARTY_BASE_DIR}
        OUTPUT_VARIABLE AUX)
endif()

add_subdirectory(${THIRD_PARTY_BASE_DIR}/SrvLib)

target_link_libraries(${PROJECT_NAME} PRIVATE srvlib)

#[[5.Native file dialog]]
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
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/src/templates/config" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    )

    # Language Files
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/lang" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/lang"
    )

    # Template Files
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/src/templates/templates" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/templates"
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
