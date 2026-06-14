/*
 * Keil / Arm Compiler 6 裸机 retarget 层。
 *
 * 链接 rlottie 后会拉入一部分 C/C++ runtime。若工程没有提供 _sys_* 等
 * 底层函数，Arm C library 可能退回到 semihosting stub，在目标板上执行
 * BKPT 指令，表现为程序运行一段时间后停住、调试器需要反复 Continue。
 *
 * 本文件做两件事：
 * 1. 请求 no semihosting，避免 printf/fopen 等走调试器半主机接口。
 * 2. 把 stdout/stderr 重定向到 SEGGER RTT，方便保留日志输出。
 */

#if defined(__ARMCC_VERSION)

#include <stdio.h>
#include <string.h>
#include <rt_sys.h>
#include <rt_misc.h>

#include "SEGGER_RTT.h"

/* 告诉 Arm C library：本工程不使用 semihosting。 */
__asm(".global __use_no_semihosting");

/*
 * Arm C library 用 FILEHANDLE 表示底层文件句柄。
 * 这里给 stdin/stdout/stderr 三个伪终端分配固定句柄。
 */
#define STDIN_HANDLE  0x8001
#define STDOUT_HANDLE 0x8002
#define STDERR_HANDLE 0x8003

/* :tt 表示 terminal，Arm C library 会把标准流当作终端设备。 */
const char __stdin_name[]  = ":tt";
const char __stdout_name[] = ":tt";
const char __stderr_name[] = ":tt";

FILEHANDLE _sys_open(const char *name, int openmode)
{
    (void)openmode;

    /*
     * 当前移植不支持真实文件系统打开。
     * 对标准流或其他名字统一返回 stdout 句柄，避免 C library 继续寻找
     * semihosting 打开文件。
     */
    if (name == NULL) {
        return STDOUT_HANDLE;
    }
    return STDOUT_HANDLE;
}

int _sys_close(FILEHANDLE fh)
{
    (void)fh;
    /* 伪终端无需关闭，返回 0 表示成功。 */
    return 0;
}

int _sys_write(FILEHANDLE fh, const unsigned char *buf, unsigned len, int mode)
{
    (void)mode;

    if (buf == NULL) {
        return 0;
    }

    /* stdout/stderr 输出到 RTT channel 0。 */
    if (fh == STDOUT_HANDLE || fh == STDERR_HANDLE) {
        SEGGER_RTT_Write(0, (const char *)buf, len);
    }

    /*
     * _sys_write 返回未写入的字节数。
     * 这里返回 0 表示全部写入，避免 printf 判断为失败。
     */
    return 0;
}

int _sys_read(FILEHANDLE fh, unsigned char *buf, unsigned len, int mode)
{
    (void)fh;
    (void)buf;
    (void)len;
    (void)mode;
    /*
     * 当前没有实现 RTT 输入或串口输入。
     * 返回 -1 表示读取失败，避免 scanf/getchar 阻塞在这里。
     */
    return -1;
}

void _ttywrch(int ch)
{
    /* Arm C library 某些字符输出路径会直接调用 _ttywrch。 */
    char c = (char)ch;
    SEGGER_RTT_Write(0, &c, 1U);
}

int _sys_istty(FILEHANDLE fh)
{
    /* 告诉 C library 这三个句柄是终端设备。 */
    if (fh == STDIN_HANDLE || fh == STDOUT_HANDLE || fh == STDERR_HANDLE) {
        return 1;
    }
    return 0;
}

int _sys_seek(FILEHANDLE fh, long pos)
{
    (void)fh;
    (void)pos;
    /* 没有真实文件系统，不支持 seek。 */
    return -1;
}

int _sys_ensure(FILEHANDLE fh)
{
    (void)fh;
    /* 没有文件缓存需要刷盘。 */
    return -1;
}

long _sys_flen(FILEHANDLE fh)
{
    (void)fh;
    /* 没有真实文件长度，返回 0。 */
    return 0;
}

char *_sys_command_string(char *cmd, int len)
{
    (void)len;
    /* 裸机没有命令行参数，原样返回。 */
    return cmd;
}

void _sys_exit(int return_code)
{
    (void)return_code;
    /* 裸机程序没有操作系统可返回，停在这里便于调试。 */
    for (;;) {
    }
}

#endif
