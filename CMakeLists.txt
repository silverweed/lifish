cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(lifish)
set(PROJECT_NAME lifish)
set(MAJOR 1)
set(MINOR 0)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

include(CheckCXXCompilerFlag OPTIONAL RESULT_VARIABLE HAS_CHECK_CXX)

# detect architecture
if(CMAKE_SIZEOF_VOID_P MATCHES 8)
	set(TARGET_ARCH 64)
else()
	set(TARGET_ARCH 32)
endif()

# main
set(MAIN_FILE src/main.cpp)
# core/ contains the "game-independent" source
include_directories(src/core)
# core/components/ contains a collection of generic components
include_directories(src/core/components)
# core/entities/ contains a collection of generic entities
include_directories(src/core/entities)
# core/collisions contains the classes handling collisions
include_directories(src/core/collisions)
# ui/ contains the ui parts
include_directories(src/ui)
# third_party/ contains all external code
include_directories(src/third_party)
# lifish/ contains Lifish-specific code
include_directories(src/lifish)
# lifish/components/ contains all the Lifish-specific Component classes
include_directories(src/lifish/components)
# lifish/entities/ contains all the Lifish-specific Entity classes
include_directories(src/lifish/entities)
# lifish/level/ contains classes for managing game levels
include_directories(src/lifish/level)
# lifish/conf/ contains the gameplay constants
#include_directories(src/lifish/conf)
file(GLOB LIFISH_SRC ${MAIN_FILE}
	src/core/*cpp
	src/core/entities/*cpp
	src/core/components/*cpp
	src/core/collisions/*cpp
	src/ui/*cpp
	src/third_party/*cpp
	src/lifish/*cpp
	src/lifish/entities/*cpp
	src/lifish/components/*cpp
	src/lifish/level/*cpp
	src/lifish/conf/*cpp
)
if(${RELEASE})
else()
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

# Platform-specific source
if(MSYS)
	include_directories(src)
	file(GLOB WIN_LIFISH_SRC src/win/*.cpp)	
	add_definitions(-D_WIN32_WINNT=_WIN32_WINNT_VISTA)
	set(LIFISH_SRC ${LIFISH_SRC} ${WIN_LIFISH_SRC})
endif()
add_executable(${PROJECT_NAME} ${LIFISH_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

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
endif(CCACHE_FOUND)

# SFML
find_package(SFML 2.4 REQUIRED graphics window audio system)
include_directories(${SFML_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${SFML_LIBRARIES})

# Use separate threads for logic and rendering (experimental)
if(${MULTITHREADED})
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
	add_definitions(-DRELEASE=1)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FORTIFY_SOURCE=2 -O2 -Wall -pedantic -Wextra")

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
			target_link_libraries(${PROJECT_NAME} "-lprofiler")
			message(STATUS "Compiling with gperftools")
		endif()
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
