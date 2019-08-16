add_library(DataContainer SHARED OpticalCluster.cc OpticalHit.cc WireCluster.cc WireHit.cc)
target_link_libraries(DataContainer ${ROOT_LIBRARIES})
