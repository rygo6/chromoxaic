#pragma once

#include "include/capi/cef_base_capi.h"
#include <stdio.h>
#include <unistd.h>

static inline void ref_print(const char* prefix, const char* function, int ref_count) {
//  printf("%s%s=%d ", prefix, function, ref_count);
}

#define DEBUG_CALLBACK(x)      \
  {                            \
    static int first_call = 1; \
    if (first_call == 1) {     \
      first_call = 0;          \
      printf(x);               \
    }                          \
  }

#define CEF_REF_CALLBACKS(prefix, type)                               \
  static void CEF_CALLBACK prefix##_add_ref(type* self) {             \
    __atomic_fetch_add(&self->ref_count, 1, __ATOMIC_RELAXED);        \
    ref_print(#prefix, "_add_ref=%d ", self->ref_count);              \
  }                                                                   \
  static int CEF_CALLBACK prefix##_release(type* self) {              \
    __atomic_fetch_sub(&self->ref_count, 1, __ATOMIC_RELAXED);        \
    ref_print(#prefix, "_release=%d ", self->ref_count);              \
    return 1;                                                         \
  }                                                                   \
  static int CEF_CALLBACK prefix##_has_one_ref(type* self) {          \
    int val;                                                          \
    __atomic_load(&self->ref_count, &val, __ATOMIC_RELAXED);          \
    ref_print(#prefix, "_has_one_ref=%d ", val);                      \
    __builtin_trap();                                                 \
  }                                                                   \
  static int CEF_CALLBACK prefix##_has_at_least_one_ref(type* self) { \
    int val;                                                          \
    __atomic_load(&self->ref_count, &val, __ATOMIC_RELAXED);          \
    ref_print(#prefix, "_has_at_least_one_ref=%d ", val);             \
    __builtin_trap();                                                 \
  }

#define CEF_SET_REF_CALLBACKS(prefix, type)                                                                         \
  {                                                                                                                 \
    type->cef.base.add_ref = (void (*)(struct _cef_base_ref_counted_t*)) prefix##_add_ref;                          \
    type->cef.base.release = (int (*)(struct _cef_base_ref_counted_t*)) prefix##_release;                           \
    type->cef.base.has_one_ref = (int (*)(struct _cef_base_ref_counted_t*)) prefix##_has_one_ref;                   \
    type->cef.base.has_at_least_one_ref = (int (*)(struct _cef_base_ref_counted_t*)) prefix##_has_at_least_one_ref; \
  }