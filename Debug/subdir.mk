################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../MailServer.c \
../POP3Processor.c \
../POP3Server.c \
../SMTPProcessor.c \
../SMTPServer.c \
../Server.c \
../SqliteConnector.c \
../stringProcessor.c 

OBJS += \
./MailServer.o \
./POP3Processor.o \
./POP3Server.o \
./SMTPProcessor.o \
./SMTPServer.o \
./Server.o \
./SqliteConnector.o \
./stringProcessor.o 

C_DEPS += \
./MailServer.d \
./POP3Processor.d \
./POP3Server.d \
./SMTPProcessor.d \
./SMTPServer.d \
./Server.d \
./SqliteConnector.d \
./stringProcessor.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o"$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


