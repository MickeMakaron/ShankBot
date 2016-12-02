cmake_minimum_required(VERSION 3.0)
project(ShankBot)

set(QT_INCLUDE_PATH "" CACHE PATH "Qt headers path")
set(QT_BIN_PATH "" CACHE PATH "Qt binaries path")
set(GLEW_INCLUDE_PATH "" CACHE PATH "glew headers path")


macro( mark_as_internal _var)
	set(${_var} ${${_var}} CACHE INTERNAL "" FORCE)
endmacro(mark_as_internal _var)

find_library(QT_CORE_LIB NAMES Qt5Core PATHS "${QT_BIN_PATH}" NO_DEFAULT_PATH)
find_library(QT_GUI_LIB NAMES Qt5Gui PATHS "${QT_BIN_PATH}" NO_DEFAULT_PATH)
mark_as_internal(QT_CORE_LIB)
mark_as_internal(QT_GUI_LIB)
if(NOT QT_CORE_LIB OR NOT QT_GUI_LIB)
	message(FATAL_ERROR "Please set QT_BIN_PATH to the path containing the Qt DLLs.")
endif(NOT QT_CORE_LIB OR NOT QT_GUI_LIB)

find_path(QT_CORE_INCL NAMES QtCore PATHS "${QT_INCLUDE_PATH}" NO_DEFAULT_PATH)
find_path(QT_GUI_INCL NAMES QtGui PATHS "${QT_INCLUDE_PATH}" NO_DEFAULT_PATH)
mark_as_internal(QT_CORE_INCL)
mark_as_internal(QT_GUI_INCL)
if(NOT QT_CORE_INCL OR NOT QT_GUI_INCL)
	message(FATAL_ERROR "Please set QT_INCLUDE_PATH to the path containing the Qt headers.")
endif(NOT QT_CORE_INCL OR NOT QT_GUI_INCL)

find_path(GLEW_H NAMES GL/glew.h PATHS "${GLEW_INCLUDE_PATH}")
mark_as_internal(GLEW_H)
if(NOT GLEW_H)
	message(FATAL_ERROR "Please set GLEW_INCLUDE_PATH to the path containing the glew headers. The path should contain GL/glew.h")
endif(NOT GLEW_H)

include_directories(. "${QT_INCLUDE_PATH}")

get_filename_component(COMPILER_BIN_PATH "${CMAKE_CXX_COMPILER}" DIRECTORY)
message(STATUS "${COMPILER_BIN_PATH}")
find_library(STD_CXX_LIB NAMES stdc++-6 PATHS "${COMPILER_BIN_PATH}" NO_DEFAULT_PATH)
if(NOT STD_CXX_LIB)
	message(FATAL_ERROR "Could not find stdc++-6 DLL.")
endif(NOT STD_CXX_LIB)


add_compile_options(-std=c++14 -Wall)

file(GLOB_RECURSE sources utility/src/*.cpp)
add_library(ShankBotUtility SHARED ${sources})
target_compile_definitions(ShankBotUtility PRIVATE -DBUILD_SHANK_BOT_UTILITY)

file(GLOB_RECURSE sources lzma/src/*.cpp lzma/src/*.c)
add_library(ShankBotLzma SHARED ${sources})
target_compile_options(ShankBotLzma PRIVATE -DBUILD_SHANK_BOT_LZMA)
target_include_directories(ShankBotLzma PRIVATE . lzma)

file(GLOB_RECURSE sources tibiaassets/src/*.cpp tibiaassets/src/*.c)
add_library(ShankBotTibiaAssets SHARED ${sources})
target_compile_options(ShankBotTibiaAssets PRIVATE -DBUILD_SHANK_BOT_TIBIAASSETS)
target_link_libraries(ShankBotTibiaAssets ${QT_CORE_LIB} ${QT_GUI_LIB} ShankBotUtility ShankBotLzma)

file(GLOB_RECURSE sources messaging/src/*.cpp messaging/src/*.c)
add_library(ShankBotMessaging SHARED ${sources})
message(STATUS "${sources}")
target_compile_options(ShankBotMessaging PRIVATE -DBUILD_SHANK_BOT_MESSAGING)

target_link_libraries(ShankBotMessaging ShankBotUtility)

file(GLOB_RECURSE sources monitor/src/inject.cpp)
add_library(ShankBotMonitorInjection SHARED ${sources})

target_link_libraries(ShankBotMonitorInjection ShankBotUtility)
target_include_directories(ShankBotMonitorInjection PRIVATE ${GLEW_INCLUDE_PATH})

file(GLOB_RECURSE sources monitor/src/*.cpp monitor/src/*.c)
FOREACH(item ${sources})
	IF(${item} MATCHES "monitor/src/inject.cpp")
		LIST(REMOVE_ITEM sources ${item})
		break()
	ENDIF(${item} MATCHES "monitor/src/inject.cpp")
ENDFOREACH(item ${sources})
add_executable(ShankBotMonitor ${sources})
target_link_libraries(ShankBotMonitor ${QT_CORE_LIB} ${QT_GUI_LIB} ShankBotMessaging ShankBotTibiaAssets ShankBotUtility)

add_custom_command(TARGET ShankBotMonitor POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different 
	${QT_CORE_LIB} $<TARGET_FILE_DIR:ShankBotMonitor>)
add_custom_command(TARGET ShankBotMonitor POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different 
	${QT_GUI_LIB} $<TARGET_FILE_DIR:ShankBotMonitor>)
add_custom_command(TARGET ShankBotMonitor POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different 
	${STD_CXX_LIB} $<TARGET_FILE_DIR:ShankBotMonitor>)
