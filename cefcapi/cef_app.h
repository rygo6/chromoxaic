
#pragma once

#include "cef_base.h"
#include "include/capi/cef_app_capi.h"
#include <stdbool.h>

//
//// browser_process_handler
typedef struct mxc_cef_browser_process_handler_t {
  cef_browser_process_handler_t cef;
  int schedule_message_pump_work;
  int ref_count;
} mxc_cef_browser_process_handler_t;

extern mxc_cef_browser_process_handler_t g_cef_browser_process_handler;

void CEF_CALLBACK on_schedule_message_pump_work(
    struct _cef_browser_process_handler_t* self,
    int64_t delay_ms) {
  printf("on_schedule_message_pump_work %lld\n", delay_ms);
  __atomic_fetch_add(&(((mxc_cef_browser_process_handler_t*)self)->schedule_message_pump_work), 1, __ATOMIC_RELAXED);
}

CEF_REF_CALLBACKS(browser_process_handler, mxc_cef_browser_process_handler_t);

void initialize_cef_browser_process_handler(mxc_cef_browser_process_handler_t* browser_process_handler) {
  printf("initialize_cef_browser_process_handler\n");

  CEF_SET_REF_CALLBACKS(browser_process_handler, browser_process_handler);
  browser_process_handler->cef.base.size = sizeof(mxc_cef_browser_process_handler_t);
  browser_process_handler->cef.on_schedule_message_pump_work = on_schedule_message_pump_work;
}

//
//// app
typedef struct mxc_cef_app_t {
  cef_app_t cef;
  int ref_count;
} mxc_cef_app_t;

///
// Provides an opportunity to view and/or modify command-line arguments before
// processing by CEF and Chromium. The |process_type| value will be NULL for
// the browser process. Do not keep a reference to the cef_command_line_t
// object passed to this function. The CefSettings.command_line_args_disabled
// value can be used to start with an NULL command-line object. Any values
// specified in CefSettings that equate to command-line arguments will be set
// before this function is called. Be cautious when using this function to
// modify command-line arguments for non-browser processes as this may result
// in undefined behavior including crashes.
///
void CEF_CALLBACK on_before_command_line_processing(
    struct _cef_app_t* self, const cef_string_t* process_type,
    struct _cef_command_line_t* command_line) {
  DEBUG_CALLBACK("on_before_command_line_processing\n");
}

///
// Provides an opportunity to register custom schemes. Do not keep a reference
// to the |registrar| object. This function is called on the main thread for
// each process and the registered schemes should be the same across all
// processes.
///
void CEF_CALLBACK on_register_custom_schemes(
    struct _cef_app_t* self,
    struct _cef_scheme_registrar_t* registrar) {
  DEBUG_CALLBACK("on_register_custom_schemes\n");
}

///
// Return the handler for resource bundle events. If
// CefSettings.pack_loading_disabled is true (1) a handler must be returned.
// If no handler is returned resources will be loaded from pack files. This
// function is called by the browser and render processes on multiple threads.
///
struct _cef_resource_bundle_handler_t* CEF_CALLBACK get_resource_bundle_handler(struct _cef_app_t* self) {
  DEBUG_CALLBACK("get_resource_bundle_handler\n");
  return NULL;
}

///
// Return the handler for functionality specific to the browser process. This
// function is called on multiple threads in the browser process.
///
struct _cef_browser_process_handler_t* CEF_CALLBACK get_browser_process_handler(struct _cef_app_t* self) {
  DEBUG_CALLBACK("get_browser_process_handler\n");
  return (struct _cef_browser_process_handler_t*) &g_cef_browser_process_handler;
}

///
// Return the handler for functionality specific to the render process. This
// function is called on the render process main thread.
///
struct _cef_render_process_handler_t* CEF_CALLBACK get_render_process_handler(struct _cef_app_t* self) {
  DEBUG_CALLBACK("get_render_process_handler\n");
  return NULL;
}

CEF_REF_CALLBACKS(app, mxc_cef_app_t);

void initialize_cef_app(mxc_cef_app_t* app) {
  printf("initialize_cef_app\n");

  initialize_cef_browser_process_handler(&g_cef_browser_process_handler);

  CEF_SET_REF_CALLBACKS(app, app);
  app->cef.base.size = sizeof(mxc_cef_app_t);
  app->cef.on_before_command_line_processing = on_before_command_line_processing;
  app->cef.on_register_custom_schemes = on_register_custom_schemes;
  app->cef.get_resource_bundle_handler = get_resource_bundle_handler;
  app->cef.get_browser_process_handler = get_browser_process_handler;
  app->cef.get_render_process_handler = get_render_process_handler;
  app->cef.get_render_process_handler = get_render_process_handler;
}
