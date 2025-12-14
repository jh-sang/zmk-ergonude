# Ergonude custom module CMake configuration

# src/ 폴더 안의 모든 .c 파일을 재귀적으로 스캔
file(GLOB_RECURSE sources src/*.c)

# Zephyr 라이브러리 선언
zephyr_library()

# include 디렉토리가 있다면 자동 포함
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include)
  zephyr_include_directories(include)
endif()

# 스캔된 모든 C 파일을 라이브러리에 추가
zephyr_library_sources(${sources})
