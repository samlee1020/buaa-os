# lab1实验报告

## 思考题

### Thinking 1.1

> Thinking 1.1 在阅读 附录中的编译链接详解 以及本章内容后，尝试分别使用实验环境中 的原生 x86 工具链（gcc、ld、readelf、objdump 等）和 MIPS 交叉编译工具链（带有 mips-linux-gnu- 前缀，如 mips-linux-gnu-gcc、mips-linux-gnu-ld），重复其中的编 译和解析过程，观察相应的结果，并解释其中向 objdump 传入的参数的含义。 

编写一个简单的C语言程序如下

```c
// main.c
#include<stdio.h>
int main() {
        printf("hello,world!\n");
        return 0;
}
```

分别调用x86和交叉编译器来编译该文件

```bash
gcc -o main_x86 main.c
mips-linux-gnu-gcc -o main_mips hello.c
```

对编译结果解析之后可以发现两者不同，最直观的不同是反汇编之后出现了不同架构的汇编代码。

#### 一、解析main_x86

##### readelf

执行`readelf -a main_x86`得到以下结果

```bash
ELF 头：
  Magic：   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00 
  类别:                              ELF64
  数据:                              2 补码，小端序 (little endian)
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI 版本:                          0
  类型:                              DYN (Position-Independent Executable file)
  系统架构:                          Advanced Micro Devices X86-64
  版本:                              0x1
  入口点地址：               0x1060
  程序头起点：          64 (bytes into file)
  Start of section headers:          13976 (bytes into file)
  标志：             0x0
  Size of this header:               64 (bytes)
  Size of program headers:           56 (bytes)
  Number of program headers:         13
  Size of section headers:           64 (bytes)
  Number of section headers:         31
  Section header string table index: 30

节头：
  [号] 名称              类型             地址              偏移量
       大小              全体大小          旗标   链接   信息   对齐
  [ 0]                   NULL             0000000000000000  00000000
       0000000000000000  0000000000000000           0     0     0
  [ 1] .interp           PROGBITS         0000000000000318  00000318
       000000000000001c  0000000000000000   A       0     0     1
  [ 2] .note.gnu.pr[...] NOTE             0000000000000338  00000338
       0000000000000030  0000000000000000   A       0     0     8
  [ 3] .note.gnu.bu[...] NOTE             0000000000000368  00000368
       0000000000000024  0000000000000000   A       0     0     4
  [ 4] .note.ABI-tag     NOTE             000000000000038c  0000038c
       0000000000000020  0000000000000000   A       0     0     4
  [ 5] .gnu.hash         GNU_HASH         00000000000003b0  000003b0
       0000000000000024  0000000000000000   A       6     0     8
  [ 6] .dynsym           DYNSYM           00000000000003d8  000003d8
       00000000000000a8  0000000000000018   A       7     1     8
  [ 7] .dynstr           STRTAB           0000000000000480  00000480
       000000000000008d  0000000000000000   A       0     0     1
  [ 8] .gnu.version      VERSYM           000000000000050e  0000050e
       000000000000000e  0000000000000002   A       6     0     2
  [ 9] .gnu.version_r    VERNEED          0000000000000520  00000520
       0000000000000030  0000000000000000   A       7     1     8
  [10] .rela.dyn         RELA             0000000000000550  00000550
       00000000000000c0  0000000000000018   A       6     0     8
  [11] .rela.plt         RELA             0000000000000610  00000610
       0000000000000018  0000000000000018  AI       6    24     8
  [12] .init             PROGBITS         0000000000001000  00001000
       000000000000001b  0000000000000000  AX       0     0     4
  [13] .plt              PROGBITS         0000000000001020  00001020
       0000000000000020  0000000000000010  AX       0     0     16
  [14] .plt.got          PROGBITS         0000000000001040  00001040
       0000000000000010  0000000000000010  AX       0     0     16
  [15] .plt.sec          PROGBITS         0000000000001050  00001050
       0000000000000010  0000000000000010  AX       0     0     16
  [16] .text             PROGBITS         0000000000001060  00001060
       0000000000000107  0000000000000000  AX       0     0     16
  [17] .fini             PROGBITS         0000000000001168  00001168
       000000000000000d  0000000000000000  AX       0     0     4
  [18] .rodata           PROGBITS         0000000000002000  00002000
       0000000000000011  0000000000000000   A       0     0     4
  [19] .eh_frame_hdr     PROGBITS         0000000000002014  00002014
       0000000000000034  0000000000000000   A       0     0     4
  [20] .eh_frame         PROGBITS         0000000000002048  00002048
       00000000000000ac  0000000000000000   A       0     0     8
  [21] .init_array       INIT_ARRAY       0000000000003db8  00002db8
       0000000000000008  0000000000000008  WA       0     0     8
  [22] .fini_array       FINI_ARRAY       0000000000003dc0  00002dc0
       0000000000000008  0000000000000008  WA       0     0     8
  [23] .dynamic          DYNAMIC          0000000000003dc8  00002dc8
       00000000000001f0  0000000000000010  WA       7     0     8
  [24] .got              PROGBITS         0000000000003fb8  00002fb8
       0000000000000048  0000000000000008  WA       0     0     8
  [25] .data             PROGBITS         0000000000004000  00003000
       0000000000000010  0000000000000000  WA       0     0     8
  [26] .bss              NOBITS           0000000000004010  00003010
       0000000000000008  0000000000000000  WA       0     0     1
  [27] .comment          PROGBITS         0000000000000000  00003010
       000000000000002b  0000000000000001  MS       0     0     1
  [28] .symtab           SYMTAB           0000000000000000  00003040
       0000000000000360  0000000000000018          29    18     8
  [29] .strtab           STRTAB           0000000000000000  000033a0
       00000000000001da  0000000000000000           0     0     1
  [30] .shstrtab         STRTAB           0000000000000000  0000357a
       000000000000011a  0000000000000000           0     0     1
Key to Flags:
  W (write), A (alloc), X (execute), M (merge), S (strings), I (info),
  L (link order), O (extra OS processing required), G (group), T (TLS),
  C (compressed), x (unknown), o (OS specific), E (exclude),
  D (mbind), l (large), p (processor specific)

There are no section groups in this file.

程序头：
  Type           Offset             VirtAddr           PhysAddr
                 FileSiz            MemSiz              Flags  Align
  PHDR           0x0000000000000040 0x0000000000000040 0x0000000000000040
                 0x00000000000002d8 0x00000000000002d8  R      0x8
  INTERP         0x0000000000000318 0x0000000000000318 0x0000000000000318
                 0x000000000000001c 0x000000000000001c  R      0x1
      [Requesting program interpreter: /lib64/ld-linux-x86-64.so.2]
  LOAD           0x0000000000000000 0x0000000000000000 0x0000000000000000
                 0x0000000000000628 0x0000000000000628  R      0x1000
  LOAD           0x0000000000001000 0x0000000000001000 0x0000000000001000
                 0x0000000000000175 0x0000000000000175  R E    0x1000
  LOAD           0x0000000000002000 0x0000000000002000 0x0000000000002000
                 0x00000000000000f4 0x00000000000000f4  R      0x1000
  LOAD           0x0000000000002db8 0x0000000000003db8 0x0000000000003db8
                 0x0000000000000258 0x0000000000000260  RW     0x1000
  DYNAMIC        0x0000000000002dc8 0x0000000000003dc8 0x0000000000003dc8
                 0x00000000000001f0 0x00000000000001f0  RW     0x8
  NOTE           0x0000000000000338 0x0000000000000338 0x0000000000000338
                 0x0000000000000030 0x0000000000000030  R      0x8
  NOTE           0x0000000000000368 0x0000000000000368 0x0000000000000368
                 0x0000000000000044 0x0000000000000044  R      0x4
  GNU_PROPERTY   0x0000000000000338 0x0000000000000338 0x0000000000000338
                 0x0000000000000030 0x0000000000000030  R      0x8
  GNU_EH_FRAME   0x0000000000002014 0x0000000000002014 0x0000000000002014
                 0x0000000000000034 0x0000000000000034  R      0x4
  GNU_STACK      0x0000000000000000 0x0000000000000000 0x0000000000000000
                 0x0000000000000000 0x0000000000000000  RW     0x10
  GNU_RELRO      0x0000000000002db8 0x0000000000003db8 0x0000000000003db8
                 0x0000000000000248 0x0000000000000248  R      0x1

 Section to Segment mapping:
  段节...
   00     
   01     .interp 
   02     .interp .note.gnu.property .note.gnu.build-id .note.ABI-tag .gnu.hash .dynsym .dynstr .gnu.version .gnu.version_r .rela.dyn .rela.plt 
   03     .init .plt .plt.got .plt.sec .text .fini 
   04     .rodata .eh_frame_hdr .eh_frame 
   05     .init_array .fini_array .dynamic .got .data .bss 
   06     .dynamic 
   07     .note.gnu.property 
   08     .note.gnu.build-id .note.ABI-tag 
   09     .note.gnu.property 
   10     .eh_frame_hdr 
   11     
   12     .init_array .fini_array .dynamic .got 

Dynamic section at offset 0x2dc8 contains 27 entries:
  标记        类型                         名称/值
 0x0000000000000001 (NEEDED)             共享库：[libc.so.6]
 0x000000000000000c (INIT)               0x1000
 0x000000000000000d (FINI)               0x1168
 0x0000000000000019 (INIT_ARRAY)         0x3db8
 0x000000000000001b (INIT_ARRAYSZ)       8 (bytes)
 0x000000000000001a (FINI_ARRAY)         0x3dc0
 0x000000000000001c (FINI_ARRAYSZ)       8 (bytes)
 0x000000006ffffef5 (GNU_HASH)           0x3b0
 0x0000000000000005 (STRTAB)             0x480
 0x0000000000000006 (SYMTAB)             0x3d8
 0x000000000000000a (STRSZ)              141 (bytes)
 0x000000000000000b (SYMENT)             24 (bytes)
 0x0000000000000015 (DEBUG)              0x0
 0x0000000000000003 (PLTGOT)             0x3fb8
 0x0000000000000002 (PLTRELSZ)           24 (bytes)
 0x0000000000000014 (PLTREL)             RELA
 0x0000000000000017 (JMPREL)             0x610
 0x0000000000000007 (RELA)               0x550
 0x0000000000000008 (RELASZ)             192 (bytes)
 0x0000000000000009 (RELAENT)            24 (bytes)
 0x000000000000001e (FLAGS)              BIND_NOW
 0x000000006ffffffb (FLAGS_1)            标志： NOW PIE
 0x000000006ffffffe (VERNEED)            0x520
 0x000000006fffffff (VERNEEDNUM)         1
 0x000000006ffffff0 (VERSYM)             0x50e
 0x000000006ffffff9 (RELACOUNT)          3
 0x0000000000000000 (NULL)               0x0

重定位节 '.rela.dyn' at offset 0x550 contains 8 entries:
  偏移量          信息           类型           符号值        符号名称 + 加数
000000003db8  000000000008 R_X86_64_RELATIVE                    1140
000000003dc0  000000000008 R_X86_64_RELATIVE                    1100
000000004008  000000000008 R_X86_64_RELATIVE                    4008
000000003fd8  000100000006 R_X86_64_GLOB_DAT 0000000000000000 __libc_start_main@GLIBC_2.34 + 0
000000003fe0  000200000006 R_X86_64_GLOB_DAT 0000000000000000 _ITM_deregisterTM[...] + 0
000000003fe8  000400000006 R_X86_64_GLOB_DAT 0000000000000000 __gmon_start__ + 0
000000003ff0  000500000006 R_X86_64_GLOB_DAT 0000000000000000 _ITM_registerTMCl[...] + 0
000000003ff8  000600000006 R_X86_64_GLOB_DAT 0000000000000000 __cxa_finalize@GLIBC_2.2.5 + 0

重定位节 '.rela.plt' at offset 0x610 contains 1 entry:
  偏移量          信息           类型           符号值        符号名称 + 加数
000000003fd0  000300000007 R_X86_64_JUMP_SLO 0000000000000000 puts@GLIBC_2.2.5 + 0
No processor specific unwind information to decode

Symbol table '.dynsym' contains 7 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND _[...]@GLIBC_2.34 (2)
     2: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_deregisterT[...]
     3: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND puts@GLIBC_2.2.5 (3)
     4: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND __gmon_start__
     5: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_registerTMC[...]
     6: 0000000000000000     0 FUNC    WEAK   DEFAULT  UND [...]@GLIBC_2.2.5 (3)

Symbol table '.symtab' contains 36 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS Scrt1.o
     2: 000000000000038c    32 OBJECT  LOCAL  DEFAULT    4 __abi_tag
     3: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS crtstuff.c
     4: 0000000000001090     0 FUNC    LOCAL  DEFAULT   16 deregister_tm_clones
     5: 00000000000010c0     0 FUNC    LOCAL  DEFAULT   16 register_tm_clones
     6: 0000000000001100     0 FUNC    LOCAL  DEFAULT   16 __do_global_dtors_aux
     7: 0000000000004010     1 OBJECT  LOCAL  DEFAULT   26 completed.0
     8: 0000000000003dc0     0 OBJECT  LOCAL  DEFAULT   22 __do_global_dtor[...]
     9: 0000000000001140     0 FUNC    LOCAL  DEFAULT   16 frame_dummy
    10: 0000000000003db8     0 OBJECT  LOCAL  DEFAULT   21 __frame_dummy_in[...]
    11: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS main.c
    12: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS crtstuff.c
    13: 00000000000020f0     0 OBJECT  LOCAL  DEFAULT   20 __FRAME_END__
    14: 0000000000000000     0 FILE    LOCAL  DEFAULT  ABS 
    15: 0000000000003dc8     0 OBJECT  LOCAL  DEFAULT   23 _DYNAMIC
    16: 0000000000002014     0 NOTYPE  LOCAL  DEFAULT   19 __GNU_EH_FRAME_HDR
    17: 0000000000003fb8     0 OBJECT  LOCAL  DEFAULT   24 _GLOBAL_OFFSET_TABLE_
    18: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND __libc_start_mai[...]
    19: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_deregisterT[...]
    20: 0000000000004000     0 NOTYPE  WEAK   DEFAULT   25 data_start
    21: 0000000000000000     0 FUNC    GLOBAL DEFAULT  UND puts@GLIBC_2.2.5
    22: 0000000000004010     0 NOTYPE  GLOBAL DEFAULT   25 _edata
    23: 0000000000001168     0 FUNC    GLOBAL HIDDEN    17 _fini
    24: 0000000000004000     0 NOTYPE  GLOBAL DEFAULT   25 __data_start
    25: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND __gmon_start__
    26: 0000000000004008     0 OBJECT  GLOBAL HIDDEN    25 __dso_handle
    27: 0000000000002000     4 OBJECT  GLOBAL DEFAULT   18 _IO_stdin_used
    28: 0000000000004018     0 NOTYPE  GLOBAL DEFAULT   26 _end
    29: 0000000000001060    38 FUNC    GLOBAL DEFAULT   16 _start
    30: 0000000000004010     0 NOTYPE  GLOBAL DEFAULT   26 __bss_start
    31: 0000000000001149    30 FUNC    GLOBAL DEFAULT   16 main
    32: 0000000000004010     0 OBJECT  GLOBAL HIDDEN    25 __TMC_END__
    33: 0000000000000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_registerTMC[...]
    34: 0000000000000000     0 FUNC    WEAK   DEFAULT  UND __cxa_finalize@G[...]
    35: 0000000000001000     0 FUNC    GLOBAL HIDDEN    12 _init

Histogram for `.gnu.hash' bucket list length (total of 2 buckets):
 Length  Number     % of total  Coverage
      0  1          ( 50.0%)
      1  1          ( 50.0%)    100.0%

