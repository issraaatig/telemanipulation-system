idf_component_register(
    SRCS "src/calibration.cpp"
    INCLUDE_DIRS "include"
    REQUIRES nvs_flash common_utils common_config
)
