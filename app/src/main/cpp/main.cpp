#include "memory.h"
#include "bypass.h"
#include "aimbot.h"
#include <jni.h>



// JNI가 로드될 때 실행되는 기본 설정 (필요 시 작성)
extern "C" JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM* vm, void* reserved) {
    return JNI_VERSION_1_6;
}