Version symbols section '.gnu.version' contains 7 entries:
 Addr: 0x000000000000050e  Offset: 0x0000050e  Link: 6 (.dynsym)
  000:   0 (*本地*)       2 (GLIBC_2.34)    1 (*全局*)      3 (GLIBC_2.2.5)
  004:   1 (*全局*)      1 (*全局*)      3 (GLIBC_2.2.5)

Version needs section '.gnu.version_r' contains 1 entry:
 Addr: 0x0000000000000520  Offset: 0x00000520  Link: 7 (.dynstr)
  000000: Version: 1  文件：libc.so.6  计数：2
  0x0010:   Name: GLIBC_2.2.5  标志：无  版本：3
  0x0020:   Name: GLIBC_2.34  标志：无  版本：2

Displaying notes found in: .note.gnu.property
  所有者            Data size   Description
  GNU                  0x00000020       NT_GNU_PROPERTY_TYPE_0
      Properties: x86 feature: IBT, SHSTK
        x86 ISA needed: x86-64-baseline

Displaying notes found in: .note.gnu.build-id
  所有者            Data size   Description
  GNU                  0x00000014       NT_GNU_BUILD_ID (unique build ID bitstring)
    Build ID: 5aa0972205128976248014c187fa1d21c7b705f8

Displaying notes found in: .note.ABI-tag
  所有者            Data size   Description
  GNU                  0x00000010       NT_GNU_ABI_TAG (ABI version tag)
    OS: Linux, ABI: 3.2.0
