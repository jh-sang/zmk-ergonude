# ==========================================================
# ZMK Custom Module CMake 설정
# ==========================================================

# 1. 모든 소스 파일 재귀적 스캔 및 ZMK 타겟에 추가
# CMAKE_CURRENT_SOURCE_DIR 모듈 디렉토리 아래의 모든 *.c 파일을 찾습니다.
# (예: behaviors/*.c, app/src/*.c 등)
file(GLOB_RECURSE SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

# 찾은 모든 C 파일을 ZMK 앱의 메인 타겟에 추가합니다.
target_sources(zmk PRIVATE
    ${SOURCES}
)

# 2. 모든 디렉토리를 인클루드 경로에 추가 (재귀적으로)
# Behavior 파일 내에서 필요한 헤더 파일 (예: #include <zmk/behavior.h>)을 찾을 수 있도록 합니다.
file(GLOB_RECURSE INCLUDE_DIRS
    LIST_DIRECTORIES true
    "${CMAKE_CURRENT_SOURCE_DIR}/*"
)
target_include_directories(zmk PRIVATE
    ${INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
