cmake_minimum_required(VERSION 3.15)
project(WordMemorizationSystem VERSION 1.0.0 LANGUAGES CXX)

# 设置 C++ 标准和编译选项
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Qt 自动处理
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 设置编译选项
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# 设置 Qt 路径
set(CMAKE_PREFIX_PATH "C:/Qt/Qt5.14.2/5.14.2/mingw73_64")

# 查找 Qt 包
find_package(Qt5 COMPONENTS
    Core
    Gui
    Widgets
    Sql
    REQUIRED
)

# 源文件
set(SOURCES
    main.cpp
    mainwindow.cpp
    user.cpp
    word.cpp
    vocabulary.cpp
    review.cpp
    statistics.cpp
    study.cpp
    views/LoginView.cpp
    views/VocabularyView.cpp
    views/ReviewView.cpp
    views/StatisticsView.cpp
)

# 头文件
set(HEADERS
    mainwindow.h
    user.h
    word.h
    vocabulary.h
    review.h
    statistics.h
    database.h
    views/LoginView.h
    views/VocabularyView.h
    views/ReviewView.h
    views/StatisticsView.h
)

# 资源文件
set(RESOURCES
    resources.qrc
)

# 创建可执行文件
add_executable(${PROJECT_NAME} 
    ${SOURCES} 
    ${HEADERS}
    ${RESOURCES}
)

# 设置输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)

# 链接 Qt 库
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Sql
)

# 安装规则
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# 复制 Qt DLL（仅在 Windows 上）
if(WIN32)
    get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    
    # 部署 Qt DLLs
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND "${_qt_bin_dir}/windeployqt.exe"
                --no-compiler-runtime
                --no-translations
                --no-opengl-sw
                "$<TARGET_FILE:${PROJECT_NAME}>"
        COMMENT "Deploying Qt dependencies..."
    )
endif()
