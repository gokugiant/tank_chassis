#pragma once
using esp_timer_handle_t = void*;

struct esp_timer_create_args_t {
  void (*callback)(void*);
  void* arg;
  const char* name;
};

constexpr int ESP_OK = 0;
extern esp_timer_create_args_t testTimer;

inline int esp_timer_create(esp_timer_create_args_t* a, esp_timer_handle_t* out) {
  testTimer = *a;
  *out = a;
  return 0;
}

inline int esp_timer_start_periodic(esp_timer_handle_t, uint64_t) {
  return 0;
}
