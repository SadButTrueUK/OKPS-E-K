#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/OKPS-EK.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/OKPS-EK.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=-mafrlcsj
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS
SUB_IMAGE_ADDRESS_COMMAND=--image-address $(SUB_IMAGE_ADDRESS)
else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../src/ActivityManager/ActivityManager.c ../src/ActivityManager/ActivityManager_connect.c ../src/ActivityManager/ActivityManager_internal.c ../src/ActivityManager/BlockExch.c ../src/AnalogInput/AnalogInput.c ../src/AnalogInput/Adc.c ../src/AnalogInput/DigitalFilter.c ../src/AnalogInput/dsPIC33_FIR_filter.s ../src/AnalogMeasurement/AnalogMeasurement.c ../src/BinIn/BinIn.c ../src/BinIn/BinInDecoder.s ../src/BlackBox/BlackBox.c ../src/CheckSupply/CheckSupply.c ../src/ConfigMK/ConfigMK.c ../src/DebugTools/Tracing.c ../src/DebugTools/DebugTools.c ../src/DeviceAddress/deviceAddress.c ../src/drv/turnOffdsPIC33.c ../src/drv/AdcDriver_PIC33.c ../src/drv/checkRegistersDrv.c ../src/drv/Ecan_driverPIC33.c ../src/drv/EcanAck_driverPIC33.c ../src/drv/EepromDrv.c ../src/drv/I2Cdrv_PIC33.c ../src/drv/dsPIC33_rs422.c ../src/drv/shuntShiftGenDrv.c ../src/drv/posDetGen_drv.c ../src/drv/relayCtrlDrv.c ../src/drv/BlockExch_driver.c ../src/Eeprom/Eeprom.c ../src/Fsm/Fsm.c ../src/FsmStates/Sheduler.c ../src/FsmStates/StateActive.c ../src/FsmStates/StateInitial.c ../src/FsmStates/StateMain.c ../src/FsmStates/StatePassive.c ../src/FsmStates/StateSafeActive.c ../src/FsmStates/StateSafePassive.c ../src/FsmStates/StateTop.c ../src/FsmStates/StateWorking.c ../src/HysteresisFilter/HystFltr.c ../src/Indication/Indication.c ../src/Indication/LedFailure.c ../src/Initial/CheckCauseReset.c ../src/Initial/IdentHex.c ../src/Initial/TimeBeginSynchronization.c ../src/IntegrCtrl/IntegrCtrl.c ../src/IntegrCtrl/IntegrCtrlGen.c ../src/InterChannel/InterChannel.c ../src/InterChannel/log2.c ../src/InterChannel/pwr2.c ../src/Main/ControlSystem.c ../src/Main/InterruptsHandlers.c ../src/Main/UnusedInterrupts.c ../src/ModeProtection/ModeProtection.c ../src/OverloadDet/OverloadDet.c ../src/PositionDet/posDetGenerator.c ../src/PositionDet/positionDetFiveEC.c ../src/PositionDet/positionDetNineWire.c ../src/PositionDet/positionDet.c ../src/RelayCtrl/relayCtrl.c ../src/RelayCtrl/relayCtrlFiveEC.c ../src/RelayCtrl/relayCtrlNineWire.c ../src/RS422/Rs422.c ../src/RS422/Rs422_crc8.c ../src/RS422/Rs422_lineExch.c ../src/RS422/Rs422_transceiver.c ../src/SafetyPowerControl/SafetyPowerControl.c ../src/ShuntShift/ShuntShiftGen.c ../src/ShuntShift/shuntShift.c ../src/ShuntShift/shuntShiftFiveEC.c ../src/ShuntShift/shuntShiftMotor.c ../src/ShuntShift/shuntShiftNineWire.c ../src/systems/asserts.c ../src/Testing/CheckCallFunctions.c ../src/Testing/CheckRAM.c ../src/Testing/CheckRegisters.c ../src/Testing/ControlMK.c ../src/Testing/CheckCPU.s ../src/Testing/FlashCheck.c ../src/Utility/SqrtTOpt.s ../src/Utility/wait.c ../src/Utility/CopySwap.c ../src/Main/Main.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/1813651324/ActivityManager.o ${OBJECTDIR}/_ext/1813651324/ActivityManager_connect.o ${OBJECTDIR}/_ext/1813651324/ActivityManager_internal.o ${OBJECTDIR}/_ext/1813651324/BlockExch.o ${OBJECTDIR}/_ext/1031660192/AnalogInput.o ${OBJECTDIR}/_ext/1031660192/Adc.o ${OBJECTDIR}/_ext/1031660192/DigitalFilter.o ${OBJECTDIR}/_ext/1031660192/dsPIC33_FIR_filter.o ${OBJECTDIR}/_ext/161780206/AnalogMeasurement.o ${OBJECTDIR}/_ext/1567386894/BinIn.o ${OBJECTDIR}/_ext/1567386894/BinInDecoder.o ${OBJECTDIR}/_ext/1119163098/BlackBox.o ${OBJECTDIR}/_ext/1684595197/CheckSupply.o ${OBJECTDIR}/_ext/633884614/ConfigMK.o ${OBJECTDIR}/_ext/1959551838/Tracing.o ${OBJECTDIR}/_ext/1959551838/DebugTools.o ${OBJECTDIR}/_ext/970708260/deviceAddress.o ${OBJECTDIR}/_ext/659853742/turnOffdsPIC33.o ${OBJECTDIR}/_ext/659853742/AdcDriver_PIC33.o ${OBJECTDIR}/_ext/659853742/checkRegistersDrv.o ${OBJECTDIR}/_ext/659853742/Ecan_driverPIC33.o ${OBJECTDIR}/_ext/659853742/EcanAck_driverPIC33.o ${OBJECTDIR}/_ext/659853742/EepromDrv.o ${OBJECTDIR}/_ext/659853742/I2Cdrv_PIC33.o ${OBJECTDIR}/_ext/659853742/dsPIC33_rs422.o ${OBJECTDIR}/_ext/659853742/shuntShiftGenDrv.o ${OBJECTDIR}/_ext/659853742/posDetGen_drv.o ${OBJECTDIR}/_ext/659853742/relayCtrlDrv.o ${OBJECTDIR}/_ext/659853742/BlockExch_driver.o ${OBJECTDIR}/_ext/1262060966/Eeprom.o ${OBJECTDIR}/_ext/659824934/Fsm.o ${OBJECTDIR}/_ext/1951852232/Sheduler.o ${OBJECTDIR}/_ext/1951852232/StateActive.o ${OBJECTDIR}/_ext/1951852232/StateInitial.o ${OBJECTDIR}/_ext/1951852232/StateMain.o ${OBJECTDIR}/_ext/1951852232/StatePassive.o ${OBJECTDIR}/_ext/1951852232/StateSafeActive.o ${OBJECTDIR}/_ext/1951852232/StateSafePassive.o ${OBJECTDIR}/_ext/1951852232/StateTop.o ${OBJECTDIR}/_ext/1951852232/StateWorking.o ${OBJECTDIR}/_ext/355834263/HystFltr.o ${OBJECTDIR}/_ext/706196810/Indication.o ${OBJECTDIR}/_ext/706196810/LedFailure.o ${OBJECTDIR}/_ext/962885590/CheckCauseReset.o ${OBJECTDIR}/_ext/962885590/IdentHex.o ${OBJECTDIR}/_ext/962885590/TimeBeginSynchronization.o ${OBJECTDIR}/_ext/2012282586/IntegrCtrl.o ${OBJECTDIR}/_ext/2012282586/IntegrCtrlGen.o ${OBJECTDIR}/_ext/1093587103/InterChannel.o ${OBJECTDIR}/_ext/1093587103/log2.o ${OBJECTDIR}/_ext/1093587103/pwr2.o ${OBJECTDIR}/_ext/1020072301/ControlSystem.o ${OBJECTDIR}/_ext/1020072301/InterruptsHandlers.o ${OBJECTDIR}/_ext/1020072301/UnusedInterrupts.o ${OBJECTDIR}/_ext/985603786/ModeProtection.o ${OBJECTDIR}/_ext/887427039/OverloadDet.o ${OBJECTDIR}/_ext/777634832/posDetGenerator.o ${OBJECTDIR}/_ext/777634832/positionDetFiveEC.o ${OBJECTDIR}/_ext/777634832/positionDetNineWire.o ${OBJECTDIR}/_ext/777634832/positionDet.o ${OBJECTDIR}/_ext/1670244350/relayCtrl.o ${OBJECTDIR}/_ext/1670244350/relayCtrlFiveEC.o ${OBJECTDIR}/_ext/1670244350/relayCtrlNineWire.o ${OBJECTDIR}/_ext/1553322471/Rs422.o ${OBJECTDIR}/_ext/1553322471/Rs422_crc8.o ${OBJECTDIR}/_ext/1553322471/Rs422_lineExch.o ${OBJECTDIR}/_ext/1553322471/Rs422_transceiver.o ${OBJECTDIR}/_ext/1645518876/SafetyPowerControl.o ${OBJECTDIR}/_ext/237575434/ShuntShiftGen.o ${OBJECTDIR}/_ext/237575434/shuntShift.o ${OBJECTDIR}/_ext/237575434/shuntShiftFiveEC.o ${OBJECTDIR}/_ext/237575434/shuntShiftMotor.o ${OBJECTDIR}/_ext/237575434/shuntShiftNineWire.o ${OBJECTDIR}/_ext/2018284246/asserts.o ${OBJECTDIR}/_ext/38706442/CheckCallFunctions.o ${OBJECTDIR}/_ext/38706442/CheckRAM.o ${OBJECTDIR}/_ext/38706442/CheckRegisters.o ${OBJECTDIR}/_ext/38706442/ControlMK.o ${OBJECTDIR}/_ext/38706442/CheckCPU.o ${OBJECTDIR}/_ext/38706442/FlashCheck.o ${OBJECTDIR}/_ext/1268761170/SqrtTOpt.o ${OBJECTDIR}/_ext/1268761170/wait.o ${OBJECTDIR}/_ext/1268761170/CopySwap.o ${OBJECTDIR}/_ext/1020072301/Main.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/1813651324/ActivityManager.o.d ${OBJECTDIR}/_ext/1813651324/ActivityManager_connect.o.d ${OBJECTDIR}/_ext/1813651324/ActivityManager_internal.o.d ${OBJECTDIR}/_ext/1813651324/BlockExch.o.d ${OBJECTDIR}/_ext/1031660192/AnalogInput.o.d ${OBJECTDIR}/_ext/1031660192/Adc.o.d ${OBJECTDIR}/_ext/1031660192/DigitalFilter.o.d ${OBJECTDIR}/_ext/1031660192/dsPIC33_FIR_filter.o.d ${OBJECTDIR}/_ext/161780206/AnalogMeasurement.o.d ${OBJECTDIR}/_ext/1567386894/BinIn.o.d ${OBJECTDIR}/_ext/1567386894/BinInDecoder.o.d ${OBJECTDIR}/_ext/1119163098/BlackBox.o.d ${OBJECTDIR}/_ext/1684595197/CheckSupply.o.d ${OBJECTDIR}/_ext/633884614/ConfigMK.o.d ${OBJECTDIR}/_ext/1959551838/Tracing.o.d ${OBJECTDIR}/_ext/1959551838/DebugTools.o.d ${OBJECTDIR}/_ext/970708260/deviceAddress.o.d ${OBJECTDIR}/_ext/659853742/turnOffdsPIC33.o.d ${OBJECTDIR}/_ext/659853742/AdcDriver_PIC33.o.d ${OBJECTDIR}/_ext/659853742/checkRegistersDrv.o.d ${OBJECTDIR}/_ext/659853742/Ecan_driverPIC33.o.d ${OBJECTDIR}/_ext/659853742/EcanAck_driverPIC33.o.d ${OBJECTDIR}/_ext/659853742/EepromDrv.o.d ${OBJECTDIR}/_ext/659853742/I2Cdrv_PIC33.o.d ${OBJECTDIR}/_ext/659853742/dsPIC33_rs422.o.d ${OBJECTDIR}/_ext/659853742/shuntShiftGenDrv.o.d ${OBJECTDIR}/_ext/659853742/posDetGen_drv.o.d ${OBJECTDIR}/_ext/659853742/relayCtrlDrv.o.d ${OBJECTDIR}/_ext/659853742/BlockExch_driver.o.d ${OBJECTDIR}/_ext/1262060966/Eeprom.o.d ${OBJECTDIR}/_ext/659824934/Fsm.o.d ${OBJECTDIR}/_ext/1951852232/Sheduler.o.d ${OBJECTDIR}/_ext/1951852232/StateActive.o.d ${OBJECTDIR}/_ext/1951852232/StateInitial.o.d ${OBJECTDIR}/_ext/1951852232/StateMain.o.d ${OBJECTDIR}/_ext/1951852232/StatePassive.o.d ${OBJECTDIR}/_ext/1951852232/StateSafeActive.o.d ${OBJECTDIR}/_ext/1951852232/StateSafePassive.o.d ${OBJECTDIR}/_ext/1951852232/StateTop.o.d ${OBJECTDIR}/_ext/1951852232/StateWorking.o.d ${OBJECTDIR}/_ext/355834263/HystFltr.o.d ${OBJECTDIR}/_ext/706196810/Indication.o.d ${OBJECTDIR}/_ext/706196810/LedFailure.o.d ${OBJECTDIR}/_ext/962885590/CheckCauseReset.o.d ${OBJECTDIR}/_ext/962885590/IdentHex.o.d ${OBJECTDIR}/_ext/962885590/TimeBeginSynchronization.o.d ${OBJECTDIR}/_ext/2012282586/IntegrCtrl.o.d ${OBJECTDIR}/_ext/2012282586/IntegrCtrlGen.o.d ${OBJECTDIR}/_ext/1093587103/InterChannel.o.d ${OBJECTDIR}/_ext/1093587103/log2.o.d ${OBJECTDIR}/_ext/1093587103/pwr2.o.d ${OBJECTDIR}/_ext/1020072301/ControlSystem.o.d ${OBJECTDIR}/_ext/1020072301/InterruptsHandlers.o.d ${OBJECTDIR}/_ext/1020072301/UnusedInterrupts.o.d ${OBJECTDIR}/_ext/985603786/ModeProtection.o.d ${OBJECTDIR}/_ext/887427039/OverloadDet.o.d ${OBJECTDIR}/_ext/777634832/posDetGenerator.o.d ${OBJECTDIR}/_ext/777634832/positionDetFiveEC.o.d ${OBJECTDIR}/_ext/777634832/positionDetNineWire.o.d ${OBJECTDIR}/_ext/777634832/positionDet.o.d ${OBJECTDIR}/_ext/1670244350/relayCtrl.o.d ${OBJECTDIR}/_ext/1670244350/relayCtrlFiveEC.o.d ${OBJECTDIR}/_ext/1670244350/relayCtrlNineWire.o.d ${OBJECTDIR}/_ext/1553322471/Rs422.o.d ${OBJECTDIR}/_ext/1553322471/Rs422_crc8.o.d ${OBJECTDIR}/_ext/1553322471/Rs422_lineExch.o.d ${OBJECTDIR}/_ext/1553322471/Rs422_transceiver.o.d ${OBJECTDIR}/_ext/1645518876/SafetyPowerControl.o.d ${OBJECTDIR}/_ext/237575434/ShuntShiftGen.o.d ${OBJECTDIR}/_ext/237575434/shuntShift.o.d ${OBJECTDIR}/_ext/237575434/shuntShiftFiveEC.o.d ${OBJECTDIR}/_ext/237575434/shuntShiftMotor.o.d ${OBJECTDIR}/_ext/237575434/shuntShiftNineWire.o.d ${OBJECTDIR}/_ext/2018284246/asserts.o.d ${OBJECTDIR}/_ext/38706442/CheckCallFunctions.o.d ${OBJECTDIR}/_ext/38706442/CheckRAM.o.d ${OBJECTDIR}/_ext/38706442/CheckRegisters.o.d ${OBJECTDIR}/_ext/38706442/ControlMK.o.d ${OBJECTDIR}/_ext/38706442/CheckCPU.o.d ${OBJECTDIR}/_ext/38706442/FlashCheck.o.d ${OBJECTDIR}/_ext/1268761170/SqrtTOpt.o.d ${OBJECTDIR}/_ext/1268761170/wait.o.d ${OBJECTDIR}/_ext/1268761170/CopySwap.o.d ${OBJECTDIR}/_ext/1020072301/Main.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/1813651324/ActivityManager.o ${OBJECTDIR}/_ext/1813651324/ActivityManager_connect.o ${OBJECTDIR}/_ext/1813651324/ActivityManager_internal.o ${OBJECTDIR}/_ext/1813651324/BlockExch.o ${OBJECTDIR}/_ext/1031660192/AnalogInput.o ${OBJECTDIR}/_ext/1031660192/Adc.o ${OBJECTDIR}/_ext/1031660192/DigitalFilter.o ${OBJECTDIR}/_ext/1031660192/dsPIC33_FIR_filter.o ${OBJECTDIR}/_ext/161780206/AnalogMeasurement.o ${OBJECTDIR}/_ext/1567386894/BinIn.o ${OBJECTDIR}/_ext/1567386894/BinInDecoder.o ${OBJECTDIR}/_ext/1119163098/BlackBox.o ${OBJECTDIR}/_ext/1684595197/CheckSupply.o ${OBJECTDIR}/_ext/633884614/ConfigMK.o ${OBJECTDIR}/_ext/1959551838/Tracing.o ${OBJECTDIR}/_ext/1959551838/DebugTools.o ${OBJECTDIR}/_ext/970708260/deviceAddress.o ${OBJECTDIR}/_ext/659853742/turnOffdsPIC33.o ${OBJECTDIR}/_ext/659853742/AdcDriver_PIC33.o ${OBJECTDIR}/_ext/659853742/checkRegistersDrv.o ${OBJECTDIR}/_ext/659853742/Ecan_driverPIC33.o ${OBJECTDIR}/_ext/659853742/EcanAck_driverPIC33.o ${OBJECTDIR}/_ext/659853742/EepromDrv.o ${OBJECTDIR}/_ext/659853742/I2Cdrv_PIC33.o ${OBJECTDIR}/_ext/659853742/dsPIC33_rs422.o ${OBJECTDIR}/_ext/659853742/shuntShiftGenDrv.o ${OBJECTDIR}/_ext/659853742/posDetGen_drv.o ${OBJECTDIR}/_ext/659853742/relayCtrlDrv.o ${OBJECTDIR}/_ext/659853742/BlockExch_driver.o ${OBJECTDIR}/_ext/1262060966/Eeprom.o ${OBJECTDIR}/_ext/659824934/Fsm.o ${OBJECTDIR}/_ext/1951852232/Sheduler.o ${OBJECTDIR}/_ext/1951852232/StateActive.o ${OBJECTDIR}/_ext/1951852232/StateInitial.o ${OBJECTDIR}/_ext/1951852232/StateMain.o ${OBJECTDIR}/_ext/1951852232/StatePassive.o ${OBJECTDIR}/_ext/1951852232/StateSafeActive.o ${OBJECTDIR}/_ext/1951852232/StateSafePassive.o ${OBJECTDIR}/_ext/1951852232/StateTop.o ${OBJECTDIR}/_ext/1951852232/StateWorking.o ${OBJECTDIR}/_ext/355834263/HystFltr.o ${OBJECTDIR}/_ext/706196810/Indication.o ${OBJECTDIR}/_ext/706196810/LedFailure.o ${OBJECTDIR}/_ext/962885590/CheckCauseReset.o ${OBJECTDIR}/_ext/962885590/IdentHex.o ${OBJECTDIR}/_ext/962885590/TimeBeginSynchronization.o ${OBJECTDIR}/_ext/2012282586/IntegrCtrl.o ${OBJECTDIR}/_ext/2012282586/IntegrCtrlGen.o ${OBJECTDIR}/_ext/1093587103/InterChannel.o ${OBJECTDIR}/_ext/1093587103/log2.o ${OBJECTDIR}/_ext/1093587103/pwr2.o ${OBJECTDIR}/_ext/1020072301/ControlSystem.o ${OBJECTDIR}/_ext/1020072301/InterruptsHandlers.o ${OBJECTDIR}/_ext/1020072301/UnusedInterrupts.o ${OBJECTDIR}/_ext/985603786/ModeProtection.o ${OBJECTDIR}/_ext/887427039/OverloadDet.o ${OBJECTDIR}/_ext/777634832/posDetGenerator.o ${OBJECTDIR}/_ext/777634832/positionDetFiveEC.o ${OBJECTDIR}/_ext/777634832/positionDetNineWire.o ${OBJECTDIR}/_ext/777634832/positionDet.o ${OBJECTDIR}/_ext/1670244350/relayCtrl.o ${OBJECTDIR}/_ext/1670244350/relayCtrlFiveEC.o ${OBJECTDIR}/_ext/1670244350/relayCtrlNineWire.o ${OBJECTDIR}/_ext/1553322471/Rs422.o ${OBJECTDIR}/_ext/1553322471/Rs422_crc8.o ${OBJECTDIR}/_ext/1553322471/Rs422_lineExch.o ${OBJECTDIR}/_ext/1553322471/Rs422_transceiver.o ${OBJECTDIR}/_ext/1645518876/SafetyPowerControl.o ${OBJECTDIR}/_ext/237575434/ShuntShiftGen.o ${OBJECTDIR}/_ext/237575434/shuntShift.o ${OBJECTDIR}/_ext/237575434/shuntShiftFiveEC.o ${OBJECTDIR}/_ext/237575434/shuntShiftMotor.o ${OBJECTDIR}/_ext/237575434/shuntShiftNineWire.o ${OBJECTDIR}/_ext/2018284246/asserts.o ${OBJECTDIR}/_ext/38706442/CheckCallFunctions.o ${OBJECTDIR}/_ext/38706442/CheckRAM.o ${OBJECTDIR}/_ext/38706442/CheckRegisters.o ${OBJECTDIR}/_ext/38706442/ControlMK.o ${OBJECTDIR}/_ext/38706442/CheckCPU.o ${OBJECTDIR}/_ext/38706442/FlashCheck.o ${OBJECTDIR}/_ext/1268761170/SqrtTOpt.o ${OBJECTDIR}/_ext/1268761170/wait.o ${OBJECTDIR}/_ext/1268761170/CopySwap.o ${OBJECTDIR}/_ext/1020072301/Main.o

