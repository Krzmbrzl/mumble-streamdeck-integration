# Copyright 2021 The Mumble Developers. All rights reserved.
# Use of this source code is governed by a BSD-style license
# that can be found in the LICENSE file at the root of the
# source tree.

option(static "Prefer static libraries instead of shared ones" OFF)

set(JSON_BuildTests OFF CACHE INTERNAL "")
set(JSON_Install OFF CACHE INTERNAL "")
set(JSON_ImplicitConversions OFF CACHE INTERNAL "")

add_subdirectory("${3RDPARTY_DIR}/nlohmann" "nlohmann")
add_subdirectory("${3RDPARTY_DIR}/websocketpp" "websocketpp")

add_executable(streamdeck_integration
	main.cpp
	MumblePlugin.cpp
	ConnectionManager.cpp
	Utils.cpp
)

set(Boost_USE_STATIC_LIBS ${static})
find_package(Boost COMPONENTS thread random regex filesystem REQUIRED)

target_link_libraries(streamdeck_integration
	${Boost_LIBRARIES}
)

target_include_directories(streamdeck_integration PRIVATE
	"${3RDPARTY_DIR}/nlohmann/include"
	"${3RDPARTY_DIR}/websocketpp/"
	${Boost_INCLUDE_DIRS}
)
