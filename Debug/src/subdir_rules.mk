################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
src/A_Wellth_Main.obj: ../src/A_Wellth_Main.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/home/preetham/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/bin/armcl" -mv7M3 --code_state=16 -me --include_path="/home/preetham/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/include" --include_path="/home/preetham/workspace_v6_1_3/Modbus-Slave/F28M35H52C_System" --include_path="/home/preetham/workspace_v6_1_3/Modbus-Slave/inc" -g --gcc --define=ccs --define=__GNUC__ --diag_wrap=off --display_error_number --diag_warning=225 --gen_func_subsections=on --abi=eabi --ual --preproc_with_compile --preproc_dependency="src/A_Wellth_Main.d" --obj_directory="src" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

src/ModeBus_Interface.obj: ../src/ModeBus_Interface.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/home/preetham/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/bin/armcl" -mv7M3 --code_state=16 -me --include_path="/home/preetham/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/include" --include_path="/home/preetham/workspace_v6_1_3/Modbus-Slave/F28M35H52C_System" --include_path="/home/preetham/workspace_v6_1_3/Modbus-Slave/inc" -g --gcc --define=ccs --define=__GNUC__ --diag_wrap=off --display_error_number --diag_warning=225 --gen_func_subsections=on --abi=eabi --ual --preproc_with_compile --preproc_dependency="src/ModeBus_Interface.d" --obj_directory="src" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

src/UART_Interface.obj: ../src/UART_Interface.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/home/preetham/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/bin/armcl" -mv7M3 --code_state=16 -me --include_path="/home/preetham/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/include" --include_path="/home/preetham/workspace_v6_1_3/Modbus-Slave/F28M35H52C_System" --include_path="/home/preetham/workspace_v6_1_3/Modbus-Slave/inc" -g --gcc --define=ccs --define=__GNUC__ --diag_wrap=off --display_error_number --diag_warning=225 --gen_func_subsections=on --abi=eabi --ual --preproc_with_compile --preproc_dependency="src/UART_Interface.d" --obj_directory="src" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '

src/UART_Interrupt.obj: ../src/UART_Interrupt.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"/home/preetham/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/bin/armcl" -mv7M3 --code_state=16 -me --include_path="/home/preetham/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/include" --include_path="/home/preetham/workspace_v6_1_3/Modbus-Slave/F28M35H52C_System" --include_path="/home/preetham/workspace_v6_1_3/Modbus-Slave/inc" -g --gcc --define=ccs --define=__GNUC__ --diag_wrap=off --display_error_number --diag_warning=225 --gen_func_subsections=on --abi=eabi --ual --preproc_with_compile --preproc_dependency="src/UART_Interrupt.d" --obj_directory="src" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: $<'
	@echo ' '


