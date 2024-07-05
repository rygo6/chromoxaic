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
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <dxgi1_6.h>
#include <stddef.h>
#include <windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <include/cef_version.h>

#include "cef_app.h"
#include "cef_base.h"
#include "cef_client.h"
#include "cef_life_span_handler.h"
#include "cef_render_handler.h"

#define MID_DEBUG
[[noreturn]] void Panic(const char* file, const int line, const char* message) {
  fprintf(stderr, "\n%s:%d Error! %s\n", file, line, message);
  __builtin_trap();
}
#define PANIC(message) Panic(__FILE__, __LINE__, message)
#define REQUIRE(condition, message)        \
  if (__builtin_expect(!(condition), 0)) { \
    PANIC(message);                        \
  }

#define MID_WINDOW_IMPLEMENTATION
#include "mid_window.h"

#define MID_WIN32_DEBUG
[[noreturn]] void PanicWin32(const char* file, const int line, DWORD err) {
  fprintf(stderr, "\n%s:%d Error! ", file, line);
  LPWSTR err_str;
  if (FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), (LPWSTR) &err_str, 0, NULL)) {
    OutputDebugStringW(err_str);
    LocalFree(err_str);
  }
  __builtin_trap();
}
#define PANIC_WIN32(err) PanicWin32(__FILE__, __LINE__, err)
#define REQUIRE_WIN32(condition, err)      \
  if (__builtin_expect(!(condition), 0)) { \
    PANIC_WIN32(err);                      \
  }
#define DX_REQUIRE(command)           \
  {                                   \
    HRESULT hr = command;             \
    REQUIRE_WIN32(SUCCEEDED(hr), hr); \
  }

bool g_main_process;
mxc_cef_life_span_handler_t g_life_span_handler = {};
mxc_cef_render_handler_t g_cef_render_handler = {};

int main(int argc, char* argv[]) {

  printf("\nProcess args: ");
  switch (argc) {
    case 0:
      printf("CEF version: %s\n", CEF_VERSION);
      break;
    case 1:
      printf("none (Main process)");
    default:
      for (int i = 1; i < argc; i++) {
        printf("%s ", argv[i]);
      }
  }

  mxc_cef_app_t app = {};
  initialize_cef_app(&app);

  printf("cef_execute_process, argc=%d\n", argc);
  cef_main_args_t main_args = {.instance = GetModuleHandle(NULL)};
  int code = cef_execute_process(&main_args, (cef_app_t*) &app, NULL);
  if (code >= 0) {
    _exit(code);
  }

  {// Test window
    midCreateWindow();

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
    DX_REQUIRE(ID3D11DeviceContext_QueryInterface(render_context, &IID_ID3D11DeviceContext1, (void**) &render_context1));

    IDXGIFactory* factory;
    DX_REQUIRE(CreateDXGIFactory(&IID_IDXGIFactory, (void**) &factory));

    DXGI_SWAP_CHAIN_DESC desc = {
        .BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
        .BufferDesc.RefreshRate.Numerator = 60,
        .BufferDesc.RefreshRate.Denominator = 1,
        .SampleDesc.Count = 1,
        .SampleDesc.Quality = 0,
        .BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
        .OutputWindow = midWindow.hWnd,
        .Windowed = TRUE,
        .BufferCount = 2,
        .SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
        .Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT,
    };
    DX_REQUIRE(IDXGIFactory_CreateSwapChain(factory, (IUnknown*) render_device, &desc, &render_swapchain));
  }

  {// CEF
    cef_settings_t settings = {
        .size = sizeof(cef_settings_t),
        .log_severity = LOGSEVERITY_WARNING,
        .no_sandbox = 1,
        .windowless_rendering_enabled = 1,
    };

    printf("cef_initialize\n");
    cef_initialize(&main_args, &settings, (cef_app_t*) &app, NULL);

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
        .windowless_rendering_enabled = 1,
        .shared_texture_enabled = 1,
        //                .external_begin_frame_enabled = 1,
    };

    cef_browser_settings_t browser_settings = {
        .size = sizeof(cef_browser_settings_t),
    };

    mxc_cef_client_t client = {};
    initialize_cef_client(&client);

    initialize_cef_render_handler(&g_cef_render_handler);
    initialize_cef_life_span_handler(&g_life_span_handler);

    char url[] = "https://www.google.com";
    cef_string_t cef_url = {};
    cef_string_utf8_to_utf16(url, strlen(url), &cef_url);

    printf("cef_browser_host_create_browser\n");
    cef_browser_host_create_browser(&window_info, (struct _cef_client_t*) &client, &cef_url, &browser_settings, NULL, NULL);

    printf("cef_run_message_loop\n");
    cef_run_message_loop();

    printf("cef_shutdown\n");
    cef_shutdown();
  }

  return 0;
}
