add_library(BiTrigger SHARED BiTrigger.cc)
target_link_libraries(BiTrigger ${ROOT_LIBRARIES})
add_subdirectory(app)
