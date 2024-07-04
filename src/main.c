// https://github.com/cztomczak/cefcapi/blob/master/examples/main_win.c
// https://gist.github.com/msmshazan/dfd5362004be37ff5e016b6a42be5083


#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

// do you need depth buffer?
#define WINDOW_DEPTH 1

// do you need stencil buffer?
#define WINDOW_STENCIL 0

// use sRGB or MSAA for color buffer
// set 0 to disable, or 1 to enable sRGB
// typical values for MSAA are 2, 4, 8, 16, ...
// when enabled, D3D11 cannot use more modern lower-latency flip swap effect on Windows 8.1/10
// instead you can use sRGB/MSAA render target and copy it to non-sRGB window
#define WINDOW_SRGB 0
#define WINDOW_MSAA 0

// do you need vsync?
#define WINDOW_VSYNC 1

// keep this enabled when debugging
#define USE_DEBUG_MODE 1

#define D3D11_NO_HELPERS
#define CINTERFACE
#define COBJMACROS
#define WIN32_LEAN_AND_MEAN
//#define INITGUID
//#include <guiddef.h>

#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <dxgi1_6.h>
#include <stddef.h>
#include <windows.h>

//EXTERN_C const IID IID_ID3D11DeviceContext1 = {0x1841e5c8, 0x16b0, 0x489b, {0xbc, 0xc8, 0x44, 0xcf, 0xb2, 0x6e, 0xd2, 0x16}};


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "cef_app.h"
#include "cef_base.h"
#include "cef_client.h"
#include "cef_life_span_handler.h"
#include <include/cef_version.h>


cef_life_span_handler_t g_life_span_handler = {};


[[noreturn]] void Panic(const char* file, const int line, const char* message) {
  fprintf(stderr, "\n%s:%d Error! %s\n", file, line, message);
  __builtin_trap();
}
[[noreturn]] void PanicWin32(const char* file, const int line, DWORD err) {
  fprintf(stderr, "\n%s:%d Error! ", file, line);
  LPWSTR err_str;
  if (FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), (LPWSTR) &err_str, 0, NULL)) {
    OutputDebugStringW(err_str);
    LocalFree(err_str);
  }
  __builtin_trap();
}
#define PANIC(message) Panic(__FILE__, __LINE__, message)
#define PANIC_WIN32(err) PanicWin32(__FILE__, __LINE__, err)
#define REQUIRE(condition, message)        \
  if (__builtin_expect(!(condition), 0)) { \
    PANIC(message);                        \
  }
#define REQUIRE_WIN32(condition, err)      \
  if (__builtin_expect(!(condition), 0)) { \
    PANIC_WIN32(err);                      \
  }
#define DX_REQUIRE(command)           \
  {                                   \
    HRESULT hr = command;             \
    REQUIRE_WIN32(SUCCEEDED(hr), hr); \
  }


int main(int argc, char* argv[]) {

  printf("\nProcess args: ");
  if (argc == 1) {
    printf("none (Main process)");
  } else {
    for (int i = 1; i < argc; i++) {
      if (strlen(argv[i]) > 128)
        printf("... ");
      else
        printf("%s ", argv[i]);
    }
  }
  printf("\n\n");

  { // DX
    IDXGISwapChain* render_swapchain;
    ID3D11Device* render_device;
    ID3D11DeviceContext* render_context;
    ID3D11DeviceContext1* render_context1;
    D3D_FEATURE_LEVEL level;

    UINT flags = 0;
#ifdef _DEBUG
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    DX_REQUIRE(D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, (const D3D_FEATURE_LEVEL[]){D3D_FEATURE_LEVEL_11_1}, 1, D3D11_SDK_VERSION, &render_device, &level, &render_context));
    DX_REQUIRE(ID3D11DeviceContext_QueryInterface(render_context, &IID_ID3D11DeviceContext1,(void **) &render_context1));

    IDXGIFactory* factory;
    DX_REQUIRE(CreateDXGIFactory(&IID_IDXGIFactory,(void **) &factory));

//    DXGI_SWAP_CHAIN_DESC desc = {};
//    desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//    desc.BufferDesc.RefreshRate.Numerator = 60;
//    desc.BufferDesc.RefreshRate.Denominator = 1;
//    desc.SampleDesc.Count = 1;
//    desc.SampleDesc.Quality = 0;
//    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//    desc.OutputWindow = wnd;
//    desc.Windowed = TRUE;
//    desc.BufferCount = 2;
//    desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
//    desc.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

  }

  { // CEF
    if (argc == 0) {
      printf("CEF version: %s\n", CEF_VERSION);
    }

    cef_main_args_t main_args = {
        .instance = GetModuleHandle(NULL),
    };

    cef_app_t app = {};
    initialize_cef_app(&app);

    printf("cef_execute_process, argc=%d\n", argc);
    int code = cef_execute_process(&main_args, &app, NULL);
    if (code >= 0) {
      _exit(code);
    }

    cef_settings_t settings = {
        .size = sizeof(cef_settings_t),
        .log_severity = LOGSEVERITY_WARNING,
        .no_sandbox = 1,
    };

    printf("cef_initialize\n");
    cef_initialize(&main_args, &settings, &app, NULL);

    char window_name[] = "moxaic";
    cef_string_t cef_window_name = {};
    cef_string_utf8_to_utf16(window_name, strlen(window_name), &cef_window_name);

    cef_window_info_t window_info = {
        .window_name = cef_window_name,
        .style = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
        .parent_window = NULL,
        .bounds.x = CW_USEDEFAULT,
        .bounds.y = CW_USEDEFAULT,
        .bounds.width = CW_USEDEFAULT,
        .bounds.height = CW_USEDEFAULT,
    };

    cef_browser_settings_t browser_settings = {
        .size = sizeof(cef_browser_settings_t),
    };

    cef_client_t client = {};
    initialize_cef_client(&client);
    initialize_cef_life_span_handler(&g_life_span_handler);

    char url[] = "https://www.google.com";
    cef_string_t cef_url = {};
    cef_string_utf8_to_utf16(url, strlen(url), &cef_url);

    printf("cef_browser_host_create_browser\n");
    cef_browser_host_create_browser(&window_info, &client, &cef_url, &browser_settings, NULL, NULL);

    printf("cef_run_message_loop\n");
    cef_run_message_loop();

    printf("cef_shutdown\n");
    cef_shutdown();
  }

  return 0;
}