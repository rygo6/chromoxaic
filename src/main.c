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

mxc_cef_life_span_handler_t g_life_span_handler = {};
mxc_cef_browser_process_handler_t g_cef_browser_process_handler = {};
mxc_cef_render_handler_t g_cef_render_handler = {};


IDXGISwapChain* render_swapchain;
ID3D11Device* render_device;
ID3D11Device1* render_device1;
ID3D11DeviceContext* render_context;
ID3D11DeviceContext1* render_context1;
ID3D11RenderTargetView* render_window_rtview;
HANDLE render_frame_latency_wait;

void CEF_CALLBACK on_paint(struct _cef_render_handler_t* self,
                           struct _cef_browser_t* browser,
                           cef_paint_element_type_t type,
                           size_t dirtyRectsCount,
                           cef_rect_t const* dirtyRects,
                           const void* buffer,
                           int width,
                           int height) {
  PANIC("on_paint is not supposed to be called");
}

void CEF_CALLBACK on_accelerated_paint(
    struct _cef_render_handler_t* self,
    struct _cef_browser_t* browser,
    cef_paint_element_type_t type,
    size_t dirtyRectsCount,
    cef_rect_t const* dirtyRects,
    const cef_accelerated_paint_info_t* info) {

  float clear_color[4] = {1.0f, 0.0f, 0.0f, 1.0f};// Red color
  ID3D11DeviceContext_ClearRenderTargetView(render_context, render_window_rtview, clear_color);

  ID3D11Texture2D* tex;
  ID3D11Device1_OpenSharedResource1(render_device1, info->shared_texture_handle, &IID_ID3D11Texture2D, (void**) &tex);
  D3D11_TEXTURE2D_DESC td;
  ID3D11Texture2D_GetDesc(tex, &td);

  //  D3D11_VIEWPORT vp = { 0.0f, 0.0f, (FLOAT)td.Width, (FLOAT)td.Height, 0.0f, 1.0f };
  //  ID3D11DeviceContext1_RSSetViewports(render_context1, 1, &vp);

  ID3D11Texture2D* temp_window_buffer;
  DX_REQUIRE(IDXGISwapChain_GetBuffer(render_swapchain, 0, &IID_ID3D11Texture2D, (void**) &temp_window_buffer));
  ID3D11DeviceContext1_CopyResource(render_context1, (ID3D11Resource*) temp_window_buffer, (ID3D11Resource*) tex);

  IDXGISwapChain_Present(render_swapchain, 0, 0);

  //  WaitForSingleObjectEx(render_frame_latency_wait, INFINITE, TRUE);
}


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

    D3D_FEATURE_LEVEL feature_levels[] = {D3D_FEATURE_LEVEL_11_1};
    D3D_FEATURE_LEVEL feature_level;

    UINT flags = D3D11_CREATE_DEVICE_DEBUG;
    DX_REQUIRE(D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, feature_levels, _countof(feature_levels), D3D11_SDK_VERSION, &render_device, &feature_level, &render_context));
    DX_REQUIRE(ID3D11Device_QueryInterface(render_device, &IID_ID3D11Device1, (void**) &render_device1));
    DX_REQUIRE(ID3D11DeviceContext_QueryInterface(render_context, &IID_ID3D11DeviceContext1, (void**) &render_context1));

    IDXGIFactory* factory;
    DX_REQUIRE(CreateDXGIFactory(&IID_IDXGIFactory, (void**) &factory));
    DXGI_SWAP_CHAIN_DESC desc = {
        .BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM,
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

    IDXGISwapChain2* swapchain2;
    //    DX_REQUIRE(IDXGISwapChain_QueryInterface(render_swapchain, &IID_IDXGISwapChain2, (void**) &swapchain2));
    //    render_frame_latency_wait = IDXGISwapChain2_GetFrameLatencyWaitableObject(swapchain2);
    //    IDXGISwapChain2_Release(swapchain2);
    //    DX_REQUIRE(IDXGIFactory_MakeWindowAssociation(factory, midWindow.hWnd, DXGI_MWA_NO_WINDOW_CHANGES | DXGI_MWA_NO_ALT_ENTER));

    IDXGIFactory_Release(factory);

    ID3D11Texture2D* temp_window_buffer;
    DX_REQUIRE(IDXGISwapChain_GetBuffer(render_swapchain, 0, &IID_ID3D11Texture2D, (void**) &temp_window_buffer));
    DX_REQUIRE(ID3D11Device_CreateRenderTargetView(render_device, (ID3D11Resource*) temp_window_buffer, NULL, &render_window_rtview));
    ID3D11Texture2D_Release(temp_window_buffer);

    ShowWindow(midWindow.hWnd, SW_SHOWDEFAULT);
    UpdateWindow(midWindow.hWnd);
  }

  {// CEF

    char exe_path[MAX_PATH];
    REQUIRE(GetModuleFileName(NULL, exe_path, MAX_PATH), "Couldn't get exe path");
    char* last_slash = strrchr(exe_path, '\\');
    if (last_slash != NULL) *last_slash = '\0';
    printf("Executable directory: %s\n", exe_path);
    const char* cache_folder = "\\cache";
    char root_cache_path[MAX_PATH + strlen(cache_folder)];
    sprintf(root_cache_path, "%s%s", exe_path, cache_folder);
    printf("Cache directory: %s\n", root_cache_path);
    cef_string_t cef_root_cache_path = {};
    cef_string_utf8_to_utf16(root_cache_path, strlen(root_cache_path), &cef_root_cache_path);
    cef_settings_t settings = {
        .size = sizeof(cef_settings_t),
        .log_severity = LOGSEVERITY_WARNING,
        .no_sandbox = 1,
        .windowless_rendering_enabled = 1,
        .multi_threaded_message_loop = 0,
        .external_message_pump = 1,
        .root_cache_path = cef_root_cache_path,
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
        .bounds.x = 0,
        .bounds.y = 0,
        .bounds.width = DEFAULT_WIDTH,
        .bounds.height = DEFAULT_HEIGHT,
        .windowless_rendering_enabled = 1,
        .shared_texture_enabled = 1,
        .external_begin_frame_enabled = 1,
    };

    cef_browser_settings_t browser_settings = {
        .size = sizeof(cef_browser_settings_t),
        .windowless_frame_rate = 30,
    };

    mxc_cef_client_t client = {};
    initialize_cef_client(&client);

    g_cef_render_handler.cef.on_accelerated_paint = on_accelerated_paint;
    g_cef_render_handler.cef.on_paint = on_paint;
    initialize_cef_render_handler(&g_cef_render_handler);
    initialize_cef_life_span_handler(&g_life_span_handler);

    char url[] = "https://www.google.com";
    cef_string_t cef_url = {};
    cef_string_utf8_to_utf16(url, strlen(url), &cef_url);

    printf("cef_browser_host_create_browser\n");
    cef_browser_t* browser = cef_browser_host_create_browser_sync(&window_info, (struct _cef_client_t*) &client, &cef_url, &browser_settings, NULL, NULL);
    cef_browser_host_t* browser_host = browser->get_host(browser);

    printf("cef_run_message_loop\n");
    //    cef_run_message_loop();

    while (midWindow.running) {

      midUpdateWindowInput();

      const cef_mouse_event_t mouse_event = {
          .x = midWindowInput.mouseX,
          .y = midWindowInput.mouseY,
      };
      browser_host->send_mouse_move_event(browser_host, &mouse_event, false);

      switch (midWindowInput.leftMouse) {
        case MID_PHASE_PRESS:
          browser_host->send_mouse_click_event(browser_host, &mouse_event, MBT_LEFT, false, 1);
          break;
        case MID_PHASE_RELEASE:
          browser_host->send_mouse_click_event(browser_host, &mouse_event, MBT_LEFT, true, 1);
          break;
      }

      int work_count;
      __atomic_load(&g_cef_browser_process_handler.schedule_message_pump_work, &work_count, __ATOMIC_RELAXED);
      while (work_count > 0) {
        cef_do_message_loop_work();
        work_count = __atomic_sub_fetch(&g_cef_browser_process_handler.schedule_message_pump_work, 1, __ATOMIC_RELAXED);
      }

      browser_host->send_external_begin_frame(browser_host);

      usleep((useconds_t) ((1.0f / 60.0f) * 1000000.0f));
    }

    printf("cef_shutdown\n");
    cef_shutdown();
  }

  return 0;
}
