add_library(AsteroidMinerLib STATIC EXCLUDE_FROM_ALL)

set(PANGO_INCLUDE_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/pango-1.0")
set(PANGO_LIBRARY "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/pango-1.0.lib")
set(PANGO_CAIRO_LIBRARY "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/pangocairo-1.0.lib")

set(GLIB_INCLUDE_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/glib-2.0")
set(GLIB_LIBRARY "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/glib-2.0.lib")

set(GLIB_OBJECT_LIBRARY "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/gobject-2.0.lib")
set(GLIB_CONFIG_INCLUDE_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/glib-2.0/include")

set(HARFBUZZ_INCLUDE_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/harfbuzz")
set(HARFBUZZ_LIBRARY "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/harfbuzz.lib")

set(CAIRO_INCLUDE_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/cairo")
set(CAIRO_LIBRARY "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib/cairo.lib")

find_path(PANGO_INCLUDE_DIR pango/pango.h)
find_library(PANGO_LIBRARY pango-1.0)

find_path(GLIB_INCLUDE_DIR glib/glib.h)
find_path(GLIB_INCLUDE_DIR glib/glib-object.h)
find_path(GLIB_CONFIG_INCLUDE_DIR glibconfig.h)

find_library(GLIB_LIBRARY glib-2.0)
find_library(GLIB_OBJECT_LIBRARY gobject-2.0)

find_path(HARFBUZZ_INCLUDE_DIR hb.h)
find_library(HARFBUZZ_LIBRARY harfbuzz)

find_path(HARFBUZZ_INCLUDE_DIR cairo.h)
find_library(HARFBUZZ_LIBRARY cairo)

target_include_directories(AsteroidMinerLib PUBLIC ${GLIB_INCLUDE_DIR} ${GLIB_CONFIG_INCLUDE_DIR} ${CAIRO_INCLUDE_DIR} ${PANGO_INCLUDE_DIR} ${HARFBUZZ_INCLUDE_DIR})

# Set the appropriate compiler flags.
if(MSVC)
	target_compile_options(AsteroidMinerLib PUBLIC "/W3" "/permissive-" "/GR-" "/analyze-"
		"-Wno-nonportable-include-path" "$<$<CONFIG:Release>:/Gy;/WX;/Oi>")
	target_compile_definitions(AsteroidMinerLib PUBLIC "_UNICODE" "UNICODE")
else()
	target_compile_options(AsteroidMinerLib PUBLIC
		"-Wall" "-pedantic-errors" "-Wold-style-cast" "-fno-rtti" "$<$<CONFIG:Release>:-Werror>")
endif()

# Link with the general libraries.
target_link_libraries(AsteroidMinerLib PUBLIC ${GLIB_OBJECT_LIBRARY} ${CAIRO_LIBRARY} ${PANGO_CAIRO_LIBRARY} ${HARFBUZZ_LIBRARY} ${GLIB_LIBRARY} ${PANGO_LIBRARY} Fontconfig::Fontconfig msdf-atlas-gen::msdf-atlas-gen Freetype::Freetype msdfgen::msdfgen msdfgen::msdfgen-ext PNG::PNG glm::glm nlohmann_json::nlohmann_json glfw glad::glad)
# Link the needed OS-specific dependencies, if any.
if(WIN32)
    target_link_options(AsteroidMinerLib PRIVATE "/SUBSYSTEM:WINDOWS" "/ENTRY:mainCRTStartup")
	target_link_libraries(AsteroidMinerLib PUBLIC rpcrt4 Winmm)
else()
	target_link_libraries(AsteroidMinerLib PUBLIC pthread)
endif()

# Link with the UUID library, which is different for each OS.
if(UNIX)
	if(APPLE)
		find_library(UUID_LIB NAMES System PATHS /lib /usr/lib /usr/local/lib)
		find_path(UUID_INCLUDE uuid/uuid.h /usr/local/include /usr/include)
		target_link_libraries(AsteroidMinerLib PUBLIC "${UUID_LIB}")
		target_include_directories(AsteroidMinerLib PUBLIC "${UUID_INCLUDE}")
	elseif(AM_USE_SYSTEM_LIBRARIES)
		find_library(UUID_LIB uuid)
		target_link_libraries(AsteroidMinerLib PUBLIC "${UUID_LIB}")
	else()
		find_package(unofficial-libuuid CONFIG REQUIRED)
		target_link_libraries(AsteroidMinerLib PUBLIC unofficial::UUID::uuid)
	endif()
endif()

# Every source file (and header file) should be listed here, except main.cpp.
# If you add a new file, add it to this list.
target_sources(AsteroidMinerLib PRIVATE
    game/asteroid.cpp
    game/asteroid.h
    game/asteroidSpawner.cpp
    game/asteroidSpawner.h
    constants.h
    game/constants.h
    game/drone.cpp
    game/drone.h
    game/droneFactory.cpp
    game/droneFactory.h
	error.h	
    event.h
    flatScene.cpp
    flatScene.h
    game/game.cpp
    game/game.h
    global.cpp
	global.h
    lineRenderer.cpp
    lineRenderer.h
    util/multiRange.h
    game/outerSpaceScene.cpp
    game/outerSpaceScene.h
    game/outerSpaceUi.cpp
    game/outerSpaceUi.h
    random.cpp
    random.h
    util/range.h
    shaderProgram.cpp
    shaderProgram.h
    sprite.cpp
    sprite.h
    spriteBuffer.cpp
    spriteBuffer.h
    spriteFactory.cpp
    spriteFactory.h
    spriteRenderer.cpp
    spriteRenderer.h
    spriteSheet.cpp
    spriteSheet.h
    stb_image.h
    util/string.h
    textRenderer.cpp
    textRenderer.h
    time.cpp
    time.h
    winConsole.cpp
    winConsole.h
	window.cpp
	window.h
    windowedApplication.h
)
