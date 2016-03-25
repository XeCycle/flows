add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure)

function(flows_deftest name)
  add_executable(${name} EXCLUDE_FROM_ALL ${name}.cc)
  add_test(${name} ${name})
  add_dependencies(check ${name})
endfunction(flows_deftest)

add_subdirectory(flow)
