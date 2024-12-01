cmake_minimum_required(VERSION 3.15)
project(inherit_memorizing_word_system VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_PREFIX_PATH "C:/Qt/Qt5.14.2/5.14.2/mingw73_64")

# Find Qt packages
find_package(Qt5 COMPONENTS 
    Core
    Gui
    Widgets
    Sql
    Charts
    REQUIRED
)

# Set warning flags
if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# Source files
set(SOURCES
    main.cpp
    
    # Models
    models/user.cpp
    models/word.cpp
    models/review_session.cpp
    
    # Repositories
    repositories/base_repository.cpp
    repositories/user_repository.cpp
    repositories/word_repository.cpp
    
    # Services
    services/user_service.cpp
    services/word_service.cpp
    services/review_service.cpp
    services/statistics_service.cpp
    
    # UI Views
    ui/views/login_view.cpp
    ui/views/vocabulary_view.cpp
    ui/views/review_view.cpp
    ui/views/statistics_view.cpp
    
    # UI Dialogs
    ui/dialogs/word_dialog.cpp
    
    # Resources
    resources.qrc
)

# Header files
set(HEADERS
    # Models
    models/user.h
    models/word.h
    models/review_session.h
    
    # Repositories
    repositories/base_repository.h
    repositories/user_repository.h
    repositories/word_repository.h
    
    # Services
    services/user_service.h
    services/word_service.h
    services/review_service.h
    services/statistics_service.h
    
    # UI Views
    ui/views/login_view.h
    ui/views/vocabulary_view.h
    ui/views/review_view.h
    ui/views/statistics_view.h
    
    # UI Dialogs
    ui/dialogs/word_dialog.h
)

# Create executable
add_executable(${PROJECT_NAME} 
    ${SOURCES} 
    ${HEADERS}
    ui/dialogs/word_dialog.h
    ui/dialogs/word_dialog.cpp
)

# Ensure Qt MOC processing for dialog
set_target_properties(${PROJECT_NAME} PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::Sql
    Qt5::Charts
)

# Set include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Install rules
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static
)

# Copy Qt DLLs and plugins to output directory (Windows only)
if(WIN32)
    get_target_property(QT_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
    get_filename_component(QT_BIN_DIR "${QT_QMAKE_EXECUTABLE}" DIRECTORY)
    get_filename_component(QT_ROOT_DIR "${QT_BIN_DIR}" DIRECTORY)
    
    # Copy Qt DLLs
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${QT_BIN_DIR}/Qt5Core.dll
            ${QT_BIN_DIR}/Qt5Gui.dll
            ${QT_BIN_DIR}/Qt5Widgets.dll
            ${QT_BIN_DIR}/Qt5Sql.dll
            ${QT_BIN_DIR}/Qt5Charts.dll
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
    
    # Create platforms directory and copy platform plugins
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/platforms
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${QT_ROOT_DIR}/plugins/platforms/qwindows.dll
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/platforms
    )
    
    # Create styles directory and copy style plugins
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/styles
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${QT_ROOT_DIR}/plugins/styles/qwindowsvistastyle.dll
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/styles
    )
    
    # Create sqldrivers directory and copy SQL driver plugin
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/sqldrivers
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${QT_ROOT_DIR}/plugins/sqldrivers/qsqlite.dll
            $<TARGET_FILE_DIR:${PROJECT_NAME}>/sqldrivers
    )
endif()
