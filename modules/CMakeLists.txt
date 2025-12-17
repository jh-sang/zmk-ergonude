# 모듈을 위한 전용 라이브러리 생성
zephyr_library()

# 소스 파일 추가
zephyr_library_sources(behaviors/behavior_rgb_ug_solid_breathing.c)

# ZMK 및 Zephyr 헤더 파일 경로 연결
zephyr_library_include_directories(${ZEPHYR_BASE}/include)
zephyr_library_include_directories(${CMAKE_SOURCE_DIR}/include) 
zephyr_library_include_directories(${CMAKE_SOURCE_DIR}/src) 
