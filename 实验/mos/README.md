# 北航操作系统课程 MOS 内核实验开源代码仓库

本仓库包含了北航操作系统课程 MOS 内核实验的开源代码，包括：
- 实验框架代码与答案，分别位于 `lab-exercise` 与 `lab-solution` 目录下；
- 开源许可证文件 `LICENSE`；
- 本 `README.md` 文件。

注：
- 本仓库所包含的实验代码仅供参考，**不保证时效性**，请参与课程的学生以课程实际下发的代码为准。
- 建议学生**在完成实验后**，或者**在遇到问题时**，再查看参考答案，以免影响自己的学习效果。

## 实验框架代码与答案使用

- `lab-exercise` 目录下包含了实验的框架代码，学生需要在此基础上完成实验；
- `lab-solution` 目录下包含了实验的参考答案，学生可以参考此目录下的代码。

### 开发环境

实验需在 Linux 系统上进行，依赖的软件主要包括：
- host 工具链：git、gcc、gdb、make、python3
- 工具链与硬件模拟器：
  - MIPS 32 实验（位于 `lab-{exercise,solution}/mips32` 下）：mips-linux-gnu-gcc、mips-linux-gnu-gdb 或 gdb-multiarch、qemu-system-mipsel
  - LoongArch 32 Reduced 实验（位于 `lab-{exercise,solution}/la32r` 下）：loongarch32r-linux-gnusf-gcc、loongarch32r-linux-gnusf-gdb、qemu-system-loongarch32

### 测试验证

在代码的根目录下，运行形如命令 `make test lab=x_y` 的命令，即可编译对应的测试点 `tests/labx_y`，随后运行 `make run` 即可运行该测试点。学生可通过比对输出结果与预期结果，自行检查代码的正确性。

设置环境变量 `MOS_PROFILE=release` 可启用编译优化。若开启编译优化后，发现测试点的输出结果不符合预期，则可能是由于代码中存在未定义行为，编译优化后产生了非预期的机器码。通常，编译警告指示了潜在的问题，建议学生尽可能消除编译警告。

## 报告缺陷或提供建议

如果你发现实验框架代码中的缺陷，或者有任何建议，请新建 issue。

## 时效性说明

课程组定期将课程代码同步至本仓库，但不提供实时更新的担保。
