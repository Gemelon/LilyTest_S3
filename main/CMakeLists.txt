idf_component_register(
                SRCS    "main.c"
                        "ui/ui.c"
                        "ui/images.c"
                        "ui/screens.c"
                        "ui/styles.c"

                INCLUDE_DIRS 
                        "./ui"
                        "."

                REQUIRES 
                        esp_lcd
                )