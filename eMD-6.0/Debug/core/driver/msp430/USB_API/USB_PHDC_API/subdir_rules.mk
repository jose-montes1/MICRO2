################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
core/driver/msp430/USB_API/USB_PHDC_API/UsbPHDC.obj: ../core/driver/msp430/USB_API/USB_PHDC_API/UsbPHDC.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: MSP430 Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.3/bin/cl430" -vmspx --abi=coffabi --code_model=large --data_model=restricted --near_data=globals -O0 --opt_for_speed=5 -g --include_path="C:/ti/ccsv6/ccs_base/msp430/include" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-msp430_4.4.3/include" --include_path="//fs.ece.uprm.edu/students/jose.montes1/My Documents/MICRO2/eMD-6.0/core/driver/eMPL" --include_path="//fs.ece.uprm.edu/students/jose.montes1/My Documents/MICRO2/eMD-6.0/core/driver/include" --include_path="//fs.ece.uprm.edu/students/jose.montes1/My Documents/MICRO2/eMD-6.0/core/driver/msp430" --include_path="//fs.ece.uprm.edu/students/jose.montes1/My Documents/MICRO2/eMD-6.0/core/driver/msp430/F5xx_F6xx_Core_Lib" --include_path="//fs.ece.uprm.edu/students/jose.montes1/My Documents/MICRO2/eMD-6.0/core/driver/msp430/USB_API" --include_path="//fs.ece.uprm.edu/students/jose.montes1/My Documents/MICRO2/eMD-6.0/core/driver/msp430/USB_eMPL" --include_path="//fs.ece.uprm.edu/students/jose.montes1/My Documents/MICRO2/eMD-6.0/core/eMPL-hal" --include_path="//fs.ece.uprm.edu/students/jose.montes1/My Documents/MICRO2/eMD-6.0/core/mllite" --include_path="//fs.ece.uprm.edu/students/jose.montes1/My Documents/MICRO2/eMD-6.0/core/mpl" --include_path="//fs.ece.uprm.edu/students/jose.montes1/My Documents/MICRO2/eMD-6.0/simple_apps/msp430" --gcc --define=__MSP430F5528__ --define=USE_DMP --define=MPL_LOG_NDEBUG=1 --define=CONFIG_INTERFACE_USB --define=MPU9250 --define=I2C_B0 --define=EMPL --define=EMPL_TARGET_MSP430 --diag_warning=225 --display_error_number --silicon_errata=CPU21 --silicon_errata=CPU22 --silicon_errata=CPU23 --silicon_errata=CPU40 --printf_support=full --preproc_with_compile --preproc_dependency="core/driver/msp430/USB_API/USB_PHDC_API/UsbPHDC.pp" --obj_directory="core/driver/msp430/USB_API/USB_PHDC_API" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