```

##### objdump

执行`objdump -S main_x86`得到以下结果

```bash
main_x86：     文件格式 elf64-x86-64


Disassembly of section .init:

0000000000001000 <_init>:
    1000:       f3 0f 1e fa             endbr64
    1004:       48 83 ec 08             sub    $0x8,%rsp
    1008:       48 8b 05 d9 2f 00 00    mov    0x2fd9(%rip),%rax        # 3fe8 <__gmon_start__@Base>
    100f:       48 85 c0                test   %rax,%rax
    1012:       74 02                   je     1016 <_init+0x16>
    1014:       ff d0                   call   *%rax
    1016:       48 83 c4 08             add    $0x8,%rsp
    101a:       c3                      ret

Disassembly of section .plt:

0000000000001020 <.plt>:
    1020:       ff 35 9a 2f 00 00       push   0x2f9a(%rip)        # 3fc0 <_GLOBAL_OFFSET_TABLE_+0x8>
    1026:       ff 25 9c 2f 00 00       jmp    *0x2f9c(%rip)        # 3fc8 <_GLOBAL_OFFSET_TABLE_+0x10>
    102c:       0f 1f 40 00             nopl   0x0(%rax)
    1030:       f3 0f 1e fa             endbr64
    1034:       68 00 00 00 00          push   $0x0
    1039:       e9 e2 ff ff ff          jmp    1020 <_init+0x20>
    103e:       66 90                   xchg   %ax,%ax

Disassembly of section .plt.got:

0000000000001040 <__cxa_finalize@plt>:
    1040:       f3 0f 1e fa             endbr64
    1044:       ff 25 ae 2f 00 00       jmp    *0x2fae(%rip)        # 3ff8 <__cxa_finalize@GLIBC_2.2.5>
    104a:       66 0f 1f 44 00 00       nopw   0x0(%rax,%rax,1)

Disassembly of section .plt.sec:

0000000000001050 <puts@plt>:
    1050:       f3 0f 1e fa             endbr64
    1054:       ff 25 76 2f 00 00       jmp    *0x2f76(%rip)        # 3fd0 <puts@GLIBC_2.2.5>
    105a:       66 0f 1f 44 00 00       nopw   0x0(%rax,%rax,1)

Disassembly of section .text:

0000000000001060 <_start>:
    1060:       f3 0f 1e fa             endbr64
    1064:       31 ed                   xor    %ebp,%ebp
    1066:       49 89 d1                mov    %rdx,%r9
    1069:       5e                      pop    %rsi
    106a:       48 89 e2                mov    %rsp,%rdx
    106d:       48 83 e4 f0             and    $0xfffffffffffffff0,%rsp
    1071:       50                      push   %rax
    1072:       54                      push   %rsp
    1073:       45 31 c0                xor    %r8d,%r8d
    1076:       31 c9                   xor    %ecx,%ecx
    1078:       48 8d 3d ca 00 00 00    lea    0xca(%rip),%rdi        # 1149 <main>
    107f:       ff 15 53 2f 00 00       call   *0x2f53(%rip)        # 3fd8 <__libc_start_main@GLIBC_2.34>
    1085:       f4                      hlt
    1086:       66 2e 0f 1f 84 00 00    cs nopw 0x0(%rax,%rax,1)
    108d:       00 00 00 

0000000000001090 <deregister_tm_clones>:
    1090:       48 8d 3d 79 2f 00 00    lea    0x2f79(%rip),%rdi        # 4010 <__TMC_END__>
    1097:       48 8d 05 72 2f 00 00    lea    0x2f72(%rip),%rax        # 4010 <__TMC_END__>
    109e:       48 39 f8                cmp    %rdi,%rax
    10a1:       74 15                   je     10b8 <deregister_tm_clones+0x28>
    10a3:       48 8b 05 36 2f 00 00    mov    0x2f36(%rip),%rax        # 3fe0 <_ITM_deregisterTMCloneTable@Base>
    10aa:       48 85 c0                test   %rax,%rax
    10ad:       74 09                   je     10b8 <deregister_tm_clones+0x28>
    10af:       ff e0                   jmp    *%rax
    10b1:       0f 1f 80 00 00 00 00    nopl   0x0(%rax)
    10b8:       c3                      ret
    10b9:       0f 1f 80 00 00 00 00    nopl   0x0(%rax)

00000000000010c0 <register_tm_clones>:
    10c0:       48 8d 3d 49 2f 00 00    lea    0x2f49(%rip),%rdi        # 4010 <__TMC_END__>
    10c7:       48 8d 35 42 2f 00 00    lea    0x2f42(%rip),%rsi        # 4010 <__TMC_END__>
    10ce:       48 29 fe                sub    %rdi,%rsi
    10d1:       48 89 f0                mov    %rsi,%rax
    10d4:       48 c1 ee 3f             shr    $0x3f,%rsi
    10d8:       48 c1 f8 03             sar    $0x3,%rax
    10dc:       48 01 c6                add    %rax,%rsi
    10df:       48 d1 fe                sar    $1,%rsi
    10e2:       74 14                   je     10f8 <register_tm_clones+0x38>
    10e4:       48 8b 05 05 2f 00 00    mov    0x2f05(%rip),%rax        # 3ff0 <_ITM_registerTMCloneTable@Base>
    10eb:       48 85 c0                test   %rax,%rax
    10ee:       74 08                   je     10f8 <register_tm_clones+0x38>
    10f0:       ff e0                   jmp    *%rax
    10f2:       66 0f 1f 44 00 00       nopw   0x0(%rax,%rax,1)
    10f8:       c3                      ret
    10f9:       0f 1f 80 00 00 00 00    nopl   0x0(%rax)

0000000000001100 <__do_global_dtors_aux>:
    1100:       f3 0f 1e fa             endbr64
    1104:       80 3d 05 2f 00 00 00    cmpb   $0x0,0x2f05(%rip)        # 4010 <__TMC_END__>
    110b:       75 2b                   jne    1138 <__do_global_dtors_aux+0x38>
    110d:       55                      push   %rbp
    110e:       48 83 3d e2 2e 00 00    cmpq   $0x0,0x2ee2(%rip)        # 3ff8 <__cxa_finalize@GLIBC_2.2.5>
    1115:       00 
    1116:       48 89 e5                mov    %rsp,%rbp
    1119:       74 0c                   je     1127 <__do_global_dtors_aux+0x27>
    111b:       48 8b 3d e6 2e 00 00    mov    0x2ee6(%rip),%rdi        # 4008 <__dso_handle>
    1122:       e8 19 ff ff ff          call   1040 <__cxa_finalize@plt>
    1127:       e8 64 ff ff ff          call   1090 <deregister_tm_clones>
    112c:       c6 05 dd 2e 00 00 01    movb   $0x1,0x2edd(%rip)        # 4010 <__TMC_END__>
    1133:       5d                      pop    %rbp
    1134:       c3                      ret
    1135:       0f 1f 00                nopl   (%rax)
    1138:       c3                      ret
    1139:       0f 1f 80 00 00 00 00    nopl   0x0(%rax)

0000000000001140 <frame_dummy>:
    1140:       f3 0f 1e fa             endbr64
    1144:       e9 77 ff ff ff          jmp    10c0 <register_tm_clones>

