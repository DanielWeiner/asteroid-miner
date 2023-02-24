cmake_minimum_required(VERSION 3.16..3.25)

include(CMakeDependentOption)
option(AM_USE_VCPKG "Use vcpkg to get dependencies." ON)
cmake_dependent_option(AM_GLES "Build the game with OpenGL ES" OFF UNIX OFF)
cmake_dependent_option(AM_USE_SYSTEM_LIBRARIES "Use system libraries instead of the vcpkg ones." ON UNIX OFF)
cmake_dependent_option(AM_CREATE_BUNDLE "Create a Bundle instead of an executable" OFF APPLE OFF)

# Support Debug and Release configurations.
set(CMAKE_CONFIGURATION_TYPES "Debug" "Release" CACHE STRING "" FORCE)

# Use C++11 without any compiler specific extensions.
set(CMAKE_CXX_STANDARD 20 CACHE STRING "")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
# Use LTO for Release builds only.
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG FALSE)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
# On Linux use relative RPATH.
set(CMAKE_BUILD_RPATH_USE_ORIGIN ON)

include(vcpkg_bootstrap.cmake)
x_vcpkg_bootstrap()

set(VCPKG_BOOTSTRAP_OPTIONS "-disableMetrics")
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
set(X_VCPKG_APPLOCAL_DEPS_INSTALL ON)
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake"
    CACHE STRING "Vcpkg toolchain file")

if(AM_USE_SYSTEM_LIBRARIES)
	if(UNIX AND NOT APPLE)
		# If using the system libraries from a Linux distro, statically link every other library needed.
		# This is to make sure that the resulting binary has no dependencies on libraries (other than
		# the system ones) and can thus be distributed as a simple binary.
		set(VCPKG_TARGET_TRIPLET "x64-linux")
	endif()
else()
	# Tell vcpkg to use the required libraries.
	list(APPEND VCPKG_MANIFEST_FEATURES "system-libs")
endif()

# Various helpful options for IDEs.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT AsteroidMiner)
set(CMAKE_VS_JUST_MY_CODE_DEBUGGING ON)

project("Asteroid Miner" VERSION 0.0.0
	DESCRIPTION ""
	LANGUAGES CXX)


