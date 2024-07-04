cmake_minimum_required(VERSION 3.28)
project(chromoxaic C)

set(CMAKE_C_STANDARD 11)

set(TARGET_NAME chromoxaic)

file(GLOB SRC_FILES
        src/*.c
        src/*.h
        cefcapi/*.h
)

add_executable(${TARGET_NAME}
        ${SRC_FILES}
)

set(CEF_PATH "C:/Developer/cef_binary_126.2.9")

target_link_directories(${TARGET_NAME} PUBLIC
        "${CEF_PATH}/Release"
)

target_include_directories(${TARGET_NAME} PUBLIC
        src
        cefcapi
        "${CEF_PATH}"
)

target_link_libraries(${TARGET_NAME}
        user32
        dxgi
        d3d11
        dxguid
        libcef
        cef_sandbox
)