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

#define MID_DECLARE_CEF_REF_COUNT_CALLBACKS2(type)                         \
  static void CEF_CALLBACK type##_add_ref(type##_t* self) {                \
    int count = __atomic_add_fetch(&self->ref_count, 1, __ATOMIC_RELAXED); \
    ref_print(#type, "_add_ref", count);                                   \
  }                                                                        \
  static int CEF_CALLBACK type##_release(type##_t* self) {                 \
    int count = __atomic_sub_fetch(&self->ref_count, 1, __ATOMIC_RELAXED); \
    ref_print(#type, "_release", count);                                   \
    return count == 0;                                                     \
  }                                                                        \
  static int CEF_CALLBACK type##_has_one_ref(type##_t* self) {             \
    int count;                                                             \
    __atomic_load(&self->ref_count, &count, __ATOMIC_RELAXED);             \
    ref_print(#type, "_has_one_ref", count);                               \
    return count == 1;                                                     \
  }                                                                        \
  static int CEF_CALLBACK type##_has_at_least_one_ref(type##_t* self) {    \
    int count;                                                             \
    __atomic_load(&self->ref_count, &count, __ATOMIC_RELAXED);             \
    ref_print(#type, "_has_at_least_one_ref", count);                      \
    return count >= 1;                                                     \
  }

#define MID_SET_CEF_REF_COUNT_CALLBACKS2(type)                                                                    \
  {                                                                                                               \
    type->cef.base.add_ref = (void (*)(struct _cef_base_ref_counted_t*)) type##_add_ref;                          \
    type->cef.base.release = (int (*)(struct _cef_base_ref_counted_t*)) type##_release;                           \
    type->cef.base.has_one_ref = (int (*)(struct _cef_base_ref_counted_t*)) type##_has_one_ref;                   \
    type->cef.base.has_at_least_one_ref = (int (*)(struct _cef_base_ref_counted_t*)) type##_has_at_least_one_ref; \
  }
