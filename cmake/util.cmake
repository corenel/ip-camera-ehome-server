# add executables with project library
macro(add_simple_test name)
    add_executable(${name} tests/${name}.cc)
    target_link_libraries(${name} ${PROJECT_NAME})
endmacro()

# add executables with libs
macro(add_simple_test_with_libs name libs)
    add_executable(${name} tests/${name}.cc)
    target_link_libraries(${name} ${libs})
endmacro()