# Source Files
SOURCEFILES=../src/ActivityManager/ActivityManager.c ../src/ActivityManager/ActivityManager_connect.c ../src/ActivityManager/ActivityManager_internal.c ../src/ActivityManager/BlockExch.c ../src/AnalogInput/AnalogInput.c ../src/AnalogInput/Adc.c ../src/AnalogInput/DigitalFilter.c ../src/AnalogInput/dsPIC33_FIR_filter.s ../src/AnalogMeasurement/AnalogMeasurement.c ../src/BinIn/BinIn.c ../src/BinIn/BinInDecoder.s ../src/BlackBox/BlackBox.c ../src/CheckSupply/CheckSupply.c ../src/ConfigMK/ConfigMK.c ../src/DebugTools/Tracing.c ../src/DebugTools/DebugTools.c ../src/DeviceAddress/deviceAddress.c ../src/drv/turnOffdsPIC33.c ../src/drv/AdcDriver_PIC33.c ../src/drv/checkRegistersDrv.c ../src/drv/Ecan_driverPIC33.c ../src/drv/EcanAck_driverPIC33.c ../src/drv/EepromDrv.c ../src/drv/I2Cdrv_PIC33.c ../src/drv/dsPIC33_rs422.c ../src/drv/shuntShiftGenDrv.c ../src/drv/posDetGen_drv.c ../src/drv/relayCtrlDrv.c ../src/drv/BlockExch_driver.c ../src/Eeprom/Eeprom.c ../src/Fsm/Fsm.c ../src/FsmStates/Sheduler.c ../src/FsmStates/StateActive.c ../src/FsmStates/StateInitial.c ../src/FsmStates/StateMain.c ../src/FsmStates/StatePassive.c ../src/FsmStates/StateSafeActive.c ../src/FsmStates/StateSafePassive.c ../src/FsmStates/StateTop.c ../src/FsmStates/StateWorking.c ../src/HysteresisFilter/HystFltr.c ../src/Indication/Indication.c ../src/Indication/LedFailure.c ../src/Initial/CheckCauseReset.c ../src/Initial/IdentHex.c ../src/Initial/TimeBeginSynchronization.c ../src/IntegrCtrl/IntegrCtrl.c ../src/IntegrCtrl/IntegrCtrlGen.c ../src/InterChannel/InterChannel.c ../src/InterChannel/log2.c ../src/InterChannel/pwr2.c ../src/Main/ControlSystem.c ../src/Main/InterruptsHandlers.c ../src/Main/UnusedInterrupts.c ../src/ModeProtection/ModeProtection.c ../src/OverloadDet/OverloadDet.c ../src/PositionDet/posDetGenerator.c ../src/PositionDet/positionDetFiveEC.c ../src/PositionDet/positionDetNineWire.c ../src/PositionDet/positionDet.c ../src/RelayCtrl/relayCtrl.c ../src/RelayCtrl/relayCtrlFiveEC.c ../src/RelayCtrl/relayCtrlNineWire.c ../src/RS422/Rs422.c ../src/RS422/Rs422_crc8.c ../src/RS422/Rs422_lineExch.c ../src/RS422/Rs422_transceiver.c ../src/SafetyPowerControl/SafetyPowerControl.c ../src/ShuntShift/ShuntShiftGen.c ../src/ShuntShift/shuntShift.c ../src/ShuntShift/shuntShiftFiveEC.c ../src/ShuntShift/shuntShiftMotor.c ../src/ShuntShift/shuntShiftNineWire.c ../src/systems/asserts.c ../src/Testing/CheckCallFunctions.c ../src/Testing/CheckRAM.c ../src/Testing/CheckRegisters.c ../src/Testing/ControlMK.c ../src/Testing/CheckCPU.s ../src/Testing/FlashCheck.c ../src/Utility/SqrtTOpt.s ../src/Utility/wait.c ../src/Utility/CopySwap.c ../src/Main/Main.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/OKPS-EK.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=33EP512GM710
MP_LINKER_FILE_OPTION=,--script=p33EP512GM710.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1813651324/ActivityManager.o: ../src/ActivityManager/ActivityManager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1813651324" 
	@${RM} ${OBJECTDIR}/_ext/1813651324/ActivityManager.o.d 
	@${RM} ${OBJECTDIR}/_ext/1813651324/ActivityManager.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ActivityManager/ActivityManager.c  -o ${OBJECTDIR}/_ext/1813651324/ActivityManager.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1813651324/ActivityManager.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1813651324/ActivityManager.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1813651324/ActivityManager_connect.o: ../src/ActivityManager/ActivityManager_connect.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1813651324" 
	@${RM} ${OBJECTDIR}/_ext/1813651324/ActivityManager_connect.o.d 
	@${RM} ${OBJECTDIR}/_ext/1813651324/ActivityManager_connect.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ActivityManager/ActivityManager_connect.c  -o ${OBJECTDIR}/_ext/1813651324/ActivityManager_connect.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1813651324/ActivityManager_connect.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1813651324/ActivityManager_connect.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1813651324/ActivityManager_internal.o: ../src/ActivityManager/ActivityManager_internal.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1813651324" 
	@${RM} ${OBJECTDIR}/_ext/1813651324/ActivityManager_internal.o.d 
	@${RM} ${OBJECTDIR}/_ext/1813651324/ActivityManager_internal.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ActivityManager/ActivityManager_internal.c  -o ${OBJECTDIR}/_ext/1813651324/ActivityManager_internal.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1813651324/ActivityManager_internal.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1813651324/ActivityManager_internal.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1813651324/BlockExch.o: ../src/ActivityManager/BlockExch.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1813651324" 
	@${RM} ${OBJECTDIR}/_ext/1813651324/BlockExch.o.d 
	@${RM} ${OBJECTDIR}/_ext/1813651324/BlockExch.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ActivityManager/BlockExch.c  -o ${OBJECTDIR}/_ext/1813651324/BlockExch.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1813651324/BlockExch.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1813651324/BlockExch.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1031660192/AnalogInput.o: ../src/AnalogInput/AnalogInput.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1031660192" 
	@${RM} ${OBJECTDIR}/_ext/1031660192/AnalogInput.o.d 
	@${RM} ${OBJECTDIR}/_ext/1031660192/AnalogInput.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/AnalogInput/AnalogInput.c  -o ${OBJECTDIR}/_ext/1031660192/AnalogInput.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1031660192/AnalogInput.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1031660192/AnalogInput.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1031660192/Adc.o: ../src/AnalogInput/Adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1031660192" 
	@${RM} ${OBJECTDIR}/_ext/1031660192/Adc.o.d 
	@${RM} ${OBJECTDIR}/_ext/1031660192/Adc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/AnalogInput/Adc.c  -o ${OBJECTDIR}/_ext/1031660192/Adc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1031660192/Adc.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1031660192/Adc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1031660192/DigitalFilter.o: ../src/AnalogInput/DigitalFilter.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1031660192" 
	@${RM} ${OBJECTDIR}/_ext/1031660192/DigitalFilter.o.d 
	@${RM} ${OBJECTDIR}/_ext/1031660192/DigitalFilter.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/AnalogInput/DigitalFilter.c  -o ${OBJECTDIR}/_ext/1031660192/DigitalFilter.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1031660192/DigitalFilter.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1031660192/DigitalFilter.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/161780206/AnalogMeasurement.o: ../src/AnalogMeasurement/AnalogMeasurement.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/161780206" 
	@${RM} ${OBJECTDIR}/_ext/161780206/AnalogMeasurement.o.d 
	@${RM} ${OBJECTDIR}/_ext/161780206/AnalogMeasurement.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/AnalogMeasurement/AnalogMeasurement.c  -o ${OBJECTDIR}/_ext/161780206/AnalogMeasurement.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/161780206/AnalogMeasurement.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/161780206/AnalogMeasurement.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1567386894/BinIn.o: ../src/BinIn/BinIn.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1567386894" 
	@${RM} ${OBJECTDIR}/_ext/1567386894/BinIn.o.d 
	@${RM} ${OBJECTDIR}/_ext/1567386894/BinIn.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/BinIn/BinIn.c  -o ${OBJECTDIR}/_ext/1567386894/BinIn.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1567386894/BinIn.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1567386894/BinIn.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1119163098/BlackBox.o: ../src/BlackBox/BlackBox.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1119163098" 
	@${RM} ${OBJECTDIR}/_ext/1119163098/BlackBox.o.d 
	@${RM} ${OBJECTDIR}/_ext/1119163098/BlackBox.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/BlackBox/BlackBox.c  -o ${OBJECTDIR}/_ext/1119163098/BlackBox.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1119163098/BlackBox.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1119163098/BlackBox.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1684595197/CheckSupply.o: ../src/CheckSupply/CheckSupply.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1684595197" 
	@${RM} ${OBJECTDIR}/_ext/1684595197/CheckSupply.o.d 
	@${RM} ${OBJECTDIR}/_ext/1684595197/CheckSupply.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/CheckSupply/CheckSupply.c  -o ${OBJECTDIR}/_ext/1684595197/CheckSupply.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1684595197/CheckSupply.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1684595197/CheckSupply.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/633884614/ConfigMK.o: ../src/ConfigMK/ConfigMK.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/633884614" 
	@${RM} ${OBJECTDIR}/_ext/633884614/ConfigMK.o.d 
	@${RM} ${OBJECTDIR}/_ext/633884614/ConfigMK.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ConfigMK/ConfigMK.c  -o ${OBJECTDIR}/_ext/633884614/ConfigMK.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/633884614/ConfigMK.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/633884614/ConfigMK.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1959551838/Tracing.o: ../src/DebugTools/Tracing.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1959551838" 
	@${RM} ${OBJECTDIR}/_ext/1959551838/Tracing.o.d 
	@${RM} ${OBJECTDIR}/_ext/1959551838/Tracing.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/DebugTools/Tracing.c  -o ${OBJECTDIR}/_ext/1959551838/Tracing.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1959551838/Tracing.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1959551838/Tracing.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1959551838/DebugTools.o: ../src/DebugTools/DebugTools.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1959551838" 
	@${RM} ${OBJECTDIR}/_ext/1959551838/DebugTools.o.d 
	@${RM} ${OBJECTDIR}/_ext/1959551838/DebugTools.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/DebugTools/DebugTools.c  -o ${OBJECTDIR}/_ext/1959551838/DebugTools.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1959551838/DebugTools.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99
	@${FIXDEPS} "${OBJECTDIR}/_ext/1959551838/DebugTools.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/970708260/deviceAddress.o: ../src/DeviceAddress/deviceAddress.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/970708260" 
	@${RM} ${OBJECTDIR}/_ext/970708260/deviceAddress.o.d 
	@${RM} ${OBJECTDIR}/_ext/970708260/deviceAddress.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/DeviceAddress/deviceAddress.c  -o ${OBJECTDIR}/_ext/970708260/deviceAddress.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/970708260/deviceAddress.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/970708260/deviceAddress.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659853742/turnOffdsPIC33.o: ../src/drv/turnOffdsPIC33.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659853742" 
	@${RM} ${OBJECTDIR}/_ext/659853742/turnOffdsPIC33.o.d 
	@${RM} ${OBJECTDIR}/_ext/659853742/turnOffdsPIC33.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/drv/turnOffdsPIC33.c  -o ${OBJECTDIR}/_ext/659853742/turnOffdsPIC33.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659853742/turnOffdsPIC33.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/659853742/turnOffdsPIC33.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659853742/AdcDriver_PIC33.o: ../src/drv/AdcDriver_PIC33.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659853742" 
	@${RM} ${OBJECTDIR}/_ext/659853742/AdcDriver_PIC33.o.d 
	@${RM} ${OBJECTDIR}/_ext/659853742/AdcDriver_PIC33.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/drv/AdcDriver_PIC33.c  -o ${OBJECTDIR}/_ext/659853742/AdcDriver_PIC33.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659853742/AdcDriver_PIC33.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/659853742/AdcDriver_PIC33.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659853742/checkRegistersDrv.o: ../src/drv/checkRegistersDrv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659853742" 
	@${RM} ${OBJECTDIR}/_ext/659853742/checkRegistersDrv.o.d 
	@${RM} ${OBJECTDIR}/_ext/659853742/checkRegistersDrv.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/drv/checkRegistersDrv.c  -o ${OBJECTDIR}/_ext/659853742/checkRegistersDrv.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659853742/checkRegistersDrv.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/659853742/checkRegistersDrv.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659853742/Ecan_driverPIC33.o: ../src/drv/Ecan_driverPIC33.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659853742" 
	@${RM} ${OBJECTDIR}/_ext/659853742/Ecan_driverPIC33.o.d 
	@${RM} ${OBJECTDIR}/_ext/659853742/Ecan_driverPIC33.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/drv/Ecan_driverPIC33.c  -o ${OBJECTDIR}/_ext/659853742/Ecan_driverPIC33.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659853742/Ecan_driverPIC33.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/659853742/Ecan_driverPIC33.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659853742/EcanAck_driverPIC33.o: ../src/drv/EcanAck_driverPIC33.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659853742" 
	@${RM} ${OBJECTDIR}/_ext/659853742/EcanAck_driverPIC33.o.d 
	@${RM} ${OBJECTDIR}/_ext/659853742/EcanAck_driverPIC33.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/drv/EcanAck_driverPIC33.c  -o ${OBJECTDIR}/_ext/659853742/EcanAck_driverPIC33.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659853742/EcanAck_driverPIC33.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/659853742/EcanAck_driverPIC33.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659853742/EepromDrv.o: ../src/drv/EepromDrv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659853742" 
	@${RM} ${OBJECTDIR}/_ext/659853742/EepromDrv.o.d 
	@${RM} ${OBJECTDIR}/_ext/659853742/EepromDrv.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/drv/EepromDrv.c  -o ${OBJECTDIR}/_ext/659853742/EepromDrv.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659853742/EepromDrv.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/659853742/EepromDrv.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659853742/I2Cdrv_PIC33.o: ../src/drv/I2Cdrv_PIC33.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659853742" 
	@${RM} ${OBJECTDIR}/_ext/659853742/I2Cdrv_PIC33.o.d 
	@${RM} ${OBJECTDIR}/_ext/659853742/I2Cdrv_PIC33.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/drv/I2Cdrv_PIC33.c  -o ${OBJECTDIR}/_ext/659853742/I2Cdrv_PIC33.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659853742/I2Cdrv_PIC33.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/659853742/I2Cdrv_PIC33.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659853742/dsPIC33_rs422.o: ../src/drv/dsPIC33_rs422.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659853742" 
	@${RM} ${OBJECTDIR}/_ext/659853742/dsPIC33_rs422.o.d 
	@${RM} ${OBJECTDIR}/_ext/659853742/dsPIC33_rs422.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/drv/dsPIC33_rs422.c  -o ${OBJECTDIR}/_ext/659853742/dsPIC33_rs422.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659853742/dsPIC33_rs422.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/659853742/dsPIC33_rs422.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659853742/shuntShiftGenDrv.o: ../src/drv/shuntShiftGenDrv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659853742" 
	@${RM} ${OBJECTDIR}/_ext/659853742/shuntShiftGenDrv.o.d 
	@${RM} ${OBJECTDIR}/_ext/659853742/shuntShiftGenDrv.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/drv/shuntShiftGenDrv.c  -o ${OBJECTDIR}/_ext/659853742/shuntShiftGenDrv.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659853742/shuntShiftGenDrv.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/659853742/shuntShiftGenDrv.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659853742/posDetGen_drv.o: ../src/drv/posDetGen_drv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659853742" 
	@${RM} ${OBJECTDIR}/_ext/659853742/posDetGen_drv.o.d 
	@${RM} ${OBJECTDIR}/_ext/659853742/posDetGen_drv.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/drv/posDetGen_drv.c  -o ${OBJECTDIR}/_ext/659853742/posDetGen_drv.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659853742/posDetGen_drv.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/659853742/posDetGen_drv.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659853742/relayCtrlDrv.o: ../src/drv/relayCtrlDrv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659853742" 
	@${RM} ${OBJECTDIR}/_ext/659853742/relayCtrlDrv.o.d 
	@${RM} ${OBJECTDIR}/_ext/659853742/relayCtrlDrv.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/drv/relayCtrlDrv.c  -o ${OBJECTDIR}/_ext/659853742/relayCtrlDrv.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659853742/relayCtrlDrv.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/659853742/relayCtrlDrv.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659853742/BlockExch_driver.o: ../src/drv/BlockExch_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659853742" 
	@${RM} ${OBJECTDIR}/_ext/659853742/BlockExch_driver.o.d 
	@${RM} ${OBJECTDIR}/_ext/659853742/BlockExch_driver.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/drv/BlockExch_driver.c  -o ${OBJECTDIR}/_ext/659853742/BlockExch_driver.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659853742/BlockExch_driver.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/659853742/BlockExch_driver.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1262060966/Eeprom.o: ../src/Eeprom/Eeprom.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1262060966" 
	@${RM} ${OBJECTDIR}/_ext/1262060966/Eeprom.o.d 
	@${RM} ${OBJECTDIR}/_ext/1262060966/Eeprom.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Eeprom/Eeprom.c  -o ${OBJECTDIR}/_ext/1262060966/Eeprom.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1262060966/Eeprom.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1262060966/Eeprom.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659824934/Fsm.o: ../src/Fsm/Fsm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659824934" 
	@${RM} ${OBJECTDIR}/_ext/659824934/Fsm.o.d 
	@${RM} ${OBJECTDIR}/_ext/659824934/Fsm.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Fsm/Fsm.c  -o ${OBJECTDIR}/_ext/659824934/Fsm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659824934/Fsm.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/659824934/Fsm.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1951852232/Sheduler.o: ../src/FsmStates/Sheduler.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1951852232" 
	@${RM} ${OBJECTDIR}/_ext/1951852232/Sheduler.o.d 
	@${RM} ${OBJECTDIR}/_ext/1951852232/Sheduler.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/FsmStates/Sheduler.c  -o ${OBJECTDIR}/_ext/1951852232/Sheduler.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1951852232/Sheduler.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1951852232/Sheduler.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1951852232/StateActive.o: ../src/FsmStates/StateActive.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1951852232" 
	@${RM} ${OBJECTDIR}/_ext/1951852232/StateActive.o.d 
	@${RM} ${OBJECTDIR}/_ext/1951852232/StateActive.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/FsmStates/StateActive.c  -o ${OBJECTDIR}/_ext/1951852232/StateActive.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1951852232/StateActive.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1951852232/StateActive.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1951852232/StateInitial.o: ../src/FsmStates/StateInitial.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1951852232" 
	@${RM} ${OBJECTDIR}/_ext/1951852232/StateInitial.o.d 
	@${RM} ${OBJECTDIR}/_ext/1951852232/StateInitial.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/FsmStates/StateInitial.c  -o ${OBJECTDIR}/_ext/1951852232/StateInitial.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1951852232/StateInitial.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1951852232/StateInitial.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1951852232/StateMain.o: ../src/FsmStates/StateMain.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1951852232" 
	@${RM} ${OBJECTDIR}/_ext/1951852232/StateMain.o.d 
	@${RM} ${OBJECTDIR}/_ext/1951852232/StateMain.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/FsmStates/StateMain.c  -o ${OBJECTDIR}/_ext/1951852232/StateMain.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1951852232/StateMain.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1951852232/StateMain.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1951852232/StatePassive.o: ../src/FsmStates/StatePassive.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1951852232" 
	@${RM} ${OBJECTDIR}/_ext/1951852232/StatePassive.o.d 
	@${RM} ${OBJECTDIR}/_ext/1951852232/StatePassive.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/FsmStates/StatePassive.c  -o ${OBJECTDIR}/_ext/1951852232/StatePassive.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1951852232/StatePassive.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1951852232/StatePassive.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1951852232/StateSafeActive.o: ../src/FsmStates/StateSafeActive.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1951852232" 
	@${RM} ${OBJECTDIR}/_ext/1951852232/StateSafeActive.o.d 
	@${RM} ${OBJECTDIR}/_ext/1951852232/StateSafeActive.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/FsmStates/StateSafeActive.c  -o ${OBJECTDIR}/_ext/1951852232/StateSafeActive.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1951852232/StateSafeActive.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1951852232/StateSafeActive.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1951852232/StateSafePassive.o: ../src/FsmStates/StateSafePassive.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1951852232" 
	@${RM} ${OBJECTDIR}/_ext/1951852232/StateSafePassive.o.d 
	@${RM} ${OBJECTDIR}/_ext/1951852232/StateSafePassive.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/FsmStates/StateSafePassive.c  -o ${OBJECTDIR}/_ext/1951852232/StateSafePassive.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1951852232/StateSafePassive.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1951852232/StateSafePassive.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1951852232/StateTop.o: ../src/FsmStates/StateTop.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1951852232" 
	@${RM} ${OBJECTDIR}/_ext/1951852232/StateTop.o.d 
	@${RM} ${OBJECTDIR}/_ext/1951852232/StateTop.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/FsmStates/StateTop.c  -o ${OBJECTDIR}/_ext/1951852232/StateTop.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1951852232/StateTop.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1951852232/StateTop.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1951852232/StateWorking.o: ../src/FsmStates/StateWorking.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1951852232" 
	@${RM} ${OBJECTDIR}/_ext/1951852232/StateWorking.o.d 
	@${RM} ${OBJECTDIR}/_ext/1951852232/StateWorking.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/FsmStates/StateWorking.c  -o ${OBJECTDIR}/_ext/1951852232/StateWorking.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1951852232/StateWorking.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1951852232/StateWorking.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/355834263/HystFltr.o: ../src/HysteresisFilter/HystFltr.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/355834263" 
	@${RM} ${OBJECTDIR}/_ext/355834263/HystFltr.o.d 
	@${RM} ${OBJECTDIR}/_ext/355834263/HystFltr.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/HysteresisFilter/HystFltr.c  -o ${OBJECTDIR}/_ext/355834263/HystFltr.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/355834263/HystFltr.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/355834263/HystFltr.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/706196810/Indication.o: ../src/Indication/Indication.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/706196810" 
	@${RM} ${OBJECTDIR}/_ext/706196810/Indication.o.d 
	@${RM} ${OBJECTDIR}/_ext/706196810/Indication.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Indication/Indication.c  -o ${OBJECTDIR}/_ext/706196810/Indication.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/706196810/Indication.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/706196810/Indication.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/706196810/LedFailure.o: ../src/Indication/LedFailure.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/706196810" 
	@${RM} ${OBJECTDIR}/_ext/706196810/LedFailure.o.d 
	@${RM} ${OBJECTDIR}/_ext/706196810/LedFailure.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Indication/LedFailure.c  -o ${OBJECTDIR}/_ext/706196810/LedFailure.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/706196810/LedFailure.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/706196810/LedFailure.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/962885590/CheckCauseReset.o: ../src/Initial/CheckCauseReset.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/962885590" 
	@${RM} ${OBJECTDIR}/_ext/962885590/CheckCauseReset.o.d 
	@${RM} ${OBJECTDIR}/_ext/962885590/CheckCauseReset.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Initial/CheckCauseReset.c  -o ${OBJECTDIR}/_ext/962885590/CheckCauseReset.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/962885590/CheckCauseReset.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/962885590/CheckCauseReset.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/962885590/IdentHex.o: ../src/Initial/IdentHex.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/962885590" 
	@${RM} ${OBJECTDIR}/_ext/962885590/IdentHex.o.d 
	@${RM} ${OBJECTDIR}/_ext/962885590/IdentHex.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Initial/IdentHex.c  -o ${OBJECTDIR}/_ext/962885590/IdentHex.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/962885590/IdentHex.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/962885590/IdentHex.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/962885590/TimeBeginSynchronization.o: ../src/Initial/TimeBeginSynchronization.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/962885590" 
	@${RM} ${OBJECTDIR}/_ext/962885590/TimeBeginSynchronization.o.d 
	@${RM} ${OBJECTDIR}/_ext/962885590/TimeBeginSynchronization.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Initial/TimeBeginSynchronization.c  -o ${OBJECTDIR}/_ext/962885590/TimeBeginSynchronization.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/962885590/TimeBeginSynchronization.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/962885590/TimeBeginSynchronization.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2012282586/IntegrCtrl.o: ../src/IntegrCtrl/IntegrCtrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2012282586" 
	@${RM} ${OBJECTDIR}/_ext/2012282586/IntegrCtrl.o.d 
	@${RM} ${OBJECTDIR}/_ext/2012282586/IntegrCtrl.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/IntegrCtrl/IntegrCtrl.c  -o ${OBJECTDIR}/_ext/2012282586/IntegrCtrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2012282586/IntegrCtrl.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2012282586/IntegrCtrl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2012282586/IntegrCtrlGen.o: ../src/IntegrCtrl/IntegrCtrlGen.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2012282586" 
	@${RM} ${OBJECTDIR}/_ext/2012282586/IntegrCtrlGen.o.d 
	@${RM} ${OBJECTDIR}/_ext/2012282586/IntegrCtrlGen.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/IntegrCtrl/IntegrCtrlGen.c  -o ${OBJECTDIR}/_ext/2012282586/IntegrCtrlGen.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2012282586/IntegrCtrlGen.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2012282586/IntegrCtrlGen.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1093587103/InterChannel.o: ../src/InterChannel/InterChannel.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1093587103" 
	@${RM} ${OBJECTDIR}/_ext/1093587103/InterChannel.o.d 
	@${RM} ${OBJECTDIR}/_ext/1093587103/InterChannel.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/InterChannel/InterChannel.c  -o ${OBJECTDIR}/_ext/1093587103/InterChannel.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1093587103/InterChannel.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1093587103/InterChannel.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1093587103/log2.o: ../src/InterChannel/log2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1093587103" 
	@${RM} ${OBJECTDIR}/_ext/1093587103/log2.o.d 
	@${RM} ${OBJECTDIR}/_ext/1093587103/log2.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/InterChannel/log2.c  -o ${OBJECTDIR}/_ext/1093587103/log2.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1093587103/log2.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1093587103/log2.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1093587103/pwr2.o: ../src/InterChannel/pwr2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1093587103" 
	@${RM} ${OBJECTDIR}/_ext/1093587103/pwr2.o.d 
	@${RM} ${OBJECTDIR}/_ext/1093587103/pwr2.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/InterChannel/pwr2.c  -o ${OBJECTDIR}/_ext/1093587103/pwr2.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1093587103/pwr2.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1093587103/pwr2.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1020072301/ControlSystem.o: ../src/Main/ControlSystem.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1020072301" 
	@${RM} ${OBJECTDIR}/_ext/1020072301/ControlSystem.o.d 
	@${RM} ${OBJECTDIR}/_ext/1020072301/ControlSystem.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Main/ControlSystem.c  -o ${OBJECTDIR}/_ext/1020072301/ControlSystem.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1020072301/ControlSystem.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mlarge-code -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1020072301/ControlSystem.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1020072301/InterruptsHandlers.o: ../src/Main/InterruptsHandlers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1020072301" 
	@${RM} ${OBJECTDIR}/_ext/1020072301/InterruptsHandlers.o.d 
	@${RM} ${OBJECTDIR}/_ext/1020072301/InterruptsHandlers.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Main/InterruptsHandlers.c  -o ${OBJECTDIR}/_ext/1020072301/InterruptsHandlers.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1020072301/InterruptsHandlers.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1020072301/InterruptsHandlers.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1020072301/UnusedInterrupts.o: ../src/Main/UnusedInterrupts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1020072301" 
	@${RM} ${OBJECTDIR}/_ext/1020072301/UnusedInterrupts.o.d 
	@${RM} ${OBJECTDIR}/_ext/1020072301/UnusedInterrupts.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Main/UnusedInterrupts.c  -o ${OBJECTDIR}/_ext/1020072301/UnusedInterrupts.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1020072301/UnusedInterrupts.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1020072301/UnusedInterrupts.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/985603786/ModeProtection.o: ../src/ModeProtection/ModeProtection.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/985603786" 
	@${RM} ${OBJECTDIR}/_ext/985603786/ModeProtection.o.d 
	@${RM} ${OBJECTDIR}/_ext/985603786/ModeProtection.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ModeProtection/ModeProtection.c  -o ${OBJECTDIR}/_ext/985603786/ModeProtection.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/985603786/ModeProtection.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/985603786/ModeProtection.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/887427039/OverloadDet.o: ../src/OverloadDet/OverloadDet.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/887427039" 
	@${RM} ${OBJECTDIR}/_ext/887427039/OverloadDet.o.d 
	@${RM} ${OBJECTDIR}/_ext/887427039/OverloadDet.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/OverloadDet/OverloadDet.c  -o ${OBJECTDIR}/_ext/887427039/OverloadDet.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/887427039/OverloadDet.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/887427039/OverloadDet.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/777634832/posDetGenerator.o: ../src/PositionDet/posDetGenerator.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/777634832" 
	@${RM} ${OBJECTDIR}/_ext/777634832/posDetGenerator.o.d 
	@${RM} ${OBJECTDIR}/_ext/777634832/posDetGenerator.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/PositionDet/posDetGenerator.c  -o ${OBJECTDIR}/_ext/777634832/posDetGenerator.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/777634832/posDetGenerator.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/777634832/posDetGenerator.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/777634832/positionDetFiveEC.o: ../src/PositionDet/positionDetFiveEC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/777634832" 
	@${RM} ${OBJECTDIR}/_ext/777634832/positionDetFiveEC.o.d 
	@${RM} ${OBJECTDIR}/_ext/777634832/positionDetFiveEC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/PositionDet/positionDetFiveEC.c  -o ${OBJECTDIR}/_ext/777634832/positionDetFiveEC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/777634832/positionDetFiveEC.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/777634832/positionDetFiveEC.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/777634832/positionDetNineWire.o: ../src/PositionDet/positionDetNineWire.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/777634832" 
	@${RM} ${OBJECTDIR}/_ext/777634832/positionDetNineWire.o.d 
	@${RM} ${OBJECTDIR}/_ext/777634832/positionDetNineWire.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/PositionDet/positionDetNineWire.c  -o ${OBJECTDIR}/_ext/777634832/positionDetNineWire.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/777634832/positionDetNineWire.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/777634832/positionDetNineWire.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/777634832/positionDet.o: ../src/PositionDet/positionDet.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/777634832" 
	@${RM} ${OBJECTDIR}/_ext/777634832/positionDet.o.d 
	@${RM} ${OBJECTDIR}/_ext/777634832/positionDet.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/PositionDet/positionDet.c  -o ${OBJECTDIR}/_ext/777634832/positionDet.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/777634832/positionDet.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/777634832/positionDet.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1670244350/relayCtrl.o: ../src/RelayCtrl/relayCtrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1670244350" 
	@${RM} ${OBJECTDIR}/_ext/1670244350/relayCtrl.o.d 
	@${RM} ${OBJECTDIR}/_ext/1670244350/relayCtrl.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/RelayCtrl/relayCtrl.c  -o ${OBJECTDIR}/_ext/1670244350/relayCtrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1670244350/relayCtrl.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1670244350/relayCtrl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1670244350/relayCtrlFiveEC.o: ../src/RelayCtrl/relayCtrlFiveEC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1670244350" 
	@${RM} ${OBJECTDIR}/_ext/1670244350/relayCtrlFiveEC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1670244350/relayCtrlFiveEC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/RelayCtrl/relayCtrlFiveEC.c  -o ${OBJECTDIR}/_ext/1670244350/relayCtrlFiveEC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1670244350/relayCtrlFiveEC.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1670244350/relayCtrlFiveEC.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1670244350/relayCtrlNineWire.o: ../src/RelayCtrl/relayCtrlNineWire.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1670244350" 
	@${RM} ${OBJECTDIR}/_ext/1670244350/relayCtrlNineWire.o.d 
	@${RM} ${OBJECTDIR}/_ext/1670244350/relayCtrlNineWire.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/RelayCtrl/relayCtrlNineWire.c  -o ${OBJECTDIR}/_ext/1670244350/relayCtrlNineWire.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1670244350/relayCtrlNineWire.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1670244350/relayCtrlNineWire.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1553322471/Rs422.o: ../src/RS422/Rs422.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1553322471" 
	@${RM} ${OBJECTDIR}/_ext/1553322471/Rs422.o.d 
	@${RM} ${OBJECTDIR}/_ext/1553322471/Rs422.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/RS422/Rs422.c  -o ${OBJECTDIR}/_ext/1553322471/Rs422.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1553322471/Rs422.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1553322471/Rs422.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1553322471/Rs422_crc8.o: ../src/RS422/Rs422_crc8.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1553322471" 
	@${RM} ${OBJECTDIR}/_ext/1553322471/Rs422_crc8.o.d 
	@${RM} ${OBJECTDIR}/_ext/1553322471/Rs422_crc8.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/RS422/Rs422_crc8.c  -o ${OBJECTDIR}/_ext/1553322471/Rs422_crc8.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1553322471/Rs422_crc8.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1553322471/Rs422_crc8.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1553322471/Rs422_lineExch.o: ../src/RS422/Rs422_lineExch.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1553322471" 
	@${RM} ${OBJECTDIR}/_ext/1553322471/Rs422_lineExch.o.d 
	@${RM} ${OBJECTDIR}/_ext/1553322471/Rs422_lineExch.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/RS422/Rs422_lineExch.c  -o ${OBJECTDIR}/_ext/1553322471/Rs422_lineExch.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1553322471/Rs422_lineExch.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1553322471/Rs422_lineExch.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1553322471/Rs422_transceiver.o: ../src/RS422/Rs422_transceiver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1553322471" 
	@${RM} ${OBJECTDIR}/_ext/1553322471/Rs422_transceiver.o.d 
	@${RM} ${OBJECTDIR}/_ext/1553322471/Rs422_transceiver.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/RS422/Rs422_transceiver.c  -o ${OBJECTDIR}/_ext/1553322471/Rs422_transceiver.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1553322471/Rs422_transceiver.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1553322471/Rs422_transceiver.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1645518876/SafetyPowerControl.o: ../src/SafetyPowerControl/SafetyPowerControl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1645518876" 
	@${RM} ${OBJECTDIR}/_ext/1645518876/SafetyPowerControl.o.d 
	@${RM} ${OBJECTDIR}/_ext/1645518876/SafetyPowerControl.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/SafetyPowerControl/SafetyPowerControl.c  -o ${OBJECTDIR}/_ext/1645518876/SafetyPowerControl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1645518876/SafetyPowerControl.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/BinIn" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1645518876/SafetyPowerControl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/237575434/ShuntShiftGen.o: ../src/ShuntShift/ShuntShiftGen.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/237575434" 
	@${RM} ${OBJECTDIR}/_ext/237575434/ShuntShiftGen.o.d 
	@${RM} ${OBJECTDIR}/_ext/237575434/ShuntShiftGen.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ShuntShift/ShuntShiftGen.c  -o ${OBJECTDIR}/_ext/237575434/ShuntShiftGen.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/237575434/ShuntShiftGen.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/237575434/ShuntShiftGen.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/237575434/shuntShift.o: ../src/ShuntShift/shuntShift.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/237575434" 
	@${RM} ${OBJECTDIR}/_ext/237575434/shuntShift.o.d 
	@${RM} ${OBJECTDIR}/_ext/237575434/shuntShift.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ShuntShift/shuntShift.c  -o ${OBJECTDIR}/_ext/237575434/shuntShift.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/237575434/shuntShift.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/237575434/shuntShift.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/237575434/shuntShiftFiveEC.o: ../src/ShuntShift/shuntShiftFiveEC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/237575434" 
	@${RM} ${OBJECTDIR}/_ext/237575434/shuntShiftFiveEC.o.d 
	@${RM} ${OBJECTDIR}/_ext/237575434/shuntShiftFiveEC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ShuntShift/shuntShiftFiveEC.c  -o ${OBJECTDIR}/_ext/237575434/shuntShiftFiveEC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/237575434/shuntShiftFiveEC.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/237575434/shuntShiftFiveEC.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/237575434/shuntShiftMotor.o: ../src/ShuntShift/shuntShiftMotor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/237575434" 
	@${RM} ${OBJECTDIR}/_ext/237575434/shuntShiftMotor.o.d 
	@${RM} ${OBJECTDIR}/_ext/237575434/shuntShiftMotor.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ShuntShift/shuntShiftMotor.c  -o ${OBJECTDIR}/_ext/237575434/shuntShiftMotor.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/237575434/shuntShiftMotor.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/237575434/shuntShiftMotor.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/237575434/shuntShiftNineWire.o: ../src/ShuntShift/shuntShiftNineWire.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/237575434" 
	@${RM} ${OBJECTDIR}/_ext/237575434/shuntShiftNineWire.o.d 
	@${RM} ${OBJECTDIR}/_ext/237575434/shuntShiftNineWire.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ShuntShift/shuntShiftNineWire.c  -o ${OBJECTDIR}/_ext/237575434/shuntShiftNineWire.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/237575434/shuntShiftNineWire.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/237575434/shuntShiftNineWire.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2018284246/asserts.o: ../src/systems/asserts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2018284246" 
	@${RM} ${OBJECTDIR}/_ext/2018284246/asserts.o.d 
	@${RM} ${OBJECTDIR}/_ext/2018284246/asserts.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/systems/asserts.c  -o ${OBJECTDIR}/_ext/2018284246/asserts.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2018284246/asserts.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2018284246/asserts.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/38706442/CheckCallFunctions.o: ../src/Testing/CheckCallFunctions.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/38706442" 
	@${RM} ${OBJECTDIR}/_ext/38706442/CheckCallFunctions.o.d 
	@${RM} ${OBJECTDIR}/_ext/38706442/CheckCallFunctions.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Testing/CheckCallFunctions.c  -o ${OBJECTDIR}/_ext/38706442/CheckCallFunctions.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/38706442/CheckCallFunctions.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/38706442/CheckCallFunctions.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/38706442/CheckRAM.o: ../src/Testing/CheckRAM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/38706442" 
	@${RM} ${OBJECTDIR}/_ext/38706442/CheckRAM.o.d 
	@${RM} ${OBJECTDIR}/_ext/38706442/CheckRAM.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Testing/CheckRAM.c  -o ${OBJECTDIR}/_ext/38706442/CheckRAM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/38706442/CheckRAM.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/38706442/CheckRAM.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/38706442/CheckRegisters.o: ../src/Testing/CheckRegisters.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/38706442" 
	@${RM} ${OBJECTDIR}/_ext/38706442/CheckRegisters.o.d 
	@${RM} ${OBJECTDIR}/_ext/38706442/CheckRegisters.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Testing/CheckRegisters.c  -o ${OBJECTDIR}/_ext/38706442/CheckRegisters.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/38706442/CheckRegisters.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/38706442/CheckRegisters.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/38706442/ControlMK.o: ../src/Testing/ControlMK.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/38706442" 
	@${RM} ${OBJECTDIR}/_ext/38706442/ControlMK.o.d 
	@${RM} ${OBJECTDIR}/_ext/38706442/ControlMK.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Testing/ControlMK.c  -o ${OBJECTDIR}/_ext/38706442/ControlMK.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/38706442/ControlMK.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/38706442/ControlMK.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/38706442/FlashCheck.o: ../src/Testing/FlashCheck.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/38706442" 
	@${RM} ${OBJECTDIR}/_ext/38706442/FlashCheck.o.d 
	@${RM} ${OBJECTDIR}/_ext/38706442/FlashCheck.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Testing/FlashCheck.c  -o ${OBJECTDIR}/_ext/38706442/FlashCheck.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/38706442/FlashCheck.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/38706442/FlashCheck.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1268761170/wait.o: ../src/Utility/wait.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1268761170" 
	@${RM} ${OBJECTDIR}/_ext/1268761170/wait.o.d 
	@${RM} ${OBJECTDIR}/_ext/1268761170/wait.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Utility/wait.c  -o ${OBJECTDIR}/_ext/1268761170/wait.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1268761170/wait.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1268761170/wait.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1268761170/CopySwap.o: ../src/Utility/CopySwap.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1268761170" 
	@${RM} ${OBJECTDIR}/_ext/1268761170/CopySwap.o.d 
	@${RM} ${OBJECTDIR}/_ext/1268761170/CopySwap.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Utility/CopySwap.c  -o ${OBJECTDIR}/_ext/1268761170/CopySwap.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1268761170/CopySwap.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1268761170/CopySwap.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1020072301/Main.o: ../src/Main/Main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1020072301" 
	@${RM} ${OBJECTDIR}/_ext/1020072301/Main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1020072301/Main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Main/Main.c  -o ${OBJECTDIR}/_ext/1020072301/Main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1020072301/Main.o.d"      -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -mno-eds-warn  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1020072301/Main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
