set(PROJECT_TYPE EXECUTABLE)

# Order depends on dependencies of modules
add_subdirectory(AbstractionLayer)
add_subdirectory(GeLib)
add_subdirectory(RingBuffer)