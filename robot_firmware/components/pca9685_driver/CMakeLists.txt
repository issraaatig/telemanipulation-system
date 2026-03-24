idf_component_register(
    SRCS "src/pca9685_driver.cpp"
    INCLUDE_DIRS "include"
    REQUIRES driver common_utils common_config
)
