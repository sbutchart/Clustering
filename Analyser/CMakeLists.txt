add_executable(AnalyseDAQClustering            AnalyseDAQClustering.cxx           )
add_executable(GetEffBackRate                  GetEffBackRate.cxx                 )

target_link_libraries(AnalyseDAQClustering     ${ROOT_LIBRARIES})
target_link_libraries(GetEffBackRate                  ${ROOT_LIBRARIES})
