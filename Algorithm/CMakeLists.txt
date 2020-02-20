
add_library(Algorithm SHARED ClusterEnergyEstimator.cc ClusterEngine.cc SmartTrigger.cc)
target_link_libraries(Algorithm DataContainer)
