
#pragma once

#include "cef_base.h"
#include "include/capi/cef_app_capi.h"
#include "include/capi/cef_browser_process_handler_capi.h"
#include <stdbool.h>

//
//// browser_process_handler
typedef struct mxc_cef_browser_process_handler_t {
  cef_browser_process_handler_t cef;
  int schedule_message_pump_work;
  int ref_count;
} mxc_cef_browser_process_handler_t;

extern mxc_cef_browser_process_handler_t g_browser_process_handler;

void CEF_CALLBACK on_schedule_message_pump_work(
    struct _cef_browser_process_handler_t* self,
    int64_t delay_ms) {
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

void CEF_CALLBACK on_before_command_line_processing(
    cef_app_t* self,
    const cef_string_t* process_type,
    cef_command_line_t* command_line) {
  DEBUG_CALLBACK("on_before_command_line_processing\n");
}

void CEF_CALLBACK on_register_custom_schemes(
    cef_app_t* self,
    cef_scheme_registrar_t* registrar) {
  printf("on_register_custom_schemes\n");
}

cef_resource_bundle_handler_t* CEF_CALLBACK get_resource_bundle_handler(cef_app_t* self) {
  DEBUG_CALLBACK("get_resource_bundle_handler\n");
  return NULL;
}

cef_browser_process_handler_t* CEF_CALLBACK get_browser_process_handler(cef_app_t* self) {
  DEBUG_CALLBACK("get_browser_process_handler\n");
  return (struct _cef_browser_process_handler_t*) &g_browser_process_handler;
}

cef_render_process_handler_t* CEF_CALLBACK get_render_process_handler(cef_app_t* self) {
  DEBUG_CALLBACK("get_render_process_handler\n");
  return NULL;
}

CEF_REF_CALLBACKS(app, mxc_cef_app_t);

void initialize_cef_app(mxc_cef_app_t* app) {
  printf("initialize_cef_app\n");

  initialize_cef_browser_process_handler(&g_browser_process_handler);

  CEF_SET_REF_CALLBACKS(app, app);
  app->cef.base.size = sizeof(mxc_cef_app_t);
  app->cef.on_before_command_line_processing = on_before_command_line_processing;
  app->cef.on_register_custom_schemes = on_register_custom_schemes;
  app->cef.get_resource_bundle_handler = get_resource_bundle_handler;
  app->cef.get_browser_process_handler = get_browser_process_handler;
  app->cef.get_render_process_handler = get_render_process_handler;
}
