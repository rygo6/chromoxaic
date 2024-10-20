#pragma once

#include "cef_base.h"
#include "include/capi/cef_life_span_handler_capi.h"

typedef struct mxc_cef_life_span_handler_t {
  cef_life_span_handler_t cef;
  int ref_count;
} mxc_cef_life_span_handler_t;

extern mxc_cef_life_span_handler_t g_life_span_handler;

void CEF_CALLBACK on_before_close(cef_life_span_handler_t* self, cef_browser_t* browser) {
  printf("on_before_close\n");
}

//MID_DECLARE_CEF_REF_COUNT_CALLBACKS2(life_span_handler, mxc_cef_life_span_handler_t);

void initialize_cef_life_span_handler(mxc_cef_life_span_handler_t* handler) {
  printf("initialize_cef_life_span_handler\n");
  handler->cef.base.size = sizeof(mxc_cef_life_span_handler_t);

//  MID_SET_CEF_REF_COUNT_CALLBACKS2(life_span_handler, handler);

  handler->cef.on_before_close = on_before_close;
}