0000000000001149 <main>:
    1149:       f3 0f 1e fa             endbr64
    114d:       55                      push   %rbp
    114e:       48 89 e5                mov    %rsp,%rbp
    1151:       48 8d 05 ac 0e 00 00    lea    0xeac(%rip),%rax        # 2004 <_IO_stdin_used+0x4>
    1158:       48 89 c7                mov    %rax,%rdi
    115b:       e8 f0 fe ff ff          call   1050 <puts@plt>
    1160:       b8 00 00 00 00          mov    $0x0,%eax
    1165:       5d                      pop    %rbp
    1166:       c3                      ret

Disassembly of section .fini:

0000000000001168 <_fini>:
    1168:       f3 0f 1e fa             endbr64
    116c:       48 83 ec 08             sub    $0x8,%rsp
    1170:       48 83 c4 08             add    $0x8,%rsp
    1174:       c3                      ret
```

#### 二、解析main_mips

##### readelf

执行`mips-linux-gnu-readelf -a main_mips`得到以下结果

```bash
ELF 头：
  Magic：   7f 45 4c 46 01 02 01 00 00 00 00 00 00 00 00 00 
  类别:                              ELF32
  数据:                              2 补码，大端序 (big endian)
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI 版本:                          0
  类型:                              EXEC (可执行文件)
  系统架构:                          MIPS R3000
  版本:                              0x1
  入口点地址：               0x400520
  程序头起点：          52 (bytes into file)
  Start of section headers:          67612 (bytes into file)
  标志：             0x70001007, noreorder, pic, cpic, o32, mips32r2
  Size of this header:               52 (bytes)
  Size of program headers:           32 (bytes)
  Number of program headers:         11
  Size of section headers:           40 (bytes)
  Number of section headers:         32
  Section header string table index: 31

节头：
  [Nr] Name              Type            Addr     Off    Size   ES Flg Lk Inf Al
  [ 0]                   NULL            00000000 000000 000000 00      0   0  0
  [ 1] .interp           PROGBITS        00400194 000194 00000d 00   A  0   0  1
  [ 2] .MIPS.abiflags    MIPS_ABIFLAGS   004001a8 0001a8 000018 18   A  0   0  8
  [ 3] .reginfo          MIPS_REGINFO    004001c0 0001c0 000018 18   A  0   0  4
  [ 4] .note.gnu.bu[...] NOTE            004001d8 0001d8 000024 00   A  0   0  4
  [ 5] .note.ABI-tag     NOTE            004001fc 0001fc 000020 00   A  0   0  4
  [ 6] .dynamic          DYNAMIC         0040021c 00021c 000100 08   A  9   0  4
  [ 7] .hash             HASH            0040031c 00031c 00003c 04   A  8   0  4
  [ 8] .dynsym           DYNSYM          00400358 000358 0000a0 10   A  9   1  4
  [ 9] .dynstr           STRTAB          004003f8 0003f8 0000a6 00   A  0   0  1
  [10] .gnu.version      VERSYM          0040049e 00049e 000014 02   A  8   0  2
  [11] .gnu.version_r    VERNEED         004004b4 0004b4 000030 00   A  9   1  4
  [12] .init             PROGBITS        004004e4 0004e4 00003c 00  AX  0   0  4
  [13] .text             PROGBITS        00400520 000520 000190 00  AX  0   0 16
  [14] .MIPS.stubs       PROGBITS        004006b0 0006b0 000030 00  AX  0   0  4
  [15] .fini             PROGBITS        004006e0 0006e0 000024 00  AX  0   0  4
  [16] .rodata           PROGBITS        00400710 000710 000020 00   A  0   0 16
  [17] .eh_frame         PROGBITS        00400730 000730 000004 00   A  0   0  4
  [18] .init_array       INIT_ARRAY      0041fff8 00fff8 000004 04  WA  0   0  4
  [19] .fini_array       FINI_ARRAY      0041fffc 00fffc 000004 04  WA  0   0  4
  [20] .data             PROGBITS        00420000 010000 000010 00  WA  0   0 16
  [21] .rld_map          PROGBITS        00420010 010010 000004 00  WA  0   0  4
  [22] .got              PROGBITS        00420020 010020 000020 04 WAp  0   0 16
  [23] .sdata            PROGBITS        00420040 010040 000004 00 WAp  0   0  4
  [24] .bss              NOBITS          00420050 010044 000010 00  WA  0   0 16
  [25] .comment          PROGBITS        00000000 010044 000026 01  MS  0   0  1
  [26] .pdr              PROGBITS        00000000 01006c 000020 00      0   0  4
  [27] .gnu.attributes   GNU_ATTRIBUTES  00000000 01008c 000010 00      0   0  1
  [28] .mdebug.abi32     PROGBITS        00000000 01009c 000000 00      0   0  1
  [29] .symtab           SYMTAB          00000000 01009c 000460 10     30  47  4
  [30] .strtab           STRTAB          00000000 0104fc 0001f5 00      0   0  1
  [31] .shstrtab         STRTAB          00000000 0106f1 00012b 00      0   0  1
Key to Flags:
  W (write), A (alloc), X (execute), M (merge), S (strings), I (info),
  L (link order), O (extra OS processing required), G (group), T (TLS),
  C (compressed), x (unknown), o (OS specific), E (exclude),
  D (mbind), p (processor specific)

There are no section groups in this file.

程序头：
  Type           Offset   VirtAddr   PhysAddr   FileSiz MemSiz  Flg Align
  PHDR           0x000034 0x00400034 0x00400034 0x00160 0x00160 R   0x4
  INTERP         0x000194 0x00400194 0x00400194 0x0000d 0x0000d R   0x1
      [Requesting program interpreter: /lib/ld.so.1]
  ABIFLAGS       0x0001a8 0x004001a8 0x004001a8 0x00018 0x00018 R   0x8
  REGINFO        0x0001c0 0x004001c0 0x004001c0 0x00018 0x00018 R   0x4
  LOAD           0x000000 0x00400000 0x00400000 0x00734 0x00734 R E 0x10000
  LOAD           0x00fff8 0x0041fff8 0x0041fff8 0x0004c 0x00068 RW  0x10000
  DYNAMIC        0x00021c 0x0040021c 0x0040021c 0x00100 0x00100 R   0x4
  NOTE           0x0001d8 0x004001d8 0x004001d8 0x00044 0x00044 R   0x4
  GNU_STACK      0x000000 0x00000000 0x00000000 0x00000 0x00000 RWE 0x10
  GNU_RELRO      0x00fff8 0x0041fff8 0x0041fff8 0x00008 0x00008 R   0x1
  NULL           0x000000 0x00000000 0x00000000 0x00000 0x00000     0x4

 Section to Segment mapping:
  段节...
   00     
   01     .interp 
   02     .MIPS.abiflags 
   03     .reginfo 
   04     .interp .MIPS.abiflags .reginfo .note.gnu.build-id .note.ABI-tag .dynamic .hash .dynsym .dynstr .gnu.version .gnu.version_r .init .text .MIPS.stubs .fini .rodata .eh_frame 
   05     .init_array .fini_array .data .rld_map .got .sdata .bss 
   06     .dynamic 
   07     .note.gnu.build-id .note.ABI-tag 
   08     
   09     .init_array .fini_array 
   10     

Dynamic section at offset 0x21c contains 27 entries:
  标记        类型                         名称/值
 0x00000001 (NEEDED)                     共享库：[libc.so.6]
 0x0000000c (INIT)                       0x4004e4
 0x0000000d (FINI)                       0x4006e0
 0x00000019 (INIT_ARRAY)                 0x41fff8
 0x0000001b (INIT_ARRAYSZ)               4 (bytes)
 0x0000001a (FINI_ARRAY)                 0x41fffc
 0x0000001c (FINI_ARRAYSZ)               4 (bytes)
 0x00000004 (HASH)                       0x40031c
 0x00000005 (STRTAB)                     0x4003f8
 0x00000006 (SYMTAB)                     0x400358
 0x0000000a (STRSZ)                      166 (bytes)
 0x0000000b (SYMENT)                     16 (bytes)
 0x70000016 (MIPS_RLD_MAP)               0x420010
 0x70000035 (MIPS_RLD_MAP_REL)           0x1fd8c
 0x00000015 (DEBUG)                      0x0
 0x00000003 (PLTGOT)                     0x420020
 0x70000001 (MIPS_RLD_VERSION)           1
 0x70000005 (MIPS_FLAGS)                 NOTPOT
 0x70000006 (MIPS_BASE_ADDRESS)          0x400000
 0x7000000a (MIPS_LOCAL_GOTNO)           3
 0x70000011 (MIPS_SYMTABNO)              10
 0x70000012 (MIPS_UNREFEXTNO)            29
 0x70000013 (MIPS_GOTSYM)                0x5
 0x6ffffffe (VERNEED)                    0x4004b4
 0x6fffffff (VERNEEDNUM)                 1
 0x6ffffff0 (VERSYM)                     0x40049e
 0x00000000 (NULL)                       0x0

