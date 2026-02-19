cmake_minimum_required(VERSION 3.20)

idf_component_register(
  SRCS
  "esp-idf/ClockOutput.cpp"
  "esp-idf/Gpio.cpp"
  "esp-idf/Timer.cpp"
  "RTOS/Task/Task.cpp"
  "RTOS/Task/Timed.cpp"
  "RTOS/Task/EventDriven.cpp"
  "RTOS/Task/Monitor.cpp"
  "stdlib/stdlib.cpp"
  PRIV_INCLUDE_DIRS
  "."
  "../apps/AbstractionLayer/inc"
  "../apps/GeLib/inc"
  "../apps/Trace/inc"
  PRIV_REQUIRES
  json_parser
  spiffs
  driver
  nvs_flash
  esp_timer
)
