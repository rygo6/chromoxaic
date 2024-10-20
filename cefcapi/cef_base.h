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

#define MID_DECLARE_REF_COUNT_TYPE(type) \
  typedef struct mid_##type {            \
    cef_##type##_t cef;                  \
    int ref_count;                       \
  } mid_##type;                          \
  MID_DECLARE_REF_COUNT_CALLBACKS(type)

#define MID_DECLARE_REF_COUNT_CALLBACKS(type)                                                                     \
  static void CEF_CALLBACK type##_add_ref(mid_##type* self) {                                                     \
    int count = __atomic_add_fetch(&self->ref_count, 1, __ATOMIC_RELAXED);                                        \
    ref_print(#type, "_add_ref", count);                                                                          \
  }                                                                                                               \
  static int CEF_CALLBACK type##_release(mid_##type* self) {                                                      \
    int count = __atomic_sub_fetch(&self->ref_count, 1, __ATOMIC_RELAXED);                                        \
    ref_print(#type, "_release", count);                                                                          \
    return count == 0;                                                                                            \
  }                                                                                                               \
  static int CEF_CALLBACK type##_has_one_ref(mid_##type* self) {                                                  \
    int count;                                                                                                    \
    __atomic_load(&self->ref_count, &count, __ATOMIC_RELAXED);                                                    \
    ref_print(#type, "_has_one_ref", count);                                                                      \
    return count == 1;                                                                                            \
  }                                                                                                               \
  static int CEF_CALLBACK type##_has_at_least_one_ref(mid_##type* self) {                                         \
    int count;                                                                                                    \
    __atomic_load(&self->ref_count, &count, __ATOMIC_RELAXED);                                                    \
    ref_print(#type, "_has_at_least_one_ref", count);                                                             \
    return count >= 1;                                                                                            \
  }                                                                                                               \
                                                                                                                  \
  void mid_initialize_##type##_ref_count(mid_##type* type) {                                                      \
    printf("mid_initialize_cef_##type##_ref_count \n");                                                           \
    type->cef.base.add_ref = (void (*)(struct _cef_base_ref_counted_t*)) type##_add_ref;                          \
    type->cef.base.release = (int (*)(struct _cef_base_ref_counted_t*)) type##_release;                           \
    type->cef.base.has_one_ref = (int (*)(struct _cef_base_ref_counted_t*)) type##_has_one_ref;                   \
    type->cef.base.has_at_least_one_ref = (int (*)(struct _cef_base_ref_counted_t*)) type##_has_at_least_one_ref; \
    type->cef.base.size = sizeof(mid_##type);                                                                     \
  }


#define MID_DECLARE_CEF_REF_COUNT_CALLBACKS2(prefix, type)                 \
  static void CEF_CALLBACK prefix##_add_ref(type* self) {                  \
    int count = __atomic_add_fetch(&self->ref_count, 1, __ATOMIC_RELAXED); \
    ref_print(#prefix, "_add_ref", count);                                 \
  }                                                                        \
  static int CEF_CALLBACK prefix##_release(type* self) {                   \
    int count = __atomic_sub_fetch(&self->ref_count, 1, __ATOMIC_RELAXED); \
    ref_print(#prefix, "_release", count);                                 \
    return count == 0;                                                     \
  }                                                                        \
  static int CEF_CALLBACK prefix##_has_one_ref(type* self) {               \
    int count;                                                             \
    __atomic_load(&self->ref_count, &count, __ATOMIC_RELAXED);             \
    ref_print(#prefix, "_has_one_ref", count);                             \
    return count == 1;                                                     \
  }                                                                        \
  static int CEF_CALLBACK prefix##_has_at_least_one_ref(type* self) {      \
    int count;                                                             \
    __atomic_load(&self->ref_count, &count, __ATOMIC_RELAXED);             \
    ref_print(#prefix, "_has_at_least_one_ref", count);                    \
    return count >= 1;                                                     \
  }

#define MID_SET_CEF_REF_COUNT_CALLBACKS2(prefix, type)                                                              \
  {                                                                                                                 \
    type->cef.base.add_ref = (void (*)(struct _cef_base_ref_counted_t*)) prefix##_add_ref;                          \
    type->cef.base.release = (int (*)(struct _cef_base_ref_counted_t*)) prefix##_release;                           \
    type->cef.base.has_one_ref = (int (*)(struct _cef_base_ref_counted_t*)) prefix##_has_one_ref;                   \
    type->cef.base.has_at_least_one_ref = (int (*)(struct _cef_base_ref_counted_t*)) prefix##_has_at_least_one_ref; \
  }

