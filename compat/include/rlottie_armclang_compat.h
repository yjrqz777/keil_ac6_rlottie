#ifndef AC6_RLOTTIE_ARMCLANG_COMPAT_H
#define AC6_RLOTTIE_ARMCLANG_COMPAT_H

/*
 * rlottie 的 Keil AC6 裸机兼容入口。
 *
 * armclang-cortex-m7.cmake 使用：
 *
 *   -include rlottie_armclang_compat.h
 *
 * 强制让每个 C++ 编译单元先包含本文件。这样可以集中补齐 AC6 裸机
 * libc++ 缺失的声明，并避免在上游 rlottie 源码里到处打补丁。
 */

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
/*
 * 这里包含的是 compat/include 下的本地兼容头。
 * 因为工具链参数已把 compat/include 放到 include 搜索路径前面，
 * 所以 <condition_variable> 会先命中本目录里的占位实现。
 */
#include <condition_variable>
#include <functional>
#endif

#ifndef __has_include
/* 某些编译环境没有 __has_include，给上游条件编译一个保底定义。 */
#define __has_include(x) 0
#endif

#ifndef RLOTTIE_BUILD
/* 标记当前正在构建 rlottie 库本体。 */
#define RLOTTIE_BUILD
#endif

#ifndef RLOTTIE_DISABLE_DYNAMIC_LOADING
/* 裸机环境没有动态库，关闭 rlottie 的动态加载分支。 */
#define RLOTTIE_DISABLE_DYNAMIC_LOADING 1
#endif

#if defined(__ARMCC_VERSION) && !defined(RLOTTIE_AC6_HAVE_STRDUP)
#define RLOTTIE_AC6_HAVE_STRDUP 1
/*
 * AC6 裸机 C library 不一定声明 strdup。
 * rlottie 上游源码会使用 strdup，因此这里提供一个基于 malloc/memcpy
 * 的最小实现。调用方仍需要按普通 strdup 语义释放返回指针。
 */
static inline char *strdup(const char *s)
{
    if (!s) {
        return NULL;
    }

    size_t len = strlen(s) + 1U;
    char *copy = (char *)malloc(len);
    if (copy) {
        memcpy(copy, s, len);
    }
    return copy;
}
#endif

#endif
