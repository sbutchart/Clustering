add_library(Display SHARED PhotonTimingDisplay.cc PhotonWirePositionDisplay.cc WirePositionTimingDisplay.cc)
target_link_libraries(Display ${ROOT_LIBRARIES})

add_subdirectory(app)
