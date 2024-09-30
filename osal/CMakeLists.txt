cmake_minimum_required(VERSION 3.20)

idf_component_register(
  SRCS
  "esp-idf/ClockOutput.cpp"
  "esp-idf/EspMatterDataModel.cpp"
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
  esp_matter
  ble
  json_parser
  spiffs
  )

# create SPIFFS partition in ESP32 flash and initialize with content located in
# "spiffs_image" directory.
spiffs_create_partition_image(storage ../spiffs_image FLASH_IN_PROJECT)

include(config/cmake/GenerateGpioConfigHeader.cmake)
generate_gpio_config_header()

include(config/cmake/GenerateTimerConfigHeader.cmake)
generate_timer_config_header()

include(config/cmake/GenerateSpiConfigHeader.cmake)
generate_spi_config_header()

include(config/cmake/GenerateClockOutputConfigHeader.cmake)
generate_clock_output_config_header()

include(config/cmake/GeneratePeripheralTypesHeader.cmake)
generate_peripheral_types_header(config/GpioConfig.json config/headerTemplates/GpioTypes.h.in GpioTypes.h gpios)
generate_peripheral_types_header(config/SpiConfig.json config/headerTemplates/SpiTypes.h.in SpiTypes.h spis)
generate_peripheral_types_header(config/TimerConfig.json config/headerTemplates/TimerTypes.h.in TimerTypes.h timers)
generate_peripheral_types_header(config/ClockOutputConfig.json config/headerTemplates/ClockOutputTypes.h.in ClockOutputTypes.h clockOutputs)