该文件中没有重定位信息。

The decoding of unwind sections for machine type MIPS R3000 is not currently supported.

Symbol table '.dynsym' contains 10 entries:
   Num:    Value  Size Type    Bind   Vis      Ndx Name
     0: 00000000     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: 00000001     0 SECTION GLOBAL DEFAULT  ABS _DYNAMIC_LINKING
     2: 00400710     4 OBJECT  GLOBAL DEFAULT   16 _IO_stdin_used
     3: 00420010     0 OBJECT  GLOBAL DEFAULT   21 __RLD_MAP
     4: 00400650    84 FUNC    GLOBAL DEFAULT   13 main
     5: 00000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_registerTMC[...]
     6: 00000000     0 FUNC    WEAK   DEFAULT  UND __gmon_start__
     7: 004006c0     0 FUNC    GLOBAL DEFAULT  UND puts@GLIBC_2.0 (3)
     8: 00000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_deregisterT[...]
     9: 004006b0     0 FUNC    GLOBAL DEFAULT  UND _[...]@GLIBC_2.34 (2)

Symbol table '.symtab' contains 70 entries:
   Num:    Value  Size Type    Bind   Vis      Ndx Name
     0: 00000000     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: 00400194     0 SECTION LOCAL  DEFAULT    1 .interp
     2: 004001a8     0 SECTION LOCAL  DEFAULT    2 .MIPS.abiflags
     3: 004001c0     0 SECTION LOCAL  DEFAULT    3 .reginfo
     4: 004001d8     0 SECTION LOCAL  DEFAULT    4 .note.gnu.build-id
     5: 004001fc     0 SECTION LOCAL  DEFAULT    5 .note.ABI-tag
     6: 0040021c     0 SECTION LOCAL  DEFAULT    6 .dynamic
     7: 0040031c     0 SECTION LOCAL  DEFAULT    7 .hash
     8: 00400358     0 SECTION LOCAL  DEFAULT    8 .dynsym
     9: 004003f8     0 SECTION LOCAL  DEFAULT    9 .dynstr
    10: 0040049e     0 SECTION LOCAL  DEFAULT   10 .gnu.version
    11: 004004b4     0 SECTION LOCAL  DEFAULT   11 .gnu.version_r
    12: 004004e4     0 SECTION LOCAL  DEFAULT   12 .init
    13: 00400520     0 SECTION LOCAL  DEFAULT   13 .text
    14: 004006b0     0 SECTION LOCAL  DEFAULT   14 .MIPS.stubs
    15: 004006e0     0 SECTION LOCAL  DEFAULT   15 .fini
    16: 00400710     0 SECTION LOCAL  DEFAULT   16 .rodata
    17: 00400730     0 SECTION LOCAL  DEFAULT   17 .eh_frame
    18: 0041fff8     0 SECTION LOCAL  DEFAULT   18 .init_array
    19: 0041fffc     0 SECTION LOCAL  DEFAULT   19 .fini_array
    20: 00420000     0 SECTION LOCAL  DEFAULT   20 .data
    21: 00420010     0 SECTION LOCAL  DEFAULT   21 .rld_map
    22: 00420020     0 SECTION LOCAL  DEFAULT   22 .got
    23: 00420040     0 SECTION LOCAL  DEFAULT   23 .sdata
    24: 00420050     0 SECTION LOCAL  DEFAULT   24 .bss
    25: 00000000     0 SECTION LOCAL  DEFAULT   25 .comment
    26: 00000000     0 SECTION LOCAL  DEFAULT   26 .pdr
    27: 00000000     0 SECTION LOCAL  DEFAULT   27 .gnu.attributes
    28: 00000000     0 SECTION LOCAL  DEFAULT   28 .mdebug.abi32
    29: 00000000     0 FILE    LOCAL  DEFAULT  ABS crt1.o
    30: 004001fc    32 OBJECT  LOCAL  DEFAULT    5 __abi_tag
    31: 00400570     0 NOTYPE  LOCAL  DEFAULT   13 hlt
    32: 00000000     0 FILE    LOCAL  DEFAULT  ABS crtstuff.c
    33: 00400580     0 FUNC    LOCAL  DEFAULT   13 deregister_tm_clones
    34: 004005b8     0 FUNC    LOCAL  DEFAULT   13 register_tm_clones
    35: 00400604     0 FUNC    LOCAL  DEFAULT   13 __do_global_dtors_aux
    36: 00420050     1 OBJECT  LOCAL  DEFAULT   24 completed.0
    37: 0041fffc     0 OBJECT  LOCAL  DEFAULT   19 __do_global_dtor[...]
    38: 00400640     0 FUNC    LOCAL  DEFAULT   13 frame_dummy
    39: 0041fff8     0 OBJECT  LOCAL  DEFAULT   18 __frame_dummy_in[...]
    40: 00000000     0 FILE    LOCAL  DEFAULT  ABS main.c
    41: 00000000     0 FILE    LOCAL  DEFAULT  ABS crtstuff.c
    42: 00400730     0 OBJECT  LOCAL  DEFAULT   17 __FRAME_END__
    43: 00000000     0 FILE    LOCAL  DEFAULT  ABS 
    44: 004006b0     0 FUNC    LOCAL  DEFAULT   14 _MIPS_STUBS_
    45: 0040021c     0 OBJECT  LOCAL  DEFAULT    6 _DYNAMIC
    46: 00428010     0 NOTYPE  LOCAL  DEFAULT   22 _gp
    47: 004006b0     0 FUNC    GLOBAL DEFAULT  UND __libc_start_mai[...]
    48: 00000001     0 SECTION GLOBAL DEFAULT  ABS _DYNAMIC_LINKING
    49: 00000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_deregisterT[...]
    50: 00420000     0 NOTYPE  WEAK   DEFAULT   20 data_start
    51: 00420044     0 NOTYPE  GLOBAL DEFAULT   23 _edata
    52: 004006e0     0 FUNC    GLOBAL HIDDEN    15 _fini
    53: 00420000     0 NOTYPE  GLOBAL DEFAULT   20 __data_start
    54: 004006c0     0 FUNC    GLOBAL DEFAULT  UND puts@GLIBC_2.0
    55: 00000000     0 FUNC    WEAK   DEFAULT  UND __gmon_start__
    56: 00420040     0 OBJECT  GLOBAL HIDDEN    23 __dso_handle
    57: 00400710     4 OBJECT  GLOBAL DEFAULT   16 _IO_stdin_used
    58: 00420000     0 NOTYPE  GLOBAL DEFAULT   20 _fdata
    59: 00420010     0 OBJECT  GLOBAL DEFAULT   21 __RLD_MAP
    60: 00420060     0 NOTYPE  GLOBAL DEFAULT   24 _end
    61: 00420050     0 NOTYPE  GLOBAL DEFAULT   24 __bss_start
    62: 00400650    84 FUNC    GLOBAL DEFAULT   13 main
    63: 00400520     0 FUNC    GLOBAL DEFAULT   13 __start
    64: 00400520     0 NOTYPE  GLOBAL DEFAULT   13 _ftext
    65: 00420014     0 OBJECT  GLOBAL HIDDEN    21 __TMC_END__
    66: 00420020     0 OBJECT  GLOBAL HIDDEN    22 _GLOBAL_OFFSET_TABLE_
    67: 00420050     0 NOTYPE  GLOBAL DEFAULT   24 _fbss
    68: 00000000     0 NOTYPE  WEAK   DEFAULT  UND _ITM_registerTMC[...]
    69: 004004e4     0 FUNC    GLOBAL HIDDEN    12 _init

