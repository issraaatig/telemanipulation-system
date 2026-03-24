idf_component_register(
    SRCS "src/glove_app.cpp"
    INCLUDE_DIRS "include"
    REQUIRES sensor_reader filter_pipeline calibration espnow_tx common_utils control_packet common_config
)