else
${OBJECTDIR}/_ext/1813651324/ActivityManager.o: ../src/ActivityManager/ActivityManager.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1813651324" 
	@${RM} ${OBJECTDIR}/_ext/1813651324/ActivityManager.o.d 
	@${RM} ${OBJECTDIR}/_ext/1813651324/ActivityManager.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ActivityManager/ActivityManager.c  -o ${OBJECTDIR}/_ext/1813651324/ActivityManager.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1813651324/ActivityManager.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1813651324/ActivityManager.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1813651324/ActivityManager_connect.o: ../src/ActivityManager/ActivityManager_connect.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1813651324" 
	@${RM} ${OBJECTDIR}/_ext/1813651324/ActivityManager_connect.o.d 
	@${RM} ${OBJECTDIR}/_ext/1813651324/ActivityManager_connect.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ActivityManager/ActivityManager_connect.c  -o ${OBJECTDIR}/_ext/1813651324/ActivityManager_connect.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1813651324/ActivityManager_connect.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1813651324/ActivityManager_connect.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1813651324/ActivityManager_internal.o: ../src/ActivityManager/ActivityManager_internal.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1813651324" 
	@${RM} ${OBJECTDIR}/_ext/1813651324/ActivityManager_internal.o.d 
	@${RM} ${OBJECTDIR}/_ext/1813651324/ActivityManager_internal.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ActivityManager/ActivityManager_internal.c  -o ${OBJECTDIR}/_ext/1813651324/ActivityManager_internal.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1813651324/ActivityManager_internal.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1813651324/ActivityManager_internal.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1813651324/BlockExch.o: ../src/ActivityManager/BlockExch.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1813651324" 
	@${RM} ${OBJECTDIR}/_ext/1813651324/BlockExch.o.d 
	@${RM} ${OBJECTDIR}/_ext/1813651324/BlockExch.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ActivityManager/BlockExch.c  -o ${OBJECTDIR}/_ext/1813651324/BlockExch.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1813651324/BlockExch.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1813651324/BlockExch.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1031660192/AnalogInput.o: ../src/AnalogInput/AnalogInput.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1031660192" 
	@${RM} ${OBJECTDIR}/_ext/1031660192/AnalogInput.o.d 
	@${RM} ${OBJECTDIR}/_ext/1031660192/AnalogInput.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/AnalogInput/AnalogInput.c  -o ${OBJECTDIR}/_ext/1031660192/AnalogInput.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1031660192/AnalogInput.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1031660192/AnalogInput.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1031660192/Adc.o: ../src/AnalogInput/Adc.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1031660192" 
	@${RM} ${OBJECTDIR}/_ext/1031660192/Adc.o.d 
	@${RM} ${OBJECTDIR}/_ext/1031660192/Adc.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/AnalogInput/Adc.c  -o ${OBJECTDIR}/_ext/1031660192/Adc.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1031660192/Adc.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1031660192/Adc.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1031660192/DigitalFilter.o: ../src/AnalogInput/DigitalFilter.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1031660192" 
	@${RM} ${OBJECTDIR}/_ext/1031660192/DigitalFilter.o.d 
	@${RM} ${OBJECTDIR}/_ext/1031660192/DigitalFilter.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/AnalogInput/DigitalFilter.c  -o ${OBJECTDIR}/_ext/1031660192/DigitalFilter.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1031660192/DigitalFilter.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1031660192/DigitalFilter.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/161780206/AnalogMeasurement.o: ../src/AnalogMeasurement/AnalogMeasurement.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/161780206" 
	@${RM} ${OBJECTDIR}/_ext/161780206/AnalogMeasurement.o.d 
	@${RM} ${OBJECTDIR}/_ext/161780206/AnalogMeasurement.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/AnalogMeasurement/AnalogMeasurement.c  -o ${OBJECTDIR}/_ext/161780206/AnalogMeasurement.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/161780206/AnalogMeasurement.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/161780206/AnalogMeasurement.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1567386894/BinIn.o: ../src/BinIn/BinIn.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1567386894" 
	@${RM} ${OBJECTDIR}/_ext/1567386894/BinIn.o.d 
	@${RM} ${OBJECTDIR}/_ext/1567386894/BinIn.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/BinIn/BinIn.c  -o ${OBJECTDIR}/_ext/1567386894/BinIn.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1567386894/BinIn.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1567386894/BinIn.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1119163098/BlackBox.o: ../src/BlackBox/BlackBox.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1119163098" 
	@${RM} ${OBJECTDIR}/_ext/1119163098/BlackBox.o.d 
	@${RM} ${OBJECTDIR}/_ext/1119163098/BlackBox.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/BlackBox/BlackBox.c  -o ${OBJECTDIR}/_ext/1119163098/BlackBox.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1119163098/BlackBox.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1119163098/BlackBox.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1684595197/CheckSupply.o: ../src/CheckSupply/CheckSupply.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1684595197" 
	@${RM} ${OBJECTDIR}/_ext/1684595197/CheckSupply.o.d 
	@${RM} ${OBJECTDIR}/_ext/1684595197/CheckSupply.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/CheckSupply/CheckSupply.c  -o ${OBJECTDIR}/_ext/1684595197/CheckSupply.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1684595197/CheckSupply.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1684595197/CheckSupply.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/633884614/ConfigMK.o: ../src/ConfigMK/ConfigMK.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/633884614" 
	@${RM} ${OBJECTDIR}/_ext/633884614/ConfigMK.o.d 
	@${RM} ${OBJECTDIR}/_ext/633884614/ConfigMK.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ConfigMK/ConfigMK.c  -o ${OBJECTDIR}/_ext/633884614/ConfigMK.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/633884614/ConfigMK.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/633884614/ConfigMK.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1959551838/Tracing.o: ../src/DebugTools/Tracing.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1959551838" 
	@${RM} ${OBJECTDIR}/_ext/1959551838/Tracing.o.d 
	@${RM} ${OBJECTDIR}/_ext/1959551838/Tracing.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/DebugTools/Tracing.c  -o ${OBJECTDIR}/_ext/1959551838/Tracing.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1959551838/Tracing.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1959551838/Tracing.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1959551838/DebugTools.o: ../src/DebugTools/DebugTools.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1959551838" 
	@${RM} ${OBJECTDIR}/_ext/1959551838/DebugTools.o.d 
	@${RM} ${OBJECTDIR}/_ext/1959551838/DebugTools.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/DebugTools/DebugTools.c  -o ${OBJECTDIR}/_ext/1959551838/DebugTools.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1959551838/DebugTools.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99
	@${FIXDEPS} "${OBJECTDIR}/_ext/1959551838/DebugTools.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/970708260/deviceAddress.o: ../src/DeviceAddress/deviceAddress.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/970708260" 
	@${RM} ${OBJECTDIR}/_ext/970708260/deviceAddress.o.d 
	@${RM} ${OBJECTDIR}/_ext/970708260/deviceAddress.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/DeviceAddress/deviceAddress.c  -o ${OBJECTDIR}/_ext/970708260/deviceAddress.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/970708260/deviceAddress.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/970708260/deviceAddress.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659853742/turnOffdsPIC33.o: ../src/drv/turnOffdsPIC33.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659853742" 
	@${RM} ${OBJECTDIR}/_ext/659853742/turnOffdsPIC33.o.d 
	@${RM} ${OBJECTDIR}/_ext/659853742/turnOffdsPIC33.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/drv/turnOffdsPIC33.c  -o ${OBJECTDIR}/_ext/659853742/turnOffdsPIC33.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659853742/turnOffdsPIC33.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/659853742/turnOffdsPIC33.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659853742/AdcDriver_PIC33.o: ../src/drv/AdcDriver_PIC33.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659853742" 
	@${RM} ${OBJECTDIR}/_ext/659853742/AdcDriver_PIC33.o.d 
	@${RM} ${OBJECTDIR}/_ext/659853742/AdcDriver_PIC33.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/drv/AdcDriver_PIC33.c  -o ${OBJECTDIR}/_ext/659853742/AdcDriver_PIC33.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659853742/AdcDriver_PIC33.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/659853742/AdcDriver_PIC33.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659853742/checkRegistersDrv.o: ../src/drv/checkRegistersDrv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659853742" 
	@${RM} ${OBJECTDIR}/_ext/659853742/checkRegistersDrv.o.d 
	@${RM} ${OBJECTDIR}/_ext/659853742/checkRegistersDrv.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/drv/checkRegistersDrv.c  -o ${OBJECTDIR}/_ext/659853742/checkRegistersDrv.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659853742/checkRegistersDrv.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/659853742/checkRegistersDrv.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659853742/Ecan_driverPIC33.o: ../src/drv/Ecan_driverPIC33.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659853742" 
	@${RM} ${OBJECTDIR}/_ext/659853742/Ecan_driverPIC33.o.d 
	@${RM} ${OBJECTDIR}/_ext/659853742/Ecan_driverPIC33.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/drv/Ecan_driverPIC33.c  -o ${OBJECTDIR}/_ext/659853742/Ecan_driverPIC33.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659853742/Ecan_driverPIC33.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/659853742/Ecan_driverPIC33.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659853742/EcanAck_driverPIC33.o: ../src/drv/EcanAck_driverPIC33.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659853742" 
	@${RM} ${OBJECTDIR}/_ext/659853742/EcanAck_driverPIC33.o.d 
	@${RM} ${OBJECTDIR}/_ext/659853742/EcanAck_driverPIC33.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/drv/EcanAck_driverPIC33.c  -o ${OBJECTDIR}/_ext/659853742/EcanAck_driverPIC33.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659853742/EcanAck_driverPIC33.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/659853742/EcanAck_driverPIC33.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659853742/EepromDrv.o: ../src/drv/EepromDrv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659853742" 
	@${RM} ${OBJECTDIR}/_ext/659853742/EepromDrv.o.d 
	@${RM} ${OBJECTDIR}/_ext/659853742/EepromDrv.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/drv/EepromDrv.c  -o ${OBJECTDIR}/_ext/659853742/EepromDrv.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659853742/EepromDrv.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/659853742/EepromDrv.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659853742/I2Cdrv_PIC33.o: ../src/drv/I2Cdrv_PIC33.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659853742" 
	@${RM} ${OBJECTDIR}/_ext/659853742/I2Cdrv_PIC33.o.d 
	@${RM} ${OBJECTDIR}/_ext/659853742/I2Cdrv_PIC33.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/drv/I2Cdrv_PIC33.c  -o ${OBJECTDIR}/_ext/659853742/I2Cdrv_PIC33.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659853742/I2Cdrv_PIC33.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/659853742/I2Cdrv_PIC33.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659853742/dsPIC33_rs422.o: ../src/drv/dsPIC33_rs422.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659853742" 
	@${RM} ${OBJECTDIR}/_ext/659853742/dsPIC33_rs422.o.d 
	@${RM} ${OBJECTDIR}/_ext/659853742/dsPIC33_rs422.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/drv/dsPIC33_rs422.c  -o ${OBJECTDIR}/_ext/659853742/dsPIC33_rs422.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659853742/dsPIC33_rs422.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/659853742/dsPIC33_rs422.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659853742/shuntShiftGenDrv.o: ../src/drv/shuntShiftGenDrv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659853742" 
	@${RM} ${OBJECTDIR}/_ext/659853742/shuntShiftGenDrv.o.d 
	@${RM} ${OBJECTDIR}/_ext/659853742/shuntShiftGenDrv.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/drv/shuntShiftGenDrv.c  -o ${OBJECTDIR}/_ext/659853742/shuntShiftGenDrv.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659853742/shuntShiftGenDrv.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/659853742/shuntShiftGenDrv.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659853742/posDetGen_drv.o: ../src/drv/posDetGen_drv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659853742" 
	@${RM} ${OBJECTDIR}/_ext/659853742/posDetGen_drv.o.d 
	@${RM} ${OBJECTDIR}/_ext/659853742/posDetGen_drv.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/drv/posDetGen_drv.c  -o ${OBJECTDIR}/_ext/659853742/posDetGen_drv.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659853742/posDetGen_drv.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/659853742/posDetGen_drv.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659853742/relayCtrlDrv.o: ../src/drv/relayCtrlDrv.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659853742" 
	@${RM} ${OBJECTDIR}/_ext/659853742/relayCtrlDrv.o.d 
	@${RM} ${OBJECTDIR}/_ext/659853742/relayCtrlDrv.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/drv/relayCtrlDrv.c  -o ${OBJECTDIR}/_ext/659853742/relayCtrlDrv.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659853742/relayCtrlDrv.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/659853742/relayCtrlDrv.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659853742/BlockExch_driver.o: ../src/drv/BlockExch_driver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659853742" 
	@${RM} ${OBJECTDIR}/_ext/659853742/BlockExch_driver.o.d 
	@${RM} ${OBJECTDIR}/_ext/659853742/BlockExch_driver.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/drv/BlockExch_driver.c  -o ${OBJECTDIR}/_ext/659853742/BlockExch_driver.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659853742/BlockExch_driver.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/659853742/BlockExch_driver.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1262060966/Eeprom.o: ../src/Eeprom/Eeprom.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1262060966" 
	@${RM} ${OBJECTDIR}/_ext/1262060966/Eeprom.o.d 
	@${RM} ${OBJECTDIR}/_ext/1262060966/Eeprom.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Eeprom/Eeprom.c  -o ${OBJECTDIR}/_ext/1262060966/Eeprom.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1262060966/Eeprom.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1262060966/Eeprom.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/659824934/Fsm.o: ../src/Fsm/Fsm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/659824934" 
	@${RM} ${OBJECTDIR}/_ext/659824934/Fsm.o.d 
	@${RM} ${OBJECTDIR}/_ext/659824934/Fsm.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Fsm/Fsm.c  -o ${OBJECTDIR}/_ext/659824934/Fsm.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/659824934/Fsm.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/659824934/Fsm.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1951852232/Sheduler.o: ../src/FsmStates/Sheduler.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1951852232" 
	@${RM} ${OBJECTDIR}/_ext/1951852232/Sheduler.o.d 
	@${RM} ${OBJECTDIR}/_ext/1951852232/Sheduler.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/FsmStates/Sheduler.c  -o ${OBJECTDIR}/_ext/1951852232/Sheduler.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1951852232/Sheduler.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1951852232/Sheduler.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1951852232/StateActive.o: ../src/FsmStates/StateActive.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1951852232" 
	@${RM} ${OBJECTDIR}/_ext/1951852232/StateActive.o.d 
	@${RM} ${OBJECTDIR}/_ext/1951852232/StateActive.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/FsmStates/StateActive.c  -o ${OBJECTDIR}/_ext/1951852232/StateActive.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1951852232/StateActive.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1951852232/StateActive.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1951852232/StateInitial.o: ../src/FsmStates/StateInitial.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1951852232" 
	@${RM} ${OBJECTDIR}/_ext/1951852232/StateInitial.o.d 
	@${RM} ${OBJECTDIR}/_ext/1951852232/StateInitial.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/FsmStates/StateInitial.c  -o ${OBJECTDIR}/_ext/1951852232/StateInitial.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1951852232/StateInitial.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1951852232/StateInitial.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1951852232/StateMain.o: ../src/FsmStates/StateMain.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1951852232" 
	@${RM} ${OBJECTDIR}/_ext/1951852232/StateMain.o.d 
	@${RM} ${OBJECTDIR}/_ext/1951852232/StateMain.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/FsmStates/StateMain.c  -o ${OBJECTDIR}/_ext/1951852232/StateMain.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1951852232/StateMain.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1951852232/StateMain.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1951852232/StatePassive.o: ../src/FsmStates/StatePassive.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1951852232" 
	@${RM} ${OBJECTDIR}/_ext/1951852232/StatePassive.o.d 
	@${RM} ${OBJECTDIR}/_ext/1951852232/StatePassive.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/FsmStates/StatePassive.c  -o ${OBJECTDIR}/_ext/1951852232/StatePassive.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1951852232/StatePassive.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1951852232/StatePassive.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1951852232/StateSafeActive.o: ../src/FsmStates/StateSafeActive.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1951852232" 
	@${RM} ${OBJECTDIR}/_ext/1951852232/StateSafeActive.o.d 
	@${RM} ${OBJECTDIR}/_ext/1951852232/StateSafeActive.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/FsmStates/StateSafeActive.c  -o ${OBJECTDIR}/_ext/1951852232/StateSafeActive.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1951852232/StateSafeActive.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1951852232/StateSafeActive.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1951852232/StateSafePassive.o: ../src/FsmStates/StateSafePassive.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1951852232" 
	@${RM} ${OBJECTDIR}/_ext/1951852232/StateSafePassive.o.d 
	@${RM} ${OBJECTDIR}/_ext/1951852232/StateSafePassive.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/FsmStates/StateSafePassive.c  -o ${OBJECTDIR}/_ext/1951852232/StateSafePassive.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1951852232/StateSafePassive.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1951852232/StateSafePassive.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1951852232/StateTop.o: ../src/FsmStates/StateTop.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1951852232" 
	@${RM} ${OBJECTDIR}/_ext/1951852232/StateTop.o.d 
	@${RM} ${OBJECTDIR}/_ext/1951852232/StateTop.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/FsmStates/StateTop.c  -o ${OBJECTDIR}/_ext/1951852232/StateTop.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1951852232/StateTop.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1951852232/StateTop.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1951852232/StateWorking.o: ../src/FsmStates/StateWorking.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1951852232" 
	@${RM} ${OBJECTDIR}/_ext/1951852232/StateWorking.o.d 
	@${RM} ${OBJECTDIR}/_ext/1951852232/StateWorking.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/FsmStates/StateWorking.c  -o ${OBJECTDIR}/_ext/1951852232/StateWorking.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1951852232/StateWorking.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1951852232/StateWorking.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/355834263/HystFltr.o: ../src/HysteresisFilter/HystFltr.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/355834263" 
	@${RM} ${OBJECTDIR}/_ext/355834263/HystFltr.o.d 
	@${RM} ${OBJECTDIR}/_ext/355834263/HystFltr.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/HysteresisFilter/HystFltr.c  -o ${OBJECTDIR}/_ext/355834263/HystFltr.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/355834263/HystFltr.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/355834263/HystFltr.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/706196810/Indication.o: ../src/Indication/Indication.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/706196810" 
	@${RM} ${OBJECTDIR}/_ext/706196810/Indication.o.d 
	@${RM} ${OBJECTDIR}/_ext/706196810/Indication.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Indication/Indication.c  -o ${OBJECTDIR}/_ext/706196810/Indication.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/706196810/Indication.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/706196810/Indication.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/706196810/LedFailure.o: ../src/Indication/LedFailure.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/706196810" 
	@${RM} ${OBJECTDIR}/_ext/706196810/LedFailure.o.d 
	@${RM} ${OBJECTDIR}/_ext/706196810/LedFailure.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Indication/LedFailure.c  -o ${OBJECTDIR}/_ext/706196810/LedFailure.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/706196810/LedFailure.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/706196810/LedFailure.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/962885590/CheckCauseReset.o: ../src/Initial/CheckCauseReset.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/962885590" 
	@${RM} ${OBJECTDIR}/_ext/962885590/CheckCauseReset.o.d 
	@${RM} ${OBJECTDIR}/_ext/962885590/CheckCauseReset.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Initial/CheckCauseReset.c  -o ${OBJECTDIR}/_ext/962885590/CheckCauseReset.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/962885590/CheckCauseReset.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/962885590/CheckCauseReset.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/962885590/IdentHex.o: ../src/Initial/IdentHex.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/962885590" 
	@${RM} ${OBJECTDIR}/_ext/962885590/IdentHex.o.d 
	@${RM} ${OBJECTDIR}/_ext/962885590/IdentHex.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Initial/IdentHex.c  -o ${OBJECTDIR}/_ext/962885590/IdentHex.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/962885590/IdentHex.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/962885590/IdentHex.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/962885590/TimeBeginSynchronization.o: ../src/Initial/TimeBeginSynchronization.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/962885590" 
	@${RM} ${OBJECTDIR}/_ext/962885590/TimeBeginSynchronization.o.d 
	@${RM} ${OBJECTDIR}/_ext/962885590/TimeBeginSynchronization.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Initial/TimeBeginSynchronization.c  -o ${OBJECTDIR}/_ext/962885590/TimeBeginSynchronization.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/962885590/TimeBeginSynchronization.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/962885590/TimeBeginSynchronization.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2012282586/IntegrCtrl.o: ../src/IntegrCtrl/IntegrCtrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2012282586" 
	@${RM} ${OBJECTDIR}/_ext/2012282586/IntegrCtrl.o.d 
	@${RM} ${OBJECTDIR}/_ext/2012282586/IntegrCtrl.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/IntegrCtrl/IntegrCtrl.c  -o ${OBJECTDIR}/_ext/2012282586/IntegrCtrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2012282586/IntegrCtrl.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2012282586/IntegrCtrl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2012282586/IntegrCtrlGen.o: ../src/IntegrCtrl/IntegrCtrlGen.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2012282586" 
	@${RM} ${OBJECTDIR}/_ext/2012282586/IntegrCtrlGen.o.d 
	@${RM} ${OBJECTDIR}/_ext/2012282586/IntegrCtrlGen.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/IntegrCtrl/IntegrCtrlGen.c  -o ${OBJECTDIR}/_ext/2012282586/IntegrCtrlGen.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2012282586/IntegrCtrlGen.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2012282586/IntegrCtrlGen.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1093587103/InterChannel.o: ../src/InterChannel/InterChannel.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1093587103" 
	@${RM} ${OBJECTDIR}/_ext/1093587103/InterChannel.o.d 
	@${RM} ${OBJECTDIR}/_ext/1093587103/InterChannel.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/InterChannel/InterChannel.c  -o ${OBJECTDIR}/_ext/1093587103/InterChannel.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1093587103/InterChannel.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1093587103/InterChannel.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1093587103/log2.o: ../src/InterChannel/log2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1093587103" 
	@${RM} ${OBJECTDIR}/_ext/1093587103/log2.o.d 
	@${RM} ${OBJECTDIR}/_ext/1093587103/log2.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/InterChannel/log2.c  -o ${OBJECTDIR}/_ext/1093587103/log2.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1093587103/log2.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1093587103/log2.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1093587103/pwr2.o: ../src/InterChannel/pwr2.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1093587103" 
	@${RM} ${OBJECTDIR}/_ext/1093587103/pwr2.o.d 
	@${RM} ${OBJECTDIR}/_ext/1093587103/pwr2.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/InterChannel/pwr2.c  -o ${OBJECTDIR}/_ext/1093587103/pwr2.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1093587103/pwr2.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1093587103/pwr2.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1020072301/ControlSystem.o: ../src/Main/ControlSystem.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1020072301" 
	@${RM} ${OBJECTDIR}/_ext/1020072301/ControlSystem.o.d 
	@${RM} ${OBJECTDIR}/_ext/1020072301/ControlSystem.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Main/ControlSystem.c  -o ${OBJECTDIR}/_ext/1020072301/ControlSystem.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1020072301/ControlSystem.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mlarge-code -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1020072301/ControlSystem.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1020072301/InterruptsHandlers.o: ../src/Main/InterruptsHandlers.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1020072301" 
	@${RM} ${OBJECTDIR}/_ext/1020072301/InterruptsHandlers.o.d 
	@${RM} ${OBJECTDIR}/_ext/1020072301/InterruptsHandlers.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Main/InterruptsHandlers.c  -o ${OBJECTDIR}/_ext/1020072301/InterruptsHandlers.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1020072301/InterruptsHandlers.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1020072301/InterruptsHandlers.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1020072301/UnusedInterrupts.o: ../src/Main/UnusedInterrupts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1020072301" 
	@${RM} ${OBJECTDIR}/_ext/1020072301/UnusedInterrupts.o.d 
	@${RM} ${OBJECTDIR}/_ext/1020072301/UnusedInterrupts.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Main/UnusedInterrupts.c  -o ${OBJECTDIR}/_ext/1020072301/UnusedInterrupts.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1020072301/UnusedInterrupts.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1020072301/UnusedInterrupts.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/985603786/ModeProtection.o: ../src/ModeProtection/ModeProtection.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/985603786" 
	@${RM} ${OBJECTDIR}/_ext/985603786/ModeProtection.o.d 
	@${RM} ${OBJECTDIR}/_ext/985603786/ModeProtection.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ModeProtection/ModeProtection.c  -o ${OBJECTDIR}/_ext/985603786/ModeProtection.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/985603786/ModeProtection.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/985603786/ModeProtection.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/887427039/OverloadDet.o: ../src/OverloadDet/OverloadDet.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/887427039" 
	@${RM} ${OBJECTDIR}/_ext/887427039/OverloadDet.o.d 
	@${RM} ${OBJECTDIR}/_ext/887427039/OverloadDet.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/OverloadDet/OverloadDet.c  -o ${OBJECTDIR}/_ext/887427039/OverloadDet.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/887427039/OverloadDet.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/887427039/OverloadDet.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/777634832/posDetGenerator.o: ../src/PositionDet/posDetGenerator.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/777634832" 
	@${RM} ${OBJECTDIR}/_ext/777634832/posDetGenerator.o.d 
	@${RM} ${OBJECTDIR}/_ext/777634832/posDetGenerator.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/PositionDet/posDetGenerator.c  -o ${OBJECTDIR}/_ext/777634832/posDetGenerator.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/777634832/posDetGenerator.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/777634832/posDetGenerator.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/777634832/positionDetFiveEC.o: ../src/PositionDet/positionDetFiveEC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/777634832" 
	@${RM} ${OBJECTDIR}/_ext/777634832/positionDetFiveEC.o.d 
	@${RM} ${OBJECTDIR}/_ext/777634832/positionDetFiveEC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/PositionDet/positionDetFiveEC.c  -o ${OBJECTDIR}/_ext/777634832/positionDetFiveEC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/777634832/positionDetFiveEC.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/777634832/positionDetFiveEC.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/777634832/positionDetNineWire.o: ../src/PositionDet/positionDetNineWire.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/777634832" 
	@${RM} ${OBJECTDIR}/_ext/777634832/positionDetNineWire.o.d 
	@${RM} ${OBJECTDIR}/_ext/777634832/positionDetNineWire.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/PositionDet/positionDetNineWire.c  -o ${OBJECTDIR}/_ext/777634832/positionDetNineWire.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/777634832/positionDetNineWire.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/777634832/positionDetNineWire.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/777634832/positionDet.o: ../src/PositionDet/positionDet.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/777634832" 
	@${RM} ${OBJECTDIR}/_ext/777634832/positionDet.o.d 
	@${RM} ${OBJECTDIR}/_ext/777634832/positionDet.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/PositionDet/positionDet.c  -o ${OBJECTDIR}/_ext/777634832/positionDet.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/777634832/positionDet.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/777634832/positionDet.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1670244350/relayCtrl.o: ../src/RelayCtrl/relayCtrl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1670244350" 
	@${RM} ${OBJECTDIR}/_ext/1670244350/relayCtrl.o.d 
	@${RM} ${OBJECTDIR}/_ext/1670244350/relayCtrl.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/RelayCtrl/relayCtrl.c  -o ${OBJECTDIR}/_ext/1670244350/relayCtrl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1670244350/relayCtrl.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1670244350/relayCtrl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1670244350/relayCtrlFiveEC.o: ../src/RelayCtrl/relayCtrlFiveEC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1670244350" 
	@${RM} ${OBJECTDIR}/_ext/1670244350/relayCtrlFiveEC.o.d 
	@${RM} ${OBJECTDIR}/_ext/1670244350/relayCtrlFiveEC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/RelayCtrl/relayCtrlFiveEC.c  -o ${OBJECTDIR}/_ext/1670244350/relayCtrlFiveEC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1670244350/relayCtrlFiveEC.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1670244350/relayCtrlFiveEC.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1670244350/relayCtrlNineWire.o: ../src/RelayCtrl/relayCtrlNineWire.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1670244350" 
	@${RM} ${OBJECTDIR}/_ext/1670244350/relayCtrlNineWire.o.d 
	@${RM} ${OBJECTDIR}/_ext/1670244350/relayCtrlNineWire.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/RelayCtrl/relayCtrlNineWire.c  -o ${OBJECTDIR}/_ext/1670244350/relayCtrlNineWire.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1670244350/relayCtrlNineWire.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1670244350/relayCtrlNineWire.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1553322471/Rs422.o: ../src/RS422/Rs422.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1553322471" 
	@${RM} ${OBJECTDIR}/_ext/1553322471/Rs422.o.d 
	@${RM} ${OBJECTDIR}/_ext/1553322471/Rs422.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/RS422/Rs422.c  -o ${OBJECTDIR}/_ext/1553322471/Rs422.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1553322471/Rs422.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1553322471/Rs422.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1553322471/Rs422_crc8.o: ../src/RS422/Rs422_crc8.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1553322471" 
	@${RM} ${OBJECTDIR}/_ext/1553322471/Rs422_crc8.o.d 
	@${RM} ${OBJECTDIR}/_ext/1553322471/Rs422_crc8.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/RS422/Rs422_crc8.c  -o ${OBJECTDIR}/_ext/1553322471/Rs422_crc8.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1553322471/Rs422_crc8.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1553322471/Rs422_crc8.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1553322471/Rs422_lineExch.o: ../src/RS422/Rs422_lineExch.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1553322471" 
	@${RM} ${OBJECTDIR}/_ext/1553322471/Rs422_lineExch.o.d 
	@${RM} ${OBJECTDIR}/_ext/1553322471/Rs422_lineExch.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/RS422/Rs422_lineExch.c  -o ${OBJECTDIR}/_ext/1553322471/Rs422_lineExch.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1553322471/Rs422_lineExch.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1553322471/Rs422_lineExch.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1553322471/Rs422_transceiver.o: ../src/RS422/Rs422_transceiver.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1553322471" 
	@${RM} ${OBJECTDIR}/_ext/1553322471/Rs422_transceiver.o.d 
	@${RM} ${OBJECTDIR}/_ext/1553322471/Rs422_transceiver.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/RS422/Rs422_transceiver.c  -o ${OBJECTDIR}/_ext/1553322471/Rs422_transceiver.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1553322471/Rs422_transceiver.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -std=gnu99  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1553322471/Rs422_transceiver.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1645518876/SafetyPowerControl.o: ../src/SafetyPowerControl/SafetyPowerControl.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1645518876" 
	@${RM} ${OBJECTDIR}/_ext/1645518876/SafetyPowerControl.o.d 
	@${RM} ${OBJECTDIR}/_ext/1645518876/SafetyPowerControl.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/SafetyPowerControl/SafetyPowerControl.c  -o ${OBJECTDIR}/_ext/1645518876/SafetyPowerControl.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1645518876/SafetyPowerControl.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mlarge-code -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/BinIn" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -msmart-io=1 -Wall -msfr-warn=off  
	@${FIXDEPS} "${OBJECTDIR}/_ext/1645518876/SafetyPowerControl.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/237575434/ShuntShiftGen.o: ../src/ShuntShift/ShuntShiftGen.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/237575434" 
	@${RM} ${OBJECTDIR}/_ext/237575434/ShuntShiftGen.o.d 
	@${RM} ${OBJECTDIR}/_ext/237575434/ShuntShiftGen.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ShuntShift/ShuntShiftGen.c  -o ${OBJECTDIR}/_ext/237575434/ShuntShiftGen.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/237575434/ShuntShiftGen.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/237575434/ShuntShiftGen.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/237575434/shuntShift.o: ../src/ShuntShift/shuntShift.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/237575434" 
	@${RM} ${OBJECTDIR}/_ext/237575434/shuntShift.o.d 
	@${RM} ${OBJECTDIR}/_ext/237575434/shuntShift.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ShuntShift/shuntShift.c  -o ${OBJECTDIR}/_ext/237575434/shuntShift.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/237575434/shuntShift.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/237575434/shuntShift.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/237575434/shuntShiftFiveEC.o: ../src/ShuntShift/shuntShiftFiveEC.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/237575434" 
	@${RM} ${OBJECTDIR}/_ext/237575434/shuntShiftFiveEC.o.d 
	@${RM} ${OBJECTDIR}/_ext/237575434/shuntShiftFiveEC.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ShuntShift/shuntShiftFiveEC.c  -o ${OBJECTDIR}/_ext/237575434/shuntShiftFiveEC.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/237575434/shuntShiftFiveEC.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/237575434/shuntShiftFiveEC.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/237575434/shuntShiftMotor.o: ../src/ShuntShift/shuntShiftMotor.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/237575434" 
	@${RM} ${OBJECTDIR}/_ext/237575434/shuntShiftMotor.o.d 
	@${RM} ${OBJECTDIR}/_ext/237575434/shuntShiftMotor.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ShuntShift/shuntShiftMotor.c  -o ${OBJECTDIR}/_ext/237575434/shuntShiftMotor.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/237575434/shuntShiftMotor.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/237575434/shuntShiftMotor.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/237575434/shuntShiftNineWire.o: ../src/ShuntShift/shuntShiftNineWire.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/237575434" 
	@${RM} ${OBJECTDIR}/_ext/237575434/shuntShiftNineWire.o.d 
	@${RM} ${OBJECTDIR}/_ext/237575434/shuntShiftNineWire.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/ShuntShift/shuntShiftNineWire.c  -o ${OBJECTDIR}/_ext/237575434/shuntShiftNineWire.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/237575434/shuntShiftNineWire.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/237575434/shuntShiftNineWire.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/2018284246/asserts.o: ../src/systems/asserts.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2018284246" 
	@${RM} ${OBJECTDIR}/_ext/2018284246/asserts.o.d 
	@${RM} ${OBJECTDIR}/_ext/2018284246/asserts.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/systems/asserts.c  -o ${OBJECTDIR}/_ext/2018284246/asserts.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/2018284246/asserts.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2018284246/asserts.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/38706442/CheckCallFunctions.o: ../src/Testing/CheckCallFunctions.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/38706442" 
	@${RM} ${OBJECTDIR}/_ext/38706442/CheckCallFunctions.o.d 
	@${RM} ${OBJECTDIR}/_ext/38706442/CheckCallFunctions.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Testing/CheckCallFunctions.c  -o ${OBJECTDIR}/_ext/38706442/CheckCallFunctions.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/38706442/CheckCallFunctions.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/38706442/CheckCallFunctions.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/38706442/CheckRAM.o: ../src/Testing/CheckRAM.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/38706442" 
	@${RM} ${OBJECTDIR}/_ext/38706442/CheckRAM.o.d 
	@${RM} ${OBJECTDIR}/_ext/38706442/CheckRAM.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Testing/CheckRAM.c  -o ${OBJECTDIR}/_ext/38706442/CheckRAM.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/38706442/CheckRAM.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/38706442/CheckRAM.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/38706442/CheckRegisters.o: ../src/Testing/CheckRegisters.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/38706442" 
	@${RM} ${OBJECTDIR}/_ext/38706442/CheckRegisters.o.d 
	@${RM} ${OBJECTDIR}/_ext/38706442/CheckRegisters.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Testing/CheckRegisters.c  -o ${OBJECTDIR}/_ext/38706442/CheckRegisters.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/38706442/CheckRegisters.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/38706442/CheckRegisters.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/38706442/ControlMK.o: ../src/Testing/ControlMK.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/38706442" 
	@${RM} ${OBJECTDIR}/_ext/38706442/ControlMK.o.d 
	@${RM} ${OBJECTDIR}/_ext/38706442/ControlMK.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Testing/ControlMK.c  -o ${OBJECTDIR}/_ext/38706442/ControlMK.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/38706442/ControlMK.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/38706442/ControlMK.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/38706442/FlashCheck.o: ../src/Testing/FlashCheck.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/38706442" 
	@${RM} ${OBJECTDIR}/_ext/38706442/FlashCheck.o.d 
	@${RM} ${OBJECTDIR}/_ext/38706442/FlashCheck.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Testing/FlashCheck.c  -o ${OBJECTDIR}/_ext/38706442/FlashCheck.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/38706442/FlashCheck.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/38706442/FlashCheck.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1268761170/wait.o: ../src/Utility/wait.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1268761170" 
	@${RM} ${OBJECTDIR}/_ext/1268761170/wait.o.d 
	@${RM} ${OBJECTDIR}/_ext/1268761170/wait.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Utility/wait.c  -o ${OBJECTDIR}/_ext/1268761170/wait.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1268761170/wait.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1268761170/wait.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1268761170/CopySwap.o: ../src/Utility/CopySwap.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1268761170" 
	@${RM} ${OBJECTDIR}/_ext/1268761170/CopySwap.o.d 
	@${RM} ${OBJECTDIR}/_ext/1268761170/CopySwap.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Utility/CopySwap.c  -o ${OBJECTDIR}/_ext/1268761170/CopySwap.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1268761170/CopySwap.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1268761170/CopySwap.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
