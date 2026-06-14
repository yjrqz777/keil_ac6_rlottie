# 从零编译 rlottie

本文说明如何在 Windows + Keil MDK / Arm Compiler 6 环境下，从零把 `rlottie-0.2` 编译成 STM32H743 Keil 工程可链接的静态库。

目标产物：

```text
External/keil_ac6_rlottie/install/lib/rlottie.lib
External/keil_ac6_rlottie/install/include/rlottie.h
External/keil_ac6_rlottie/install/include/rlottie_capi.h
External/keil_ac6_rlottie/install/include/rlottiecommon.h
```

## 1. 准备工具

需要安装：

```text
Keil MDK / Arm Compiler 6
CMake
Ninja
```

确认 Keil AC6 工具链目录存在，例如：

```text
D:/App/Keil/Keil_v5/ARM/ARMCLANG/bin
```

该目录下应能看到：

```text
armclang.exe
armasm.exe
armar.exe
```

如果你的路径不同，后面需要修改 `CMakePresets.json` 里的 `ARMCLANG_BIN`。

## 2. 准备源码目录

目录应类似：

```text
External/keil_ac6_rlottie/
  CMakeLists.txt
  CMakePresets.json
  armclang-cortex-m7.cmake
  rlottie-0.2/
  compat/include/
  keil/keil_ac6_retarget.c
```

其中 `rlottie-0.2/` 是上游 rlottie 源码目录。本移植包已经对上游 CMake 做了少量 Keil AC6 适配：

```text
输出 rlottie.lib
允许关闭 example
跳过会拉入 std::thread 的 vdebug.cpp
修正外层 add_subdirectory 时 rlottie.expmap 的路径
```

## 3. 目录和文件作用

这一层目录不是单纯存放 rlottie 源码，而是一个“Keil AC6 移植包”。每个新增文件都有明确职责：

| 路径 | 作用 | 为什么需要 |
| --- | --- | --- |
| `CMakeLists.txt` | 外层 CMake wrapper，调用 `rlottie-0.2` | 不直接从上游目录构建，而是在外层统一关闭 thread/cache/example/test 等不适合裸机的选项 |
| `CMakePresets.json` | 固定 CMake 配置参数 | 避免每次手敲很长的 `-D...` 参数，也固定 `build/`、`install/`、toolchain 路径 |
| `armclang-cortex-m7.cmake` | CMake 工具链文件 | 告诉 CMake 使用 Keil `armclang.exe`、`armasm.exe`、`armar.exe`，目标是 Cortex-M7 硬浮点裸机 |
| `build.bat` | Windows 一键构建入口 | 方便直接双击或在 cmd/PowerShell 中执行三步 CMake 命令 |
| `build_rlottie_armclang.ps1` | 旧 PowerShell 构建入口 | 早期移植用的脚本，保留兼容；现在推荐使用 CMake preset |
| `compat/include/` | AC6 裸机 libc++ 兼容头目录 | 上游 rlottie 关闭线程后仍残留 `<future>`、`<mutex>`、`<condition_variable>`、`dlfcn.h` 等引用；Keil AC6 裸机库缺这些头或类型，所以用本目录里的同名兼容头接管 |
| `compat/include/rlottie_armclang_compat.h` | 每个 C++ 编译单元强制包含的兼容入口 | 通过工具链参数 `-include rlottie_armclang_compat.h` 自动注入，集中补齐 `strdup`、禁用动态加载，并提前拉入兼容头 |
| `compat/include/future` | 最小 `std::future/std::promise` 占位实现 | 消除上游残留 `std::future` / `std::promise` 类型声明导致的编译错误；同步渲染路径只需要类型存在，不需要真正异步线程 |
| `compat/include/mutex` | 最小 `std::mutex/lock_guard/unique_lock` 占位实现 | 消除 `std::mutex`、`std::lock_guard`、`std::unique_lock` 不存在的错误；裸机单线程路径下 lock/unlock 做空操作 |
| `compat/include/condition_variable` | 最小 `std::condition_variable` 占位实现 | 消除 `std::condition_variable` 不存在的错误；单线程构建不会依赖真实等待/唤醒 |
| `compat/include/dlfcn.h` | `dlopen/dlsym/dlclose` 占位头 | 消除上游动态加载代码包含 POSIX `dlfcn.h` 时的找不到头文件错误；`LOTTIE_MODULE=OFF` 后这些函数不会真实用于加载模块 |
| `keil/keil_ac6_retarget.c` | Keil AC6 no-semihosting + RTT retarget | 链接 C++ 库后 Arm C library 可能引用 `_sys_*`，需要提供裸机实现并避免 semihosting BKPT |
| `build/` | CMake/Ninja 构建目录 | 由 CMake 自动生成，保存中间文件和临时构建出的库 |
| `install/` | 安装目录 | 保存 Keil 工程真正应该引用的头文件和 `rlottie.lib` |

