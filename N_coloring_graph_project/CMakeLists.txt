set(CMAKE_CXX_STANDARD 20)
cmake_minimum_required(VERSION 3.15)
project(N_coloring_graph)

find_package(MPI)


set(sources 
	"${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp" 
	"${CMAKE_CURRENT_SOURCE_DIR}/src/node.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/Threads.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/withMPI.cpp"
)
set(includes 
	"${CMAKE_CURRENT_SOURCE_DIR}/inc/node.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/inc/Threads.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/inc/withMPI.hpp"
)

add_executable(N_coloring_graph ${sources} ${includes})
target_include_directories(
	N_coloring_graph 
		PUBLIC 
			"${CMAKE_CURRENT_SOURCE_DIR}/inc"
			${MPI_INCLUDE_PATH}
)

target_link_libraries(N_coloring_graph ${MPI_CXX_LIBRARIES})