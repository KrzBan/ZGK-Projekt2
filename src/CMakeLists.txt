add_executable (src "Source.cpp" "Source.hpp")

target_sources(src PRIVATE 
"Utils.hpp" "Utils.cpp"
"FpCameraManipulator.cpp" "FpCameraManipulator.hpp" 
"Ray.hpp" "Ray.cpp"
"Sphere.hpp" "Sphere.cpp"
)

find_package(OpenSceneGraph)
find_package(spdlog)

target_link_libraries(src PRIVATE OPENSCENEGRAPH::OPENSCENEGRAPH spdlog::spdlog)
target_include_directories(src PRIVATE ${CMAKE_CURRENT_LIST_DIR})

set_property(TARGET src PROPERTY CXX_STANDARD 20)

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_LIST_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/assets
)
add_dependencies(src copy_assets)