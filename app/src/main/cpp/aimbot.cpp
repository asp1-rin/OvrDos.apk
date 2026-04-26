#include <jni.h>
#include <math.h>
#include <algorithm>
#include "memory.cpp"

struct Vector3 { float x, y, z; };
struct Vector2 { float x, y; };

// 월드 좌표를 화면 좌표로 변환할 때 필요한 구조체 (게임마다 오프셋 다름)
struct ViewMatrix {
    float matrix[16];
};

// 월드 좌표(3D)를 내 화면의 2D 좌표로 변환하는 함수 (FOV 체크용)
bool world_to_screen(Vector3 pos, Vector2 &screen, float matrix[16], int windowWidth, int windowHeight) {
    float clip_x = pos.x * matrix[0] + pos.y * matrix[1] + pos.z * matrix[2] + matrix[3];
    float clip_y = pos.x * matrix[4] + pos.y * matrix[5] + pos.z * matrix[6] + matrix[7];
    float clip_w = pos.x * matrix[12] + pos.y * matrix[13] + pos.z * matrix[14] + matrix[15];

    if (clip_w < 0.1f) return false;

    Vector3 ndc;
    ndc.x = clip_x / clip_w;
    ndc.y = clip_y / clip_w;

    screen.x = (windowWidth / 2 * ndc.x) + (ndc.x + windowWidth / 2);
    screen.y = -(windowHeight / 2 * ndc.y) + (ndc.y + windowHeight / 2);
    return true;
}

// 두 2D 지점 사이의 거리 계산 (FOV 원 안인지 확인용)
float get_distance_2d(Vector2 p1, Vector2 p2) {
    return sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
}

void run_aimbot_with_fov(int pid, uintptr_t game_base, float fov_radius, int screen_width, int screen_height) {
    uintptr_t local_pos_ptr = game_base + 0x2e42084;
    uintptr_t rotation_ptr = game_base + 0x2c5d0f0;
    uintptr_t view_matrix_ptr = game_base + 0x2e3c230; // ViewMatrix 위치 (가정)
    uintptr_t entity_list = game_base + 0x2e421cc;

    Vector3 local_pos;
    read_mem(pid, local_pos_ptr, &local_pos, sizeof(local_pos));

    float v_matrix[16];
    read_mem(pid, view_matrix_ptr, &v_matrix, sizeof(v_matrix));

    Vector2 screen_center = {(float)screen_width / 2, (float)screen_height / 2};
    float closest_dist_3d = 99999.0f;
    Vector3 best_target_pos = {0, 0, 0};
    bool target_found = false;

    for (int i = 0; i < 20; i++) {
        uintptr_t enemy_ptr;
        read_mem(pid, entity_list + (i * 0x10), &enemy_ptr, sizeof(enemy_ptr));
        if (enemy_ptr == 0) continue;

        Vector3 enemy_pos;
        read_mem(pid, enemy_ptr + 0x10, &enemy_pos, sizeof(enemy_pos));

        // 1. 적의 3D 좌표를 2D 화면 좌표로 변환
        Vector2 enemy_screen_pos;
        if (!world_to_screen(enemy_pos, enemy_screen_pos, v_matrix, screen_width, screen_height)) continue;

        // 2. 화면 중앙(에임)으로부터의 거리 계산 (FOV 체크)
        float dist_from_center = get_distance_2d(screen_center, enemy_screen_pos);

        // 3. 사용자가 설정한 FOV 원 안에 있는지 확인
        if (dist_from_center <= fov_radius) {
            // 4. 원 안에 있는 적들 중 실제 거리가 가장 가까운 대상 선정
            float dist_3d = sqrt(pow(enemy_pos.x - local_pos.x, 2) + pow(enemy_pos.z - local_pos.z, 2));
            if (dist_3d < closest_dist_3d) {
                closest_dist_3d = dist_3d;
                best_target_pos = enemy_pos;
                target_found = true;
            }
        }
    }

    // 최종 조준
    if (target_found) {
        // ... (calculate_angle 및 write_mem 로직)
    }
}
