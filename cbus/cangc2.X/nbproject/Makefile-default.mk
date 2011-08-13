#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
include Makefile

# Environment
MKDIR=mkdir -p
RM=rm -f 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/cangc2.X.${IMAGE_TYPE}.cof
else
IMAGE_TYPE=production
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/cangc2.X.${IMAGE_TYPE}.cof
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Object Files
OBJECTFILES=${OBJECTDIR}/cangc2.o ${OBJECTDIR}/cbus_common.o


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

# Path to java used to run MPLAB X when this makefile was created
MP_JAVA_PATH=/usr/lib/jvm/java-6-openjdk/jre/bin/
OS_CURRENT="$(shell uname -s)"
############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
MP_CC=/opt/microchip/mplabc18/v3.40/bin/mcc18
# MP_BC is not defined
MP_AS=/opt/microchip/mplabc18/v3.40/bin/../mpasm/MPASMWIN
MP_LD=/opt/microchip/mplabc18/v3.40/bin/mplink
MP_AR=/opt/microchip/mplabc18/v3.40/bin/mplib
# MP_BC is not defined
MP_CC_DIR=/opt/microchip/mplabc18/v3.40/bin
# MP_BC_DIR is not defined
MP_AS_DIR=/opt/microchip/mplabc18/v3.40/bin/../mpasm
MP_LD_DIR=/opt/microchip/mplabc18/v3.40/bin
MP_AR_DIR=/opt/microchip/mplabc18/v3.40/bin
# MP_BC_DIR is not defined

# This makefile will use a C preprocessor to generate dependency files
MP_CPP=/opt/microchip/mplabx/mplab_ide/mplab_ide/modules/../../bin/mplab-cpp

.build-conf: ${BUILD_SUBPROJECTS}
	${MAKE}  -f nbproject/Makefile-default.mk dist/${CND_CONF}/${IMAGE_TYPE}/cangc2.X.${IMAGE_TYPE}.cof

MP_PROCESSOR_OPTION=18F2480
MP_PROCESSOR_OPTION_LD=18f2480
MP_LINKER_DEBUG_OPTION= -u_DEBUGCODESTART=0x3dc0 -u_DEBUGCODELEN=0x240 -u_DEBUGDATASTART=0x2f4 -u_DEBUGDATALEN=0xb
# ------------------------------------------------------------------------------------
# Rules for buildStep: createRevGrep
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
__revgrep__:   nbproject/Makefile-${CND_CONF}.mk
	@echo 'grep -q $$@' > __revgrep__
	@echo 'if [ "$$?" -ne "0" ]; then' >> __revgrep__
	@echo '  exit 0' >> __revgrep__
	@echo 'else' >> __revgrep__
	@echo '  exit 1' >> __revgrep__
	@echo 'fi' >> __revgrep__
	@chmod +x __revgrep__
else
__revgrep__:   nbproject/Makefile-${CND_CONF}.mk
	@echo 'grep -q $$@' > __revgrep__
	@echo 'if [ "$$?" -ne "0" ]; then' >> __revgrep__
	@echo '  exit 0' >> __revgrep__
	@echo 'else' >> __revgrep__
	@echo '  exit 1' >> __revgrep__
	@echo 'fi' >> __revgrep__
	@chmod +x __revgrep__
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/cangc2.o: cangc2.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/cangc2.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION)   -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/cangc2.o   cangc2.c  > ${OBJECTDIR}/cangc2.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/cangc2.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/cangc2.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/cangc2.o.temp cangc2.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/cangc2.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/cangc2.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/cangc2.o.d
else
	cat ${OBJECTDIR}/cangc2.o.temp >> ${OBJECTDIR}/cangc2.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/cbus_common.o: cbus_common.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/cbus_common.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -p$(MP_PROCESSOR_OPTION)   -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/cbus_common.o   cbus_common.c  > ${OBJECTDIR}/cbus_common.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/cbus_common.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/cbus_common.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/cbus_common.o.temp cbus_common.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/cbus_common.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/cbus_common.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/cbus_common.o.d
