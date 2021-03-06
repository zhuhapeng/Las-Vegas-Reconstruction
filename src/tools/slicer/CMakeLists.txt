#####################################################################################
# Set source files
#####################################################################################

set(SLICER_SOURCES
    Options.cpp
    Main.cpp
)

#####################################################################################
# Setup dependencies to external libraries
#####################################################################################

set(LVR_SLICER_DEPENDENCIES
	lvr_static
	lvrlas_static
	lvrrply_static
	lvrslam6d_static
	${OPENGL_LIBRARIES}
	${GLUT_LIBRARIES}
	${OpenCV_LIBS}
	)

if(PCL_FOUND)
  set(LVR_SLICER_DEPENDENCIES  ${LVR_SLICER_DEPENDENCIES} ${PCL_LIBRARIES} )
endif(PCL_FOUND)

if( ${NABO_FOUND} )
  set(LVR_SLICER_DEPENDENCIES  ${LVR_SLICER_DEPENDENCIES} ${NABO_LIBRARY} )
endif( ${NABO_FOUND} )


#####################################################################################
# Add executable
#####################################################################################

add_executable(lvr_slicer ${SLICER_SOURCES})
target_link_libraries(lvr_slicer ${LVR_SLICER_DEPENDENCIES} )

install(TARGETS lvr_slicer
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
