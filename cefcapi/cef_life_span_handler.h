#pragma once

#include "cef_base.h"
#include "include/capi/cef_life_span_handler_capi.h"

typedef struct _life_span_handler_t {
  cef_life_span_handler_t cef;
  int ref_count;
} life_span_handler_t;

void CEF_CALLBACK on_after_created(struct _cef_life_span_handler_t* self,
                                   struct _cef_browser_t* browser) {
  printf("on_after_created\n");
}

void CEF_CALLBACK on_before_close(cef_life_span_handler_t* self, cef_browser_t* browser) {
  printf("on_before_close\n");
}

MID_DECLARE_CEF_REF_COUNT_CALLBACKS2(life_span_handler);

extern life_span_handler_t g_life_span_handler;
void initialize_cef_life_span_handler(life_span_handler_t* life_span_handler) {
  printf("initialize_cef_life_span_handler\n");
  life_span_handler->cef.base.size = sizeof(life_span_handler_t);

  MID_SET_CEF_REF_COUNT_CALLBACKS2(life_span_handler);

  life_span_handler->cef.on_after_created = on_before_close;
  life_span_handler->cef.on_before_close = on_before_close;
}
