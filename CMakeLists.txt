cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(lifish)
set(PROJECT_NAME lifish)
set(MAJOR 0)
set(MINOR 0)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

include(CheckCXXCompilerFlag OPTIONAL RESULT_VARIABLE HAS_CHECK_CXX)

# detect architecture
if(CMAKE_SIZEOF_VOID_P MATCHES 8)
	set(TARGET_ARCH 64)
else()
	set(TARGET_ARCH 32)
endif()

file(GLOB LIFISH_HEADERS src/*.hpp)
set(LIFISH_SRC src/collision_layers.cpp src/Killable.cpp src/Animation.cpp src/Bomb.cpp src/AnimatedSprite.cpp src/Animated.cpp src/EntityType.cpp src/GameCache.cpp src/Level.cpp tests/test_level.cpp src/LevelSet.cpp src/LevelNumText.cpp src/Music.cpp src/Game.cpp src/Texture.cpp src/Points.cpp src/ShadedText.cpp src/Track.cpp src/utils.cpp src/Entity.cpp src/EntityGroup.cpp src/Coin.cpp src/AxisMoving.cpp src/Moving.cpp src/controls.cpp src/Shooting.cpp src/MovingAnimator.cpp src/Player.cpp src/Enemy.cpp src/AlienSprite.cpp src/Direction.cpp src/Teleport.cpp src/Flash.cpp src/BossExplosion.cpp src/FixedWall.cpp src/BreakableWall.cpp src/TransparentWall.cpp src/Sprite.cpp src/Bullet.cpp src/AxisBullet.cpp src/FreeBullet.cpp src/FreeMoving.cpp src/Explosion.cpp src/Screen.cpp src/ScreenHandler.cpp src/HomeScreen.cpp src/Controllable.cpp src/ControlsScreen.cpp src/PreferencesScreen.cpp src/AboutScreen.cpp src/PauseScreen.cpp src/Sighted.cpp src/CollisionDetector.cpp src/Collider.cpp src/LevelManager.cpp src/LevelLoader.cpp src/LevelRenderer.cpp src/game_logic.cpp src/Bonus.cpp src/SidePanel.cpp src/LevelTime.cpp src/AI.cpp src/DroppingText.cpp)

# Platform-specific source
if(MSYS)
	file(GLOB WIN_LIFISH_SRC src/win/*.cpp)	
	add_definitions(-D_WIN32_WINNT=_WIN32_WINNT_VISTA)
	set(LIFISH_SRC ${LIFISH_SRC} ${WIN_LIFISH_SRC})
endif()
add_executable(${PROJECT_NAME} ${LIFISH_SRC} ${LIFISH_HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

# Add git commit information (thanks, hamcha)
include(FindGit OPTIONAL)
if(GIT_FOUND)
	execute_process(WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} COMMAND ${GIT_EXECUTABLE} describe --always --long --dirty
        OUTPUT_VARIABLE CURRENTREV
        OUTPUT_STRIP_TRAILING_WHITESPACE)
	string(REGEX REPLACE "(-0)?-[^-]+((-dirty)?)$" "\\2" CURRENTREV "${CURRENTREV}")
else()
	set(CURRENTREV "unknown")
endif()

add_definitions(-DVERSION="${MAJOR}.${MINOR}" -DCOMMIT="${CURRENTREV}" -DARCH="${TARGET_ARCH}")

# SFML
find_package(SFML 2.3 REQUIRED graphics window audio system)
include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${SFML_LIBRARIES})

# Check if we have support for PIE
if(NOT ${HAS_CHECK_CXX} STREQUAL "")
	check_cxx_compiler_flag("-pie -fPIE -Wl,-fPIE" SUPPORTS_PIE)
	if(${SUPPORTS_PIE})
		if(NOT MSYS)
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pie -fPIE -Wl,-pie")
		endif()
	endif()
	check_cxx_compiler_flag("-Qunused-arguments" SUPPORTS_UNUSED_ARGS)
	if(${SUPPORTS_UNUSED_ARGS})
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments")
	endif()
endif()


# On Linux / OSX we use NFD
if(UNIX AND NOT APPLE)
	find_package(GTK3 3.0 OPTIONAL_COMPONENTS gtk)
	if(GTK3_FOUND)
		add_definitions(-DHAVE_NFD="true")
		if(TARGET_ARCH EQUAL 64)
			if(${SUPPORTS_PIE})
				target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/libnfd_linux_x86_64_pie.a)
			else()
				target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/libnfd.a)
			endif()
		else()
			if(${SUPPORTS_PIE})
				target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/libnfd_linux_i386_pie.a)
			else()
				target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/libnfd-32.a)
			endif()
		endif()
		target_link_libraries(${PROJECT_NAME} ${GTK3_LIBRARIES})
	else()
		message(WARNING "Didn't find GTK: compiling without NFD support.")
	endif()
elseif(APPLE)
	find_library(APPKIT AppKit)
	add_definitions(-DHAVE_NFD="true")
	target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/libnfd-mac.a)
	target_link_libraries(${PROJECT_NAME} ${APPKIT})
endif()

if(CMAKE_VERSION VERSION_LESS 3.0.2)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
	target_compile_features(lifish PRIVATE cxx_range_for)
endif()

# Debug flags
if(${RELEASE})
	message(STATUS "Compiling in RELEASE mode")
	# Disable assertions
	add_definitions(-DNDEBUG=1)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -pedantic -Wextra")

	# Hardening
	if(NOT ${HAS_CHECK_CXX} STREQUAL "")
		check_cxx_compiler_flag("-fstack-protector-strong" SUPPORTS_STACK_PROTECTOR_STRONG)
		if(${SUPPORTS_STACK_PROTECTOR_STRONG})
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-protector-strong")
		endif()
		check_cxx_compiler_flag("-Wl,-z,relro,-z,now" SUPPORTS_RELRO)
		if(${SUPPORTS_RELRO})
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,-z,relro,-z,now")
		endif()
	endif()
else()
	message(STATUS "Compiling in DEBUG mode")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -ggdb -Wall -pedantic -Wextra")
endif()

# Profiling tools
if(${GPROF})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
	message(STATUS "Compiling with gprof support")
endif()
if(${PPROF})
	find_package(Gperftools)
	if(GPERFTOOLS_FOUND)
		target_link_libraries(${PROJECT_NAME} ${GPERFTOOLS_LIBRARIES})
		message(STATUS "Compiling with gperftools")
	endif()
endif()

# Linker / platform-specific flags
if(UNIX AND NOT APPLE)
	execute_process(
		COMMAND ${CMAKE_C_COMPILER} -fuse-ld=gold -Wl,--version
		ERROR_QUIET OUTPUT_VARIABLE LD_VERSION)
	if("${LD_VERSION}" MATCHES "GNU gold")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=gold")
		message(STATUS "Using GNU gold as linker")
	endif()
elseif(MSYS)
	if(${RELEASE})
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
	endif()
endif()
