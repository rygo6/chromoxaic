cmake_minimum_required(VERSION 3.28)
project(chromoxaic C)

set(CMAKE_C_STANDARD 99)

set(TARGET_NAME chromoxaic)

file(GLOB SRC_FILES
        src/*.c
        src/*.h
        cefcapi/*.h
)

add_executable(${TARGET_NAME}
        ${SRC_FILES}
)

#set(CEF_LINK_PATH "C:/Developer/cef_binary_126.2.9/Release")
#set(CEF_INCLUDE_PATH "C:/Developer/cef_binary_126.2.9")

#set(CEF_LINK_PATH "C:/Developer/cef_binary_127.3.5/Release")
#set(CEF_INCLUDE_PATH "C:/Developer/cef_binary_127.3.5")

#set(CEF_LINK_PATH "C:/Developer/cef_binary_129.0.12/Release")
#set(CEF_INCLUDE_PATH "C:/Developer/cef_binary_129.0.12")

set(CEF_LINK_PATH "C:/Developer/cef_binary_130.1.16/Release")
set(CEF_INCLUDE_PATH "C:/Developer/cef_binary_130.1.16")

#set(CEF_LINK_PATH "C:/code/chromium_git/chromium/src/out/Debug_GN_x64")
#set(CEF_INCLUDE_PATH "C:/code/chromium_git/chromium/src/cef")

target_link_directories(${TARGET_NAME} PUBLIC
        "${CEF_LINK_PATH}"
)

target_include_directories(${TARGET_NAME} PUBLIC
        src
        cefcapi
        "${CEF_INCLUDE_PATH}"
)

target_link_libraries(${TARGET_NAME}
#        user32
        dxgi
        d3d11
        cef
)

target_compile_definitions(${TARGET_NAME} PRIVATE

)