// CEF C API example
// Project website: https://github.com/cztomczak/cefcapi

#pragma once

#include "cef_base.h"
#include "cef_life_span_handler.h"
#include "include/capi/cef_client_capi.h"

typedef struct mxc_cef_client_t {
  cef_client_t cef;
  int ref_count;
} mxc_cef_client_t;

///
// Return the handler for context menus. If no handler is
// provided the default implementation will be used.
///
struct _cef_context_menu_handler_t* CEF_CALLBACK get_context_menu_handler(
    struct _cef_client_t* self) {
  DEBUG_CALLBACK("get_context_menu_handler\n");
  return NULL;
}

///
// Return the handler for dialogs. If no handler is provided the default
// implementation will be used.
///
struct _cef_dialog_handler_t* CEF_CALLBACK get_dialog_handler(
    struct _cef_client_t* self) {
  DEBUG_CALLBACK("get_dialog_handler\n");
  return NULL;
}

///
// Return the handler for browser display state events.
///
struct _cef_display_handler_t* CEF_CALLBACK get_display_handler(
    struct _cef_client_t* self) {
  DEBUG_CALLBACK("get_display_handler\n");
  return NULL;
}

///
// Return the handler for download events. If no handler is returned downloads
// will not be allowed.
///
struct _cef_download_handler_t* CEF_CALLBACK get_download_handler(
    struct _cef_client_t* self) {
  DEBUG_CALLBACK("get_download_handler\n");
  return NULL;
}

///
// Return the handler for drag events.
///
struct _cef_drag_handler_t* CEF_CALLBACK get_drag_handler(
    struct _cef_client_t* self) {
  DEBUG_CALLBACK("get_drag_handler\n");
  return NULL;
}

///
// Return the handler for focus events.
///
struct _cef_focus_handler_t* CEF_CALLBACK get_focus_handler(
    struct _cef_client_t* self) {
  DEBUG_CALLBACK("get_focus_handler\n");
  return NULL;
}

///
// Return the handler for geolocation permissions requests. If no handler is
// provided geolocation access will be denied by default.
///
struct _cef_permission_handler_t* CEF_CALLBACK get_geolocation_handler(
    struct _cef_client_t* self) {
  DEBUG_CALLBACK("get_geolocation_handler\n");
  return NULL;
}

///
// Return the handler for JavaScript dialogs. If no handler is provided the
// default implementation will be used.
///
struct _cef_jsdialog_handler_t* CEF_CALLBACK get_jsdialog_handler(
    struct _cef_client_t* self) {
  DEBUG_CALLBACK("get_jsdialog_handler\n");
  return NULL;
}

///
// Return the handler for keyboard events.
///
struct _cef_keyboard_handler_t* CEF_CALLBACK get_keyboard_handler(
    struct _cef_client_t* self) {
  DEBUG_CALLBACK("get_keyboard_handler\n");
  return NULL;
}

///
// Return the handler for browser life span events.
///
struct _cef_life_span_handler_t* CEF_CALLBACK get_life_span_handler(
    struct _cef_client_t* self) {
  DEBUG_CALLBACK("get_life_span_handler\n");
  // Implemented!
  return &g_life_span_handler;
}

///
// Return the handler for browser load status events.
///
struct _cef_load_handler_t* CEF_CALLBACK get_load_handler(
    struct _cef_client_t* self) {
  DEBUG_CALLBACK("get_load_handler\n");
  return NULL;
}

///
// Return the handler for off-screen rendering events.
///
struct _cef_render_handler_t* CEF_CALLBACK get_render_handler(
    struct _cef_client_t* self) {
  DEBUG_CALLBACK("get_render_handler\n");
  return NULL;
}

///
// Return the handler for browser request events.
///
struct _cef_request_handler_t* CEF_CALLBACK get_request_handler(
    struct _cef_client_t* self) {
  DEBUG_CALLBACK("get_request_handler\n");
  return NULL;
}

///
// Called when a new message is received from a different process. Return true
// (1) if the message was handled or false (0) otherwise. Do not keep a
// reference to or attempt to access the message outside of this callback.
///
int CEF_CALLBACK on_process_message_received(
    struct _cef_client_t* self,
    struct _cef_browser_t* browser,
    struct _cef_frame_t* frame,
    cef_process_id_t source_process,
    struct _cef_process_message_t* message) {
  DEBUG_CALLBACK("on_process_message_received\n");
  return 0;
}

CEF_REF_CALLBACKS(client, mxc_cef_client_t);

void initialize_cef_client(mxc_cef_client_t* client) {
  printf("initialize_client_handler\n");

  client->cef.base.size = sizeof(cef_client_t);

  CEF_SET_REF_CALLBACKS(client, client);

  client->cef.get_context_menu_handler = get_context_menu_handler;
  client->cef.get_dialog_handler = get_dialog_handler;
  client->cef.get_display_handler = get_display_handler;
  client->cef.get_download_handler = get_download_handler;
  client->cef.get_drag_handler = get_drag_handler;
  client->cef.get_focus_handler = get_focus_handler;
  client->cef.get_permission_handler = get_geolocation_handler;
  client->cef.get_jsdialog_handler = get_jsdialog_handler;
  client->cef.get_keyboard_handler = get_keyboard_handler;
  client->cef.get_life_span_handler = get_life_span_handler;// Implemented!
  client->cef.get_load_handler = get_load_handler;
  client->cef.get_render_handler = get_render_handler;
  client->cef.get_request_handler = get_request_handler;
  client->cef.on_process_message_received = on_process_message_received;
}
