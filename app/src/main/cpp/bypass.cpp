#include <jni.h>
#include <stdint.h>
#include "memory.cpp" // 앞에서 만든 메모리 도구를 가져옵니다.

// 사인코드 무력화 핵심 패치 코드 (ARM64)
// mov w0, #0 (결과값 정상 설정)
// ret        (함수 즉시 종료)
uint8_t bypass_patch[] = {0x00, 0x00, 0x80, 0x52, 0xC0, 0x03, 0x5F, 0xD6};

void apply_xigncode_bypass(int pid) {
    // 1. libxigncode.so의 베이스 주소를 찾습니다.
    // 문자열 스캔 피하기 위해 나중에 암호화 처리 가능
    uintptr_t xign_base = get_module_base(pid, "libxigncode.so");
    
    if (xign_base == 0) return;

    // 2. 우리가 분석해서 찾아낸 핵심 탐지 함수 주소들에 패치를 적용합니다.
    
    // [분석 결과 1] 핵심 체크 함수 (0x12468)
    write_mem(pid, xign_base + 0x12468, bypass_patch, sizeof(bypass_patch));
    
    // [분석 결과 2] 활동 재개 시 스캔 함수 (0x16248)
    write_mem(pid, xign_base + 0x16248, bypass_patch, sizeof(bypass_patch));
    
    // [분석 결과 3] 기타 초기화 관련 함수 (0x1a628)
    write_mem(pid, xign_base + 0x1a628, bypass_patch, sizeof(bypass_patch));
}

// 나중에 자바(Java) 레이어에서 호출할 수 있도록 연결하는 인터페이스
extern "C" JNIEXPORT void JNICALL
Java_com_sys_opt_MainActivity_startBypass(JNIEnv *env, jobject thiz, jint pid) {
    apply_xigncode_bypass(pid);
}
