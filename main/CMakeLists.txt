cmake_minimum_required(VERSION 3.20)

idf_component_register(
  SRCS
  "app_main.cpp"
  PRIV_INCLUDE_DIRS
  "."
  "../apps/AbstractionLayer/inc"
  "../apps/GeLib/inc"
  "../apps/Trace/inc"
  "../apps/ring-buffer/inc"
  PRIV_REQUIRES
  osal)

set_property(TARGET ${COMPONENT_LIB} PROPERTY CXX_STANDARD 20)
target_compile_options(${COMPONENT_LIB} PRIVATE "-DCHIP_HAVE_CONFIG_H")
