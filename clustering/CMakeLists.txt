project(DAQWork)
message(${CMAKE_CXX_COMPILER})
# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED COMPONENTS RIO Net XMLIO)
find_program(LSB_RELEASE_EXEC lsb_release)
find_package(TBB REQUIRED)

include(${ROOT_USE_FILE})
string(CONCAT CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} " -Wall -Wextra -Wpedantic -Werror")
#grabs the distribution
execute_process(COMMAND ${LSB_RELEASE_EXEC} -is
  OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )

# set(Gperftools_ROOT_DIR "/Users/plasorak/Documents/Sussex-Postdoc/Physics/gperftools/")
# string(CONCAT CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "/Users/plasorak/Documents/Sussex-Postdoc/Physics/Clustering/clustering/")

# if the distribution is scientific linux, then setup boost in a different way (not using the cmake file from boost)
string(FIND "${LSB_RELEASE_ID_SHORT}" "Scientific" _cmp)
if (_cmp GREATER -1)
  set(Boost_NO_BOOST_CMAKE ON)
endif()

find_package(Boost REQUIRED)
#find_package(Gperftools REQUIRED) 
#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${Boost_INCLUDE_DIRS})

#add_library(Clustering SHARED WireCluster.cc WireHit.cc OpticalCluster.cc OpticalHit.cc ClusterEngine.cc ClusterEnergyEstimator.cc Clustering.cc SmartTrigger.cc SmartTriggerToyThrower.cc BiTrigger.cc)
add_library(Clustering SHARED WireCluster.cc WireHit.cc OpticalCluster.cc OpticalHit.cc ClusterEngine.cc ClusterEnergyEstimator.cc Clustering.cc)
target_link_libraries(Clustering ${ROOT_LIBRARIES})

add_library(Display SHARED PhotonTimingDisplay.cc PhotonWirePositionDisplay.cc WirePositionTimingDisplay.cc)
target_link_libraries(Display ${ROOT_LIBRARIES})

add_library(SimpleOptHit SHARED HitSimplePlot.cc)
target_link_libraries(SimpleOptHit ${ROOT_LIBRARIES})

add_executable(RunDAQClustering RunDAQClustering.cxx)
target_link_libraries(RunDAQClustering Clustering)

add_executable(RunDAQClustering_PhotonOnly RunDAQClustering_PhotonOnly.cxx)
target_link_libraries(RunDAQClustering_PhotonOnly Clustering)

add_executable(RunDAQClustering_WireOnly RunDAQClustering_WireOnly.cxx)
target_link_libraries(RunDAQClustering_WireOnly Clustering)

add_executable(AnalyseOpticalCluster AnalyseOpticalCluster.cxx)
target_link_libraries(AnalyseOpticalCluster Clustering)

add_executable(AnalyseBackgroundOpticalCluster AnalyseBackgroundOpticalCluster.cxx)
target_link_libraries(AnalyseBackgroundOpticalCluster Clustering)

add_executable(OptimiseNHitCut OptimiseNHitCut.cxx)
target_link_libraries(OptimiseNHitCut Clustering)

add_executable(OptimiseNPECut OptimiseNPECut.cxx)
target_link_libraries(OptimiseNPECut Clustering)

add_executable(GetEffBackRate GetEffBackRate.cxx)
target_link_libraries(GetEffBackRate Clustering)

add_executable(RunEventDiplay RunEventDisplay.cxx)
target_link_libraries(RunEventDiplay Display)

add_executable(AnalyseDAQClustering AnalyseDAQClustering.cxx)
target_link_libraries(AnalyseDAQClustering Clustering)

add_executable(RunSimpleHitInfo RunSimpleHitInfo.cxx)
target_link_libraries(RunSimpleHitInfo SimpleOptHit)

add_executable(PDSTiming PDSTiming.cxx)
target_link_libraries(PDSTiming ${ROOT_LIBRARIES})

