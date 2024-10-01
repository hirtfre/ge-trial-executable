# Components at this paths are added to the project by macro project(). It
# requires a CMakeLists.txt file in this component directory which calls the
# function idf_component_register. You can find an example in main folder. It is
# not possible to define an own compilation way at this point which is the
# reason why we add our libraries at the end of this file.
set(EXTRA_COMPONENT_DIRS
    "thirdparty/idf-extra-components"
    "osal"
    ${extra_components_dirs_append})

set(COMPONENTS
    main
    )

# Check if the build type is not Release
if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    list(APPEND COMPONENTS
        app_trace
        )
endif()

# Exclude  most components (dependencies will still be included!)
set(EXCLUDE_COMPONENTS
    app_update
    bootloader
    bootloader_support
    bt
    chip
    console
    cxx
    driver
    efuse
    esp-serial-flasher
    espcoredump
    esptool_py
    esp_app_format
    esp_bootloader_format
    esp_coex
    esp_common
    esp_encrypted_img
    esp_eth
    esp_event
    esp_gdbstub
    esp_https_ota
    esp_https_server
    esp_http_client
    esp_http_server
    esp_hw_support
    esp_lcd
    esp_local_ctrl
    esp_matter
    esp_mm
    esp_netif
    esp_netif_stack
    esp_partition
    esp_phy
    esp_pm
    esp_psram
    esp_ringbuf
    esp_rom
    esp_secure_cert_mgr
    esp_system
    esp_timer
    esp_wifi
    fatfs
    freertos
    hal
    heap
    ieee802154
    jsmn
    json
    json_generator
    json_parser
    log
    lwip
    newlib
    nvs_flash
    nvs_sec_provider
    openthread
    partition_table
    protobuf-c
    protocomm
    pthread
    riscv
    sdmmc
    soc
    spiffs
    spi_flash
    tcp_transport
    ulp
    unity
    vfs
    wear_levelling
    wifi_provisioning
    wpa_supplicant
    )