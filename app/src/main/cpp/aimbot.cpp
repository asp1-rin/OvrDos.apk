#include <jni.h>
#include <math.h>
#include "memory.cpp"

struct Vector3 { float x, y, z; };
struct Vector2 { float x, y; };

// 각도 계산 함수
Vector2 calculate_angle(Vector3 local, Vector3 target) {
    Vector2 angle;
    float dx = target.x - local.x;
    float dy = target.y - local.y;
    float dz = target.z - local.z;
    float distance = sqrt(dx * dx + dz * dz);
    angle.x = atan2(dz, dx) * 180.0f / M_PI;
    angle.y = atan2(dy, distance) * 180.0f / M_PI;
    return angle;
}

// 에임봇 실행 (FOV + Smooth 적용)
void run_aimbot(int pid, uintptr_t game_base, float fov, float smooth, int sw, int sh) {
    uintptr_t rotation_ptr = game_base + 0x2c5d0f0; // 분석된 에임 주소
    
    // 1. 내 위치와 적 위치 읽기 (반복문 생략, 대상 선정 로직은 기존과 동일)
    Vector3 local_pos = {0, 0, 0}; // 실제 구현시 read_mem 사용
    Vector3 target_pos = {10, 5, 10}; // 찾은 타겟의 좌표

    // 2. 목표 각도 계산
    Vector2 target_angle = calculate_angle(local_pos, target_pos);
    
    // 3. 현재 내 각도 읽기
    Vector2 current_angle;
    read_mem(pid, rotation_ptr, &current_angle, sizeof(current_angle));

    // 4. Smoothing (부드러운 이동) 처리
    // smooth가 1.0이면 빡고, 숫자가 클수록 느릿하게 따라감
    float diff_x = target_angle.x - current_angle.x;
    float diff_y = target_angle.y - current_angle.y;

    // 각도 보정 (360도 회전 시 튀는 현상 방지)
    if (diff_x > 180) diff_x -= 360;
    if (diff_x < -180) diff_x += 360;

    Vector2 final_angle;
    final_angle.x = current_angle.x + (diff_x / smooth);
    final_angle.y = current_angle.y + (diff_y / smooth);

    // 5. 메모리 주입
    write_mem(pid, rotation_ptr, &final_angle, sizeof(final_angle));
}

extern "C" JNIEXPORT void JNICALL
Java_com_sys_opt_MainActivity_tickAimbot(JNIEnv *env, jobject thiz, jint pid, jlong base, jfloat fov, jfloat smooth, jint sw, jint sh) {
    run_aimbot(pid, (uintptr_t)base, fov, smooth, sw, sh);
}
