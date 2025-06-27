project(BCCircle)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# "D:/soft/Qt/Qt5.14.2/5.14.2/msvc2017_64"
# "D:/soft/Qt/Qt5.14.2/5.14.2/mingw73_64"
# "D:/soft/Qt/Qt6.8/6.8.1/msvc2022_64"
# "D:/soft/Qt/Qt6.8/6.8.1/mingw_64"
set(QT_PATH "D:/soft/Qt/Qt5.14.2/5.14.2/msvc2017_64" CACHE PATH "QT_PATH")

# set(CMAKE_PREFIX_PATH ${QT_PATH})

# set(Qt5_DIR " ${QT_PATH}/lib/cmake/Qt5 ")
# set(ENV{QT_QPA_PLATFORM_PLUGIN_PATH} "D:/soft/Qt/Qt5.14.2/5.14.2/msvc2017_64/plugins/platforms")
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Core Gui)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

if(Qt${QT_VERSION_MAJOR}_FOUND)
    message(STATUS "QT_PATH: " ${QT_PATH})
    message(STATUS "Qt${QT_VERSION_MAJOR}::Widgets found at " ${Qt${QT_VERSION_MAJOR}_DIR})
    message(STATUS ${CMAKE_PREFIX_PATH})
endif()

file(GLOB_RECURSE INCS CONFIGURE_DEPENDS inc/*.h)
file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS src/*.cpp src/*.def)
file(GLOB_RECURSE UIS CONFIGURE_DEPENDS ui/*.ui)
file(GLOB_RECURSE QRCS CONFIGURE_DEPENDS qrc/*.qrc)
file(GLOB_RECURSE LIBS CONFIGURE_DEPENDS ${zrxsdk_dir}/lib-${CMAKE_VS_PLATFORM_NAME}/*.lib)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRCS} ${INCS} ${UIS} ${QRCS})

list(APPEND CMAKE_AUTOUIC_SEARCH_PATHS "ui")
list(APPEND CMAKE_AUTORCC_SEARCH_PATHS "qrc")

add_library(${PROJECT_NAME} SHARED ${INCS} ${SRCS} ${QRCS})

target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/Zc:wchar_t /MD>
    $<$<CXX_COMPILER_ID:GNU>:-Wall>
)

if(MSVC)
    target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE _AFXDLL _WIN32_WINNT=0x0600)
    target_precompile_headers(${PROJECT_NAME} PRIVATE inc/stdafx.h)
elseif(CMAKE_COMPILER_IS_GNUCXX)
    target_compile_definitions(${PROJECT_NAME} PRIVATE UNICODE _UNICODE)
    target_compile_options(${PROJECT_NAME} PRIVATE -fpch-preprocess)
    target_precompile_headers(${PROJECT_NAME} PRIVATE inc/stdafx.h)
else()
    message(WARNING "Unsupported compiler for precompiled headers or definitions!")
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
    ${zrxsdk_dir}/inc
    ${zrxsdk_dir}/arxport/inc
    ${zrxsdk_dir}/arxport/inc-x64
    inc
)

target_link_directories(${PROJECT_NAME} PRIVATE ${zrxsdk_dir}/lib-${CMAKE_VS_PLATFORM_NAME})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LIBS}
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui)

set_target_properties(${PROJECT_NAME} PROPERTIES
    FOLDER "Component"
    OUTPUT_NAME ${PROJECT_NAME}
    SUFFIX ".zrx"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/bin
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/bin
)

if(MSVC)
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES
        VS_DEBUGGER_COMMAND ${ZWCAD_INSTALL_PATH}/ZwCAD.exe
        VS_DEBUGGER_WORKING_DIRECTORY ${ZWCAD_INSTALL_PATH}
    )
endif()