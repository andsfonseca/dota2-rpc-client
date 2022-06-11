
target_sources(${PROJECT_NAME}
PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/extensions/StringExtensions.cpp
    ${CMAKE_CURRENT_LIST_DIR}/managers/ConfigurationManager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/managers/LanguageManager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/persistence/FileLoader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/persistence/JsonLoader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/persistence/TemplateLoader.cpp
    ${CMAKE_INCLUDE_PATH}/extensions/StringExtensions.h
    ${CMAKE_INCLUDE_PATH}/managers/ConfigurationManager.h
    ${CMAKE_INCLUDE_PATH}/managers/LanguageManager.h
    ${CMAKE_INCLUDE_PATH}/persistence/FileLoader.h
    ${CMAKE_INCLUDE_PATH}/persistence/JsonLoader.h
    ${CMAKE_INCLUDE_PATH}/persistence/TemplateLoader.h
)


