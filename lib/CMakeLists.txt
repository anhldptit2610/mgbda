project(gbc)

add_library(${PROJECT_NAME} SHARED src/cpu.c
                                   src/gb.c
                                   src/rom.c
                                   src/mmu.c)
target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include/)