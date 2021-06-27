cmake_minimum_required (VERSION 2.8)

# sfScope
add_executable(sfScope ${SCOPEFUN_EXE_TYPE}
#cpp
"${CMAKE_SOURCE_DIR}/source/core/input/input.cpp"
"${CMAKE_SOURCE_DIR}/source/core/core.cpp"
"${CMAKE_SOURCE_DIR}/source/core/string/corestring.cpp"
"${CMAKE_SOURCE_DIR}/source/core/format/format.cpp"
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
"${CMAKE_SOURCE_DIR}/source/core/memory/memory.cpp"
"${CMAKE_SOURCE_DIR}/source/core/manager/manager.cpp"
"${CMAKE_SOURCE_DIR}/source/core/file/file.cpp"
"${CMAKE_SOURCE_DIR}/source/core/timer/timer.cpp"
"${CMAKE_SOURCE_DIR}/source/osciloscope/osciloscope.cpp"
"${CMAKE_SOURCE_DIR}/source/osciloscope/gui/OsciloskopOsciloskop.cpp"
"${CMAKE_SOURCE_DIR}/source/osciloscope/gui/OsciloskopMeasure.cpp"
"${CMAKE_SOURCE_DIR}/source/osciloscope/gui/OsciloskopDebug.cpp"
"${CMAKE_SOURCE_DIR}/source/osciloscope/gui/OsciloskopHardwareGenerator.cpp"
"${CMAKE_SOURCE_DIR}/source/osciloscope/gui/OsciloskopThermal.cpp"
"${CMAKE_SOURCE_DIR}/source/osciloscope/gui/OsciloskopInfo.cpp"
"${CMAKE_SOURCE_DIR}/source/osciloscope/gui/osc.cpp"
"${CMAKE_SOURCE_DIR}/source/osciloscope/gui/OsciloskopDisplay.cpp"
"${CMAKE_SOURCE_DIR}/source/osciloscope/gui/OsciloskopStorage.cpp"
"${CMAKE_SOURCE_DIR}/source/osciloscope/gui/OsciloskopConnection.cpp"
"${CMAKE_SOURCE_DIR}/source/osciloscope/gui/OsciloskopSoftwareGenerator.cpp"
"${CMAKE_SOURCE_DIR}/source/osciloscope/osciloscope/oscrender.cpp"
"${CMAKE_SOURCE_DIR}/source/osciloscope/osciloscope/oscsignal.cpp"
"${CMAKE_SOURCE_DIR}/source/osciloscope/osciloscope/oscfile.cpp"
"${CMAKE_SOURCE_DIR}/source/osciloscope/osciloscope/oscfft.cpp"
"${CMAKE_SOURCE_DIR}/source/osciloscope/osciloscope/oscsettings.cpp"
"${CMAKE_SOURCE_DIR}/source/osciloscope/osciloscope/oscmng.cpp"
"${CMAKE_SOURCE_DIR}/source/osciloscope/osciloscope/osccontrol.cpp"
"${CMAKE_SOURCE_DIR}/source/osciloscope/window/wndshadow.cpp"
"${CMAKE_SOURCE_DIR}/source/osciloscope/window/wndmain.cpp"
"${CMAKE_SOURCE_DIR}/source/osciloscope/window/tool.cpp"
"${CMAKE_SOURCE_DIR}/source/osciloscope/window/wnddisplay.cpp"
"${CMAKE_SOURCE_DIR}/source/osciloscope/window/wndgenerate.cpp"
"${CMAKE_SOURCE_DIR}/source/osciloscope/app/managers.cpp"
"${CMAKE_SOURCE_DIR}/source/osciloscope/app/app.cpp"
#c
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
"${CMAKE_SOURCE_DIR}/source/osciloscope/rc/dpi.manifest"
#resources
"${SF_DATA}"
"${SF_LOCALE}"
)

# output
set_target_properties(sfScope
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY  "${CMAKE_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin"
)
                   
# sfScope name
set_target_properties(sfScope PROPERTIES OUTPUT_NAME "sfScope${SCOPEFUN_TYPE}")

# 32 bit?
if(SCOPEFUN_32BIT)
  if(SCOPEFUN_LINUX)
	set_target_properties(sfScope  PROPERTIES COMPILE_FLAGS "-m32 -no-pie" LINK_FLAGS "-m32 -no-pie")
  else()
    set_target_properties(sfScope  PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
  endif()
else()
  if(SCOPEFUN_LINUX)
	set_target_properties(sfScope  PROPERTIES COMPILE_FLAGS "-no-pie" LINK_FLAGS "-no-pie")
  else()
    #set_target_properties(sfAPI    PROPERTIES COMPILE_FLAGS "" LINK_FLAGS "-pie")
  endif()
endif ()

# link sfScope
target_link_libraries(sfScope "${SCOPEFUN_LIBS}")