简单说：

```text
rlottie-0.2/                 是上游源码
CMakeLists.txt               决定编译哪些功能
CMakePresets.json            决定怎么调用 CMake
armclang-cortex-m7.cmake      决定用什么工具链编
compat/include/              解决 AC6 裸机 C++ 库缺失的类型/头文件
keil/keil_ac6_retarget.c      解决 Keil 链接和运行时 semihosting 问题
install/                     给 Keil 工程引用
```

这些文件不是为了增加复杂度，而是把“上游桌面 C++ 库”和“STM32H743 裸机 Keil 工程”之间的差异隔离在 `External/keil_ac6_rlottie/` 里，避免把补丁散落到主工程各处。

### 兼容头的生效链路

新增 `future`、`mutex`、`condition_variable`、`dlfcn.h` 不是为了给 STM32 裸机补一套完整的 C++ 线程库，而是为了解决上游 rlottie 残留引用带来的编译错误。它们的生效过程是：

```text
armclang-cortex-m7.cmake
  -> 给 C/C++ 编译参数增加 -I External/keil_ac6_rlottie/compat/include
  -> 给 C++ 编译参数增加 -include rlottie_armclang_compat.h
  -> rlottie 源码 include <future> / <mutex> / <condition_variable> / "dlfcn.h"
  -> 编译器优先找到 compat/include/ 下的本地兼容头
  -> 缺失的类型、函数声明被补齐，编译继续通过
```

也就是说，新建这些文件后，rlottie 原来的源码不用到处改。只要用本目录的 CMake preset 构建，`compat/include/` 就会排在 include 搜索路径前面，AC6 编译器会先看到这里的兼容头，而不是去使用裸机 libc++ 里缺失线程支持的头。

### 兼容头如何消除编译错误

上游 rlottie 是按桌面 C++ 环境设计的，即使关闭了 `LOTTIE_THREAD`，源码里仍然有一些无条件残留的类型或头文件引用。Keil AC6 裸机 libc++ 没有线程支持，所以直接编译会遇到类似错误：

```text
fatal error: <future> is not supported since libc++ has been configured without support for threads
error: no type named 'mutex' in namespace 'std'
error: no type named 'condition_variable' in namespace 'std'
fatal error: 'dlfcn.h' file not found
error: use of undeclared identifier 'strdup'
```

本移植包通过 `compat/include/` 里的几个最小兼容头，把这些错误挡在编译阶段：

