cmake_minimum_required(VERSION 2.8)
message( "\nConfiguring python wrapper:" )

# dependencies
find_package( Boost REQUIRED COMPONENTS python serialization )
find_package( PythonInterp REQUIRED )

if(WIN32)
  GET_FILENAME_COMPONENT(PYTHON_PREFIX "${PYTHON_EXECUTABLE}" PATH)
  message(STATUS "PYTHON_PREFIX ${PYTHON_PREFIX}")
  find_library(PYTHON_LIBRARY
        NAMES python${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR}.lib
        HINTS "${PYTHON_PREFIX}/libs"
        NO_DEFAULT_PATH)
  set(PYTHON_LIBRARIES "${PYTHON_LIBRARY}" CACHE FILEPATH "the Python library")
  
  find_path(PYTHON_INCLUDE_DIRS
        NAMES Python.h
        HINTS "${PYTHON_PREFIX}/include"
        NO_DEFAULT_PATH)
  set(PYTHON_INCLUDE_DIRS "${PYTHON_INCLUDE_DIRS}" CACHE PATH "the Python include directory")
else()
  find_package( PythonLibs REQUIRED )
endif()

# include external headers as system includes so we do not have to cope with their warnings
include_directories( SYSTEM ${Boost_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS} ${PYTHON_INCLUDE_DIRS}/python2.7 )

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/pypgmlink.cpp.cmake ${CMAKE_CURRENT_BINARY_DIR}/pypgmlink.cpp @ONLY)

# pypgmlink
set(PYPGMLINK_SRCS
  pyfield_of_view.cpp
  pyUncertaintyParameter.cpp
  pyhypotheses.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/pypgmlink.cpp
  pytrack.cpp
  pytraxels.cpp
  pymerger_resolving.cpp
  pyextract_region_features.cpp
  pyevaluation.cpp
  )

add_library(pypgmlink${SUFFIX} SHARED ${PYPGMLINK_SRCS})#
target_link_libraries(pypgmlink${SUFFIX} pgmlink${SUFFIX} ${Boost_LIBRARIES} ${PYTHON_LIBRARIES} )
if(WIN32)
  set_target_properties(pypgmlink${SUFFIX} PROPERTIES PREFIX "" OUTPUT_NAME "pgmlink${SUFFIX}" SUFFIX ".pyd")
else()
  set_target_properties(pypgmlink${SUFFIX} PROPERTIES PREFIX "" OUTPUT_NAME "pgmlink${SUFFIX}" SUFFIX ".so")
endif()

##generate python module directory
#add_custom_command(TARGET pypgmlink POST_BUILD
#                   COMMAND ${CMAKE_COMMAND} -E copy
#                       ${PROJECT_SOURCE_DIR}/python/pgmlink/__init__.py
#                       ${PYPGMLINK_INSTALL_DIR}/__init__.py
#                   COMMAND ${CMAKE_COMMAND} -E copy
#                       ${PROJECT_SOURCE_DIR}/python/pgmlink/TransitionClassifier.py
#                       ${PYPGMLINK_INSTALL_DIR}/TransitionClassifier.py  
#                        )

# install

# this will set the install dir to $PYTHON_SITE_PACKAGES
# make use of ${PYTHON_EXECUTABLE} instead of /usr/bin/python
if(NOT DEFINED PYPGMLINK_INSTALL_DIR OR PYPGMLINK_INSTALL_DIR MATCHES "^$")
    execute_process(COMMAND ${PYTHON_EXECUTABLE} -c 
                    "from distutils.sysconfig import *; print get_python_lib(1)"
                    OUTPUT_VARIABLE PYTHON_SITE_PACKAGES OUTPUT_STRIP_TRAILING_WHITESPACE)
    file(TO_CMAKE_PATH ${PYTHON_SITE_PACKAGES}/pgmlink PYPGMLINK_INSTALL_DIR)
endif()

set(PYPGMLINK_INSTALL_DIR ${PYPGMLINK_INSTALL_DIR}
    CACHE PATH "where to install the pgmlink Python package" FORCE)

# this is the install path relative to CMAKE_INSTALL_PREFIX,
# use this in INSTALL() commands to get packaging right
file(RELATIVE_PATH PYPGMLINK_INSTALL_DIR ${CMAKE_INSTALL_PREFIX} ${PYPGMLINK_INSTALL_DIR})

install(TARGETS pypgmlink${SUFFIX} 
	RUNTIME DESTINATION ${PYPGMLINK_INSTALL_DIR}
	LIBRARY DESTINATION ${PYPGMLINK_INSTALL_DIR})

#configure_file( ${PROJECT_SOURCE_DIR}/python/pgmlink/TransitionClassifier.py ${PYPGMLINK_INSTALL_DIR}/TransitionClassifier.py COPYONLY)
#file(COPY ${PROJECT_SOURCE_DIR}/python/pgmlink DESTINATION ${PYPGMLINK_INSTALL_DIR}/pgmlink)