${OBJECTDIR}/_ext/1020072301/Main.o: ../src/Main/Main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1020072301" 
	@${RM} ${OBJECTDIR}/_ext/1020072301/Main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1020072301/Main.o 
	${MP_CC} $(MP_EXTRA_CC_PRE)  ../src/Main/Main.c  -o ${OBJECTDIR}/_ext/1020072301/Main.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1020072301/Main.o.d"      -mno-eds-warn  -g -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -mconst-in-code -O3 -I"../src" -I"../src/drv" -I"../src/systems" -I"../src/ActivityManager" -I"../src/AnalogInput" -I"../src/AnalogMeasurement" -I"../src/OverloadDet" -I"../src/BlackBox" -I"../src/CheckSupply" -I"../src/ConfigMK" -I"../src/DebugTools" -I"../src/DeviceAddress" -I"../src/Eeprom" -I"../src/Fsm" -I"../src/FsmStates" -I"../src/HysteresisFilter" -I"../src/Indication" -I"../src/InterChannel" -I"../src/Main" -I"../src/Initial" -I"../src/ModeProtection" -I"../src/PositionDet" -I"../src/RelayCtrl" -I"../src/RS422" -I"../src/SafetyPowerControl" -I"../src/ShuntShift" -I"../src/Testing" -I"../src/Utility" -I"../src/Testing/ControlMK" -I"../src/IntegrCtrl" -I"../src/BinIn" -msmart-io=1 -Wall -msfr-warn=off   -std=gnu99 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1020072301/Main.o.d" $(SILENT)  -rsi ${MP_CC_DIR}../ 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/1031660192/dsPIC33_FIR_filter.o: ../src/AnalogInput/dsPIC33_FIR_filter.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1031660192" 
	@${RM} ${OBJECTDIR}/_ext/1031660192/dsPIC33_FIR_filter.o.d 
	@${RM} ${OBJECTDIR}/_ext/1031660192/dsPIC33_FIR_filter.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../src/AnalogInput/dsPIC33_FIR_filter.s  -o ${OBJECTDIR}/_ext/1031660192/dsPIC33_FIR_filter.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  -Wa,-MD,"${OBJECTDIR}/_ext/1031660192/dsPIC33_FIR_filter.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1031660192/dsPIC33_FIR_filter.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1567386894/BinInDecoder.o: ../src/BinIn/BinInDecoder.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1567386894" 
	@${RM} ${OBJECTDIR}/_ext/1567386894/BinInDecoder.o.d 
	@${RM} ${OBJECTDIR}/_ext/1567386894/BinInDecoder.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../src/BinIn/BinInDecoder.s  -o ${OBJECTDIR}/_ext/1567386894/BinInDecoder.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  -Wa,-MD,"${OBJECTDIR}/_ext/1567386894/BinInDecoder.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1567386894/BinInDecoder.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/38706442/CheckCPU.o: ../src/Testing/CheckCPU.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/38706442" 
	@${RM} ${OBJECTDIR}/_ext/38706442/CheckCPU.o.d 
	@${RM} ${OBJECTDIR}/_ext/38706442/CheckCPU.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../src/Testing/CheckCPU.s  -o ${OBJECTDIR}/_ext/38706442/CheckCPU.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  -Wa,-MD,"${OBJECTDIR}/_ext/38706442/CheckCPU.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/38706442/CheckCPU.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1268761170/SqrtTOpt.o: ../src/Utility/SqrtTOpt.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1268761170" 
	@${RM} ${OBJECTDIR}/_ext/1268761170/SqrtTOpt.o.d 
	@${RM} ${OBJECTDIR}/_ext/1268761170/SqrtTOpt.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../src/Utility/SqrtTOpt.s  -o ${OBJECTDIR}/_ext/1268761170/SqrtTOpt.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  -Wa,-MD,"${OBJECTDIR}/_ext/1268761170/SqrtTOpt.o.d",--defsym=__MPLAB_BUILD=1,--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1268761170/SqrtTOpt.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
