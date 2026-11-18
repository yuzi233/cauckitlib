# CacuKit

一个主要面向单片机的 C 语言计算器库。

## 功能

- [x] 小数、负数
- [x] 复数（实部、虚部分开存储，不依赖编译器的 `_Complex`）
- [x] 加减乘除混合运算
- [x] 单位：`i`（虚数）、`p`（π）、`o`（度）
- [ ] 括号（开发中）
- [ ] 三角函数 `sin` / `cos` / `tan`（开发中）

## 算法

CacuKit 把算式切成「数字 + 运算符」的小块，只用一个固定长度的栈，从左到右扫描一遍完成计算，不用动态内存，也不用递归。

详细原理、例子和已知限制见 [算法说明](./算法说明.md)。

## 快速开始

```c
#include <stdio.h>
#include "Cacukit.h"

int main(void) {
    char expr[100] = "1+2*3=";   // 算式必须以 '=' 结尾
    double real, imag;

    LoadExpr(expr);
    int error = Calculate(&real, &imag);
    if (error != 0) {
        printf("error: 0x%02X\n", error);
        return 1;
    }
    printf("= %g + %gi\n", real, imag);
    return 0;
}
```

> 在单片机上（如 Keil）请确保 `expr` 所在的内存能被 `CacuKit.c` 访问。如果 `char expr[100]` 无法访问，可以改用 `malloc` 分配。

如果想使用编译器自带的 `double _Complex`，在头文件中定义 `_C_COMPLEX_ENABLE_`，然后调用 `int Calculate(double _Complex *result)`。部分单片机编译器对 `_Complex` 支持不稳定，推荐使用实部、虚部分开的版本。

## 接口

| 函数 | 说明 |
|---|---|
| `bool LoadExpr(char *expr)` | 加载算式并初始化，失败返回 `false` |
| `int Calculate(double *real, double *imag)` | 计算，返回错误标志，`0` 表示无错误 |
| `int Calculate(double _Complex *result)` | 复数版本，需定义 `_C_COMPLEX_ENABLE_` |

## 错误标志

返回值的每一位代表一种错误，可以同时出现多种：

| 标志 | 含义 |
|---|---|
| `0x01` | 未识别的字符 |
| `0x02` | 除数为 0 |
| `0x04` | 括号错误 |
| `0x08` | 数字格式错误 |
| `0x10` | 运行错误 |
| `0x20` | 未知错误 |

## TODO

### 功能

- [ ] 支持括号 `(` `)`
  - [ ] 在 `Expr2Block` 中增加 `(` 的识别
  - [ ] 遇到 `(` 时记录栈位置，遇到 `)` 时折叠到该位置
  - [ ] 括号不匹配时返回 `0x04`
  - [ ] 支持嵌套括号
- [ ] 支持三角函数 `sin` / `cos` / `tan`
  - [ ] 让主循环能处理 `FUNCTION` 类型的块
  - [ ] 修复 `sin` 分支缺少 `break`，避免穿透到 `cos`
  - [ ] 明确函数参数的取值方式（目前的写法是"系数 × 函数值"）
  - [ ] 复数的三角函数（目前虚部是分开独立计算的，数学上不正确）
- [ ] 括号与函数组合，例如 `2*sin(90o)+1=`

### Bug 修复

- [ ] 同级运算符混合时结果错误，例如 `1-2*3+4=` 得到 `-9`，应为 `-1`（收尾时"从右往左"折叠，对减法、除法不成立）
- [ ] `LoadExpr` 中的 `sizeof(expr)` 得到的是指针大小，不是字符串长度，改用 `strlen`
- [ ] 虚数单位 `i` 的处理：`num=imag` 没有把实部清零
- [ ] 每次计算前没有重置 `CacuErr`，上一次的错误会残留
- [ ] 复数版本的 `CacuIt` 中 `temp` 未赋值就使用
- [ ] 除零判断检查的是被除数而不是除数
- [ ] 复数除法在虚部不为 0 时，`else` 后面缺少花括号，第二行会被无条件执行
- [ ] 部分分支没有返回值（`Expr2Block`、`Calculate`）

### 整理

- [ ] 统一命名：文档写 `Caculate`，代码是 `Calculate`；文件名 `CacuKit.c` 与 `Cacukit.h` 大小写不一致
- [ ] 把错误宏定义（`NO_ERROR` 等）移到头文件，使用者才能直接使用
- [ ] 合并 `README.txt` 和 `介绍.md`，以本文件为准
- [ ] 添加测试用例（基础运算、优先级、复数、错误情况）
- [ ] 在 PC 和单片机上分别验证
