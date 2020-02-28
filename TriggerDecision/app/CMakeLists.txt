add_executable(SmartTriggerStatisticalTests RunSmartTriggerStatisticalTests.cxx)
target_link_libraries(SmartTriggerStatisticalTests Clustering)

add_executable(DumpForStatisticalTrigger DumpForStatisticalTrigger.cxx)
target_link_libraries(DumpForStatisticalTrigger Clustering)

# file(GLOB python_src
#   "*.py")

# foreach(python_file ${python_src})
#   get_filename_component(python_no_dir ${python_file} NAME)
#   add_custom_command(TARGET ${python_no_dir} POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#     ${python_file}
#     $<TARGET_FILE_DIR:${python_no_dir}>
#     )
  
#   # add_custom_command(TARGET python PRE_BUILD
#   #   COMMAND ${CMAKE_COMMAND} -E
#   #   copy ${python_file} $<TARGET_FILE_DIR:python>)
# #  add_executable(${python_src})
# endforeach(python_file)
