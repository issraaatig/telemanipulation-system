idf_component_register(
    SRCS "src/continuous_mapper.cpp"
    INCLUDE_DIRS "include"
    REQUIRES pca9685_driver common_utils common_config control_packet
)
