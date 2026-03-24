idf_component_register(
    SRCS "app_main.cpp"
    INCLUDE_DIRS "."
    REQUIRES robot_app common_config esp_wifi esp_event nvs_flash
)
