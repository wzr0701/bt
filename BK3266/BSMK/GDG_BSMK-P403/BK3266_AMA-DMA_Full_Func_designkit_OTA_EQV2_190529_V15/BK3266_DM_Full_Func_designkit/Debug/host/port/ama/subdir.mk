################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../host/port/ama/accessories.pb-c.c \
../host/port/ama/calling.pb-c.c \
../host/port/ama/central.pb-c.c \
../host/port/ama/common.pb-c.c \
../host/port/ama/device.pb-c.c \
../host/port/ama/dueros_ama.c \
../host/port/ama/media.pb-c.c \
../host/port/ama/speech.pb-c.c \
../host/port/ama/state.pb-c.c \
../host/port/ama/system.pb-c.c \
../host/port/ama/transport.pb-c.c 

OBJS += \
./host/port/ama/accessories.pb-c.o \
./host/port/ama/calling.pb-c.o \
./host/port/ama/central.pb-c.o \
./host/port/ama/common.pb-c.o \
./host/port/ama/device.pb-c.o \
./host/port/ama/dueros_ama.o \
./host/port/ama/media.pb-c.o \
./host/port/ama/speech.pb-c.o \
./host/port/ama/state.pb-c.o \
./host/port/ama/system.pb-c.o \
./host/port/ama/transport.pb-c.o 

C_DEPS += \
./host/port/ama/accessories.pb-c.d \
./host/port/ama/calling.pb-c.d \
./host/port/ama/central.pb-c.d \
./host/port/ama/common.pb-c.d \
./host/port/ama/device.pb-c.d \
./host/port/ama/dueros_ama.d \
./host/port/ama/media.pb-c.d \
./host/port/ama/speech.pb-c.d \
./host/port/ama/state.pb-c.d \
./host/port/ama/system.pb-c.d \
./host/port/ama/transport.pb-c.d 


# Each subdirectory must supply rules for building sources it contributes
host/port/ama/%.o: ../host/port/ama/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: BA ELF GNU C compiler'
	ba-elf-gcc -DBA22_DEee -DHAVE_CONFIG_H -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\host\include" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\host\bluetooth\protocol\avctp" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\host\bluetooth\protocol\avdtp" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\controller\core\le\include" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\host\libs\FatLib" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\host\port\include\os" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\host\libs\Mp3Lib" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\host" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\host\bluetooth" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\host\bluetooth\include" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\host\config" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\host\jos" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\host\pkg\sbc" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\host\jos\include" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\host\port" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\host\port\beken_app" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\host\port\beken_driver" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\host\port\beken_no_os" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\host\port\common" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\host\port\include" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\host\port\common\bluetooth" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\host\port\include\bluetooth" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\controller\core\bt\include" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\controller\core\hc\include" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\controller\core\lc\include" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\controller\core\lc\dl\include" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\controller\core\lc\uslc\include" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\controller\core\lc\lslc\include" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\controller\core\lmp\include" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\controller\core\sys\include" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\controller\core\tc\include" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\controller\core\transport\include" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\controller\core\hw\include" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\controller\hal\hw\include" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\controller\hal\hw\radio\include" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\controller\hal\beken\hw\include" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\controller\hal\beken\sys\include" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\host\port\usb\include" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\host\port\usb\include\class" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\host\port\usb\src\systems" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\host\port\usb\src\systems\none" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\host\port\usb\src\systems\none\afs" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\host\port\usb\src\drivers\msd" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\host\port\opus\include" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\host\port\dma" -I"E:\����Ŀ¼\bk3266\dam-ama\new\BK3266_AMA-DMA_Full_Func_designkit_181110-OTA_V1\BK3266_DM_Full_Func_designkit\host\port\ama" -O1 -flto -G 100000 -g3 -Wall -c -fmessage-length=0  -mno-hard-float -ffast-math -march=ba2 -mle -mabi=3 -mtj-rodata -msingle-float -mdsp -mmac -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

