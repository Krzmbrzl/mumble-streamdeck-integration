# Copyright 2020 The Mumble Developers. All rights reserved.
# Use of this source code is governed by a BSD-style license
# that can be found in the LICENSE file at the root of the
# source tree.

cmake_minimum_required(VERSION 3.15)

set(version "0.1.0" CACHE STRING "Project version")

project(mumble-streamdeck-integration
	VERSION ${version}
	DESCRIPTION "A plugin for integrating Mumble with Elgato's Stream Deck"
	LANGUAGES "C" "CXX"
)

set(3RDPARTY_DIR "${CMAKE_SOURCE_DIR}/3rdParty")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory("src")
