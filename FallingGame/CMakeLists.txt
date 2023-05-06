cmake_minimum_required(VERSION 3.16)

project(FallingGameCmake C CXX)

set(CMAKE_CXX_STANDARD 17)


set(HEADERS Bouncer.hpp Camera.hpp Cloud.hpp Coin.hpp CoinParticle.hpp Drawer.hpp Game.hpp Layer.hpp misc.hpp Player.hpp GameState.hpp)

# just c or cpp files
set(REAL_SOURCE_FILES
	glad.c GameState.cpp Bouncer.cpp Camera.cpp Cloud.cpp Coin.cpp CoinParticle.cpp Drawer.cpp Game.cpp Layer.cpp main.cpp misc.cpp Player.cpp
	)

# all the files
set(SOURCE_FILES
	${HEADERS}
	${REAL_SOURCE_FILES}
	)


# stuff like glad.h, khr, also SDL and freetype headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/include/)

if(BUILD_SHARED_LIBS)
	add_library(main SHARED)
	target_sources(main PRIVATE ${SOURCE_FILES})


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


	add_executable(${PROJECT_NAME} ${SOURCE_FILES})
	if(MSVC)
			target_compile_options(${PROJECT_NAME} PRIVATE /W4 /WX)
	else()
			target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic)
	endif()
	target_link_libraries(${PROJECT_NAME} m ${SDL2_LIBRARIES} ${FREETYPE_LIBRARIES})
endif()
