# Lifish CMakeLists
# by silverweed
#
# Command-line switches:
# - RELEASE: compile in release mode (optimization, hardening and no debug)
# - USE_STATIC_SFML: force to use static SFML libs and dependencies (default on windows)
# - MULTITHREADED: use multithreaded rendering (unstable, do NOT use)

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(lifish)
set(PROJECT_NAME lifish)
set(MAJOR 1)
set(MINOR 1)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

# detect architecture
if(CMAKE_SIZEOF_VOID_P MATCHES 8)
	set(TARGET_ARCH 64)
else()
	set(TARGET_ARCH 32)
endif()

# Setup source files
# main
set(MAIN_FILE src/main.cpp)
# core/ contains the "game-independent" source
include_directories(src/core)
# core/collisions contains the classes handling collisions
include_directories(src/core/collisions)
# core/components/ contains a collection of generic components
include_directories(src/core/components)
# core/cutscenes/ contains the cutscene subsystem
include_directories(src/core/cutscenes)
# core/entities/ contains a collection of generic entities
include_directories(src/core/entities)
# core/input/ contains classes handling events and player input
include_directories(src/core/input)
# lifish/ contains Lifish-specific code
include_directories(src/lifish)
# lifish/components/ contains all the Lifish-specific Component classes
include_directories(src/lifish/components)
# lifish/conf/ contains the gameplay constants
#include_directories(src/lifish/conf)
# lifish/entities/ contains all the Lifish-specific Entity classes
include_directories(src/lifish/entities)
# lifish/level/ contains classes for managing game levels
include_directories(src/lifish/level)
# third_party/ contains all external code
include_directories(src/third_party)
# ui/ contains the ui parts
include_directories(src/ui)
file(GLOB LIFISH_SRC ${MAIN_FILE}
	src/core/*cpp
	src/core/collisions/*cpp
	src/core/components/*cpp
	src/core/cutscenes/*cpp
	src/core/entities/*cpp
	src/core/input/*cpp
	src/lifish/*cpp
	src/lifish/components/*cpp
	src/lifish/conf/*cpp
	src/lifish/entities/*cpp
	src/lifish/level/*cpp
	src/third_party/*cpp
	src/ui/*cpp
)
if(NOT RELEASE)
	# core/debug/ contains debug utils
	include_directories(src/core/debug)
	# lifish/debug/ contains all the Lifish-specific debugging utilities
	include_directories(src/lifish/debug)
	file(GLOB LIFISH_DEBUG_SRC
		src/core/debug/*cpp
		src/lifish/debug/*cpp
	)
	set(LIFISH_SRC ${LIFISH_SRC} ${LIFISH_DEBUG_SRC})
endif()

if(MSVC)
	include_directories(src/third_party/msvc)
endif()

# Remove files which aren't being used for now
list(REMOVE_ITEM LIFISH_SRC
	"${CMAKE_SOURCE_DIR}/src/core/collisions/SimpleCollisionDetector.cpp"
	#"${CMAKE_SOURCE_DIR}/src/core/cutscenes/Cutscene.cpp"
	"${CMAKE_SOURCE_DIR}/src/lifish/entities/AlienBoss.cpp"
	"${CMAKE_SOURCE_DIR}/src/lifish/entities/Cage.cpp"
	#"${CMAKE_SOURCE_DIR}/src/lifish/entities/Torch.cpp"
	#"${CMAKE_SOURCE_DIR}/src/lifish/entities/TimedLaser.cpp"
)

# Platform-specific source
if(MSYS)
	include_directories(src)
	file(GLOB WIN_LIFISH_SRC src/win/*.cpp)	
	add_definitions(-D_WIN32_WINNT=_WIN32_WINNT_VISTA)
	set(LIFISH_SRC ${LIFISH_SRC} ${WIN_LIFISH_SRC})
endif()

add_executable(${PROJECT_NAME} ${LIFISH_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    LINKER_LANGUAGE CXX
)

# Add git commit information (thanks, hamcha)
include(FindGit OPTIONAL)
if(GIT_FOUND)
	execute_process(WORKING_DIRECTORY ${PROJECT_SOURCE_DIR} COMMAND 
	               ${GIT_EXECUTABLE} describe --always --long --dirty
                       OUTPUT_VARIABLE CURRENTREV
                       OUTPUT_STRIP_TRAILING_WHITESPACE)
	string(REGEX REPLACE "(-0)?-[^-]+((-dirty)?)$" "\\2" CURRENTREV "${CURRENTREV}")
else()
	set(CURRENTREV "unknown")
endif()

add_definitions(-DVERSION="${MAJOR}.${MINOR}" -DCOMMIT="${CURRENTREV}" -DARCH="${TARGET_ARCH}")

# Check for ccache
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
	message(STATUS "Found ccache: compilation will use it.")
	set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
	set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif()

# SFML
if(MSYS)
	set(USE_STATIC_SFML 1)
endif()
if(USE_STATIC_SFML)
	message(STATUS "Using static SFML libs")
	set(SFML_STATIC_LIBRARIES true)
endif()
find_package(SFML 2.4 REQUIRED graphics window audio system)
include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${SFML_LIBRARIES})
if(USE_STATIC_SFML)
	target_link_libraries(${PROJECT_NAME} ${SFML_DEPENDENCIES})
	if(UNIX AND NOT APPLE)
		find_package(X11 REQUIRED)
		target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES} ${X11_Xrandr_LIB})
	endif()
endif()

# Use separate threads for logic and rendering (experimental)
if(MULTITHREADED)
	message(STATUS "Compiling in MULTITHREADED mode")
	add_definitions(-DMULTITHREADED=1)

	find_package(Threads REQUIRED)
	include_directories(${Threads_INCLUDE_DIR})
	target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})

	if(UNIX AND NOT APPLE)
		find_package(X11 REQUIRED)
		include_directories(${X11_INCLUDE_DIR})
		target_link_libraries(${PROJECT_NAME} ${X11_LIBRARIES})
	endif()
endif()

# OpenMP
#include(FindOpenMP)
#if(OPENMP_FOUND)
	#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
	#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
#endif()

# Check if we have support for PIE
include(CheckCXXCompilerFlag OPTIONAL RESULT_VARIABLE HAS_CHECK_CXX)
if(NOT ${HAS_CHECK_CXX} STREQUAL "")
	check_cxx_compiler_flag("-pie -fPIE -Wl,-fPIE" SUPPORTS_PIE)
	if(${SUPPORTS_PIE})
		if(NOT MSYS)
			#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pie -fPIE -Wl,-pie")
		endif()
	endif()
	check_cxx_compiler_flag("-Qunused-arguments" SUPPORTS_UNUSED_ARGS)
	if(${SUPPORTS_UNUSED_ARGS})
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Qunused-arguments")
	endif()
endif()

# Debug / Release
if(RELEASE)
	message(STATUS "Compiling in RELEASE mode")
	# Disable assertions
	add_definitions(-DNDEBUG=1)
	add_definitions(-DRELEASE=1)
	if(MSVC)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FORTIFY_SOURCE=2 /O2 /MP /W2 /std:c++14")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FORTIFY_SOURCE=2 -O2 -Wall -pedantic -Wextra")
	endif()

	# Hardening
	if(NOT ${HAS_CHECK_CXX} STREQUAL "")
		check_cxx_compiler_flag("-fstack-protector-strong" SUPPORTS_STACK_PROTECTOR_STRONG)
		if(${SUPPORTS_STACK_PROTECTOR_STRONG})
			set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstack-protector-strong")
		endif()
		if(NOT MSYS)
			check_cxx_compiler_flag("-Wl,-z,relro,-z,now" SUPPORTS_RELRO)
			if(${SUPPORTS_RELRO})
				set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wl,-z,relro,-z,now")
			endif()
		endif()
	endif()
else()
	message(STATUS "Compiling in DEBUG mode")
	if(MSVC)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Od /MP /W2 /std:c++14")
	else()
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -ggdb -Wall -pedantic -Wextra")
	endif()

	# Profiling tools
	if(${GPROF})
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
		message(STATUS "Compiling with gprof support")
	endif()
	if(${PPROF_ALL})
		find_package(Gperftools)
		if(GPERFTOOLS_FOUND)
			target_link_libraries(${PROJECT_NAME} ${GPERFTOOLS_LIBRARIES})
                        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-builtin-malloc -fno-builtin-calloc -fno-builtin-realloc -fno-builtin-free")
		endif()
	elseif(${PPROF})
		# Only CPU (tcmalloc can crash the program on some systems)
		find_package(Gperftools)
		if(GPERFTOOLS_FOUND)
			target_link_libraries(${PROJECT_NAME} ${GPERFTOOLS_PROFILER})
			message(STATUS "Compiling with gperftools")
		endif()
	endif()
endif()

# Linker
execute_process(
	COMMAND ${CMAKE_C_COMPILER} -fuse-ld=gold -Wl,--version
	ERROR_QUIET OUTPUT_VARIABLE LD_VERSION)
if("${LD_VERSION}" MATCHES "GNU gold")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fuse-ld=gold")
	message(STATUS "Using GNU gold as linker")
endif()

if(MSYS)
	if(RELEASE)
		# No terminal on windows in release mode
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows")
	endif()
endif()
