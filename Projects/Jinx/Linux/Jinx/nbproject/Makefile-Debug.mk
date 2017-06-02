#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/5555977b/JxBuffer.o \
	${OBJECTDIR}/_ext/5555977b/JxCollection.o \
	${OBJECTDIR}/_ext/5555977b/JxCommon.o \
	${OBJECTDIR}/_ext/5555977b/JxConversion.o \
	${OBJECTDIR}/_ext/5555977b/JxFunctionSignature.o \
	${OBJECTDIR}/_ext/5555977b/JxFunctionTable.o \
	${OBJECTDIR}/_ext/5555977b/JxHash.o \
	${OBJECTDIR}/_ext/5555977b/JxLexer.o \
	${OBJECTDIR}/_ext/5555977b/JxLibCore.o \
	${OBJECTDIR}/_ext/5555977b/JxLibrary.o \
	${OBJECTDIR}/_ext/5555977b/JxLogging.o \
	${OBJECTDIR}/_ext/5555977b/JxMemory.o \
	${OBJECTDIR}/_ext/5555977b/JxMutex.o \
	${OBJECTDIR}/_ext/5555977b/JxParser.o \
	${OBJECTDIR}/_ext/5555977b/JxPropertyName.o \
	${OBJECTDIR}/_ext/5555977b/JxRuntime.o \
	${OBJECTDIR}/_ext/5555977b/JxScript.o \
	${OBJECTDIR}/_ext/5555977b/JxSerialize.o \
	${OBJECTDIR}/_ext/5555977b/JxUnicode.o \
	${OBJECTDIR}/_ext/5555977b/JxUnicodeCaseFolding.o \
	${OBJECTDIR}/_ext/5555977b/JxVariableStackFrame.o \
	${OBJECTDIR}/_ext/5555977b/JxVariant.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libjinx.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libjinx.a: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libjinx.a
	${AR} -rv ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libjinx.a ${OBJECTFILES} 
	$(RANLIB) ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libjinx.a

${OBJECTDIR}/_ext/5555977b/JxBuffer.o: ../../../../Source/JxBuffer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5555977b
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5555977b/JxBuffer.o ../../../../Source/JxBuffer.cpp

${OBJECTDIR}/_ext/5555977b/JxCollection.o: ../../../../Source/JxCollection.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5555977b
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5555977b/JxCollection.o ../../../../Source/JxCollection.cpp

${OBJECTDIR}/_ext/5555977b/JxCommon.o: ../../../../Source/JxCommon.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5555977b
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5555977b/JxCommon.o ../../../../Source/JxCommon.cpp

${OBJECTDIR}/_ext/5555977b/JxConversion.o: ../../../../Source/JxConversion.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5555977b
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5555977b/JxConversion.o ../../../../Source/JxConversion.cpp

${OBJECTDIR}/_ext/5555977b/JxFunctionSignature.o: ../../../../Source/JxFunctionSignature.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5555977b
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5555977b/JxFunctionSignature.o ../../../../Source/JxFunctionSignature.cpp

${OBJECTDIR}/_ext/5555977b/JxFunctionTable.o: ../../../../Source/JxFunctionTable.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5555977b
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5555977b/JxFunctionTable.o ../../../../Source/JxFunctionTable.cpp

${OBJECTDIR}/_ext/5555977b/JxHash.o: ../../../../Source/JxHash.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5555977b
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5555977b/JxHash.o ../../../../Source/JxHash.cpp

${OBJECTDIR}/_ext/5555977b/JxLexer.o: ../../../../Source/JxLexer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5555977b
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5555977b/JxLexer.o ../../../../Source/JxLexer.cpp

${OBJECTDIR}/_ext/5555977b/JxLibCore.o: ../../../../Source/JxLibCore.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5555977b
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5555977b/JxLibCore.o ../../../../Source/JxLibCore.cpp

${OBJECTDIR}/_ext/5555977b/JxLibrary.o: ../../../../Source/JxLibrary.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5555977b
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5555977b/JxLibrary.o ../../../../Source/JxLibrary.cpp

${OBJECTDIR}/_ext/5555977b/JxLogging.o: ../../../../Source/JxLogging.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5555977b
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5555977b/JxLogging.o ../../../../Source/JxLogging.cpp

${OBJECTDIR}/_ext/5555977b/JxMemory.o: ../../../../Source/JxMemory.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5555977b
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5555977b/JxMemory.o ../../../../Source/JxMemory.cpp

${OBJECTDIR}/_ext/5555977b/JxMutex.o: ../../../../Source/JxMutex.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5555977b
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5555977b/JxMutex.o ../../../../Source/JxMutex.cpp

${OBJECTDIR}/_ext/5555977b/JxParser.o: ../../../../Source/JxParser.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5555977b
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5555977b/JxParser.o ../../../../Source/JxParser.cpp

${OBJECTDIR}/_ext/5555977b/JxPropertyName.o: ../../../../Source/JxPropertyName.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5555977b
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5555977b/JxPropertyName.o ../../../../Source/JxPropertyName.cpp

${OBJECTDIR}/_ext/5555977b/JxRuntime.o: ../../../../Source/JxRuntime.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5555977b
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5555977b/JxRuntime.o ../../../../Source/JxRuntime.cpp

${OBJECTDIR}/_ext/5555977b/JxScript.o: ../../../../Source/JxScript.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5555977b
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5555977b/JxScript.o ../../../../Source/JxScript.cpp

${OBJECTDIR}/_ext/5555977b/JxSerialize.o: ../../../../Source/JxSerialize.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5555977b
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5555977b/JxSerialize.o ../../../../Source/JxSerialize.cpp

${OBJECTDIR}/_ext/5555977b/JxUnicode.o: ../../../../Source/JxUnicode.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5555977b
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5555977b/JxUnicode.o ../../../../Source/JxUnicode.cpp

${OBJECTDIR}/_ext/5555977b/JxUnicodeCaseFolding.o: ../../../../Source/JxUnicodeCaseFolding.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5555977b
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5555977b/JxUnicodeCaseFolding.o ../../../../Source/JxUnicodeCaseFolding.cpp

${OBJECTDIR}/_ext/5555977b/JxVariableStackFrame.o: ../../../../Source/JxVariableStackFrame.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5555977b
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5555977b/JxVariableStackFrame.o ../../../../Source/JxVariableStackFrame.cpp

${OBJECTDIR}/_ext/5555977b/JxVariant.o: ../../../../Source/JxVariant.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/5555977b
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/5555977b/JxVariant.o ../../../../Source/JxVariant.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/libjinx.a

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
