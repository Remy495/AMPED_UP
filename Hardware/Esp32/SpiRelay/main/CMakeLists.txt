
idf_component_register(
        SRCS "main.cpp" "SpiManager.cpp" "BluetoothManager.cpp" "CircularQueue.cpp"
        INCLUDE_DIRS "${CMAKE_CURRENT_BINARY_DIR}" "../../../Common")

target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++17)