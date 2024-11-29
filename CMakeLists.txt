cmake_minimum_required(VERSION 3.15)
project(WordMemorizationSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Set Qt installation path - adjust this to your Qt5 installation path
set(CMAKE_PREFIX_PATH "D:/Qt/Qt5.14.2/5.14.2/mingw73_64")

find_package(Qt5 COMPONENTS
    Core
    Gui
    Widgets
    Sql
    REQUIRED
)

set(SOURCES
    main.cpp
    user.cpp
    word.cpp
    vocabulary.cpp
    review.cpp
    statistics.cpp
    study.cpp
)

set(HEADERS
    user.h
    word.h
    vocabulary.h
    review.h
    statistics.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Sql
)
