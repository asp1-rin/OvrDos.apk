# CMake 최소 버전 설정
cmake_minimum_required(VERSION 3.18.1)

# 프로젝트 이름 설정
project("OvrDos_Native")

# C++ 표준 설정 (최신 문법 사용)
set(CMAKE_CXX_STANDARD 17)

# 1. 헤더 파일 및 소스 파일 경로 설정
# 우리가 만든 모든 cpp 파일들을 하나의 라이브러리로 묶음
add_library(
    sys_core        # 생성될 라이브러리 이름 (위장용: libsys_core.so)
    SHARED          # 공유 라이브러리 형식
    
    # 컴파일할 소스 파일들
    main.cpp        # (아래 보충 설명 참고)
    memory.cpp
    bypass.cpp
    aimbot.cpp
)

# 2. 안드로이드 시스템 라이브러리 링크
# 로그 출력(log)과 안드로이드 기본 기능들을 연결
find_library(log-lib log)

target_link_libraries(
    sys_core
    ${log-lib}
    android         # 안드로이드 네이티브 API 연동
)

# 3. 컴파일 옵션 (보안 강화)
# 심볼을 숨겨서 리버스 엔지니어링을 어렵게 만듦
set_target_properties(sys_core PROPERTIES CXX_VISIBILITY_PRESET hidden)
