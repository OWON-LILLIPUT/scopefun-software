cmake_minimum_required (VERSION 2.8)

# pic ?
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# permissions
set(CMAKE_INSTALL_DEFAULT_DIRECTORY_PERMISSIONS
     OWNER_READ
     OWNER_WRITE
     OWNER_EXECUTE
     GROUP_READ
   )

# timestamp
string(TIMESTAMP SCOPEFUN_VERSION_TIMESTAMP "%H:%M %d-%m-%Y")

# version
set (SCOPEFUN_VERSION_MAJOR  "2" CACHE STRING "major version" )
set (SCOPEFUN_VERSION_MINOR  "0" CACHE STRING "minor version" )
set (SCOPEFUN_VERSION_MICRO  "0" CACHE STRING "micro version" )

# 32bit | 64bit
set(SCOPEFUN_32BIT FALSE      CACHE BOOL "compile for 32bits?" )
set(SCOPEFUN_64BIT TRUE       CACHE BOOL "compile for 64bits?" )

# type
if (${CMAKE_BUILD_TYPE} MATCHES "^[Rr]elease")
	set (SCOPEFUN_VERSION_TYPE "release version" CACHE STRING "version information string" FORCE)
else()
   	set (SCOPEFUN_VERSION_TYPE "not for release" CACHE STRING "version information string" FORCE)
endif()

# debug | release
if (${CMAKE_BUILD_TYPE} MATCHES "^[Rr]elease")
	if( SCOPEFUN_32BIT )
		set(SCOPEFUN_TYPE "32r" CACHE STRING "temp build directory name suffix" FORCE)
		set(SCOPEFUN_BITS "32" CACHE STRING "bits" FORCE )
	endif()
	if( SCOPEFUN_64BIT )
		set(SCOPEFUN_TYPE  "64r" CACHE STRING "temp build directory name suffix" FORCE)
		set(SCOPEFUN_BITS  "64"  CACHE STRING "bits" FORCE )
	endif()
endif()

if (${CMAKE_BUILD_TYPE} MATCHES "^[Dd]ebug")
   	if( SCOPEFUN_32BIT )
		set(SCOPEFUN_TYPE "32d" CACHE STRING "temp build directory name suffix" FORCE)
		set(SCOPEFUN_BITS "32"  CACHE STRING "bits" FORCE )
	endif()
	if( SCOPEFUN_64BIT )
		set(SCOPEFUN_TYPE "64d" CACHE STRING "temp build directory name suffix" FORCE)
		set(SCOPEFUN_BITS "64"  CACHE STRING "bits" FORCE )
	endif()
endif()

# visual studio ?
set(SCOPEFUN_VISUALSTUDIO_X64 FALSE CACHE BOOL "visual studio build 64bit")
set(SCOPEFUN_VISUALSTUDIO_X86 FALSE CACHE BOOL "visual studio build 32bit")

# platform
if (WIN32)
    set(SCOPEFUN_WINDOWS TRUE CACHE BOOL "os type")
    set(SCOPEFUN_LINUX   FALSE CACHE BOOL "os type")
    set(SCOPEFUN_MACOSX  FALSE CACHE BOOL "os type")
endif()

if (UNIX AND NOT APPLE)
    set(SCOPEFUN_WINDOWS FALSE CACHE BOOL "os type")
    set(SCOPEFUN_LINUX   TRUE CACHE BOOL "os type")
    set(SCOPEFUN_MACOSX  FALSE CACHE BOOL "os type")
endif()

if (APPLE)
    set(SCOPEFUN_WINDOWS FALSE CACHE BOOL "os type")
    set(SCOPEFUN_LINUX   FALSE CACHE BOOL "os type")
    set(SCOPEFUN_MACOSX  TRUE  CACHE BOOL "os type")
endif()

# license helper
set(SCOPEFUN_LICENSE_HELPER FALSE CACHE BOOL "header helper")
 
# bundle
if(MACOSX)
	set(CMAKE_MACOSX_BUNDLE "true" )
endif()

# flags
if(WINDOWS)
	set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -mwindows -Wl,-subsystem,windows")
endif()

# project
project(ScopeFun)

# subfolders
add_subdirectory(lib)
add_subdirectory(source)
