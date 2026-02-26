set(PROJECT_TYPE EXECUTABLE)

# Order depends on dependencies of modules
add_subdirectory(AbstractionLayer)
add_subdirectory(GeLib)
add_subdirectory(Trace)
add_subdirectory(ring-buffer)