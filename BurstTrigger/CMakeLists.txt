cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(BurstTrigger)
set(CMAKE_BUILD_TYPE Debug)
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

find_package(ROOT REQUIRED COMPONENTS RIO Net)

include(${ROOT_USE_FILE})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)
ROOT_GENERATE_DICTIONARY(G__Configuration Configuration.hh LINKDEF LinkDef.h)

add_library(Configuration SHARED G__Configuration.cxx)
target_link_libraries(Configuration ${ROOT_LIBRARIES})

add_library(SNBurstTrigger SHARED SNBurstTrigger.cc)
target_link_libraries(SNBurstTrigger Configuration)

add_executable(SNBurst_Latency Module_SNBurst_Latency.cxx)
target_link_libraries(SNBurst_Latency SNBurstTrigger)

add_executable(SNBurst Module_SNBurst.cxx)
target_link_libraries(SNBurst SNBurstTrigger)

add_executable(NeutronRate NeutronRate.cxx)
target_link_libraries(NeutronRate SNBurstTrigger)

add_executable(RequirementEfficiency RequirementEfficiency.cxx)
target_link_libraries(RequirementEfficiency SNBurstTrigger)

add_executable(MakeSNTheoryDistributions MakeSNTheoryDistributions.C)
target_link_libraries(MakeSNTheoryDistributions ${ROOT_LIBRARIES})

add_executable(TimingDistribMarley TimingDistribMarley.C)
target_link_libraries(TimingDistribMarley ${ROOT_LIBRARIES})

add_executable(PlotSNBurst Plotting_SNBurst.C)
target_link_libraries(PlotSNBurst Configuration)
