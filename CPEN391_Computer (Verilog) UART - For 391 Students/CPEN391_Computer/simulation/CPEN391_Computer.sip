set_global_assignment -entity "CPEN391_Computer" -library "lib_CPEN391_Computer" -name IP_TOOL_NAME "Qsys"
set_global_assignment -entity "CPEN391_Computer" -library "lib_CPEN391_Computer" -name IP_TOOL_VERSION "15.0"
set_global_assignment -entity "CPEN391_Computer" -library "lib_CPEN391_Computer" -name IP_TOOL_ENV "Qsys"
set_global_assignment -library "lib_CPEN391_Computer" -name SPD_FILE [file join $::quartus(sip_path) "../CPEN391_Computer.spd"]
set_global_assignment -library "lib_CPEN391_Computer" -name MISC_FILE [file join $::quartus(sip_path) "../../CPEN391_Computer.qsys"]

set_global_assignment -library "lib_CPEN391_Computer" -name MISC_FILE [file join $::quartus(sip_path) "CPEN391_Computer.v"]
set_global_assignment -library "lib_CPEN391_Computer" -name MISC_FILE [file join $::quartus(sip_path) "submodules/CPEN391_Computer_ARM_A9_HPS.v"]
