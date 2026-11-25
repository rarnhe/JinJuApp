# Included by the repository root CMakeLists.txt.
add_executable(JinJuApp main.cpp)
add_library(JinJuXaml SHARED xaml_diagnostics.cpp exports.def)
add_executable(JinJuXamlUnload ../tests/unload_xaml.cpp)
foreach(target JinJuApp JinJuXaml JinJuXamlUnload)
  target_compile_options(${target} PRIVATE /utf-8 /EHsc /bigobj)
  set_target_properties(${target} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
  )
endforeach()
target_link_libraries(JinJuApp PRIVATE runtimeobject ole32)
target_link_libraries(JinJuXaml PRIVATE runtimeobject ole32 windowsapp)
target_link_libraries(JinJuXamlUnload PRIVATE Rstrtmgr user32)
if(BUILD_TESTING)
  add_executable(test_xaml ../tests/test_xaml.cpp)
  target_compile_options(test_xaml PRIVATE /utf-8 /EHsc)
  target_link_libraries(test_xaml PRIVATE ole32)
  add_test(NAME com_factory COMMAND test_xaml $<TARGET_FILE:JinJuXaml>)
  add_test(NAME bounded_log_queue COMMAND test_xaml --log-queue)
  add_test(NAME sdk_export COMMAND JinJuApp --self-test)
  add_test(NAME unload_self_test COMMAND JinJuXamlUnload --self-test)
endif()
