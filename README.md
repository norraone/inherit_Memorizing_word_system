# Memorizing Word System

## 项目简介

Memorizing Word System 是一个基于 C++ 开发的词汇学习系统，旨在帮助用户高效地学习和复习词汇。该项目使用了 Qt 作为图形用户界面框架，提供了登录、注册、词汇学习、进度统计等多种功能，方便用户管理自己的学习进度。

## 功能模块

### 1. 登录与注册
- **登录**：用户可以通过输入账号和密码进行登录，进入系统后可以管理自己的词汇学习进度。
- **注册**：未注册用户可以创建新账号并注册，注册成功后可使用新账号登录系统。

### 2. 词汇学习与复习
- **词汇模块**：提供了一个词汇学习功能，用户可以浏览和学习系统中的词汇。
- **复习模块**：用户可以查看已学过的单词，并通过复习功能巩固自己的词汇记忆。

### 3. 学习进度统计
- **统计模块**：该模块提供用户的学习进度统计，包括已学习词汇的数量、复习次数等，帮助用户了解自己的学习情况。

## 项目结构

项目主要文件和目录说明：
- **main.cpp**：项目的入口文件。
- **login.cpp / login.h**：处理用户登录相关逻辑。
- **register.cpp / register.h**：处理用户注册功能。
- **mainwindow.cpp / mainwindow.h**：主窗口的实现和定义。
- **vocabulary.cpp / vocabulary.h**：词汇学习相关的功能模块。
- **review.cpp / review.h**：复习模块的实现。
- **statistics.cpp / statistics.h**：用户学习进度统计功能。
- **user.cpp / user.h**：用户信息管理模块。

## 构建与运行

### 先决条件
- 安装 CMake 3.10 或以上版本。
- 安装 Qt 5 框架。
- 安装编译器（如 GCC 或 Clang）。
- 安装 Git。

### 构建步骤
1. 克隆此项目到本地：
   ```bash
   git clone https://github.com/1eiC/Memorizing_word_system.git
   cd Memorizing_word_system
   ```
2. 使用 CMake 进行项目构建：
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```
3. 运行可执行文件：
   ```bash
   ./MemorizingWordSystem
   ```

## 使用说明
- 运行程序后，首先会进入登录界面，已注册用户可以直接登录，新用户需要先注册。
- 登录成功后可以进入主界面进行词汇学习和复习。
- 在学习过程中可以查看进度统计，了解自己的学习情况。

## 注意事项
- 本项目仅为学习目的而开发，不保证所有功能都能正常运行。
- 项目中的词汇数据是随机生成的，不保证准确性和实用性。
- 项目中的用户数据是存储在本地的，不会上传到服务器。
- 使用前在工作空间目录下导入并命名“OIP.png”的图片，否则会出现背景纯黑的问题。

## 许可证

本项目采用 MIT 许可证，详见 [LICENSE](LICENSE) 文件。

## 贡献
欢迎对本项目提出意见和建议。如果你有改进或新增功能的想法，请提交 Pull Request。

## 联系方式
如有任何问题或建议，请联系项目维护者 1eiC：
- 邮箱：1991401181@qq.com
- GitHub：[@1eiC](https://github.com/1eiC)