else
${OBJECTDIR}/_ext/1031660192/dsPIC33_FIR_filter.o: ../src/AnalogInput/dsPIC33_FIR_filter.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1031660192" 
	@${RM} ${OBJECTDIR}/_ext/1031660192/dsPIC33_FIR_filter.o.d 
	@${RM} ${OBJECTDIR}/_ext/1031660192/dsPIC33_FIR_filter.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../src/AnalogInput/dsPIC33_FIR_filter.s  -o ${OBJECTDIR}/_ext/1031660192/dsPIC33_FIR_filter.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  -Wa,-MD,"${OBJECTDIR}/_ext/1031660192/dsPIC33_FIR_filter.o.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1031660192/dsPIC33_FIR_filter.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1567386894/BinInDecoder.o: ../src/BinIn/BinInDecoder.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1567386894" 
	@${RM} ${OBJECTDIR}/_ext/1567386894/BinInDecoder.o.d 
	@${RM} ${OBJECTDIR}/_ext/1567386894/BinInDecoder.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../src/BinIn/BinInDecoder.s  -o ${OBJECTDIR}/_ext/1567386894/BinInDecoder.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  -Wa,-MD,"${OBJECTDIR}/_ext/1567386894/BinInDecoder.o.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1567386894/BinInDecoder.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/38706442/CheckCPU.o: ../src/Testing/CheckCPU.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/38706442" 
	@${RM} ${OBJECTDIR}/_ext/38706442/CheckCPU.o.d 
	@${RM} ${OBJECTDIR}/_ext/38706442/CheckCPU.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../src/Testing/CheckCPU.s  -o ${OBJECTDIR}/_ext/38706442/CheckCPU.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  -Wa,-MD,"${OBJECTDIR}/_ext/38706442/CheckCPU.o.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/38706442/CheckCPU.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
