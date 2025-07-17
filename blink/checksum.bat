c:\ST\STM32CubeIDE_1.1.0\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.7-2018-q2-update.win32_1.0.0.201904181610\tools\arm-none-eabi\bin\objcopy.exe  -O ihex  blink.elf  "blink.hex"
..\srec_cat.exe blink.hex -Intel -fill 0xFF 0x08004000 -maximum-address blink.hex -Intel -o blink.hex -Intel 
..\srec_cat.exe blink.hex -Intel -STM32 -maximum-address blink.hex -Intel -o blink.hex -Intel 
..\srec_cat.exe blink.hex -Intel -offset - -minimum-addr blink.hex -Intel  -o blink.bin -Binary




::c:\ST\STM32CubeIDE_1.1.0\STM32CubeIDE\plugins\com.st.stm32cube.ide.mcu.externaltools.gnu-tools-for-stm32.7-2018-q2-update.win32_1.0.0.201904181610\tools\arm-none-eabi\bin\obj::copy.exe  -O ihex  blink.elf  "blink.hex"
::..\srec_cat.exe blink.hex -Intel -fill 0xFF 0x08004000 -maximum-address blink.hex -Intel -o blink.hex -Intel 
::..\srec_cat.exe blink.hex -Intel -STM32 -maximum-address blink.hex -Intel -o blink.hex -Intel 
::::..\srec_cat.exe blink.hex -Intel -o main.bin -Binary
::..\srec_cat.exe blink.hex -Intel -offset - -minimum-addr blink.hex -Intel  -o main.bin -Binary


::srec_cat fred.hex -o fred.bin -binary
::srec_cat fred.hex -offset - -minimum-addr fred.hex ?o fred.bin
