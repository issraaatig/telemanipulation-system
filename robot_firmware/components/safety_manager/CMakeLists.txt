idf_component_register(
    SRCS "src/safety_manager.cpp"
    INCLUDE_DIRS "include"
    REQUIRES driver esp_timer pca9685_driver common_config
)