add_executable(NeutronInfo NeutronInfo.cxx)
target_link_libraries(NeutronInfo ${ROOT_LIBRARIES})

# add_executable(RunSmartTrigger RunSmartTrigger.cxx)
# target_link_libraries(RunSmartTrigger Clustering)
# target_link_libraries(RunSmartTrigger ${Gperftools_LIBRARIES})

add_executable(RunSmartTriggerStatisticalTests RunSmartTriggerStatisticalTests.cxx)
target_link_libraries(RunSmartTriggerStatisticalTests Clustering)
# target_link_libraries(RunSmartTrigger ${Gperftools_LIBRARIES})

add_executable(RunSmartTrigger RunSmartTrigger.cxx)
target_link_libraries(RunSmartTrigger ${ROOT_LIBRARIES})

# add_executable(ThrowToySmartTrigger ThrowToySmartTrigger.cxx)
# target_link_libraries(ThrowToySmartTrigger Clustering)

# add_executable(RunBiTrigger RunBiTrigger.cxx)
# target_link_libraries(RunBiTrigger Clustering)

add_executable(BackgroundRateCalculator BackgroundRateCalculator.cxx)
target_link_libraries(BackgroundRateCalculator Clustering)

add_executable(CreateReweightingScheme CreateReweightingScheme.cxx)
target_link_libraries(CreateReweightingScheme ${ROOT_LIBRARIES})

add_executable(AnalyseWireClustering_SolarNu AnalyseWireClustering_SolarNu.cxx)
target_link_libraries(AnalyseWireClustering_SolarNu ${ROOT_LIBRARIES})

add_executable(EfficiencyCalculator EfficiencyCalculator.cxx)
target_link_libraries(EfficiencyCalculator Clustering)

add_executable(DumpSNRate DumpRate.cxx)
target_link_libraries(DumpSNRate ${ROOT_LIBRARIES}) 

add_executable(MakePEPlotForJose MakePEPlotForJose.C)
target_link_libraries(MakePEPlotForJose ${ROOT_LIBRARIES})

add_executable(AnalyseInputFile AnalyseInputFile.cxx)
target_link_libraries(AnalyseInputFile Clustering)

add_executable(AnalyseInputFile_Arb AnalyseInputFile_Arb.cxx)
target_link_libraries(AnalyseInputFile_Arb Clustering)

add_executable(TotalNEvents TotalNEvents.C)
target_link_libraries(TotalNEvents ${ROOT_LIBRARIES})

add_executable(Optimiser Optimiser.cxx)
target_link_libraries(Optimiser ${ROOT_LIBRARIES})

add_executable(Optimiser2D Optimiser2D.cxx)
target_link_libraries(Optimiser2D ${ROOT_LIBRARIES})

# add_executable(Outlier Outlier.cxx)
# target_link_libraries(Outlier ${ROOT_LIBRARIES})

add_executable(PlotEfficiencyAtNCut PlotEfficiencyAtNCut.cxx)
target_link_libraries(PlotEfficiencyAtNCut ${ROOT_LIBRARIES})

add_executable(GetBackgroundRateAt50PerCent GetBackgroundRateAt50PerCent.cxx)
target_link_libraries(GetBackgroundRateAt50PerCent ${ROOT_LIBRARIES})

add_executable(RunThreshold RunThreshold.cxx)
target_link_libraries(RunThreshold Clustering)

add_executable(DumpForAI DumpForAI.cxx)
target_link_libraries(DumpForAI ${ROOT_LIBRARIES})

add_executable(DumpForAI_Arbitrary DumpForAI_Arbitrary.cxx)
target_link_libraries(DumpForAI_Arbitrary Clustering)

add_executable(RunXDependancy RunXDependancy.cxx)
target_link_libraries(RunXDependancy Clustering)

add_executable(RunNeutronStudyInd NeutronStudyInd.cxx)
target_link_libraries(RunNeutronStudyInd Clustering)
