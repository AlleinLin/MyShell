# MyShell

一个用 C 语言实现的自定义 Shell 命令行解释器。

## 项目简介

MyShell 是一个功能丰富的 Shell 程序，实质上是作者在学习操作系统课程时期的课程设计，一个在OpenEuler系统上实现部分Windows上cmd命令的设计。实现了多种常用的文件操作和系统管理命令。它支持管道操作、输入输出重定向、命令历史记录等功能，可以作为学习 Shell 实现原理的教学项目。

## 功能特性

### 核心功能

- **管道操作**：支持 `|` 管道符，实现命令间的数据传递
- **重定向**：支持 `<` (输入重定向)、`>` (输出重定向)、`>>` (追加输出)
- **命令历史**：记录用户输入的命令历史
- **自定义提示符**：支持通过 `myprompt` 命令自定义命令提示符

### 内置命令

| 命令         | 功能描述                 |
| ---------- | -------------------- |
| `mycd`     | 切换当前工作目录             |
| `mycp`     | 复制文件，支持文件合并复制        |
| `mydir`    | 列出目录内容，支持多种显示格式和排序选项 |
| `myline`   | 统计文件行数               |
| `mymd`     | 创建新目录                |
| `mymv`     | 移动或重命名文件             |
| `myprompt` | 设置自定义命令提示符           |
| `myps`     | 显示进程信息               |
| `myrd`     | 删除空目录                |
| `myren`    | 重命名文件                |
| `myrm`     | 删除文件                 |
| `mysort`   | 文件内容排序               |
| `mytime`   | 显示当前系统时间             |
| `mytree`   | 以树形结构显示目录            |
| `mytype`   | 显示文件类型信息             |
| `myxcp`    | 高级文件复制功能             |
| `help`     | 显示帮助信息               |
| `exit`     | 退出 Shell             |
| `pwd`      | 显示当前工作目录             |
| `myhis`    | 显示命令历史               |

## 编译与运行

### 编译要求

- GCC 编译器
- Linux/Unix 操作系统

### 编译步骤

```bash
# 使用 make 编译
make

# 或者直接使用 gcc 编译
gcc -Wall -g -o myshell shell.c
```

### 运行

```bash
./myshell
```

## 命令使用示例

### 目录操作

```bash
# 切换目录
mycd /home/user

# 显示当前目录
pwd

# 列出目录内容
mydir

# 详细列表显示
mydir -l

# 显示隐藏文件
mydir -a

# 以树形结构显示目录
mytree /home/user
```

### 文件操作

```bash
# 复制文件
mycp source.txt dest.txt

# 强制覆盖复制
mycp -y source.txt dest.txt

# 移动文件
mymv source.txt dest.txt

# 重命名文件
myren oldname.txt newname.txt

# 删除文件
myrm filename.txt

# 创建目录
mymd newdir

# 删除空目录
myrd emptydir
```

### 管道与重定向

```bash
# 输出重定向
mydir > output.txt

# 追加输出
mydir >> output.txt

# 输入重定向
mycp < input.txt output.txt

# 管道操作
mydir | mysort
```

### 其他命令

```bash
# 显示帮助
help

# 查看命令历史
myhis

# 设置提示符
myprompt $P$G

# 显示时间
mytime

# 退出 Shell
exit
```

## 项目结构

```
MyShell/
├── shell.c          # 主程序入口和核心逻辑
├── shell.h          # 头文件和全局声明
├── mycd.c           # 目录切换命令
├── mycp.c           # 文件复制命令
├── mydir.c          # 目录列表命令
├── myline.c         # 行数统计命令
├── mymd.c           # 创建目录命令
├── mymv.c           # 文件移动命令
├── myprompt.c       # 提示符设置命令
├── myps.c           # 进程显示命令
├── myrd.c           # 删除目录命令
├── myren.c          # 文件重命名命令
├── myrm.c           # 删除文件命令
├── mysort.c         # 排序命令
├── mytime.c         # 时间显示命令
├── mytree.c         # 目录树显示命令
├── mytype.c         # 文件类型命令
├── myxcp.c          # 高级复制命令
├── makefile         # 编译配置文件
└── lib/
    └── lib.a        # 静态库文件
```

## 错误处理

Shell 提供了完善的错误处理机制，包括：

- 命令不存在错误
- 参数缺失错误
- 文件不存在错误
- 重定向符号过多错误
- 管道操作错误
- Fork 错误

## 许可证

本项目仅供学习和研究使用。
