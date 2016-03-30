cmake_minimum_required(VERSION 2.8)
message( "\nConfiguring tests:" )

# dependencies
find_package( Boost REQUIRED COMPONENTS unit_test_framework filesystem system python)

include_directories(
  ${Boost_INCLUDE_DIRS}
  ${OPTIMIZER_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/include/
)

# enable dynamic linking with boost test
add_definitions(-DBOOST_TEST_DYN_LINK)

SET(TESTS_WITH_CPLEX 
    constraint_test 
    reasoner_constracking_test 
    reasoner_pgm_test 
    merger_resolver_test 
    tracking_feature_extractor_test
    uncertainty_test)

# autodiscover test sources and add tests
file(GLOB TEST_SRCS *.cpp)
foreach(test_src ${TEST_SRCS})
  get_filename_component(test_name ${test_src} NAME_WE)
  list(FIND TESTS_WITH_CPLEX ${test_name} NeedsILP)

  # these tests have to be configured first
  if (${test_src} MATCHES "randomforest_test.cpp") 
  else()
    if((${test_src} MATCHES "reasoner_dyn_prog_constracking_test.cpp") AND NOT (WITH_DPCT))
      message(STATUS "Not building dynamic programming tests")
    elseif(NOT GUROBI_FOUND AND NOT CPLEX_FOUND AND NOT NeedsILP EQUAL -1)
      message("Excluding test ${test_name} as it depends on an ILP solver")
    else()
      add_executable( ${test_name} ${test_src} )
      target_link_libraries( ${test_name} pgmlink${SUFFIX} ${Boost_LIBRARIES})
      add_test( ${test_name} ${test_name})
    endif()
  endif()
endforeach(test_src)

# configure random forest test
## assume by default that HDF5 is installed; set definition for vigra random forest
ADD_DEFINITIONS(-DHasHDF5)
configure_file(randomforest_test.cpp.cmake randomforest_test.cpp)
add_executable(randomforest_test ${PROJECT_BINARY_DIR}/tests/randomforest_test.cpp)

target_link_libraries(randomforest_test pgmlink ${Boost_LIBRARIES})
add_test(randomforest randomforest_test)

# opengm extensions tests
file(GLOB OGM_TEST_SRCS ext_opengm/*.cxx)
foreach(test_src ${OGM_TEST_SRCS})
  get_filename_component(test_name ${test_src} NAME_WE)
  set(test_name ext_opengm_${test_name})
  add_executable( ${test_name} ${test_src} )
  target_link_libraries( ${test_name} ${OPTIMIZER_LIBRARIES} )
  add_test( ${test_name} ${test_name} )
endforeach(test_src)
