
#pragma once

#include "cef_base.h"
#include "include/capi/cef_app_capi.h"
#include "include/capi/cef_browser_process_handler_capi.h"
#include <stdbool.h>

extern int schedule_message_pump_work;
void CEF_CALLBACK on_schedule_message_pump_work(
    struct _cef_browser_process_handler_t* self,
    int64_t delay_ms) {
  __atomic_fetch_add(&schedule_message_pump_work, 1, __ATOMIC_RELAXED);
}

void initialize_browser_process_handler(cef_browser_process_handler_t* browser_process_handler) {
  printf("mid_initialize_cef_browser_process_handler\n");
  browser_process_handler->base.size = sizeof(cef_browser_process_handler_t);
  browser_process_handler->on_schedule_message_pump_work = on_schedule_message_pump_work;
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

extern cef_browser_process_handler_t browser_process_handler;
cef_browser_process_handler_t* CEF_CALLBACK get_browser_process_handler(cef_app_t* self) {
  DEBUG_CALLBACK("get_browser_process_handler\n");
  return (struct _cef_browser_process_handler_t*) &browser_process_handler;
}

cef_render_process_handler_t* CEF_CALLBACK get_render_process_handler(cef_app_t* self) {
  DEBUG_CALLBACK("get_render_process_handler\n");
  return NULL;
}

void initialize_cef_app(cef_app_t* app) {
  printf("initialize_cef_app\n");

  initialize_browser_process_handler(&browser_process_handler);

  app->base.size = sizeof(cef_app_t);
  app->on_before_command_line_processing = on_before_command_line_processing;
  app->on_register_custom_schemes = on_register_custom_schemes;
  app->get_resource_bundle_handler = get_resource_bundle_handler;
  app->get_browser_process_handler = get_browser_process_handler;
  app->get_render_process_handler = get_render_process_handler;
}


#ifdef MID_CEF_IMPLEMENTATION
cef_browser_process_handler_t browser_process_handler = {};
int schedule_message_pump_work = 0;
#endif