| 文件 | 解决的报错 | 处理方式 |
| --- | --- | --- |
| `compat/include/future` | `<future>` 不可用、`std::future` / `std::promise` 不存在 | 提供最小 `std::future<T>` 和 `std::promise<T>` 占位实现。rlottie 单线程路径里只是需要这些类型能通过编译，实际渲染仍同步执行 |
| `compat/include/mutex` | `std::mutex`、`std::lock_guard`、`std::unique_lock` 不存在 | 提供空操作版本的 mutex/lock。裸机单线程渲染没有真正的多线程竞争，所以 lock/unlock 可以是 no-op |
| `compat/include/condition_variable` | `std::condition_variable` 不存在 | 提供空操作 `notify_one()` / `wait()`。单线程路径不会依赖真实等待唤醒，只需要类型存在 |
| `compat/include/dlfcn.h` | 找不到 POSIX 的 `dlfcn.h` | 提供 `dlopen` / `dlsym` / `dlclose` / `dlerror` 占位实现。当前 `LOTTIE_MODULE=OFF`，动态加载路径不会被真实调用 |
| `compat/include/rlottie_armclang_compat.h` | `strdup` 未声明，以及需要集中启用兼容逻辑 | 每个 C++ 编译单元通过 `-include` 强制包含它；它补 `strdup`，并包含 `<future>`、`<mutex>`、`<condition_variable>` 等兼容入口 |

关键点是：这些文件不是为了给裸机“实现完整线程库”，而是为了让 rlottie 的单线程同步渲染路径能通过编译。实际线程、动态加载、异步调度都已经通过 CMake 选项关闭：

```text
LOTTIE_THREAD=OFF
LOTTIE_MODULE=OFF
LOTTIE_CACHE=OFF
LOTTIE_EXAMPLE=OFF
```

所以这些兼容实现只承担“补齐类型和函数声明，消除上游残留引用”的职责。

## 4. 检查工具链路径

打开：

```text
External/keil_ac6_rlottie/CMakePresets.json
```

确认：

```json
"ARMCLANG_BIN": "D:/App/Keil/Keil_v5/ARM/ARMCLANG/bin"
```

如果 Keil 不在这个目录，改成你的实际路径。

也可以不改文件，配置时临时覆盖：

```powershell
cmake --preset keil-ac6-m7 -DARMCLANG_BIN=D:/your/Keil/ARM/ARMCLANG/bin
```

## 5. 配置 CMake

进入目录：

```powershell
cd External\keil_ac6_rlottie
```

执行配置：

```powershell
cmake --preset keil-ac6-m7
```

这一步会：

```text
读取 CMakeLists.txt
读取 CMakePresets.json
加载 armclang-cortex-m7.cmake
检测 armclang/armasm/armar
生成 build/build.ninja
```

这一步不会真正编译源码。

## 6. 编译 rlottie

执行：

```powershell
cmake --build --preset keil-ac6-m7
```

这一步会调用 Keil AC6 工具链编译 rlottie，并生成临时构建产物：

```text
External/keil_ac6_rlottie/build/rlottie-0.2/rlottie.lib
```

Keil 工程不建议直接引用 `build/` 下的库，因为它是构建目录产物。

## 7. 安装产物

执行：

```powershell
cmake --install build
```

这一步会把头文件和库复制到稳定的安装目录：

```text
External/keil_ac6_rlottie/install/
```

最终 Keil 工程应引用 `install/` 下的文件。

## 8. 一键构建

如果不想手动执行三条命令，可以运行：

```powershell
cd External\keil_ac6_rlottie
.\build.bat
```

`build.bat` 内容等价于：

```bat
cmake --preset keil-ac6-m7
cmake --build --preset keil-ac6-m7
cmake --install build
```

## 9. 验证结果

确认文件存在：

```text
External/keil_ac6_rlottie/install/lib/rlottie.lib
External/keil_ac6_rlottie/install/include/rlottie.h
External/keil_ac6_rlottie/install/include/rlottie_capi.h
External/keil_ac6_rlottie/install/include/rlottiecommon.h
```

如果 `rlottie.lib` 不存在，说明 build 或 install 阶段失败，需要回看命令行中的第一个 error。

## 10. 接入 Keil 工程

Keil 工程需要配置：

