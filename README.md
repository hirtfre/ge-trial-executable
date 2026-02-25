# Getting Started

## Installation

After cloning `ge-trial-executable` you have to init and update the submodules with `git submodule update --init`.

To init all the dependent submodules you can run the VSCode task `Install esp-idf` or execute the following steps manually:

```shell
cd thirdparty/esp-idf
git submodule update --init --recursive
./install.sh
```

### Prerequisites

In order to build esp-idf the following modules may be required:

```shell
sudo apt-get install git wget flex bison gperf python3 python3-pip python3-venv cmake ninja-build ccache libffi-dev libssl-dev dfu-util libusb-1.0-0 gcc g++ pkg-config libdbus-1-dev libglib2.0-dev libavahi-client-dev python3-dev python3-pip unzip libgirepository1.0-dev libcairo2-dev libreadline-dev
```

## Project structure

The project is structured like follows:

```
ge-trial-executable
├── apps                        -> GE application layer
├── main                        -> Main program entry point
├── osal                        -> Operating system and hardware abstraction
└── thirdparty                  -> Vendor specific dependencies
    ├── esp-idf
    └── idf-extra-components
```

## Build project, flash and monitor device

To build project and flash device execute following VSCode tasks:

1. ESP-IDF: Build development
2. ESP-IDF: flash and monitor

## Add a new Module to Application Layer

To add a new Module to the Application Layer the following steps have to be performed:

1. Add git submodule

``` bash
git submodule add {repository} apps/{moduleName}
```

2. Add module to libraries to be linked
    - Insert `set(USED_LIBRARIES {moduleName})` to top level `CMakeLists.txt`

3. Add sub directory to Application Layer
    - Insert `add_subdirectory({moduleName})` to Application Layer `CMakeLists.txt`

4. Add include directories to app_main (if used)
    - Insert `"../apps/{moduleName}/inc"` to `PRIV_INCLUDE_DIRS` of app_main component
