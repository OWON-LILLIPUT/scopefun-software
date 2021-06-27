cmake_minimum_required (VERSION 2.8)

# libusb
if(SCOPEFUN_VISUALSTUDIO_X64 OR SCOPEFUN_VISUALSTUDIO_X86)
  # currently do custom build of libUSB
  # include_external_msproject(libUsb123 ${CMAKE_SOURCE_DIR}/lib/libusb-1.0.22/msvc/libusb_static_2017.vcxproj  )
else()
	if(SCOPEFUN_MACOSX)
	   set(LIBUSBPLATFORM  --disable-shared  --enable-shared=no )
	endif()
	if(SCOPEFUN_LINUX)
	   set(LIBUSBPLATFORM  --disable-shared --with-pic --enable-static --enable-shared=no )
	endif()
	if(SCOPEFUN_WINDOWS)
	   set(LIBUSBPLATFORM  --disable-shared  --enable-shared=no )
	endif()
	include(ExternalProject)
	externalproject_add(libUsb1022
		PREFIX            ${CMAKE_SOURCE_DIR}/lib/libusb-1.0.22
		BINARY_DIR        ${CMAKE_SOURCE_DIR}/lib/libusb-1.0.22/sfBuild${SCOPEFUN_TYPE} 
		SOURCE_DIR        ${CMAKE_SOURCE_DIR}/lib/libusb-1.0.22
		CONFIGURE_COMMAND sh ../configure ${LIBUSBPLATFORM}
		BUILD_COMMAND     make
		UPDATE_COMMAND    ""
		INSTALL_COMMAND   ""
	 )
	if(SCOPEFUN_32BIT)
	  set_target_properties(libUsb1022 PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
	endif ()
endif()


# SDL2204
if(SCOPEFUN_VISUALSTUDIO_X64 OR SCOPEFUN_VISUALSTUDIO_X86)
   # currently do custom build of SDL2
   # include_external_msproject(SDL2 ${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9/VisualC/SDL/SDL.vcxproj  )
else()
	if(SCOPEFUN_MACOSX)
		set(SDL2PLATFORM --disable-shared --with-pic --disable-joystick --with-macosx-version-min=10.9)
	endif()
	if(SCOPEFUN_LINUX)
		set(SDL2PLATFORM --disable-shared --with-pic --disable-joystick )
	endif()
	if(SCOPEFUN_WINDOWS)
		set(SDL2PLATFORM --disable-shared --disable-joystick --disable-directx )
	endif()
	include(ExternalProject)
	externalproject_add(SDL2209   
		PREFIX            ${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9
		BINARY_DIR        ${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9/sfBuild${SCOPEFUN_TYPE} 
		SOURCE_DIR        ${CMAKE_SOURCE_DIR}/lib/SDL2-2.0.9
		CONFIGURE_COMMAND sh ../configure ${SDL2PLATFORM}
		BUILD_COMMAND     make 
		UPDATE_COMMAND    ""
		INSTALL_COMMAND   ""
	 )
	 if(SCOPEFUN_32BIT)
		set_target_properties(SDL2209 PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
	 endif ()
endif()

# wxWidgets
if(SCOPEFUN_VISUALSTUDIO_X64 OR SCOPEFUN_VISUALSTUDIO_X86)
	# currently you must download windows sources for wxWidgets-3.0.4 and compile them using nmake
else()
	if(SCOPEFUN_MACOSX)
		set(WXPLATFORM --disable-shared --with-macosx-version-min=10.9)
	else()
		set(WXPLATFORM --disable-shared)
	endif()
	include(ExternalProject)
	externalproject_add(wxWidgets304
		PREFIX            ${CMAKE_SOURCE_DIR}/lib/wxWidgets-3.0.4
		BINARY_DIR        ${CMAKE_SOURCE_DIR}/lib/wxWidgets-3.0.4/sfBuild${SCOPEFUN_TYPE} 
		SOURCE_DIR        ${CMAKE_SOURCE_DIR}/lib/wxWidgets-3.0.4
		CONFIGURE_COMMAND sh ../configure ${WXPLATFORM}
		BUILD_COMMAND     make 
		UPDATE_COMMAND    ""
		INSTALL_COMMAND   ""
	  )
	if(SCOPEFUN_32BIT)
	 set_target_properties(wxWidgets304 PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
	endif()
endif()

# Python
if(SCOPEFUN_MACOSX OR SCOPEFUN_LINUX)

include(ExternalProject)

if(SCOPEFUN_MACOSX)
externalproject_add(Python2716
    PREFIX            ${CMAKE_SOURCE_DIR}/lib/Python-2.7.16
    BINARY_DIR        ${CMAKE_SOURCE_DIR}/lib/Python-2.7.16/sfBuild${SCOPEFUN_TYPE} 
    SOURCE_DIR        ${CMAKE_SOURCE_DIR}/lib/Python-2.7.16
    CONFIGURE_COMMAND sh ../configure --enable-framework --with-pic
    BUILD_COMMAND     make 
    UPDATE_COMMAND    ""
    INSTALL_COMMAND   ""
  )
endif()

if(SCOPEFUN_LINUX)
externalproject_add(Python2716
    PREFIX            ${CMAKE_SOURCE_DIR}/lib/Python-2.7.16
    BINARY_DIR        ${CMAKE_SOURCE_DIR}/lib/Python-2.7.16/sfBuild${SCOPEFUN_TYPE} 
    SOURCE_DIR        ${CMAKE_SOURCE_DIR}/lib/Python-2.7.16
    CONFIGURE_COMMAND sh ../configure --enable-shared --with-pic
    BUILD_COMMAND     make 
    UPDATE_COMMAND    ""
    INSTALL_COMMAND   ""
  )
endif()
  
if(SCOPEFUN_32BIT)
 set_target_properties(Python2715 PROPERTIES COMPILE_FLAGS "-m32 -pic" LINK_FLAGS "-m32 -pic")
endif()

endif()
