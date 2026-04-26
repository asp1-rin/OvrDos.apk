

#include <jni.h>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/syscall.h>

// 프로세스 ID를 찾는 함수
inline int get_pid(const char* process_name) {
    int pid = -1;
    DIR* dir = opendir("/proc");
    if (dir == NULL) return -1;
    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type != DT_DIR) continue;
        int id = atoi(entry->d_name);
        if (id <= 0) continue;
        char path[128];
        snprintf(path, sizeof(path), "/proc/%d/cmdline", id);
        int fd = open(path, O_RDONLY);
        if (fd != -1) {
            char cmdline[256];
            read(fd, cmdline, sizeof(cmdline));
            close(fd);
            if (strcmp(cmdline, process_name) == 0) {
                pid = id;
                break;
            }
        }
    }
    closedir(dir);
    return pid;
}

// 모듈 주소를 찾는 함수
inline uintptr_t get_module_base(int pid, const char* module_name) {
    uintptr_t addr = 0;
    char path[128];
    snprintf(path, sizeof(path), "/proc/%d/maps", pid);
    FILE* fp = fopen(path, "rt");
    if (fp != NULL) {
        char line[512];
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, module_name)) {
                addr = (uintptr_t)strtoull(line, NULL, 16);
                break;
            }
        }
        fclose(fp);
    }
    return addr;
}

// 메모리 쓰기
bool write_mem(int pid, uintptr_t addr, void* buffer, size_t size) {
    char path[128];
    snprintf(path, sizeof(path), "/proc/%d/mem", pid);
    int fd = open(path, O_RDWR);
    if (fd == -1) return false;
    bool success = pwrite64(fd, buffer, size, addr) != -1;
    close(fd);
    return success;
}

// 메모리 읽기
bool read_mem(int pid, uintptr_t addr, void* buffer, size_t size) {
    char path[128];
    snprintf(path, sizeof(path), "/proc/%d/mem", pid);
    int fd = open(path, O_RDONLY);
    if (fd == -1) return false;
    bool success = pread64(fd, buffer, size, addr) != -1;
    close(fd);
    return success;
}
