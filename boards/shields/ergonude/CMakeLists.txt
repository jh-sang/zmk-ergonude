target_sources(app PRIVATE
    #custom_pinmux.c
)
target_compile_definitions(app PRIVATE BOARD_NRFMICRO_CHARGER=0)