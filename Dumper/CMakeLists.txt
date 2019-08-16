add_executable(DumpForAI DumpForAI.cxx)
add_executable(DumpForAI_Arbitrary DumpForAI_Arbitrary.cxx)

target_link_libraries(DumpForAI ${ROOT_LIBRARIES})
target_link_libraries(DumpForAI_Arbitrary Clustering ${ROOT_LIBRARIES})