Histogram for bucket list length (total of 3 buckets):
 Length  Number     % of total  Coverage
      0  0          (  0.0%)
      1  1          ( 33.3%)     11.1%
      2  1          ( 33.3%)     33.3%
      3  0          (  0.0%)     33.3%
      4  0          (  0.0%)     33.3%
      5  0          (  0.0%)     33.3%
      6  1          ( 33.3%)    100.0%

Version symbols section '.gnu.version' contains 10 entries:
 Addr: 0x000000000040049e  Offset: 0x0000049e  Link: 8 (.dynsym)
  000:   0 (*本地*)       1 (*全局*)      1 (*全局*)      1 (*全局*)   
  004:   1 (*全局*)      1 (*全局*)      1 (*全局*)      3 (GLIBC_2.0)  
  008:   1 (*全局*)      2 (GLIBC_2.34) 

Version needs section '.gnu.version_r' contains 1 entry:
 Addr: 0x00000000004004b4  Offset: 0x000004b4  Link: 9 (.dynstr)
  000000: Version: 1  文件：libc.so.6  计数：2
  0x0010:   Name: GLIBC_2.0  标志：无  版本：3
  0x0020:   Name: GLIBC_2.34  标志：无  版本：2

Displaying notes found in: .note.gnu.build-id
  所有者            Data size   Description
  GNU                  0x00000014       NT_GNU_BUILD_ID (unique build ID bitstring)
    Build ID: e15deb55690859893a6ca4c908962cd0bf28de67

Displaying notes found in: .note.ABI-tag
  所有者            Data size   Description
  GNU                  0x00000010       NT_GNU_ABI_TAG (ABI version tag)
    OS: Linux, ABI: 3.2.0
Attribute Section: gnu
File Attributes
  Tag_GNU_MIPS_ABI_FP: Hard float (32-bit CPU, Any FPU)

MIPS ABI Flags Version: 0

ISA: MIPS32r2
GPR size: 32
CPR1 size: 32
CPR2 size: 0
FP ABI: Hard float (32-bit CPU, Any FPU)
ISA Extension: 无
ASEs:
        无
FLAGS 1: 00000000
FLAGS 2: 00000000

Primary GOT:
 Canonical gp value: 00428010

 Reserved entries:
    住址     访问  Initial Purpose
  00420020 -32752(gp) 00000000 Lazy resolver
  00420024 -32748(gp) 80000000 Module pointer (GNU extension)

 Local entries:
    住址     访问  Initial
  00420028 -32744(gp) 00400650

 Global entries:
    住址     访问  Initial Sym.Val. 类型  Ndx 名字
  0042002c -32740(gp) 00000000 00000000 NOTYPE  UND _ITM_registerTMCloneTable
  00420030 -32736(gp) 00000000 00000000 FUNC    UND __gmon_start__
  00420034 -32732(gp) 004006c0 004006c0 FUNC    UND puts
  00420038 -32728(gp) 00000000 00000000 NOTYPE  UND _ITM_deregisterTMCloneTable
  0042003c -32724(gp) 004006b0 004006b0 FUNC    UND __libc_start_main
```

##### objdump

执行`mips-linux-gnu-objdump -S main_mips`得到以下结果

```bash
main_mips：     文件格式 elf32-tradbigmips


Disassembly of section .init:

004004e4 <_init>:
  4004e4:       3c1c0002        lui     gp,0x2
  4004e8:       279c7b2c        addiu   gp,gp,31532
  4004ec:       0399e021        addu    gp,gp,t9
  4004f0:       27bdffe0        addiu   sp,sp,-32
  4004f4:       afbc0010        sw      gp,16(sp)
  4004f8:       afbf001c        sw      ra,28(sp)
  4004fc:       8f828020        lw      v0,-32736(gp)
  400500:       10400004        beqz    v0,400514 <_init+0x30>
  400504:       00000000        nop
  400508:       8f998020        lw      t9,-32736(gp)
  40050c:       0320f809        jalr    t9
  400510:       00000000        nop
  400514:       8fbf001c        lw      ra,28(sp)
  400518:       03e00008        jr      ra
  40051c:       27bd0020        addiu   sp,sp,32

Disassembly of section .text:

00400520 <__start>:
  400520:       03e00025        move    zero,ra
  400524:       04110001        bal     40052c <__start+0xc>
  400528:       00000000        nop
  40052c:       3c1c0002        lui     gp,0x2
  400530:       279c7ae4        addiu   gp,gp,31460
  400534:       039fe021        addu    gp,gp,ra
  400538:       0000f825        move    ra,zero
  40053c:       8f848018        lw      a0,-32744(gp)
  400540:       8fa50000        lw      a1,0(sp)
  400544:       27a60004        addiu   a2,sp,4
  400548:       2401fff8        li      at,-8
  40054c:       03a1e824        and     sp,sp,at
  400550:       27bdffe0        addiu   sp,sp,-32
  400554:       00003825        move    a3,zero
  400558:       afa00010        sw      zero,16(sp)
  40055c:       afa20014        sw      v0,20(sp)
  400560:       afbd0018        sw      sp,24(sp)
  400564:       8f99802c        lw      t9,-32724(gp)
  400568:       0320f809        jalr    t9
  40056c:       00000000        nop

00400570 <hlt>:
  400570:       1000ffff        b       400570 <hlt>
  400574:       00000000        nop
        ...

00400580 <deregister_tm_clones>:
  400580:       3c040042        lui     a0,0x42
  400584:       3c020042        lui     v0,0x42
  400588:       24840014        addiu   a0,a0,20
  40058c:       24420014        addiu   v0,v0,20
  400590:       10440007        beq     v0,a0,4005b0 <deregister_tm_clones+0x30>
  400594:       3c1c0043        lui     gp,0x43
  400598:       279c8010        addiu   gp,gp,-32752
  40059c:       8f998028        lw      t9,-32728(gp)
  4005a0:       13200003        beqz    t9,4005b0 <deregister_tm_clones+0x30>
  4005a4:       00000000        nop
  4005a8:       03200008        jr      t9
  4005ac:       00000000        nop
  4005b0:       03e00008        jr      ra
  4005b4:       00000000        nop

004005b8 <register_tm_clones>:
  4005b8:       3c040042        lui     a0,0x42
  4005bc:       3c020042        lui     v0,0x42
  4005c0:       24840014        addiu   a0,a0,20
  4005c4:       24450014        addiu   a1,v0,20
  4005c8:       00a42823        subu    a1,a1,a0
  4005cc:       00051083        sra     v0,a1,0x2
  4005d0:       00052fc2        srl     a1,a1,0x1f
  4005d4:       00a22821        addu    a1,a1,v0
  4005d8:       00052843        sra     a1,a1,0x1
  4005dc:       10a00007        beqz    a1,4005fc <register_tm_clones+0x44>
  4005e0:       3c1c0043        lui     gp,0x43
  4005e4:       279c8010        addiu   gp,gp,-32752
  4005e8:       8f99801c        lw      t9,-32740(gp)
  4005ec:       13200003        beqz    t9,4005fc <register_tm_clones+0x44>
  4005f0:       00000000        nop
  4005f4:       03200008        jr      t9
  4005f8:       00000000        nop
  4005fc:       03e00008        jr      ra
  400600:       00000000        nop

00400604 <__do_global_dtors_aux>:
  400604:       27bdffe0        addiu   sp,sp,-32
  400608:       afb00018        sw      s0,24(sp)
  40060c:       3c100042        lui     s0,0x42
  400610:       afbf001c        sw      ra,28(sp)
  400614:       92020050        lbu     v0,80(s0)
  400618:       14400006        bnez    v0,400634 <__do_global_dtors_aux+0x30>
  40061c:       8fbf001c        lw      ra,28(sp)
  400620:       0c100160        jal     400580 <deregister_tm_clones>
  400624:       00000000        nop
  400628:       24020001        li      v0,1
  40062c:       a2020050        sb      v0,80(s0)
  400630:       8fbf001c        lw      ra,28(sp)
  400634:       8fb00018        lw      s0,24(sp)
  400638:       03e00008        jr      ra
  40063c:       27bd0020        addiu   sp,sp,32