${OBJECTDIR}/_ext/1268761170/SqrtTOpt.o: ../src/Utility/SqrtTOpt.s  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1268761170" 
	@${RM} ${OBJECTDIR}/_ext/1268761170/SqrtTOpt.o.d 
	@${RM} ${OBJECTDIR}/_ext/1268761170/SqrtTOpt.o 
	${MP_CC} $(MP_EXTRA_AS_PRE)  ../src/Utility/SqrtTOpt.s  -o ${OBJECTDIR}/_ext/1268761170/SqrtTOpt.o  -c -mcpu=$(MP_PROCESSOR_OPTION)  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  -Wa,-MD,"${OBJECTDIR}/_ext/1268761170/SqrtTOpt.o.d",--defsym=__MPLAB_BUILD=1,-g,--no-relax,-g$(MP_EXTRA_AS_POST)
	@${FIXDEPS} "${OBJECTDIR}/_ext/1268761170/SqrtTOpt.o.d"  $(SILENT)  -rsi ${MP_CC_DIR}../  
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemblePreproc
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/OKPS-EK.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/OKPS-EK.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -D__DEBUG=__DEBUG -D__MPLAB_DEBUGGER_PK3=1  -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)   -mreserve=data@0x1000:0x101B -mreserve=data@0x101C:0x101D -mreserve=data@0x101E:0x101F -mreserve=data@0x1020:0x1021 -mreserve=data@0x1022:0x1023 -mreserve=data@0x1024:0x1027 -mreserve=data@0x1028:0x104F   -Wl,--local-stack,,--defsym=__MPLAB_BUILD=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,-D__DEBUG=__DEBUG,--defsym=__MPLAB_DEBUGGER_PK3=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST) 
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/OKPS-EK.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -o dist/${CND_CONF}/${IMAGE_TYPE}/OKPS-EK.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX}  ${OBJECTFILES_QUOTED_IF_SPACED}      -mcpu=$(MP_PROCESSOR_OPTION)        -omf=elf -DXPRJ_default=$(CND_CONF)  -legacy-libc  $(COMPARISON_BUILD)  -Wl,--local-stack,,--defsym=__MPLAB_BUILD=1,$(MP_LINKER_FILE_OPTION),--stack=16,--check-sections,--data-init,--pack-data,--handles,--isr,--no-gc-sections,--fill-upper=0,--stackguard=16,--no-force-link,--smart-io,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--report-mem,--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml$(MP_EXTRA_LD_POST) 
	${MP_CC_DIR}\\xc16-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/OKPS-EK.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -a  -omf=elf  
	
	@echo "Calculating checksum"
	@${MP_JAVA_PATH}java -jar "C:/Program Files (x86)/Microchip/MPLABX/v5.15/mplab_platform/platform/../mplab_ide/modules/../../bin/checksum.jar" -i dsPIC33EP512GM710 dist/${CND_CONF}/${IMAGE_TYPE}/OKPS-EK.X.${IMAGE_TYPE}.hex 2> NUL 1> NUL

endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
