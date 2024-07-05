#pragma once

#include <stdbool.h>

#ifdef MID_WINDOW_VULKAN
#include <vulkan/vulkan.h>
#endif

//
//// Debug
extern void Panic(const char* file, int line, const char* message);
#ifndef MID_DEBUG
#define MID_DEBUG
#define PANIC(message) Panic(__FILE__, __LINE__, message)
#define REQUIRE(condition, message)        \
  if (__builtin_expect(!(condition), 0)) { \
    PANIC(message);                        \
  }
#endif

//
//// MidWindow Header
#define DEFAULT_WIDTH  1024
#define DEFAULT_HEIGHT 1024

typedef struct MidWindow {
  volatile bool running;
  HINSTANCE hInstance;
  HWND      hWnd;
  int       width, height;
  POINT     localCenter, globalCenter;
  uint64_t  frequency, start, current;
} MidWindow;

typedef struct MidWindowInput {
  float mouseDeltaX;
  float mouseDeltaY;
  bool  mouseLocked;

  bool moveForward;
  bool moveBack;
  bool moveRight;
  bool moveLeft;

  double deltaTime;

  bool debugSwap;

} MidWindowInput;

extern MidWindow midWindow;
extern MidWindowInput midWindowInput;

void midUpdateWindowInput();
void midCreateWindow();

#ifdef MID_WINDOW_VULKAN
void midCreateVulkanSurface(const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface);
#endif

extern double timeQueryMs;

//
//// MidWindow Implementation
#ifdef MID_WINDOW_IMPLEMENTATION

#ifndef MID_DEBUG
[[noreturn]] void Panic(const char* file, const int line, const char* message) {
  fprintf(stderr, "\n%s:%d Error! %s\n", file, line, message);
  __builtin_trap();
}
#endif

#define WIN32_LEAN_AND_MEAN
#define NOCOMM
#include <windows.h>
#include <windowsx.h>

#include <stdio.h>

#ifdef MID_WINDOW_VULKAN
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
#endif

#define WINDOW_NAME "moxaic"
#define CLASS_NAME  "MoxaicWindowClass"

#ifdef MID_WINDOW_VULKAN
const char*                              VKM_PLATFORM_SURFACE_EXTENSION_NAME = VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
const char*                              VKM_EXTERNAL_MEMORY_EXTENSION_NAME = VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME;
const char*                              VKM_EXTERNAL_SEMAPHORE_EXTENSION_NAME = VK_KHR_EXTERNAL_SEMAPHORE_WIN32_EXTENSION_NAME;
const char*                              VKM_EXTERNAL_FENCE_EXTENSION_NAME = VK_KHR_EXTERNAL_FENCE_WIN32_EXTENSION_NAME;
const VkExternalMemoryHandleTypeFlagBits VKM_EXTERNAL_HANDLE_TYPE = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;
#endif

MidWindowInput midWindowInput;
MidWindow midWindow;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  switch (uMsg) {
    case WM_MOUSEMOVE:
      if (midWindowInput.mouseLocked) {
        midWindowInput.mouseDeltaX = (float)(GET_X_LPARAM(lParam) - midWindow.localCenter.x);
        midWindowInput.mouseDeltaY = (float)(GET_Y_LPARAM(lParam) - midWindow.localCenter.y);
        SetCursorPos(midWindow.globalCenter.x, midWindow.globalCenter.y);
      }
      return 0;
    case WM_LBUTTONDOWN:
      ShowCursor(FALSE);
      SetCapture(midWindow.hWnd);
      RECT rect;
      GetClientRect(midWindow.hWnd, &rect);
      midWindow.globalCenter = midWindow.localCenter = (POINT){(rect.right - rect.left) / 2, (rect.bottom - rect.top) / 2};
      ClientToScreen(midWindow.hWnd, (POINT*)&midWindow.globalCenter);
      SetCursorPos(midWindow.globalCenter.x, midWindow.globalCenter.y);
      midWindowInput.mouseLocked = true;
      return 0;
    case WM_LBUTTONUP:
      ShowCursor(TRUE);
      ReleaseCapture();
      midWindowInput.mouseLocked = false;
      return 0;
    case WM_KEYDOWN:
      switch (wParam) {
        case 'W':
          midWindowInput.moveForward = true; return 0;
        case 'S':
          midWindowInput.moveBack = true; return 0;
        case 'D':
          midWindowInput.moveRight = true; return 0;
        case 'A':
          midWindowInput.moveLeft = true; return 0;
        case 'Z':
          midWindowInput.debugSwap = !midWindowInput.debugSwap; return 0;
        default:  return 0;
      }
    case WM_KEYUP:
      switch (wParam) {
        case 'W':
          midWindowInput.moveForward = false; return 0;
        case 'S':
          midWindowInput.moveBack = false; return 0;
        case 'D':
          midWindowInput.moveRight = false; return 0;
        case 'A':
          midWindowInput.moveLeft = false; return 0;
        default:  return 0;
      }
    case WM_CLOSE:
      midWindow.running = false;
      PostQuitMessage(0);
      return 0;
    default:
      return DefWindowProc(hWnd, uMsg, wParam, lParam);
  }
}

