function(add_example name)
  file(GLOB SOURCES ${name}/*.cpp ${name}/*.h)
  add_executable(${name} ${SOURCES})
  target_link_libraries(${name} PRIVATE glcore)
  target_compile_definitions(${name} PRIVATE SHADER_PATH="${CMAKE_CURRENT_SOURCE_DIR}/${name}/shaders/")
endfunction()


add_example(collision)