00400640 <frame_dummy>:
  400640:       0810016e        j       4005b8 <register_tm_clones>
  400644:       00000000        nop
        ...

00400650 <main>:
  400650:       27bdffe0        addiu   sp,sp,-32
  400654:       afbf001c        sw      ra,28(sp)
  400658:       afbe0018        sw      s8,24(sp)
  40065c:       03a0f025        move    s8,sp
  400660:       3c1c0043        lui     gp,0x43
  400664:       279c8010        addiu   gp,gp,-32752
  400668:       afbc0010        sw      gp,16(sp)
  40066c:       3c020040        lui     v0,0x40
  400670:       24440720        addiu   a0,v0,1824
  400674:       8f828024        lw      v0,-32732(gp)
  400678:       0040c825        move    t9,v0
  40067c:       0320f809        jalr    t9
  400680:       00000000        nop
  400684:       8fdc0010        lw      gp,16(s8)
  400688:       00001025        move    v0,zero
  40068c:       03c0e825        move    sp,s8
  400690:       8fbf001c        lw      ra,28(sp)
  400694:       8fbe0018        lw      s8,24(sp)
  400698:       27bd0020        addiu   sp,sp,32
  40069c:       03e00008        jr      ra
  4006a0:       00000000        nop
        ...

Disassembly of section .MIPS.stubs:

004006b0 <_MIPS_STUBS_>:
  4006b0:       8f998010        lw      t9,-32752(gp)
  4006b4:       03e07825        move    t7,ra
  4006b8:       0320f809        jalr    t9
  4006bc:       24180009        li      t8,9
  4006c0:       8f998010        lw      t9,-32752(gp)
  4006c4:       03e07825        move    t7,ra
  4006c8:       0320f809        jalr    t9
  4006cc:       24180007        li      t8,7
        ...

Disassembly of section .fini:

004006e0 <_fini>:
  4006e0:       3c1c0002        lui     gp,0x2
  4006e4:       279c7930        addiu   gp,gp,31024
  4006e8:       0399e021        addu    gp,gp,t9
  4006ec:       27bdffe0        addiu   sp,sp,-32
  4006f0:       afbc0010        sw      gp,16(sp)
  4006f4:       afbf001c        sw      ra,28(sp)
  4006f8:       8fbf001c        lw      ra,28(sp)
  4006fc:       03e00008        jr      ra
  400700:       27bd0020        addiu   sp,sp,32
```

#### 三、传入objdump的参数

使用`man objdump`查看之后可知，需要调用其进行反汇编时：

```bash
objdump -D file # 对file反汇编，返回汇编程序
objdump -S file # 对file反汇编，同时显示机器码和汇编程序
```



### Thinking 1.2

> Thinking 1.2 思考下述问题：
>
> * 尝试使用我们编写的 readelf 程序，解析之前在 target 目录下生成的内核 ELF 文 件。
> * 也许你会发现我们编写的 readelf 程序是不能解析 readelf 文件本身的，而我们刚 才介绍的系统工具 readelf 则可以解析，这是为什么呢？（提示：尝试使用 readelf -h，并阅读 tools/readelf 目录下的 Makefile，观察 readelf 与 hello 的不同）

补全`readelf.c`文件时，使用的是32位的宏来计算ELF文件的地址，故不能对64位的ELF解析。

#### 一、解析target下的内核

执行`./tools/readelf/readelf target/mos`后得到以下结果

```bash
0:0x0
1:0x80020000
2:0x800220a0
3:0x800220b8
4:0x800220d0
5:0x0
6:0x0
7:0x0
8:0x0
9:0x0
10:0x0
11:0x0
12:0x0
13:0x0
14:0x0
15:0x0
16:0x0
17:0x0
18:0x0
```

#### 二、使用 readelf -h

在`tools/readelf/`目录下生成`hello`和`readelf`后。

执行`readelf -h readelf`得到

```bash
ELF 头：
  Magic：   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00 
  类别:                              ELF64
  数据:                              2 补码，小端序 (little endian)
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI 版本:                          0
  类型:                              DYN (Position-Independent Executable file)
  系统架构:                          Advanced Micro Devices X86-64
  版本:                              0x1
  入口点地址：               0x1180
  程序头起点：          64 (bytes into file)
  Start of section headers:          14488 (bytes into file)
  标志：             0x0
  Size of this header:               64 (bytes)
  Size of program headers:           56 (bytes)
  Number of program headers:         13
  Size of section headers:           64 (bytes)
  Number of section headers:         31
  Section header string table index: 30
```

执行`readelf -h hello`得到

```bash
ELF 头：
  Magic：   7f 45 4c 46 01 01 01 03 00 00 00 00 00 00 00 00 
  类别:                              ELF32
  数据:                              2 补码，小端序 (little endian)
  Version:                           1 (current)
  OS/ABI:                            UNIX - GNU
  ABI 版本:                          0
  类型:                              EXEC (可执行文件)
  系统架构:                          Intel 80386
  版本:                              0x1
  入口点地址：               0x8049750
  程序头起点：          52 (bytes into file)
  Start of section headers:          707128 (bytes into file)
  标志：             0x0
  Size of this header:               52 (bytes)
  Size of program headers:           32 (bytes)
  Number of program headers:         8
  Size of section headers:           40 (bytes)
  Number of section headers:         30
  Section header string table index: 29
```

可以发现，`readelf`的类别是`ELF64`，而`hello`是`ELF32`。猜测：我们的`readelf`只能解析32位的ELF文件，故其不能解析作为64位文件的自身。

### Thinking 1.3

> Thinking 1.3 在理论课上我们了解到，MIPS 体系结构上电时，启动入口地址为 0xBFC00000 （其实启动入口地址是根据具体型号而定的，由硬件逻辑确定，也有可能不是这个地址，但 一定是一个确定的地址），但实验操作系统的内核入口并没有放在上电启动地址，而是按照 内存布局图放置。思考为什么这样放置内核还能保证内核入口被正确跳转到？ （提示：思考实验中启动过程的两阶段分别由谁执行。） 

启动的第一阶段是初始化硬件设备，在 ROM 中由 bootloader 执行，第二阶段在 RAM 中，初始化该阶段硬件设备，读取并载入内核，执行引导程序，因此启动入口地址未必是内核入口地址，而引导程序的存在会把内核载入内存并跳转，保证内核入口被正确跳转到。

具体方法是使用跳转指令，就像`exercise 1.3`中使用的`jal mips_init`类似。

## 练习题

### Exercise 1.1

> Exercise 1.1 阅读 tools/readelf 目录下的 elf.h、readelf.c 和 main.c 文件，并补全 readelf.c 中缺少的代码。readelf 函数需要输出 ELF 文件中所有节头中的地址信息，对 于每个节头，输出格式为 "%d:0x%x\n"，其中的 %d 和 %x 分别代表序号和地址。 正确完成 readelf.c 之后，在 tools/readelf 目录下执行 make 命令，即可生成可执 行文件 readelf，它接受文件名作为参数，对 ELF 文件进行解析。可以执行 make hello 生成测试用的 ELF 文件 hello，然后运行 ./readelf hello 来测试 readelf。 

```c
/* Exercise 1.1: Your code here. (1/2) */
	// 计算节头表起始位置 = 文件起始地址 + 节头表偏移
    	sh_table = (const char *)binary + ehdr->e_shoff;
    	// 获取节头表项数量和单个表项大小
    	sh_entry_count = ehdr->e_shnum;
    	sh_entry_size = ehdr->e_shentsize;

/* Exercise 1.1: Your code here. (2/2) */
		// 计算当前节头的地址：表起始地址 + 索引*表项大小
       		shdr = (const Elf32_Shdr *)((const char *)sh_table + i * sh_entry_size);
        	// 获取该节在内存中的虚拟地址
        	addr = shdr->sh_addr;
