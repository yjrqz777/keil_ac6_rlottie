#ifndef AC6_RLOTTIE_DLFCN_H
#define AC6_RLOTTIE_DLFCN_H

/*
 * dlfcn.h 是 POSIX 动态库加载接口，STM32 裸机环境没有这个头文件。
 *
 * rlottie 的图片/模块加载代码在非 Windows 分支可能 include <dlfcn.h>。
 * 当前构建已关闭 LOTTIE_MODULE，动态加载不会被真正使用。这里提供占位
 * 声明和失败返回值，只用于让源码通过编译。
 */

#ifdef __cplusplus
extern "C" {
#endif

#define RTLD_LAZY 1
#define RTLD_NOW 2

static inline void *dlopen(const char *filename, int flags)
{
    (void)filename;
    (void)flags;
    /* 裸机不支持动态加载，始终返回失败。 */
    return 0;
}

static inline void *dlsym(void *handle, const char *symbol)
{
    (void)handle;
    (void)symbol;
    return 0;
}

static inline int dlclose(void *handle)
{
    (void)handle;
    return 0;
}

static inline char *dlerror(void)
{
    /* 返回静态字符串，便于上层如果打印错误时有明确原因。 */
    return "dynamic loading is not supported";
}

#ifdef __cplusplus
}
#endif

#endif