| 项目 | 路径 / 值 |
| --- | --- |
| Include Path | `../External/keil_ac6_rlottie/install/include` |
| Library Path | `../External/keil_ac6_rlottie/install/lib` |
| Linker Input | `../External/keil_ac6_rlottie/install/lib/rlottie.lib` |
| Retarget Source | `../External/keil_ac6_rlottie/keil/keil_ac6_retarget.c` |
| LVGL Config | `LV_USE_RLOTTIE 1` |

`keil_ac6_retarget.c` 需要参与 Keil 工程编译。它用于禁用 semihosting，并提供 `_sys_*` / `_ttywrch` 等 AC6 C library 需要的底层函数。

### 为什么需要 `_sys_open` 这些函数

rlottie 本身并不是直接调用 `_sys_open`。真正的因果链是：

```text
加入 rlottie
  -> 链接进更多 C++ runtime / Arm C library 对象
  -> 标准库内部可能触发 semihosting BKPT
  -> 为了避免程序运行时停在 BKPT，声明 __use_no_semihosting
  -> 声明后 Arm C library 不再使用默认半主机 I/O
  -> 工程必须自己提供 _sys_open/_sys_write/_sys_read/_sys_exit 等 retarget 函数
```

`semihosting` 是 MCU 通过调试器把 `printf`、`fopen`、`exit` 等操作交给电脑处理的机制。它在调试时方便，但在裸机目标板上容易因为 BKPT 指令导致程序停住、反复 Continue 或进入异常。

`__use_no_semihosting` 的作用是禁止 Arm C library 使用这条半主机路径。禁止之后，标准库就需要工程明确说明底层 I/O 如何处理，所以 `keil_ac6_retarget.c` 提供这些实现：

| 函数 | 当前处理方式 |
| --- | --- |
| `_sys_write` | 把 stdout/stderr 输出到 SEGGER RTT |
| `_sys_open` | 不打开真实文件，只返回标准输出伪句柄 |
| `_sys_read` | 当前不支持输入，返回失败 |
| `_sys_seek` / `_sys_flen` | 当前没有真实文件系统支持，返回失败或 0 |
| `_sys_exit` | 裸机无操作系统可返回，停在死循环 |
| `_ttywrch` | 单字符输出到 SEGGER RTT |

所以可以总结为：不是 rlottie 语法上要求 `_sys_open`，而是为了避免 rlottie 引入 C++ runtime 后触发 semihosting，我们主动声明 `__use_no_semihosting`；声明之后就必须自己提供 `_sys_*` 和 `_ttywrch`。

## 11. 常见问题

### source directory 不匹配

如果出现类似：

```text
The source ".../CMakeLists.txt" does not match the source ".../rlottie-0.2/CMakeLists.txt" used to generate cache
```

说明旧的 `build/` 目录是用另一种 `-S` 路径生成的。删除 `build/` 后重新配置：

```powershell
Remove-Item .\build -Recurse -Force
cmake --preset keil-ac6-m7
```

### 找不到 armclang.exe

检查 `CMakePresets.json` 中的 `ARMCLANG_BIN` 是否正确。

### CMake 版本提示兼容性问题

本 preset 已设置：

```json
"CMAKE_POLICY_VERSION_MINIMUM": "3.5"
```

用于兼容 rlottie 上游较老的 `cmake_minimum_required`。

### 编译时遇到 std::future / std::mutex / dlfcn.h

本移植包通过 `compat/include/` 提供兼容头，并在工具链文件里强制 include：

```text
compat/include/rlottie_armclang_compat.h
```

正常使用 preset 构建时不需要额外处理。

### Keil 链接 `_sys_*` 重复定义

不要同时加入多个 retarget/syscalls 文件。通常保留：

```text
External/keil_ac6_rlottie/keil/keil_ac6_retarget.c
```

并移除其他可能定义 `_sys_open`、`_sys_write`、`_sys_seek` 的文件。

