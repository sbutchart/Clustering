
add_executable(DAQClustering            RunDAQClustering.cxx            )
add_executable(DAQClustering_PhotonOnly RunDAQClustering_PhotonOnly.cxx )
add_executable(DAQClustering_WireOnly   RunDAQClustering_WireOnly.cxx   )

target_link_libraries(DAQClustering            Clustering)
target_link_libraries(DAQClustering_PhotonOnly Clustering)
target_link_libraries(DAQClustering_WireOnly   Clustering)

