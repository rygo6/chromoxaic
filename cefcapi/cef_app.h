
#pragma once

#include "cef_base.h"
#include "include/capi/cef_app_capi.h"
#include "include/capi/cef_browser_process_handler_capi.h"
#include <stdbool.h>

extern int g_schedule_message_pump_work;
void CEF_CALLBACK on_schedule_message_pump_work(
    struct _cef_browser_process_handler_t* self,
    int64_t delay_ms) {
  __atomic_store_n(&g_schedule_message_pump_work, 1, __ATOMIC_RELAXED);
}

void initialize_browser_process_handler(cef_browser_process_handler_t* browser_process_handler) {
  printf("%lu: initialize_browser_process_handler\n", GetCurrentThreadId());
  browser_process_handler->base.size = sizeof(cef_browser_process_handler_t);
  browser_process_handler->on_schedule_message_pump_work = on_schedule_message_pump_work;
}

////
//// Render Process handler
extern cef_browser_t* g_browser;
void CEF_CALLBACK on_browser_created(
    struct _cef_render_process_handler_t* self,
    struct _cef_browser_t* browser,
    struct _cef_dictionary_value_t* extra_info) {
  printf("%lu: on_browser_created\n", GetCurrentThreadId());
  __atomic_store_n(&g_browser, browser, __ATOMIC_RELAXED);
}

void initialize_render_process_handler(cef_render_process_handler_t* render_process_handler) {
  printf("initialize_render_process_handler\n");
  render_process_handler->base.size = sizeof(cef_render_process_handler_t);
  render_process_handler->on_browser_created = on_browser_created;
}

//
//// app
void CEF_CALLBACK on_before_command_line_processing(cef_app_t* self, const cef_string_t* process_type, cef_command_line_t* command_line) {
  DEBUG_CALLBACK("on_before_command_line_processing\n");
}

void CEF_CALLBACK on_register_custom_schemes(cef_app_t* self, cef_scheme_registrar_t* registrar) {
  printf("on_register_custom_schemes\n");
}

cef_resource_bundle_handler_t* CEF_CALLBACK get_resource_bundle_handler(cef_app_t* self) {
  DEBUG_CALLBACK("get_resource_bundle_handler\n");
  return NULL;
}

extern cef_browser_process_handler_t g_browser_process_handler;
cef_browser_process_handler_t* CEF_CALLBACK get_browser_process_handler(cef_app_t* self) {
  DEBUG_CALLBACK("get_browser_process_handler\n");
  return (struct _cef_browser_process_handler_t*) &g_browser_process_handler;
}

extern cef_render_process_handler_t g_render_process_handler;
cef_render_process_handler_t* CEF_CALLBACK get_render_process_handler(cef_app_t* self) {
  DEBUG_CALLBACK("get_render_process_handler\n");
  return (struct _cef_render_process_handler_t*) &g_render_process_handler;
}

void initialize_cef_app(cef_app_t* app) {
  printf("initialize_cef_app\n");

  initialize_browser_process_handler(&g_browser_process_handler);
  initialize_render_process_handler(&g_render_process_handler);

  app->base.size = sizeof(cef_app_t);
  app->on_before_command_line_processing = on_before_command_line_processing;
  app->on_register_custom_schemes = on_register_custom_schemes;
  app->get_resource_bundle_handler = get_resource_bundle_handler;
  app->get_browser_process_handler = get_browser_process_handler;
  app->get_render_process_handler = get_render_process_handler;
}

#ifdef MID_CEF_IMPLEMENTATION
cef_browser_process_handler_t g_browser_process_handler = {};
cef_render_process_handler_t g_render_process_handler = {};
int g_schedule_message_pump_work = 0;
cef_browser_t* g_browser = NULL;
#endif