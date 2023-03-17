cmake_minimum_required(VERSION 3.7)

project(FallingGameCmake)

set(CMAKE_CXX_STANDARD 20)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message("Setting build type to 'RelWithDebInfo' as none was specified.")
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
    "MinSizeRel" "RelWithDebInfo")
endif()

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(Freetype REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/include/)

set(default_build_type "Release")

set(SOURCES
	Bouncer.cpp Bouncer.h Camera.cpp Camera.h Cloud.cpp Cloud.h Coin.cpp Coin.h CoinParticle.cpp CoinParticle.h Drawer.cpp Drawer.h Game.cpp Game.h Layer.cpp Layer.h main.cpp misc.cpp misc.h Player.cpp Player.h stb_image.h glad.c 
)



add_executable(${PROJECT_NAME} ${SOURCES})
#target_link_libraries(${PROJECT_NAME} ${FREETYPE_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${FREETYPE_LIBRARIES})
