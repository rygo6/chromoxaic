#pragma once

#include "cef_base.h"
#include "include/capi/cef_render_handler_capi.h"

typedef struct mxc_cef_render_handler_t {
  cef_render_handler_t cef;
  int ref_count;
} mxc_cef_render_handler_t;

extern mxc_cef_render_handler_t g_cef_render_handler;

void CEF_CALLBACK get_view_rect(struct _cef_render_handler_t* self,
                                struct _cef_browser_t* browser,
                                cef_rect_t* rect) {
  printf("get_view_rect\n");
  rect->width = 1024;
  rect->height = 1024;
}

void CEF_CALLBACK on_accelerated_paint(
    struct _cef_render_handler_t* self,
    struct _cef_browser_t* browser,
    cef_paint_element_type_t type,
    size_t dirtyRectsCount,
    cef_rect_t const* dirtyRects,
    const cef_accelerated_paint_info_t* info) {
  printf("on_accelerated_paint\n");
}

CEF_REF_CALLBACKS(render_handler, mxc_cef_render_handler_t);

void initialize_cef_render_handler(mxc_cef_render_handler_t* render_handler) {
  printf("initialize_cef_render_handler\n");
  CEF_SET_REF_CALLBACKS(render_handler, render_handler);
  render_handler->cef.base.size = sizeof(mxc_cef_render_handler_t);
  render_handler->cef.get_view_rect = get_view_rect;
  render_handler->cef.on_accelerated_paint = on_accelerated_paint;
}
