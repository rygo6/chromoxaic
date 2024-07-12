#pragma once

#include "cef_base.h"
#include "include/capi/cef_permission_handler_capi.h"

typedef struct mxc_cef_permission_handler_t {
  cef_permission_handler_t cef;
  int ref_count;
} mxc_cef_permission_handler_t;

extern mxc_cef_permission_handler_t g_permission_handler;

int CEF_CALLBACK on_request_media_access_permission(
    cef_permission_handler_t* self,
    cef_browser_t* browser,
    cef_frame_t* frame,
    const cef_string_t* requesting_origin,
    uint32_t requested_permissions,
    cef_media_access_callback_t* callback) {
  printf("on_request_media_access_permission\n");
  return 0;
}

int CEF_CALLBACK on_show_permission_prompt(
    cef_permission_handler_t* self,
    cef_browser_t* browser,
    uint64_t prompt_id,
    const cef_string_t* requesting_origin,
    uint32_t requested_permissions,
    cef_permission_prompt_callback_t* callback) {
  printf("on_show_permission_prompt\n");
  return 1;
}

void CEF_CALLBACK on_dismiss_permission_prompt(
    cef_permission_handler_t* self,
    cef_browser_t* browser,
    uint64_t prompt_id,
    cef_permission_request_result_t result) {
  printf("on_dismiss_permission_prompt\n");
}

CEF_REF_CALLBACKS(permission_handler, mxc_cef_permission_handler_t);

void initialize_cef_permission_handler_t(mxc_cef_permission_handler_t* handler) {
  printf("initialize_cef_life_span_handler\n");
  handler->cef.base.size = sizeof(mxc_cef_permission_handler_t);

  CEF_SET_REF_CALLBACKS(permission_handler, handler);

  handler->cef.on_request_media_access_permission = on_request_media_access_permission;
  handler->cef.on_show_permission_prompt = on_show_permission_prompt;
  handler->cef.on_dismiss_permission_prompt = on_dismiss_permission_prompt;
}
