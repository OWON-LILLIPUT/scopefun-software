cmake_minimum_required (VERSION 2.8)

# include and libs
include( ${CMAKE_SOURCE_DIR}/source/sfLibs.cmake   )

# include
include_directories("${CMAKE_SOURCE_DIR}/source" "${SCOPEFUN_LIB_INCLUDE_USB}" "${SCOPEFUN_LIB_INCLUDE_USB_EZ}" "${SCOPEFUN_LIB_INCLUDE_SDL2}" "${SCOPEFUN_LIB_INCLUDE_WX_SETUP}" "${SCOPEFUN_LIB_INCLUDE_WX}" "${SCOPEFUN_LIB_INCLUDE_CJSON}" "${SCOPEFUN_LIB_INCLUDE_KISSFFT}" "${SCOPEFUN_LIB_INCLUDE_GLEW}" "${SCOPEFUN_LIB_INCLUDE_PYTHON}" "${SCOPEFUN_LIB_INCLUDE_PYTHON_PC}")

# resource
if(SCOPEFUN_WINDOWS)
execute_process(
    COMMAND windres 	-I ${CMAKE_SOURCE_DIR}/lib/wxWidgets-3.0.4/include -J rc -O coff -i ${CMAKE_SOURCE_DIR}/source/osciloscope/rc/osc.rc -o ${CMAKE_SOURCE_DIR}/source/osciloscope/rc/osc.res
    WORKING_DIRECTORY  ${CMAKE_SOURCE_DIR}/source/osciloscope/rc/
)        
endif(SCOPEFUN_WINDOWS)

# version
CONFIGURE_FILE(${CMAKE_SOURCE_DIR}/source/sfversion.h.in ${CMAKE_SOURCE_DIR}/source/sfversion.h @ONLY)

if(SCOPEFUN_WINDOWS)
	set( SCOPEFUN_EXE_TYPE WIN32 )
endif()
if(SCOPEFUN_MACOSX)
	set( SCOPEFUN_EXE_TYPE MACOSX_BUNDLE )
endif()
if(SCOPEFUN_LINUX)
	set( SCOPEFUN_EXE_TYPE "" )
endif()

# include
if (${CMAKE_BUILD_TYPE} MATCHES "^[Rr]elease")
	include( ${CMAKE_SOURCE_DIR}/source/sfApi.cmake    )
endif()
include( ${CMAKE_SOURCE_DIR}/source/sfServer.cmake )
include( ${CMAKE_SOURCE_DIR}/source/sfScope.cmake  )