double timeQueryMs;

void midUpdateWindowInput() {
  midWindowInput.mouseDeltaX = 0;
  midWindowInput.mouseDeltaY = 0;
  static MSG msg;
  while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }

  uint64_t prior = midWindow.current;
  QueryPerformanceCounter((LARGE_INTEGER*)&midWindow.current);
  uint64_t delta = ((midWindow.current - prior) * 1000000) / midWindow.frequency;
  midWindowInput.deltaTime = (double)delta * 0.000001f;

  static int titleUpdateRate = 64;
  if (!--titleUpdateRate) {
    titleUpdateRate = 64;
    static char titleBuffer[32];
    sprintf(titleBuffer, "%s | FPS=%.2f | TimeQuery=%.8f", WINDOW_NAME, 1.0f / midWindowInput.deltaTime, timeQueryMs);
    SetWindowText(midWindow.hWnd, titleBuffer);
  }
}

void midCreateWindow() {
  REQUIRE(midWindow.hInstance == NULL, "Window already created!");
  midWindow.hInstance = GetModuleHandle(NULL);
  const DWORD    windowStyle = WS_OVERLAPPEDWINDOW;
  const WNDCLASS wc = {.lpfnWndProc = WindowProc, .hInstance = midWindow.hInstance, .lpszClassName = CLASS_NAME};
  RegisterClass(&wc);
  RECT rect = {.right = DEFAULT_WIDTH, .bottom = DEFAULT_HEIGHT};
  AdjustWindowRect(&rect, windowStyle, FALSE);
  midWindow.width = rect.right - rect.left;
  midWindow.height = rect.bottom - rect.top;
  midWindow.hWnd = CreateWindowEx(0, CLASS_NAME, WINDOW_NAME, windowStyle, CW_USEDEFAULT, CW_USEDEFAULT, midWindow.width, midWindow.height, NULL, NULL, midWindow.hInstance, NULL);
  REQUIRE(midWindow.hWnd != NULL, "Failed to create window.");
  ShowWindow(midWindow.hWnd, SW_SHOW);
  UpdateWindow(midWindow.hWnd);
  QueryPerformanceFrequency((LARGE_INTEGER*)&midWindow.frequency);
  QueryPerformanceCounter((LARGE_INTEGER*)&midWindow.start);
}

#ifdef MID_WINDOW_VULKAN
void midCreateVulkanSurface(const VkAllocationCallbacks* pAllocator, VkSurfaceKHR* pSurface) {
  const VkWin32SurfaceCreateInfoKHR win32SurfaceCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
      .hinstance = window.hInstance,
      .hwnd = window.hWnd,
  };
  VKM_INSTANCE_FUNC(vkCreateWin32SurfaceKHR);
  VKM_REQUIRE(vkCreateWin32SurfaceKHR(instance, &win32SurfaceCreateInfo, pAllocator, pSurface));
}
#endif

#endif