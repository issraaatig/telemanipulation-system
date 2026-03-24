idf_component_register(
    SRCS "src/espnow_rx.cpp"
    INCLUDE_DIRS "include"
    REQUIRES esp_wifi esp_event packet_parser control_packet common_config
)
