add_library(AsteroidMinerLib STATIC EXCLUDE_FROM_ALL)

# Set the appropriate compiler flags.
target_include_directories(AsteroidMinerLib PUBLIC "${LIBMAD_INCLUDE_DIR}")
if(MSVC)
	target_compile_options(AsteroidMinerLib PUBLIC "/W3" "/permissive-" "/sdl" "/GR-" "/analyze-"
		"-Wno-nonportable-include-path" "$<$<CONFIG:Release>:/Gy;/WX;/Oi>")
	target_compile_definitions(AsteroidMinerLib PUBLIC "_UNICODE" "UNICODE")
else()
	target_compile_options(AsteroidMinerLib PUBLIC
		"-Wall" "-pedantic-errors" "-Wold-style-cast" "-fno-rtti" "$<$<CONFIG:Release>:-Werror>")
endif()

# Link with the general libraries.
target_link_libraries(AsteroidMinerLib PUBLIC SDL2::SDL2 PNG::PNG JPEG::JPEG OpenAL::OpenAL
	"$<IF:$<CONFIG:Debug>,${LIBMAD_LIB_DEBUG},${LIBMAD_LIB_RELEASE}>")

# Link the needed OS-specific dependencies, if any.
if(WIN32)
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
	error.h	
    event.cpp
    event.h
    eventHandler.cpp
    eventHandler.h
	globalIncludes.h
	point.cpp
	point.h
	window.cpp
	window.h
)
