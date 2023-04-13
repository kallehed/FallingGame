cmake_minimum_required(VERSION 3.7)

project(FallingGameCmake)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
	glad.c
	GameState.cpp
	Bouncer.cpp Bouncer.h Camera.cpp Camera.h Cloud.cpp Cloud.h Coin.cpp Coin.h CoinParticle.cpp CoinParticle.h Drawer.cpp Drawer.h Game.cpp Game.h Layer.cpp Layer.h main.cpp misc.cpp misc.h Player.cpp stb_image.h 

Bouncer.h
Camera.h
Cloud.h
Coin.h
CoinParticle.h
Drawer.h
Game.h
Layer.h
misc.h
Player.h
GameState.h
)


if(BUILD_SHARED_LIBS)
	add_library(main SHARED)
	target_sources(main PRIVATE ${SOURCES})

        include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/include/)

	target_link_libraries(main SDL2 freetype)
else()
	if ( ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
		set(USE_FLAGS "-s USE_SDL=2 -s USE_FREETYPE=1") 
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_FLAGS}")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USE_FLAGS}")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${USE_FLAGS} -s ALLOW_MEMORY_GROWTH=1 -s FULL_ES3=1 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 --preload-file f --use-preload-plugins")
		set(CMAKE_EXECUTABLE_SUFFIX .html)
	else()
		find_package(SDL2 REQUIRED)
		include_directories(${SDL2_INCLUDE_DIRS})

		find_package(Freetype REQUIRED)
	endif()
	# stuff like glad.h, khr, also SDL and freetype headers
	include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/include/)

	set(default_build_type "Release")

	add_executable(${PROJECT_NAME})
	target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
	target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${FREETYPE_LIBRARIES})
endif()
