#pragma once

#include "cef_base.h"
#include "include/capi/cef_permission_handler_capi.h"

typedef struct mxc_cef_permission_handler_t {
  cef_permission_handler_t cef;
  int ref_count;
} mxc_cef_permission_handler_t;

extern mxc_cef_permission_handler_t g_permission_handler;

const char* CEF_PERMISSION_TYPE_NAMES[] = {
  [CEF_PERMISSION_TYPE_NONE] = "CEF_PERMISSION_TYPE_NONE",
  [CEF_PERMISSION_TYPE_ACCESSIBILITY_EVENTS] = "CEF_PERMISSION_TYPE_ACCESSIBILITY_EVENTS",
  [CEF_PERMISSION_TYPE_AR_SESSION] = "CEF_PERMISSION_TYPE_AR_SESSION",
  [CEF_PERMISSION_TYPE_CAMERA_PAN_TILT_ZOOM] = "CEF_PERMISSION_TYPE_CAMERA_PAN_TILT_ZOOM",
  [CEF_PERMISSION_TYPE_CAMERA_STREAM] = "CEF_PERMISSION_TYPE_CAMERA_STREAM",
  [CEF_PERMISSION_TYPE_CAPTURED_SURFACE_CONTROL] = "CEF_PERMISSION_TYPE_CAPTURED_SURFACE_CONTROL",
  [CEF_PERMISSION_TYPE_CLIPBOARD] = "CEF_PERMISSION_TYPE_CLIPBOARD",
  [CEF_PERMISSION_TYPE_TOP_LEVEL_STORAGE_ACCESS] = "CEF_PERMISSION_TYPE_TOP_LEVEL_STORAGE_ACCESS",
  [CEF_PERMISSION_TYPE_DISK_QUOTA] = "CEF_PERMISSION_TYPE_DISK_QUOTA",
  [CEF_PERMISSION_TYPE_LOCAL_FONTS] = "CEF_PERMISSION_TYPE_LOCAL_FONTS",
  [CEF_PERMISSION_TYPE_GEOLOCATION] = "CEF_PERMISSION_TYPE_GEOLOCATION",
  [CEF_PERMISSION_TYPE_IDENTITY_PROVIDER] = "CEF_PERMISSION_TYPE_IDENTITY_PROVIDER",
  [CEF_PERMISSION_TYPE_IDLE_DETECTION] = "CEF_PERMISSION_TYPE_IDLE_DETECTION",
  [CEF_PERMISSION_TYPE_MIC_STREAM] = "CEF_PERMISSION_TYPE_MIC_STREAM",
  [CEF_PERMISSION_TYPE_MIDI_SYSEX] = "CEF_PERMISSION_TYPE_MIDI_SYSEX",
  [CEF_PERMISSION_TYPE_MULTIPLE_DOWNLOADS] = "CEF_PERMISSION_TYPE_MULTIPLE_DOWNLOADS",
  [CEF_PERMISSION_TYPE_NOTIFICATIONS] = "CEF_PERMISSION_TYPE_NOTIFICATIONS",
  [CEF_PERMISSION_TYPE_KEYBOARD_LOCK] = "CEF_PERMISSION_TYPE_KEYBOARD_LOCK",
  [CEF_PERMISSION_TYPE_POINTER_LOCK] = "CEF_PERMISSION_TYPE_POINTER_LOCK",
  [CEF_PERMISSION_TYPE_PROTECTED_MEDIA_IDENTIFIER] = "CEF_PERMISSION_TYPE_PROTECTED_MEDIA_IDENTIFIER",
  [CEF_PERMISSION_TYPE_REGISTER_PROTOCOL_HANDLER] = "CEF_PERMISSION_TYPE_REGISTER_PROTOCOL_HANDLER",
  [CEF_PERMISSION_TYPE_STORAGE_ACCESS] = "CEF_PERMISSION_TYPE_STORAGE_ACCESS",
  [CEF_PERMISSION_TYPE_VR_SESSION] = "CEF_PERMISSION_TYPE_VR_SESSION",
  [CEF_PERMISSION_TYPE_WINDOW_MANAGEMENT] = "CEF_PERMISSION_TYPE_WINDOW_MANAGEMENT",
  [CEF_PERMISSION_TYPE_FILE_SYSTEM_ACCESS] = "CEF_PERMISSION_TYPE_FILE_SYSTEM_ACCESS",
};

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

  uint32_t print_permissions = requested_permissions;
  uint32_t index = 0;
  while (print_permissions) {
    if (print_permissions & 1) printf("Requesting %s\n", CEF_PERMISSION_TYPE_NAMES[1U << index]);
    index++;
    print_permissions >>= 1;
  }
//  printf("on_show_permission_prompt %d %d\n", callback->base.has_one_ref(callback), callback->base.has_at_least_one_ref(callback));

  callback->cont(callback, CEF_PERMISSION_RESULT_ACCEPT);

  return 1;
}

void CEF_CALLBACK on_dismiss_permission_prompt(
    cef_permission_handler_t* self,
    cef_browser_t* browser,
    uint64_t prompt_id,
    cef_permission_request_result_t result) {
  printf("on_dismiss_permission_prompt\n");
}

//MID_DECLARE_CEF_REF_COUNT_CALLBACKS2(permission_handler, mxc_cef_permission_handler_t);

void initialize_cef_permission_handler_t(mxc_cef_permission_handler_t* handler) {
  printf("initialize_permission_handler_t\n");
  handler->cef.base.size = sizeof(mxc_cef_permission_handler_t);

//  MID_SET_CEF_REF_COUNT_CALLBACKS2(permission_handler, handler);

  handler->cef.on_request_media_access_permission = on_request_media_access_permission;
  handler->cef.on_show_permission_prompt = on_show_permission_prompt;
  handler->cef.on_dismiss_permission_prompt = on_dismiss_permission_prompt;
}
