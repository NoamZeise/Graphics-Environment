file(GLOB VK_RENDER_SOURCES
	  *
	  resources/*
	  parts/*
	  parts/helper/*
	  )

add_library(vulkan-env ${VK_RENDER_SOURCES})
	  
add_dependencies(vulkan-env resource-loader graphics-api glfw volk)

target_link_libraries(vulkan-env PRIVATE resource-loader)

if(NOT NO_FREETYPE)
  add_dependencies(vulkan-env freetype)
  target_link_libraries(vulkan-env PRIVATE freetype)
endif()

target_link_libraries(vulkan-env PUBLIC 
  graphics-api
  glfw
  volk
)

target_include_directories(vulkan-env PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include/)
