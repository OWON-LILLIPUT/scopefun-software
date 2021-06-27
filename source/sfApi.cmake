cmake_minimum_required (VERSION 2.8)

# sfAPI
add_library(sfAPI MODULE
"${CMAKE_SOURCE_DIR}/lib/cJSON/cJSON.c"
"${CMAKE_SOURCE_DIR}/lib/kiss_fft130/kiss_fft.c"
"${CMAKE_SOURCE_DIR}/lib/libusb-1.0.22/examples/ezusb.c"
"${CMAKE_SOURCE_DIR}/source/core/purec/puresocket.c"
"${CMAKE_SOURCE_DIR}/source/core/purec/pureusb.c"
"${CMAKE_SOURCE_DIR}/source/core/purec/purec.c" 
"${CMAKE_SOURCE_DIR}/source/api/scopefun.c"
"${CMAKE_SOURCE_DIR}/source/api/scopefunapi_wrap_python.c" )

# output
set_target_properties(sfAPI
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY  "${CMAKE_SOURCE_DIR}/bin"
    RUNTIME_OUTPUT_DIRECTORY  "${CMAKE_SOURCE_DIR}/bin"
)

# linker
if(SCOPEFUN_MACOSX)   
    #set_target_properties(sfAPI PROPERTIES LINK_FLAGS "-undefined dynamic_lookup")
endif()

# sfAPI name
set_target_properties(sfAPI PROPERTIES PREFIX      "")
set_target_properties(sfAPI PROPERTIES OUTPUT_NAME "_scopefunapi")
if(SCOPEFUN_WINDOWS)                             
   set_target_properties(sfAPI PROPERTIES SUFFIX ".pyd" )
else()
   set_target_properties(sfAPI PROPERTIES SUFFIX ".so" )
endif (SCOPEFUN_WINDOWS)

# 32 bit?
if(SCOPEFUN_32BIT)
  if(SCOPEFUN_LINUX)
	set_target_properties(sfAPI    PROPERTIES COMPILE_FLAGS "-m32 -pie"    LINK_FLAGS "-m32 -pie")
  else()
	set_target_properties(sfAPI    PROPERTIES COMPILE_FLAGS "-m32"         LINK_FLAGS "-m32 -pie")
  endif()
else()
  if(SCOPEFUN_LINUX)
	set_target_properties(sfAPI    PROPERTIES COMPILE_FLAGS "-fPIC -pie"        LINK_FLAGS "-fPIC -pie")
  else()
    #set_target_properties(sfAPI    PROPERTIES COMPILE_FLAGS "-pie"        LINK_FLAGS "-pie")
  endif ()
endif ()


# api specifi defines
target_compile_definitions(sfAPI PRIVATE SCOPEFUN_EXPORT )
					
# link sfAPI
if(SCOPEFUN_MACOSX)
    target_link_libraries(sfAPI ${SCOPEFUN_LIBS} ${SCOPEFUN_LIB_LINK_PYTHON}/libpython2.7.a )
else()
   if(SCOPEFUN_LINUX)
        target_link_libraries(sfAPI ${SCOPEFUN_LIBS} ${SCOPEFUN_LIB_LINK_PYTHON}/libpython2.7.a )
   else()
	target_link_libraries(sfAPI ${SCOPEFUN_LIBS} ${SCOPEFUN_LIB_LINK_PYTHON}/libpython27.a )
   endif()
endif()

# targets
if(SCOPEFUN_LINUX)
	install(TARGETS sfAPI
            PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
            LIBRARY  DESTINATION "./share/api" )
else()
    file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/api)
	install(TARGETS sfAPI
            PERMISSIONS OWNER_WRITE OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
			LIBRARY  DESTINATION "api" )
	install( FILES "${CMAKE_SOURCE_DIR}/source/api/scopefunapi.py" DESTINATION "api" )
endif()
