idf_component_register(
    SRCS "src/espnow_tx.cpp"
    INCLUDE_DIRS "include"
    REQUIRES esp_wifi esp_event control_packet common_config
)
