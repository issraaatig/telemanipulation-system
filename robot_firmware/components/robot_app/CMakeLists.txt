idf_component_register(
    SRCS "src/robot_app.cpp"
    INCLUDE_DIRS "include"
    REQUIRES espnow_rx continuous_mapper safety_manager fsr_feedback pca9685_driver common_config
)
