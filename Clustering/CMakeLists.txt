
add_library(Clustering SHARED Clustering.cc)
target_link_libraries(Clustering DataContainer Algorithm ${ROOT_LIBRARIES})

add_subdirectory(app)
