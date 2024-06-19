file(GLOB SOURCES *.cpp *.h)
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE glcore)
target_compile_definitions(${PROJECT_NAME} PRIVATE SHADER_PATH="${CMAKE_CURRENT_SOURCE_DIR}/${name}/shaders/" RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/data/")