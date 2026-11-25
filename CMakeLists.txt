cmake_minimum_required(VERSION 3.21)
project(JinJuApp LANGUAGES CXX)
if(NOT WIN32 OR NOT MSVC OR NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
  message(FATAL_ERROR "Windows MSVC x64 required")
endif()
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
function(jinju_target name)
  target_compile_definitions(${name} PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN NOMINMAX _WIN32_WINNT=0x0A00)
  target_compile_options(${name} PRIVATE /utf-8 /W4 /permissive-)
  target_link_libraries(${name} PRIVATE user32 gdi32 shell32 ole32 oleaut32 uiautomationcore dwmapi gdiplus uuid advapi32 windowscodecs)
endfunction()
add_executable(JinJuMarquee WIN32 main.cpp main.h)
jinju_target(JinJuMarquee)
include(CTest)
add_subdirectory(xaml_experiment)
if(BUILD_TESTING)
  add_subdirectory(tests)
  find_program(JINJU_POWERSHELL NAMES pwsh powershell REQUIRED)
  add_test(NAME security_report_self_test COMMAND "${JINJU_POWERSHELL}" -NoProfile -File "${CMAKE_SOURCE_DIR}/tests/JinJuSecurityReport.ps1" -SelfTest)
endif()

# Always synchronize the visual-test delivery directory after a successful build.
set(JINJU_DELIVERY_DIR "${CMAKE_SOURCE_DIR}/tests")
add_custom_target(jinju_delivery ALL
  COMMAND "${CMAKE_COMMAND}" -E make_directory "${JINJU_DELIVERY_DIR}"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:JinJuApp>" "${JINJU_DELIVERY_DIR}/JinJuApp.exe"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:JinJuXaml>" "${JINJU_DELIVERY_DIR}/JinJuXaml.dll"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:JinJuXamlUnload>" "${JINJU_DELIVERY_DIR}/JinJuXamlUnload.exe"
  COMMAND "${CMAKE_COMMAND}" -E copy_if_different "$<TARGET_FILE:JinJuMarquee>" "${JINJU_DELIVERY_DIR}/JinJuMarquee.exe"
  COMMAND "${CMAKE_COMMAND}" "-DDELIVERY_DIR=${JINJU_DELIVERY_DIR}" -P "${CMAKE_SOURCE_DIR}/tests/hash_delivery.cmake"
  DEPENDS JinJuApp JinJuXaml JinJuXamlUnload JinJuMarquee
  VERBATIM
)