# license helper
if(SCOPEFUN_LICENSE_HELPER AND SCOPEFUN_WINDOWS)
	add_executable( sfLicense WIN32 "${CMAKE_SOURCE_DIR}/source/license/source.cpp" )
	set_target_properties(sfLicense PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin" )
endif()

# common defines
add_definitions( 
-D__WX__ 
-D_FILE_OFFSET_BITS=64 
-D_LARGE_FILES
-DwxUSE_UNICODE=1
-DwxUSE_UNICODE_UTF8=0
-DwxUSE_UNICODE_WCHAR=1
-DSDL_MAIN_HANDLED
-DGLEW_STATIC
-DGLEW_NO_GLU
-DDBL_EPSILON=0.0000000000001
-DUSE_LOCKS=1
-DUSE_DL_PREFIX
-DwxUSE_COMBOCTRL=1
-DwxUSE_GLOBAL_MEMORY_OPERATORS=0 )

# debug | release
if (${CMAKE_BUILD_TYPE} MATCHES "^[Rr]elease")
	 add_definitions(-DNDEBUG)
endif()
if (${CMAKE_BUILD_TYPE} MATCHES "^[Dd]ebug")
   	 add_definitions(-D_DEBUG)
endif()

# WINDOWS
if(SCOPEFUN_WINDOWS)
if( SCOPEFUN_VISUALSTUDIO_X86 )
   add_definitions(-DWIN32)
   add_definitions(-D__WIN32__)
   add_definitions(-DMS_WIN32)
   add_definitions(-DPLATFORM_WIN)
   add_definitions(-DCODEBLOCKS)
   add_definitions(-D_WINDOWS)
elseif(SCOPEFUN_VISUALSTUDIO_X64)
   add_definitions(-DWIN64)
   add_definitions(-D__WIN64__)
   add_definitions(-DMS_WIN64)
   add_definitions(-DPLATFORM_WIN)
   add_definitions(-DCODEBLOCKS)
   add_definitions(-D_WINDOWS)
else()
   if(SCOPEFUN_32BIT)
      add_definitions(-DWIN32)
      add_definitions(-D__WIN32__)
      add_definitions(-D__MINGW32__)
      add_definitions(-DMS_WIN32)
   else()
      add_definitions(-DWIN64)
      add_definitions(-D__WIN64__)
      add_definitions(-D__MINGW64__)
      add_definitions(-DMS_WIN64)
   endif()
   add_definitions(-DPLATFORM_MINGW)
   add_definitions(-DCODEBLOCKS)
   add_definitions(-D_WINDOWS)
endif()
endif(SCOPEFUN_WINDOWS)

# macos
if(SCOPEFUN_MACOSX)
   add_definitions(-DPLATFORM_MAC)
   add_definitions(-DMAC)
   add_definitions(-D__WXOSX_COCOA__)
   add_definitions(-D__WXOSX__)
   add_definitions(-D__WXMAC__)
   add_definitions(-D_WXMAC_XCODE__=1)
endif()

# linux
if(SCOPEFUN_LINUX)
   add_definitions(-DPLATFORM_LINUX)
   add_definitions(-DLINUX)
   add_definitions(-D__LINUX__)
   add_definitions(-D__WXGTK__)
endif(SCOPEFUN_LINUX)

# permissions
set(ALL_PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_WRITE WORLD_EXECUTE )

# install
if(SCOPEFUN_LINUX)
    install( DIRECTORY "${CMAKE_SOURCE_DIR}/package/scopefun-linux-${SCOPEFUN_BITS}/DEBIAN"   DESTINATION "../" 
	FILE_PERMISSIONS ${ALL_PERMISSIONS} DIRECTORY_PERMISSIONS  ${ALL_PERMISSIONS})
    install( DIRECTORY "${CMAKE_SOURCE_DIR}/package/scopefun-linux-${SCOPEFUN_BITS}/usr"      DESTINATION "../" 
	FILE_PERMISSIONS ${ALL_PERMISSIONS} DIRECTORY_PERMISSIONS  ${ALL_PERMISSIONS})
    install( DIRECTORY "${CMAKE_SOURCE_DIR}/package/scopefun-linux-${SCOPEFUN_BITS}/lib"      DESTINATION "../" 
	FILE_PERMISSIONS ${ALL_PERMISSIONS} DIRECTORY_PERMISSIONS  ${ALL_PERMISSIONS})
    install( DIRECTORY "${CMAKE_SOURCE_DIR}/bin/locale"                                       DESTINATION "./share" 
	FILE_PERMISSIONS ${ALL_PERMISSIONS} DIRECTORY_PERMISSIONS  ${ALL_PERMISSIONS})
    install( DIRECTORY "${CMAKE_SOURCE_DIR}/bin/data"                                         DESTINATION "./lib/oscilloscope" FILE_PERMISSIONS ${ALL_PERMISSIONS} DIRECTORY_PERMISSIONS  ${ALL_PERMISSIONS})
    install(TARGETS sfScope sfServer
            PERMISSIONS ${ALL_PERMISSIONS}
            RUNTIME  DESTINATION "./bin"
            BUNDLE   DESTINATION "./bin"
            LIBRARY  DESTINATION "./share/api" )
	install( FILES "${CMAKE_SOURCE_DIR}/source/api/scopefunapi.py"  DESTINATION "./share/api" PERMISSIONS ${ALL_PERMISSIONS} )
	install( FILES "${CMAKE_SOURCE_DIR}/source/test/capture.py"     DESTINATION "./share/api" PERMISSIONS ${ALL_PERMISSIONS} )
	install( FILES "${CMAKE_SOURCE_DIR}/source/test/histogram.py"   DESTINATION "./share/api" PERMISSIONS ${ALL_PERMISSIONS} )
else()
	install(TARGETS sfScope sfServer
	        PERMISSIONS ${ALL_PERMISSIONS}
			RUNTIME  DESTINATION "."
			BUNDLE   DESTINATION "."
			LIBRARY  DESTINATION "api" )
	install( FILES "${CMAKE_SOURCE_DIR}/source/api/scopefunapi.py"  DESTINATION "api" PERMISSIONS ${ALL_PERMISSIONS} )
	install( FILES "${CMAKE_SOURCE_DIR}/source/test/capture.py"     DESTINATION "api" PERMISSIONS ${ALL_PERMISSIONS} )
	install( FILES "${CMAKE_SOURCE_DIR}/source/test/histogram.py"   DESTINATION "api" PERMISSIONS ${ALL_PERMISSIONS} )
endif()

if(SCOPEFUN_WINDOWS)
    install( DIRECTORY "${CMAKE_SOURCE_DIR}/bin/data" DESTINATION "." FILE_PERMISSIONS ${ALL_PERMISSIONS} DIRECTORY_PERMISSIONS  ${ALL_PERMISSIONS} )
    install( DIRECTORY "${CMAKE_SOURCE_DIR}/bin/locale" DESTINATION "." FILE_PERMISSIONS ${ALL_PERMISSIONS} DIRECTORY_PERMISSIONS  ${ALL_PERMISSIONS} )
    install( DIRECTORY "${CMAKE_SOURCE_DIR}/bin/api" DESTINATION "." FILE_PERMISSIONS ${ALL_PERMISSIONS} DIRECTORY_PERMISSIONS  ${ALL_PERMISSIONS} )
endif()

# macosx packing
if(SCOPEFUN_MACOSX)
	add_custom_command(
			TARGET sfScope POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_directory
					${CMAKE_SOURCE_DIR}/bin/data
					${CMAKE_SOURCE_DIR}/bin/sfScope${SCOPEFUN_TYPE}.app/Contents/Resources/data)
					
	add_custom_command(
			TARGET sfScope POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_directory
					${CMAKE_SOURCE_DIR}/bin/locale
					${CMAKE_SOURCE_DIR}/bin/sfScope${SCOPEFUN_TYPE}.app/Contents/Resources/locale)
					
	add_custom_command(
			TARGET sfScope POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy
					${CMAKE_SOURCE_DIR}/package/scopefun-mac/AppIcon.icns
					${CMAKE_SOURCE_DIR}/bin/sfScope${SCOPEFUN_TYPE}.app/Contents/Resources)
	
	add_custom_command(
			TARGET sfServer POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E make_directory
					${CMAKE_SOURCE_DIR}/bin/sfServer${SCOPEFUN_TYPE}.app/Contents/Resources)
					
	add_custom_command(
			TARGET sfServer POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy
					${CMAKE_SOURCE_DIR}/package/scopefun-mac/AppIcon.icns
					${CMAKE_SOURCE_DIR}/bin/sfServer${SCOPEFUN_TYPE}.app/Contents/Resources)
					
	set(MACOSX_BUNDLE_INFO_STRING           "ScopeFun open source oscilloscope" )
	set(MACOSX_BUNDLE_ICON_FILE    			AppIcon )
	set(MACOSX_BUNDLE_GUI_IDENTIFIER		"ScopeFun.Oscilloscope" )
	set(MACOSX_BUNDLE_BUNDLE_NAME           "ScopeFun" )
	set(MACOSX_BUNDLE_LONG_VERSION_STRING   ${SCOPEFUN_VERSION_MAJOR}.${SCOPEFUN_VERSION_MINOR}.${SCOPEFUN_VERSION_MICRO} )
	set(MACOSX_BUNDLE_SHORT_VERSION_STRING  ${SCOPEFUN_VERSION_MAJOR}.${SCOPEFUN_VERSION_MINOR}.${SCOPEFUN_VERSION_MICRO} )
	set(MACOSX_BUNDLE_BUNDLE_VERSION        ${SCOPEFUN_VERSION_MAJOR}.${SCOPEFUN_VERSION_MINOR}.${SCOPEFUN_VERSION_MICRO} )
	set(MACOSX_BUNDLE_COPYRIGHT             "Copyright (C) 2016 - 2019 David Košenina" )

	set(CPACK_BUNDLE_PLIST ${CMAKE_SOURCE_DIR}/package/info.plist )
	set(CPACK_BUNDLE_ICON  ${CMAKE_SOURCE_DIR}/package/scopefun-mac/appicon.icns )
	set(CPACK_BUNDLE_NAME "ScopeFun")
endif()

# windows packing
if(SCOPEFUN_WINDOWS)
	# generator
	set(CPACK_GENERATOR "NSIS") 

	# version
	set(CPACK_NSIS_PACKAGE_NAME "ScopeFun" )

	# create links
	LIST(APPEND CPACK_NSIS_CREATE_ICONS_EXTRA  "
		CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\sfServer.lnk' '$INSTDIR\\\\sfServer${SCOPEFUN_TYPE}.exe'
		CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\sfScope.lnk'  '$INSTDIR\\\\sfScope${SCOPEFUN_TYPE}.exe'
		CreateShortCut '$DESKTOP\\\\sfScope.lnk'  '$INSTDIR\\\\sfScope${SCOPEFUN_TYPE}.exe'
		CreateShortCut '$DESKTOP\\\\sfServer.lnk' '$INSTDIR\\\\sfServer${SCOPEFUN_TYPE}.exe'")
		
	# delete links
	LIST(APPEND CPACK_NSIS_DELETE_ICONS_EXTRA  "  
		Delete '$SMPROGRAMS\\\\$START_MENU\\\\sfScope.lnk'
		Delete '$SMPROGRAMS\\\\$START_MENU\\\\sfServer.lnk'
		Delete '$DESKTOP\\\\sfScope.lnk'
		Delete '$DESKTOP\\\\sfServer.lnk'")

	#icons
	set(CPACK_NSIS_MUI_ICON                       "${CMAKE_SOURCE_DIR}/package/scopefun-win\\\\logo.ico")
	set(CPACK_NSIS_MUI_UNIICON                    "${CMAKE_SOURCE_DIR}/package/scopefun-win\\\\logo.ico")
	set(CPACK_NSIS_MUI_WELCOMEFINISHPAGE_BITMAP   "${CMAKE_SOURCE_DIR}/package/scopefun-win\\\\setup.bmp")
	set(CPACK_NSIS_MUI_UNWELCOMEFINISHPAGE_BITMAP "${CMAKE_SOURCE_DIR}/package/scopefun-win\\\\setup.bmp")
endif(SCOPEFUN_WINDOWS)

# url
set( CPACK_PACKAGE_HOMEPAGE_URL www.scopefun.com)

#icons
if(SCOPEFUN_MACOSX)
	set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/package/header.bmp")
else()
	set(CPACK_PACKAGE_ICON "${CMAKE_SOURCE_DIR}/package\\\\header.bmp")
endif()

# license
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/COPYING.TXT" )

# version
set(CPACK_PACKAGE_VERSION_MAJOR ${SCOPEFUN_VERSION_MAJOR} )
set(CPACK_PACKAGE_VERSION_MINOR ${SCOPEFUN_VERSION_MINOR} )
set(CPACK_PACKAGE_VERSION_PATCH ${SCOPEFUN_VERSION_MICRO} )

#vendor
set(CPACK_PACKAGE_VENDOR "ScopeFun" )

#vendor
set(CPACK_PACKAGE_CONTACT "info@scopefun.com" )

#description
set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_SOURCE_DIR}/package/desc.txt" ) 
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Open Source Oscilloscope" )

#checksum
set(CPACK_PACKAGE_CHECKSUM SHA512)

#source
set(CPACK_SOURCE_IGNORE_FILES  "" 
"/\\\\.git/;"
"/\\\\sfExe/;"
"/\\\\sfSrc/;"
"/\\\\bin/sfServer64r.exe/;"
"/\\\\bin/sfScope64r.exe/;"
"/\\\\bin/_scopefunapi.pyd/;"
"/\\\\lib/libusb-1.0.22/sfBuild64r/;"
"/\\\\lib/libusb-1.0.22/src/;"
"/\\\\lib/libusb-1.0.22/tmp/;"
"/\\\\lib/SDL2-2.0.9/sfBuild64r/;"
"/\\\\lib/SDL2-2.0.9/tmp/;"
"/\\\\lib/wxWidgets-3.0.4/sfBuild64r/;"
"/\\\\lib/wxWidgets-3.0.4/src/wxWidgets304-stamp/;"
"/\\\\lib/wxWidgets-3.0.4/tmp/;"
)

# pack
include(CPack)
