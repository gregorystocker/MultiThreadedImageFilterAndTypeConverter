cmake_minimum_required(VERSION 3.20)
project(BMPThreads C)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -pthread -lm")
add_executable(other StockerFilters.c PixelProcessor.c BmpProcessor.c PpmProcessor.c  )
target_link_libraries(other m)