else
	cat ${OBJECTDIR}/cbus_common.o.temp >> ${OBJECTDIR}/cbus_common.o.d
endif
	${RM} __temp_cpp_output__
else
${OBJECTDIR}/cangc2.o: cangc2.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/cangc2.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION)   -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/cangc2.o   cangc2.c  > ${OBJECTDIR}/cangc2.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/cangc2.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/cangc2.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/cangc2.o.temp cangc2.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/cangc2.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/cangc2.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/cangc2.o.d
else
	cat ${OBJECTDIR}/cangc2.o.temp >> ${OBJECTDIR}/cangc2.o.d
endif
	${RM} __temp_cpp_output__
${OBJECTDIR}/cbus_common.o: cbus_common.c  nbproject/Makefile-${CND_CONF}.mk
	${RM} ${OBJECTDIR}/cbus_common.o.d 
	${MKDIR} ${OBJECTDIR} 
	${MP_CC} $(MP_EXTRA_CC_PRE) -p$(MP_PROCESSOR_OPTION)   -I ${MP_CC_DIR}/../h  -fo ${OBJECTDIR}/cbus_common.o   cbus_common.c  > ${OBJECTDIR}/cbus_common.err 2>&1 ; if [ $$? -eq 0 ] ; then cat ${OBJECTDIR}/cbus_common.err | sed 's/\(^.*:.*:\)\(Warning\)\(.*$$\)/\1 \2:\3/g' ; else cat ${OBJECTDIR}/cbus_common.err | sed 's/\(^.*:.*:\)\(Error\)\(.*$$\)/\1 \2:\3/g' ; exit 1 ; fi
	${MP_CPP}  -MMD ${OBJECTDIR}/cbus_common.o.temp cbus_common.c __temp_cpp_output__ -D __18F2480 -D __18CXX -I /opt/microchip/mplabc18/v3.40/bin/../h  -D__18F2480
	printf "%s/" ${OBJECTDIR} > ${OBJECTDIR}/cbus_common.o.d
ifneq (,$(findstring MINGW32,$(OS_CURRENT)))
	cat ${OBJECTDIR}/cbus_common.o.temp | sed -e 's/\\\ /__SPACES__/g' -e's/\\$$/__EOL__/g' -e 's/\\/\//g' -e 's/__SPACES__/\\\ /g' -e 's/__EOL__/\\/g' >> ${OBJECTDIR}/cbus_common.o.d
else
	cat ${OBJECTDIR}/cbus_common.o.temp >> ${OBJECTDIR}/cbus_common.o.d
endif
	${RM} __temp_cpp_output__
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/cangc2.X.${IMAGE_TYPE}.cof: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE)   -p$(MP_PROCESSOR_OPTION_LD)  -w -x -u_DEBUG   -z__MPLAB_BUILD=1  -u_CRUNTIME -z__MPLAB_DEBUG=1 -z__MPLAB_DEBUGGER_PK3=1 $(MP_LINKER_DEBUG_OPTION) -l ${MP_CC_DIR}/../lib  -odist/${CND_CONF}/${IMAGE_TYPE}/cangc2.X.${IMAGE_TYPE}.cof ${OBJECTFILES}      
else
dist/${CND_CONF}/${IMAGE_TYPE}/cangc2.X.${IMAGE_TYPE}.cof: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk
	${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_LD} $(MP_EXTRA_LD_PRE)   -p$(MP_PROCESSOR_OPTION_LD)  -w    -z__MPLAB_BUILD=1  -u_CRUNTIME -l ${MP_CC_DIR}/../lib  -odist/${CND_CONF}/${IMAGE_TYPE}/cangc2.X.${IMAGE_TYPE}.cof ${OBJECTFILES}      
endif


# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf:
	${RM} -r build/default
	${RM} -r dist/default

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc