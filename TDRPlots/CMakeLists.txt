add_executable(RunTDRPlot RunTDRPlot.cxx)
target_link_libraries(RunTDRPlot Clustering)

add_executable(RunHitPlots RunHitComparison.cxx)
target_link_libraries(RunHitPlots Clustering)
