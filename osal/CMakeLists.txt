cmake_minimum_required(VERSION 3.20)

idf_component_register(
  SRCS
  "esp-idf/ClockOutput.cpp"
  "esp-idf/FileSystem.cpp"
  "esp-idf/Gpio.cpp"
  "esp-idf/Json.cpp"
  "esp-idf/KeyValueStore.cpp"
  "esp-idf/Spi.cpp"
  "esp-idf/Timer.cpp"
  "FreeRTOS/Task.cpp"
  "stdlib/stdlib.cpp"
  PRIV_INCLUDE_DIRS
  "."
  "../apps/AbstractionLayer/inc"
  "../apps/GeLib/inc"
  PRIV_REQUIRES
  json_parser
  spiffs
  driver
  nvs_flash
  esp_timer
  )
