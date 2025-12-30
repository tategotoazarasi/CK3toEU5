cmake_minimum_required(VERSION 3.25)

project(CK3toEU5)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Release)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

if (MSVC)
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	add_compile_definitions(_CRT_SECURE_NO_WARNINGS UNICODE _UNICODE)
elseif (UNIX)
	add_compile_options(-pthread -O3)
endif ()

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(wxBUILD_SHARED OFF CACHE BOOL "" FORCE)
set(CURL_STATICLIB ON CACHE BOOL "" FORCE)
set(BUILD_STATIC_CURL ON CACHE BOOL "" FORCE)

include(FetchContent)

# 1. commonItems
FetchContent_Declare(
		commonItems
		GIT_REPOSITORY https://github.com/ParadoxGameConverters/commonItems.git
		GIT_TAG 7e5957c)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(COVERAGE OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(commonItems)

# 2. SQLiteCpp - PRE-LOAD
message(STATUS "Fetching SQLiteCpp from source to override VCPKG version...")
set(SQLITECPP_RUN_CPPCHECK OFF CACHE BOOL "" FORCE)
set(SQLITECPP_RUN_CPPLINT OFF CACHE BOOL "" FORCE)
set(SQLITECPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(SQLITECPP_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
		SQLiteCpp
		GIT_REPOSITORY https://github.com/SRombauts/SQLiteCpp.git
		GIT_TAG 3.3.3)
FetchContent_MakeAvailable(SQLiteCpp)
set(CMAKE_DISABLE_FIND_PACKAGE_SQLiteCpp ON)

# 3. Fronter
FetchContent_Declare(
		Fronter
		GIT_REPOSITORY https://github.com/tategotoazarasi/Fronter.git
		GIT_TAG f3cd85c)

file(WRITE "${CMAKE_SOURCE_DIR}/Copy_Files.sh" "#!/bin/sh\necho 'Dummy script to satisfy Fronter build requirement'\n")
if (UNIX)
	execute_process(COMMAND chmod +x "${CMAKE_SOURCE_DIR}/Copy_Files.sh")
endif ()

FetchContent_MakeAvailable(Fronter)

if (TARGET ConverterFrontend AND WIN32)
	set_target_properties(ConverterFrontend PROPERTIES WIN32_EXECUTABLE ON)
endif ()

# 4. rakaly (CK3 Save Decompression)
FetchContent_Declare(
		rakaly_header
		URL https://github.com/rakaly/librakaly/releases/download/v0.12.5/rakaly.h.tar.gz
		URL_HASH SHA256=51760761bf6b14445f1d2bd0eadb6c7add56e07cad366844d1c2e3e401a308f2)
FetchContent_MakeAvailable(rakaly_header)
FetchContent_GetProperties(rakaly_header SOURCE_DIR RAKALY_HEADER_DIR)

if (WIN32)
	FetchContent_Declare(
			rakaly_lib
			URL https://github.com/rakaly/librakaly/releases/download/v0.12.5/librakaly-0.12.5-win-msvc.tar.gz
			URL_HASH SHA256=bd767bcf1dbde722ec0e957510a097bd2a2c040ebdb5ffdf15f717b6d1c85334)
	FetchContent_MakeAvailable(rakaly_lib)
	FetchContent_GetProperties(rakaly_lib SOURCE_DIR RAKALY_LIB_DIR)
	add_library(rakaly::rakaly SHARED IMPORTED)
	set_target_properties(rakaly::rakaly PROPERTIES IMPORTED_LOCATION "${RAKALY_LIB_DIR}/rakaly.dll")
	set_target_properties(rakaly::rakaly PROPERTIES IMPORTED_IMPLIB "${RAKALY_LIB_DIR}/rakaly.dll.lib")
elseif (APPLE)
	FetchContent_Declare(
			rakaly_lib
			URL https://github.com/rakaly/librakaly/releases/download/v0.12.5/librakaly-0.12.5-macos.tar.gz
			URL_HASH SHA256=cd3c99138cd667e8e12218c346d384730600a4aa43f1b25cceffaaef5a268a27)
	FetchContent_MakeAvailable(rakaly_lib)
	FetchContent_GetProperties(rakaly_lib SOURCE_DIR RAKALY_LIB_DIR)
	add_library(rakaly::rakaly SHARED IMPORTED)
	set_target_properties(rakaly::rakaly PROPERTIES IMPORTED_LOCATION "${RAKALY_LIB_DIR}/librakaly.dylib")
elseif (UNIX)
	FetchContent_Declare(
			rakaly_lib
			URL https://github.com/rakaly/librakaly/releases/download/v0.12.5/librakaly-0.12.5-linux.tar.gz
			URL_HASH SHA256=1afa2f381e773174ca051be3c2d2d338302ddb2dd71df3a6a0a2b4052d086f4e)
	FetchContent_MakeAvailable(rakaly_lib)
	FetchContent_GetProperties(rakaly_lib SOURCE_DIR RAKALY_LIB_DIR)
	add_library(rakaly::rakaly SHARED IMPORTED)
	set_target_properties(rakaly::rakaly PROPERTIES IMPORTED_LOCATION "${RAKALY_LIB_DIR}/librakaly.so")
endif ()

# 5. ImageMagick (Flag Crafting)
find_package(ImageMagick COMPONENTS Magick++ MagickCore REQUIRED)

# --- Main Project Targets ---

# CK3 World Parsing Library
# We need to glob each subdirectory explicitly.
file(GLOB CK3WORLD_SOURCES "src/CK3World/*.cpp")
file(GLOB_RECURSE CK3WORLD_CHARS_SOURCES "src/CK3World/Characters/*.cpp")
file(GLOB_RECURSE CK3WORLD_COA_SOURCES "src/CK3World/CoatsOfArms/*.cpp")
file(GLOB_RECURSE CK3WORLD_CONFED_SOURCES "src/CK3World/Confederations/*.cpp")
file(GLOB_RECURSE CK3WORLD_CULT_SOURCES "src/CK3World/Cultures/*.cpp")
file(GLOB_RECURSE CK3WORLD_DYN_SOURCES "src/CK3World/Dynasties/*.cpp")
file(GLOB_RECURSE CK3WORLD_FLAG_SOURCES "src/CK3World/Flags/*.cpp")
file(GLOB_RECURSE CK3WORLD_GEO_SOURCES "src/CK3World/Geography/*.cpp")
file(GLOB_RECURSE CK3WORLD_REL_SOURCES "src/CK3World/Religions/*.cpp")
file(GLOB_RECURSE CK3WORLD_TITLE_SOURCES "src/CK3World/Titles/*.cpp")
file(GLOB_RECURSE MAPPERS_SOURCES "src/Mappers/CK3ReligionScraper/*.cpp"
     "src/Mappers/IAmHreMapper/*.cpp"
     "src/Mappers/LocalizationMapper/*.cpp"
     "src/Mappers/NamedColors/*.cpp"
     "src/Mappers/ShatterEmpiresMapper/*.cpp"
     "src/Mappers/TraitScraper/*.cpp"
     "src/Mappers/VassalSplitoffMapper/*.cpp")
file(GLOB CONFIG_SOURCES "src/Configuration/*.cpp")

add_library(CK3WorldLib STATIC
            ${CK3WORLD_SOURCES}
            ${CK3WORLD_CHARS_SOURCES}
            ${CK3WORLD_COA_SOURCES}
            ${CK3WORLD_CONFED_SOURCES}
            ${CK3WORLD_CULT_SOURCES}
            ${CK3WORLD_DYN_SOURCES}
            ${CK3WORLD_FLAG_SOURCES}
            ${CK3WORLD_GEO_SOURCES}
            ${CK3WORLD_REL_SOURCES}
            ${CK3WORLD_TITLE_SOURCES}
            ${MAPPERS_SOURCES}
            ${CONFIG_SOURCES})

target_include_directories(
		CK3WorldLib PUBLIC
		src
		${RAKALY_HEADER_DIR}
		${ImageMagick_INCLUDE_DIRS})
target_link_libraries(CK3WorldLib PUBLIC commonItems::commonItems rakaly::rakaly ImageMagick::Magick++ ImageMagick::MagickCore)

# Main Executable
add_executable(CK3toEU5 src/main.cpp)
target_include_directories(CK3toEU5 PRIVATE src)
target_link_libraries(CK3toEU5 PRIVATE CK3WorldLib)

# --- Post-Build Steps and Fronter Configuration ---

if (WIN32)
	add_custom_command(TARGET CK3toEU5 POST_BUILD
	                   COMMAND ${CMAKE_COMMAND} -E copy_if_different "${RAKALY_LIB_DIR}/rakaly.dll" "$<TARGET_FILE_DIR:CK3toEU5>")
elseif (UNIX AND NOT APPLE)
	add_custom_command(TARGET CK3toEU5 POST_BUILD
	                   COMMAND ${CMAKE_COMMAND} -E copy_if_different "${RAKALY_LIB_DIR}/librakaly.so" "$<TARGET_FILE_DIR:CK3toEU5>")
endif ()

set_target_properties(ConverterFrontend PROPERTIES
                      OUTPUT_NAME "CK3toEU5Frontend"
                      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

FetchContent_GetProperties(Fronter SOURCE_DIR FRONTER_SOURCE_DIR)

add_custom_target(
		DeployFronterResources ALL
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Configuration
		COMMAND ${CMAKE_COMMAND} -E
		copy_if_different "${FRONTER_SOURCE_DIR}/Fronter/Resources/converter.ico" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
		COMMAND ${CMAKE_COMMAND} -E
		copy_if_different
		"${FRONTER_SOURCE_DIR}/Fronter/Resources/converter_languages.yml"
		"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Configuration/"
		COMMAND ${CMAKE_COMMAND} -E
		copy_if_different
		"${FRONTER_SOURCE_DIR}/Fronter/Resources/converter_l_english.yml"
		"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Configuration/"
		COMMENT "Deploying Fronter resources...")

add_dependencies(DeployFronterResources ConverterFrontend)

file(WRITE "${CMAKE_BINARY_DIR}/fronter-configuration.txt"
     "name = \"CK3toEU5\"\n"
     "converterFolder = \".\"\n"
     "backendExePath = \"CK3toEU5\"\n"
     "displayName = \"Crusader Kings III to Europa Universalis 5\"\n"
     "sourceGame = \"Crusader Kings III\"\n"
     "targetGame = \"Europa Universalis 5\"\n")

add_custom_target(
		DeployFronterConfig ALL
		COMMAND ${CMAKE_COMMAND} -E
		copy_if_different
		"${CMAKE_BINARY_DIR}/fronter-configuration.txt"
		"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Configuration/"
		COMMENT "Deploying configuration...")
add_dependencies(DeployFronterConfig ConverterFrontend)

if (EXISTS "${CMAKE_SOURCE_DIR}/configurables")
	file(COPY configurables DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
else ()
	message(STATUS "Optional 'configurables' directory not found. Skipping copy. This is required for runtime.")
endif ()

if (UNIX)
	add_custom_target(
			SetExecutablePermissions ALL
			COMMAND chmod +x ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/CK3toEU5
			COMMAND chmod +x ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/CK3toEU5Frontend
			COMMENT "Setting executable permissions...")
	add_dependencies(SetExecutablePermissions CK3toEU5 ConverterFrontend)
endif ()