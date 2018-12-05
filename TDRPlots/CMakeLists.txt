project(TDRDAQWork)
message("I'm here")
# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED COMPONENTS RIO Net XMLIO)
find_program(LSB_RELEASE_EXEC lsb_release)

include(${ROOT_USE_FILE})
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../clustering/)

string(CONCAT CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} " -Wall -Wextra -Wpedantic -Werror")
#grabs the distribution
execute_process(COMMAND ${LSB_RELEASE_EXEC} -is
  OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )

# add_library(Clustering SHARED WireCluster.cc WireHit.cc OpticalCluster.cc OpticalHit.cc ClusterEngine.cc ClusterEnergyEstimator.cc Clustering.cc SmartTrigger.cc SmartTriggerToyThrower.cc BiTrigger.cc)
# target_link_libraries(Clustering ${ROOT_LIBRARIES})

add_executable(RunTDRPlot RunTDRPlot.cxx)
target_link_libraries(RunTDRPlot Clustering)
