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

FetchContent_Declare(
		commonItems
		GIT_REPOSITORY https://github.com/tategotoazarasi/commonItems.git
		GIT_TAG 45863c7
		)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(COVERAGE OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(commonItems)

FetchContent_Declare(
		Fronter
		GIT_REPOSITORY https://github.com/tategotoazarasi/Fronter.git
		GIT_TAG a9a0d24
		)

file(WRITE "${CMAKE_SOURCE_DIR}/Copy_Files.sh" "#!/bin/sh\necho 'Dummy script to satisfy Fronter build requirement'\n")
if (UNIX)
	execute_process(COMMAND chmod +x "${CMAKE_SOURCE_DIR}/Copy_Files.sh")
endif ()

FetchContent_MakeAvailable(Fronter)


add_executable(CK3toEU5 src/main.cpp)
target_link_libraries(CK3toEU5 PRIVATE commonItems::commonItems)


set_target_properties(ConverterFrontend PROPERTIES
                      OUTPUT_NAME "CK3toEU5Frontend"
                      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
                      )

FetchContent_GetProperties(Fronter SOURCE_DIR FRONTER_SOURCE_DIR)

add_custom_target(DeployFronterResources ALL
                  COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Configuration
                  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${FRONTER_SOURCE_DIR}/Fronter/Resources/converter.ico" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/"
                  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${FRONTER_SOURCE_DIR}/Fronter/Resources/converter_languages.yml" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Configuration/"
                  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${FRONTER_SOURCE_DIR}/Fronter/Resources/converter_l_english.yml" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Configuration/"
                  COMMENT "Deploying Fronter resources..."
                  )

add_dependencies(DeployFronterResources ConverterFrontend)

file(WRITE "${CMAKE_BINARY_DIR}/fronter-configuration.txt"
     "name = \"CK3toEU5\"\n"
     "converterFolder = \".\"\n"
     "backendExePath = \"CK3toEU5\"\n"
     "displayName = \"Crusader Kings III to Europa Universalis 5\"\n"
     "sourceGame = \"Crusader Kings III\"\n"
     "targetGame = \"Europa Universalis 5\"\n"
     )

add_custom_target(DeployFronterConfig ALL
                  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_BINARY_DIR}/fronter-configuration.txt" "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Configuration/"
                  COMMENT "Deploying configuration..."
                  )
add_dependencies(DeployFronterConfig ConverterFrontend)

if (UNIX)
	add_custom_target(SetExecutablePermissions ALL
	                  COMMAND chmod +x ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/CK3toEU5
	                  COMMAND chmod +x ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/CK3toEU5Frontend
	                  COMMENT "Setting executable permissions..."
	                  )
	add_dependencies(SetExecutablePermissions CK3toEU5 ConverterFrontend)
endif ()