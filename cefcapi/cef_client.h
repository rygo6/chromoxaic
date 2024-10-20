#pragma once

#include "cef_base.h"
#include "cef_life_span_handler.h"
#include "cef_permission_handler.h"
#include "cef_render_handler.h"
#include "include/capi/cef_client_capi.h"

typedef struct mxc_cef_client_t {
  cef_client_t cef;
  int ref_count;
} mxc_cef_client_t;

cef_context_menu_handler_t* CEF_CALLBACK get_context_menu_handler(cef_client_t* self) {
  DEBUG_CALLBACK("get_context_menu_handler\n");
  return NULL;
}

cef_dialog_handler_t* CEF_CALLBACK get_dialog_handler(cef_client_t* self) {
  DEBUG_CALLBACK("get_dialog_handler\n");
  return NULL;
}

cef_display_handler_t* CEF_CALLBACK get_display_handler(cef_client_t* self) {
  DEBUG_CALLBACK("get_display_handler\n");
  return NULL;
}

cef_download_handler_t* CEF_CALLBACK get_download_handler(cef_client_t* self) {
  DEBUG_CALLBACK("get_download_handler\n");
  return NULL;
}

cef_drag_handler_t* CEF_CALLBACK get_drag_handler(cef_client_t* self) {
  DEBUG_CALLBACK("get_drag_handler\n");
  return NULL;
}

cef_focus_handler_t* CEF_CALLBACK get_focus_handler(cef_client_t* self) {
  DEBUG_CALLBACK("get_focus_handler\n");
  return NULL;
}

cef_permission_handler_t* CEF_CALLBACK get_permission_handler(cef_client_t* self) {
  DEBUG_CALLBACK("get_geolocation_handler\n");
  return &g_permission_handler.cef;
}

cef_jsdialog_handler_t* CEF_CALLBACK get_jsdialog_handler(cef_client_t* self) {
  DEBUG_CALLBACK("get_jsdialog_handler\n");
  return NULL;
}

cef_keyboard_handler_t* CEF_CALLBACK get_keyboard_handler(cef_client_t* self) {
  DEBUG_CALLBACK("get_keyboard_handler\n");
  return NULL;
}

cef_life_span_handler_t* CEF_CALLBACK get_life_span_handler(cef_client_t* self) {
  DEBUG_CALLBACK("get_life_span_handler\n");
  return &g_life_span_handler.cef;
}

cef_load_handler_t* CEF_CALLBACK get_load_handler(cef_client_t* self) {
  DEBUG_CALLBACK("get_load_handler\n");
  return NULL;
}

cef_render_handler_t* CEF_CALLBACK get_render_handler(cef_client_t* self) {
  DEBUG_CALLBACK("get_render_handler\n");
  return &g_render_handler.cef;
}

cef_request_handler_t* CEF_CALLBACK get_request_handler(cef_client_t* self) {
  DEBUG_CALLBACK("get_request_handler\n");
  return NULL;
}

int CEF_CALLBACK on_process_message_received(
    cef_client_t* self,
    cef_browser_t* browser,
    cef_frame_t* frame,
    cef_process_id_t source_process,
    cef_process_message_t* message) {
  DEBUG_CALLBACK("on_process_message_received\n");
  return 0;
}

MID_DECLARE_CEF_REF_COUNT_CALLBACKS2(client, mxc_cef_client_t);

void initialize_cef_client(mxc_cef_client_t* client) {
  printf("initialize_client_handler\n");

  client->cef.base.size = sizeof(mxc_cef_client_t);

  MID_SET_CEF_REF_COUNT_CALLBACKS2(client, client);

  client->cef.get_context_menu_handler = get_context_menu_handler;
  client->cef.get_dialog_handler = get_dialog_handler;
  client->cef.get_display_handler = get_display_handler;
  client->cef.get_download_handler = get_download_handler;
  client->cef.get_drag_handler = get_drag_handler;
  client->cef.get_focus_handler = get_focus_handler;
  client->cef.get_permission_handler = get_permission_handler;
  client->cef.get_jsdialog_handler = get_jsdialog_handler;
  client->cef.get_keyboard_handler = get_keyboard_handler;
  client->cef.get_life_span_handler = get_life_span_handler;// Implemented!
  client->cef.get_load_handler = get_load_handler;
  client->cef.get_render_handler = get_render_handler;
  client->cef.get_request_handler = get_request_handler;
  client->cef.on_process_message_received = on_process_message_received;
}
