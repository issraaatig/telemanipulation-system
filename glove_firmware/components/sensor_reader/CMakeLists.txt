idf_component_register(
    SRCS "src/sensor_reader.cpp"
    INCLUDE_DIRS "include"
    REQUIRES common_config esp_adc driver
)
