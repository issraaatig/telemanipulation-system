idf_component_register(
    SRCS "src/sensor_reader.cpp"
    INCLUDE_DIRS "include"
    REQUIRES common_config esp_adc driver
)

# Keep legacy ADC warnings visible while preventing build failure under -Werror.
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-error=cpp)