# Find the needed library dependencies.
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(PNG REQUIRED)
find_package(glm REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(msdfgen CONFIG REQUIRED)
find_package(msdf-atlas-gen CONFIG REQUIRED)
find_package(Freetype REQUIRED)
find_package(Fontconfig REQUIRED)

# Find the MinGW runtime DLLs.
if(MINGW AND WIN32)
	# On Windows copy the MinGW runtime DLLs to the output folder as well.
	# This is to avoid the situation where a user has other MinGW runtime DLLs
	# in their PATH that are incompatible with the MinGW used to compile ES.    
	get_filename_component(PARENT_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
	get_filename_component(MINGW_RUNTIME_DIR "${PARENT_DIR}" DIRECTORY)

	# MinGW doesn't have seh exceptions support for 32-bit Windows unfortunately,
	# and requires libgcc_s_dw2-1.dll instead of libgcc_s_seh-1.dll. There's no
	# perfect way to figure out which one to copy, so we simply copy both.
	# The executable will choose the correct DLL anyways.
	foreach(lib "stdc++-6" "winpthread-1" "gcc_s_seh-1" "gcc_s_dw2-1")
		file(GLOB_RECURSE FILE_PATH "${MINGW_RUNTIME_DIR}/lib${lib}.dll")
		if(FILE_PATH)
			list(APPEND MINGW_RUNTIME "${FILE_PATH}")
		endif()
	endforeach()
endif()

# Add the library target.
add_subdirectory(source)

# Create game target.
if(APPLE AND AM_CREATE_BUNDLE)
	add_executable(AsteroidMiner MACOSX_BUNDLE source/main.cpp)

	# MacOS bundles are a bit special and need every resource specified when
	# creating the executable.
	foreach(folder "data" "images" "sounds")
		file(GLOB_RECURSE RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/${folder}/*")
		target_sources(AsteroidMiner PRIVATE "${RESOURCES}")

		foreach(FILE ${RESOURCES})
			# Get the relative path from the root folder to the current file.
			file(RELATIVE_PATH NEW_FILE "${CMAKE_CURRENT_SOURCE_DIR}" "${FILE}")
			# Get the parent directory for the new location.
			get_filename_component(FILE_PATH "${NEW_FILE}" DIRECTORY)

			# Resources belong under Resources/.
			set_source_files_properties("${FILE}" PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/${FILE_PATH}")
		endforeach()
	endforeach()

	# Create the icns file, required for the bundle icon.
	add_custom_command(
		OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/icons/asteroid-miner.icns"
		COMMAND iconutil -c icns -o icons/asteroid-miner.icns icons/asteroid-miner.iconset
		DEPENDS icons/asteroid-miner.iconset/icon_16x16.png icons/asteroid-miner.iconset/icon_16x16@2x.png
			icons/asteroid-miner.iconset/icon_32x32.png icons/asteroid-miner.iconset/icon_32x32@2x.png
			icons/asteroid-miner.iconset/icon_128x128.png icons/asteroid-miner.iconset/icon_128x128@2x.png
			icons/asteroid-miner.iconset/icon_256x256.png icons/asteroid-miner.iconset/icon_256x256@2x.png
			icons/asteroid-miner.iconset/icon_512x512.png icons/asteroid-miner.iconset/icon_512x512@2x.png
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" VERBATIM)

	# Now do the same to standone files.
	foreach(file "license.txt" "keys.txt" "credits.txt" "copyright" "changelog" "icons/asteroid-miner.icns")
		target_sources(AsteroidMiner PRIVATE ${file})
		set_source_files_properties(${file} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources/")
	endforeach()

	# Add plist to bundle.
	set_target_properties(AsteroidMiner PROPERTIES
		MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_LIST_DIR}/resources/AsteroidMiner-Info.plist"
		XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME asteroid-miner)
	set_target_properties(AsteroidMiner PROPERTIES OUTPUT_NAME "Asteroid Miner")
elseif(WIN32)
	add_executable(AsteroidMiner WIN32 source/main.cpp source/WinApp.rc)
	set_target_properties(AsteroidMiner PROPERTIES OUTPUT_NAME "Asteroid Miner")
else()
	add_executable(AsteroidMiner source/main.cpp)
	set_target_properties(AsteroidMiner PROPERTIES OUTPUT_NAME "asteroid-miner")
endif()

# Link with the library dependencies.
target_link_libraries(AsteroidMiner PRIVATE AsteroidMinerLib glfw glad::glad)

# Link with OpenGL or OpenGL ES.
if(AM_GLES)
	find_package(OpenGL REQUIRED OpenGL EGL)
	target_link_libraries(AsteroidMiner PRIVATE OpenGL::OpenGL OpenGL::EGL)
	target_compile_definitions(AsteroidMinerLib PUBLIC AM_GLES)
else()
	find_package(OpenGL REQUIRED)
	target_link_libraries(AsteroidMiner PRIVATE OpenGL::GL)

	if(APPLE)
		# Apple deprecated OpenGL in MacOS 10.14, but we don't care.
		target_compile_definitions(AsteroidMinerLib PUBLIC GL_SILENCE_DEPRECATION)
	endif()
endif()

# Copy the MinGW runtime DLLs if necessary.
if(MINGW AND WIN32)
	foreach(FILE_PATH ${MINGW_RUNTIME})
		add_custom_command(TARGET AsteroidMiner POST_BUILD
			COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${FILE_PATH}" "$<TARGET_FILE_DIR:AsteroidMiner>"
			COMMAND_EXPAND_LISTS VERBATIM)

		# Add an install rule for this DLLs, so that it is also included when installing.
		install(FILES ${FILE_PATH} DESTINATION .)
	endforeach()
endif()

# Installation configurations.
if(APPLE)
	install(TARGETS AsteroidMiner CONFIGURATIONS Release BUNDLE DESTINATION .)
elseif(WIN32)
    target_link_options(AsteroidMiner PRIVATE "/SUBSYSTEM:WINDOWS" "/ENTRY:mainCRTStartup")
	# Install the binary.
	install(TARGETS AsteroidMiner CONFIGURATIONS Release RUNTIME DESTINATION .)

	# The MinGW DLLs needed were already installed above, and vcpkg installs the library DLLs.

	# Install the resouce files.
	install(DIRECTORY data DESTINATION .)
	install(DIRECTORY images DESTINATION .)
	install(DIRECTORY sounds DESTINATION .)
	install(FILES credits.txt DESTINATION .)
	install(FILES keys.txt DESTINATION .)
	install(FILES copyright DESTINATION .)
	install(FILES changelog DESTINATION .)
elseif(UNIX)
	# Install the binary.
	install(TARGETS AsteroidMiner CONFIGURATIONS Release RUNTIME DESTINATION games)

	# Install the desktop file.
	install(FILES asteroid-miner.desktop DESTINATION share/applications)
    
	# Install icons, keeping track of all the paths.
	# Most Ubuntu apps supply 16, 22, 24, 32, 48, and 256, and sometimes others.
	foreach(size "16x16" "22x22" "24x24" "32x32" "48x48" "128x128" "256x256" "512x512")
		install(FILES "icons/icon_${size}.png" DESTINATION "share/icons/hicolor/${size}/apps"
			RENAME asteroid-miner.png)
	endforeach()

	# Gzip and install man file.
	add_custom_command(
		OUTPUT asteroid-miner.6.gz
		COMMAND gzip -c asteroid-miner.6 > ${CMAKE_CURRENT_BINARY_DIR}/asteroid-miner.6.gz
		DEPENDS asteroid-miner.6
		WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
		VERBATIM)
	target_sources(AsteroidMiner PRIVATE asteroid-miner.6.gz)
	install(FILES ${CMAKE_CURRENT_BINARY_DIR}/asteroid-miner.6.gz DESTINATION share/man/man6)

	# Install the resouce files.
	install(DIRECTORY data DESTINATION share/games/asteroid-miner)
	install(DIRECTORY images DESTINATION share/games/asteroid-miner)
	install(DIRECTORY sounds DESTINATION share/games/asteroid-miner)
	install(FILES credits.txt DESTINATION share/games/asteroid-miner)
	install(FILES keys.txt DESTINATION share/games/asteroid-miner)
	install(FILES copyright DESTINATION share/doc/asteroid-miner)
	install(FILES changelog DESTINATION share/doc/asteroid-miner)
endif()

if (WIN32)
    set(AM_TEXUREPACKER_BIN "C:/Program\ Files/CodeAndWeb/TexturePacker/bin/TexturePacker.exe")
endif()

add_custom_command(TARGET AsteroidMiner PRE_LINK 
    COMMAND
        ${AM_TEXUREPACKER_BIN} sprites.tps
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

file(
GLOB_RECURSE RESOURCES
RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
"data/fonts/*" 
"data/sprites/*" 
)
foreach(FILE ${RESOURCES})
    add_custom_command(TARGET AsteroidMiner PRE_LINK
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/${FILE}" "$<TARGET_FILE_DIR:AsteroidMiner>/${FILE}"
        COMMAND_EXPAND_LISTS VERBATIM)
endforeach()