```

这题本质就是计算地址偏移量，需要阅读头文件`elf.h`中的结构体内的数据结构来获取需要的对应信息。

```c
// ELF头文件结构
typedef struct {
	unsigned char e_ident[EI_NIDENT]; /* Magic number and other info */
	Elf32_Half e_type;		  /* Object file type */
	Elf32_Half e_machine;		  /* Architecture */
	Elf32_Word e_version;		  /* Object file version */
	Elf32_Addr e_entry;		  /* Entry point virtual address */
	Elf32_Off e_phoff;		  /* Program header table file offset */
	Elf32_Off e_shoff;		  /* Section header table file offset */
	Elf32_Word e_flags;		  /* Processor-specific flags */
	Elf32_Half e_ehsize;		  /* ELF header size in bytes */
	Elf32_Half e_phentsize;		  /* Program header table entry size */
	Elf32_Half e_phnum;		  /* Program header table entry count */
	Elf32_Half e_shentsize;		  /* Section header table entry size */
	Elf32_Half e_shnum;		  /* Section header table entry count */
	Elf32_Half e_shstrndx;		  /* Section header string table index */
} Elf32_Ehdr;

// 节的结构
typedef struct {
	Elf32_Word sh_name;	 /* Section name */
	Elf32_Word sh_type;	 /* Section type */
	Elf32_Word sh_flags;	 /* Section flags */
	Elf32_Addr sh_addr;	 /* Section addr */
	Elf32_Off sh_offset;	 /* Section offset */
	Elf32_Word sh_size;	 /* Section size */
	Elf32_Word sh_link;	 /* Section link */
	Elf32_Word sh_info;	 /* Section extra info */
	Elf32_Word sh_addralign; /* Section alignment */
	Elf32_Word sh_entsize;	 /* Section entry size */
} Elf32_Shdr;
```

### Exercise 1.2

> Exercise 1.2 填写 kernel.lds 中空缺的部分，在 Lab1 中，只需要填补 .text、.data 和.bss 节，将内核调整到正确的位置上即可。 

```c
SECTIONS {	
	. = 0x80020000; /* KERNBASE 的地址值可以在 include/mmu.h 中找到  */
	.text : {
    		*(.text)
	 }
	.data : {
        	*(.data)       
    	}
	bss_start = .;
	.bss : {
    		*(.bss) 
	}
	bss_end = .;
	. = 0x80400000; /* 也即 KSTACKTOP 的值 */
	end = . ;
}
```

`.text`设置在`KERNBASE`，剩下的接在其后面就行。

### Exercise 1.3 

> Exercise 1.3 完成 init/start.S 中空缺的部分。设置栈指针，跳转到 mips_init 函数。 执行命令 make run 运行仿真，或使用命令 make dbg_run 在调试模式下运行 QEMU。

```c
	/* hint: you can refer to the memory layout in include/mmu.h */
	/* set up the kernel stack */
	/* Exercise 1.3: Your code here. (1/2) */
	
	li sp, 0x80400000 /* 可以在include/mmu.h中找到，即为 KSTACKTOP  */

	/* jump to mips_init */
	/* Exercise 1.3: Your code here. (2/2) */

	jal mips_init
	nop
```

这里需要根据内存空间设置栈指针到`KSTACKTOP`的位置。跳转则直接跳即可。

这里需要注意mips架构中栈指针的方向：`.text`位于`KERNBASE`之上，`stack`则位于`KSTACKTOP`之下。也就是说`KERNBASE`到`KSTACKTOP`之间的空间存储了指令和栈，两者分别朝着不同方向增加。

### Exercise 1.4 

> Exercise 1.4 阅读相关代码和下面对于函数规格的说明，补全 lib/print.c 中 vprintfmt() 函数中两处缺失的部分来实现字符输出。第一处缺失部分：找到 % 并分析输出格式; 第二处 缺失部分：取出参数，输出格式串为 %\[flags]\[width][length] 的情况。具 体格式详见 printk 格式具体说明。 

需要利用C语言来解析格式化字符，一些工具函数已经给出。

```c
void vprintfmt(fmt_callback_t out, void *data, const char *fmt, va_list ap) {
	char c;
	const char *s;
	long num;

	int width;
	int long_flag; // output is long (rather than int)
	int neg_flag;  // output is negative
	int ladjust;   // output is left-aligned
	char padc;     // padding char

	for (;;) {
		/* scan for the next '%'
		扫描下一个百分号 */
		/* Exercise 1.4: Your code here. (1/8) */

		const char *start = fmt; 			// 记录当前指针位置
		while (*fmt != '%' && *fmt) fmt++; 		// fmt指针移动，直到找到百分号为止

		/* flush the string found so far
		输出目前发现的字符串（即输出start到当前fmt之间的不含格式化字符的字符串） */
		/* Exercise 1.4: Your code here. (2/8) */
		
		if (fmt > start) {
            		out(data, start, fmt - start);
        	}	

		/* check "are we hitting the end?"
		检查我们是否已经解析字符串完毕 */
		/* Exercise 1.4: Your code here. (3/8) */
		
		if (!*fmt) break;

		/* we found a '%'
		处理百分号  */
		/* Exercise 1.4: Your code here. (4/8) */
		
		fmt++;						// 跳过百分号

		/* check format flag 
		解析格式标志（除了neg_flag之外的四个） */
		/* Exercise 1.4: Your code here. (5/8) */
		
		// 标志初始化
		ladjust = 0;
        	padc = ' ';
       		width = 0;
        	long_flag = 0;

		// 处理对齐标志
        	if (*fmt == '-') {
            		ladjust = 1;
            		fmt++;
        	}

		// 处理填充字符
        	if (*fmt == '0') {
            		if (!ladjust) {
                		padc = '0';
            		}
            		fmt++;
        	}	

		/* get width
		获取宽度  */
		/* Exercise 1.4: Your code here. (6/8) */
		
		for ( ; *fmt >= '0' && *fmt <= '9'; fmt++ ) {
			width = *fmt - '0' + width * 10;     	// 计算宽度        
		}

		/* check for long
		处理long标志  */
		/* Exercise 1.4: Your code here. (7/8) */
		
		if (*fmt == 'l') {
            		long_flag = 1;
            		fmt++;
        	}

		neg_flag = 0;
		switch (*fmt) {
		case 'b':
			if (long_flag) {
				num = va_arg(ap, long int);
			} else {
				num = va_arg(ap, int);
			}
			print_num(out, data, num, 2, 0, width, ladjust, padc, 0);
			break;

		case 'd':
		case 'D':
			if (long_flag) {
				num = va_arg(ap, long int);
			} else {
				num = va_arg(ap, int);
			}

			/*
			 * Refer to other parts (case 'b', case 'o', etc.) and func 'print_num' to
			 * complete this part. Think the differences between case 'd' and the
			 * others. (hint: 'neg_flag').
			 */
			/* Exercise 1.4: Your code here. (8/8) */
			
			// 处理负号
			if (num < 0) {
				num = -num;
				neg_flag = 1;
			}
			
			// 输出
			print_num(out, data, num, 10, neg_flag, width, ladjust, padc, 0);

			break;

		case 'o':
		case 'O':
			if (long_flag) {
				num = va_arg(ap, long int);
			} else {
				num = va_arg(ap, int);
			}
			print_num(out, data, num, 8, 0, width, ladjust, padc, 0);
			break;

		case 'u':
		case 'U':
			if (long_flag) {
				num = va_arg(ap, long int);
			} else {
				num = va_arg(ap, int);
			}
			print_num(out, data, num, 10, 0, width, ladjust, padc, 0);
			break;

		case 'x':
			if (long_flag) {
				num = va_arg(ap, long int);
			} else {
				num = va_arg(ap, int);
			}
			print_num(out, data, num, 16, 0, width, ladjust, padc, 0);
			break;

		case 'X':
			if (long_flag) {
				num = va_arg(ap, long int);
			} else {
				num = va_arg(ap, int);
			}
			print_num(out, data, num, 16, 0, width, ladjust, padc, 1);
			break;

		case 'c':
			c = (char)va_arg(ap, int);
			print_char(out, data, c, width, ladjust);
			break;

		case 's':
			s = (char *)va_arg(ap, char *);
			print_str(out, data, s, width, ladjust);
			break;

		case '\0':
			fmt--;
			break;

		default:
			/* output this char as it is */
			out(data, fmt, 1);
		}
		fmt++;
	}
}
```

## 难点

主要难点是对实验代码的阅读，目前还是有很多地方没看懂，需要多看，理解各部分的联系。