add_library(AsteroidMinerLib STATIC EXCLUDE_FROM_ALL)

# Set the appropriate compiler flags.
if(MSVC)
	target_compile_options(AsteroidMinerLib PUBLIC "/W3" "/permissive-" "/GR-" "/analyze-"
		"-Wno-nonportable-include-path" "$<$<CONFIG:Release>:/Gy;/WX;/Oi>")
	target_compile_definitions(AsteroidMinerLib PUBLIC "_UNICODE" "UNICODE")
else()
	target_compile_options(AsteroidMinerLib PUBLIC
		"-Wall" "-pedantic-errors" "-Wold-style-cast" "-fno-rtti" "$<$<CONFIG:Release>:-Werror>")
endif()

# Link with the general libraries.SS
target_link_libraries(AsteroidMinerLib PUBLIC PNG::PNG glm::glm nlohmann_json::nlohmann_json glfw glad::glad)
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
    game/drone.cpp
    game/drone.h
	error.h	
    event.h
    flatScene.cpp
    flatScene.h
    demo/game.cpp
    demo/game.h
    game/game.cpp
    game/game.h
    global.cpp
	global.h
    lineRenderer.cpp
    lineRenderer.h
    random.cpp
    random.h
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
    winConsole.cpp
    winConsole.h
	window.cpp
	window.h
    windowedApplication.h
)
