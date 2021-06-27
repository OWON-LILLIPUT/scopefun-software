cmake_minimum_required (VERSION 2.8)

# sfServer
add_executable(sfServer ${SCOPEFUN_EXE_TYPE}
"${CMAKE_SOURCE_DIR}/source/server/main.cpp"
"${CMAKE_SOURCE_DIR}/source/core/input/input.cpp"
"${CMAKE_SOURCE_DIR}/source/core/core.cpp"
"${CMAKE_SOURCE_DIR}/source/core/string/corestring.cpp"
"${CMAKE_SOURCE_DIR}/source/core/format/format.cpp"
"${CMAKE_SOURCE_DIR}/source/core/memory/memory.cpp"
"${CMAKE_SOURCE_DIR}/source/core/manager/manager.cpp"
"${CMAKE_SOURCE_DIR}/source/core/file/file.cpp"
"${CMAKE_SOURCE_DIR}/source/core/timer/timer.cpp"
"${CMAKE_SOURCE_DIR}/source/core/render/font/font.cpp"
"${CMAKE_SOURCE_DIR}/source/core/render/canvas/canvas2d.cpp"
"${CMAKE_SOURCE_DIR}/source/core/render/canvas/canvas3d.cpp"
"${CMAKE_SOURCE_DIR}/source/core/render/render/render.cpp"
"${CMAKE_SOURCE_DIR}/source/core/render/camera/camera.cpp"
"${CMAKE_SOURCE_DIR}/source/core/opengl/opengl_devicestate.cpp"
"${CMAKE_SOURCE_DIR}/source/core/opengl/opengl_texture.cpp"
"${CMAKE_SOURCE_DIR}/source/core/opengl/opengl_device.cpp"
"${CMAKE_SOURCE_DIR}/source/core/opengl/opengl_staticmesh.cpp"
"${CMAKE_SOURCE_DIR}/source/core/opengl/opengl.cpp"
"${CMAKE_SOURCE_DIR}/source/core/opengl/opengl_shader.cpp"
"${CMAKE_SOURCE_DIR}/source/server/main.cpp"
"${CMAKE_SOURCE_DIR}/source/server/server.cpp"
"${CMAKE_SOURCE_DIR}/source/server/servermanager.cpp"
"${CMAKE_SOURCE_DIR}/source/server/usb.cpp"
"${CMAKE_SOURCE_DIR}/source/server/ui/serverui.cpp"
"${CMAKE_SOURCE_DIR}/source/server/ui/uimessage.cpp"
"${CMAKE_SOURCE_DIR}/source/server/ui/ScopeFunServerUI.cpp"
"${CMAKE_SOURCE_DIR}/lib/cJSON/cJSON.c"
"${CMAKE_SOURCE_DIR}/lib/glew-1.13.0/src/glew.c"
"${CMAKE_SOURCE_DIR}/lib/kiss_fft130/kiss_fft.c"
"${CMAKE_SOURCE_DIR}/lib/libusb-1.0.22/examples/ezusb.c"
"${CMAKE_SOURCE_DIR}/source/core/purec/puresocket.c"
"${CMAKE_SOURCE_DIR}/source/core/purec/pureusb.c"
"${CMAKE_SOURCE_DIR}/source/core/purec/purec.c"
"${CMAKE_SOURCE_DIR}/source/api/scopefun.c"
"${CMAKE_SOURCE_DIR}/source/osciloscope/xpm/xpm96.c"
"${CMAKE_SOURCE_DIR}/source/osciloscope/xpm/xpm512.c"
"${CMAKE_SOURCE_DIR}/source/osciloscope/xpm/xpm64.c"
"${CMAKE_SOURCE_DIR}/source/osciloscope/xpm/xpm256.c"
"${CMAKE_SOURCE_DIR}/source/osciloscope/xpm/xpm16.c"
"${CMAKE_SOURCE_DIR}/source/osciloscope/xpm/xpm32.c"
"${CMAKE_SOURCE_DIR}/source/osciloscope/xpm/xpm128.c"
"${CMAKE_SOURCE_DIR}/source/osciloscope/rc/dpi.manifest" )

# output
set_target_properties(sfServer
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY  "${CMAKE_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)
          
# sfServer name
set_target_properties(sfServer PROPERTIES OUTPUT_NAME "sfServer${SCOPEFUN_TYPE}")

# 32 bit?
if(SCOPEFUN_32BIT)
  if(SCOPEFUN_LINUX)
	set_target_properties(sfServer PROPERTIES COMPILE_FLAGS "-m32 -no-pie" LINK_FLAGS "-m32 -no-pie")
  else()
	set_target_properties(sfServer PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
  endif()
else()
  if(SCOPEFUN_LINUX)
	set_target_properties(sfServer PROPERTIES COMPILE_FLAGS "-no-pie" LINK_FLAGS "-no-pie")
  else()
    #set_target_properties(sfAPI    PROPERTIES COMPILE_FLAGS "" LINK_FLAGS "-pie")
  endif()
endif ()

# link sfServer
target_link_libraries(sfServer "${SCOPEFUN_LIBS}")