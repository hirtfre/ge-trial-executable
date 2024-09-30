# Introduction
This `OSAL` (Operating System Abstraction Layer) implementation provides an abstraction layer for different platform-specific APIs, including:

- [FreeRTOS](https://www.freertos.org/Documentation/00-Overview)
- [esp-idf](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
- [stdlib](https://cplusplus.com/reference/cstdlib/)

The OSAL allows the project to interact with hardware and operating system functionalities via the unified interface of the [AbstractionLayer](https://dev.azure.com/GriesserElectronicAG/IoT%20Aktor/_git/GE.IoT.AbstractionLayer), abstracting platform-specific differences. This makes it easier to port the code to different platforms or hardware in the future.

## Directory Structure
```
├── CMakeLists.txt          CMake build script
├── confi/                  Contains JSON config files and CMake scripts for generating peripheral headers
│   ├── *.json              Peripheral configuration files (GPIO, Timer, SPI, etc.)
│   ├── cmake               CMake scripts to generate config headers
│   └── headerTemplates     Header templates for peripheral configuration
├── esp-idf                 Espressif (esp-idf) specific implementations
├── FreeRTOS                FreeRTOS-specific OSAL implementations
├── stdlib/                 Standard library OSAL implementations
└── README.md               This documentation
```

# Build System
The OSAL uses CMake as the build system. Various CMake scripts are provided within the `config/cmake` directory to generate header files for the peripherals based on the JSON configuration files defined in the `config` directory.

Make sure to include the corresponding `GenerateXXXConfigHeader.cmake` script in your CMake configuration to correctly generate the peripheral headers.

# Peripheral Configuration
All peripheral configurations are defined in JSON files located inside the `config` folder. These configuration files are parsed during the build process, and corresponding headers are generated for use within the OSAL and the modules dependent of the [AbstractionLayer](https://dev.azure.com/GriesserElectronicAG/IoT%20Aktor/_git/GE.IoT.AbstractionLayer). The following sections provide details on the configuration files for each peripheral.

## GPIO
To configure GPIOs, edit the `GpioConfig.json` file in the config/ directory. The `GenerateGpioConfigHeader.cmake` script will convert this JSON file into a C header during the build process. Refer to the ESP-IDF Programming Guide for more information about GPIO configuration.

```json
{
  "name": "UpRelay",
  "pinNumber": "GPIO_NUM_11",
  "mode": "GPIO_MODE_INPUT_OUTPUT",
  "enablePullUp": "GPIO_PULLUP_DISABLE",
  "enablePullDown": "GPIO_PULLDOWN_DISABLE",
  "interruptType": "GPIO_INTR_DISABLE",
  "hysteresisFilter": "GPIO_HYS_SOFT_DISABLE",
  "isActiveLow": "false",
  "modules": ["MotorDriver"]
}
```

### Key fields
- name: Descriptive name for the GPIO.
- isActiveLow: Set to true if the pin is active low.
- modules: A list of modules that need access to this GPIO.

## Timer
To configure timers, edit the `TimerConfig.json` file in the `config` directory. Use the `GenerateTimerConfigHeader.cmake` script to convert the JSON configuration into the necessary headers. Refer to the [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/) for more details on timer configuration.

```json
{
    "name": "MotorRuntime",
    "clockSource": "GPTIMER_CLK_SRC_DEFAULT",
    "direction": "GPTIMER_COUNT_UP",
    "resolutionHz": "1000000",
    "interruptPriority": "3",
    "flags": "0",
    "modules": [
        "MotorDriver"
    ]
}
```

### Key fields
- name: Descriptive name for the timer.
- resolutionHz: Timer resolution in Hz (e.g., 1000000 for 1 microsecond resolution).
- interruptPriority: Priority level for the timer interrupt (0-4).

## SPI
To configure timers, edit the `SpiConfig.json` file in the `config` directory. Use the `GenerateSpiConfigHeader.cmake` script to convert the JSON configuration into the necessary headers. Refer to the [ESP-IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/) for more details on timer configuration.

```json
{
    {
        "name": "ADE7953",
        "host_device": "SPI2_HOST",
        "miso_io_num": 0,
        "mosi_io_num": 5,
        "sclk_io_num": 4,
        "quadwp_io_num": 1,
        "quadhd_io_num": 1,
        "max_transfer_sz": 32,
        "cs_ena_pretrans": 0,
        "cs_ena_posttrans": 0,
        "clock_speed_hz": "5000000",
        "mode": 0,
        "flags": 0,
        "spics_io_num": 1,
        "command_bits": 0,
        "address_bits": 0,
        "dummy_bits": 0,
        "queue_size": 1,
        "modules": ["ADE7953Driver"]
    }
}
```

### Key fields
- name: Descriptive name for the SPI.

## Clock Ouput
To configure Clock Outputs, edit the `ClockOutput.json` file in the `config` directory. Use the `GenerateClockOutputConfigHeader.cmake` script to convert the JSON configuration into the necessary headers. The OSAL utilizes the LED Control (LEDC) API to implement clock outputs, please refer to the [ESP_IDF Programming Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/ledc.html) for more information.

```json
{
    "name": "ADE7953",
    "speed_mode": "LEDC_LOW_SPEED_MODE",
    "timer_num": "LEDC_TIMER_0",
    "duty_resolution": "LEDC_TIMER_1_BIT",
    "freq_hz": 3580000,
    "clk_cfg": "LEDC_USE_PLL_DIV_CLK",
    "channel": "LEDC_CHANNEL_0",
    "intr_type": "LEDC_INTR_DISABLE",
    "gpio_num": 25,
    "duty": 0,
    "hpoint": 0,
    "modules": ["ADE7953Driver"]
}
```

### Key fields
- name: Descriptive name for the Clock Output.