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
	${OBJECTDIR}/_ext/1ad155ca/Main.o \
	${OBJECTDIR}/_ext/1ad155ca/TestCasts.o \
	${OBJECTDIR}/_ext/1ad155ca/TestCollections.o \
	${OBJECTDIR}/_ext/1ad155ca/TestErrors.o \
	${OBJECTDIR}/_ext/1ad155ca/TestFunctions.o \
	${OBJECTDIR}/_ext/1ad155ca/TestIfElse.o \
	${OBJECTDIR}/_ext/1ad155ca/TestLibCore.o \
	${OBJECTDIR}/_ext/1ad155ca/TestLibraries.o \
	${OBJECTDIR}/_ext/1ad155ca/TestLoops.o \
	${OBJECTDIR}/_ext/1ad155ca/TestNative.o \
	${OBJECTDIR}/_ext/1ad155ca/TestStatements.o \
	${OBJECTDIR}/_ext/1ad155ca/TestUnicode.o \
	${OBJECTDIR}/_ext/1ad155ca/UnitTest.o


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
LDLIBSOPTIONS=../../../../Jinx/Linux/Jinx/dist/Debug/GNU-Linux/libjinx.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/unittests

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/unittests: ../../../../Jinx/Linux/Jinx/dist/Debug/GNU-Linux/libjinx.a

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/unittests: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/unittests ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/1ad155ca/Main.o: ../../../Source/Main.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1ad155ca
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1ad155ca/Main.o ../../../Source/Main.cpp

${OBJECTDIR}/_ext/1ad155ca/TestCasts.o: ../../../Source/TestCasts.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1ad155ca
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1ad155ca/TestCasts.o ../../../Source/TestCasts.cpp

${OBJECTDIR}/_ext/1ad155ca/TestCollections.o: ../../../Source/TestCollections.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1ad155ca
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1ad155ca/TestCollections.o ../../../Source/TestCollections.cpp

${OBJECTDIR}/_ext/1ad155ca/TestErrors.o: ../../../Source/TestErrors.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1ad155ca
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1ad155ca/TestErrors.o ../../../Source/TestErrors.cpp

${OBJECTDIR}/_ext/1ad155ca/TestFunctions.o: ../../../Source/TestFunctions.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1ad155ca
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1ad155ca/TestFunctions.o ../../../Source/TestFunctions.cpp

${OBJECTDIR}/_ext/1ad155ca/TestIfElse.o: ../../../Source/TestIfElse.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1ad155ca
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1ad155ca/TestIfElse.o ../../../Source/TestIfElse.cpp

${OBJECTDIR}/_ext/1ad155ca/TestLibCore.o: ../../../Source/TestLibCore.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1ad155ca
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1ad155ca/TestLibCore.o ../../../Source/TestLibCore.cpp

${OBJECTDIR}/_ext/1ad155ca/TestLibraries.o: ../../../Source/TestLibraries.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1ad155ca
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1ad155ca/TestLibraries.o ../../../Source/TestLibraries.cpp

${OBJECTDIR}/_ext/1ad155ca/TestLoops.o: ../../../Source/TestLoops.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1ad155ca
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1ad155ca/TestLoops.o ../../../Source/TestLoops.cpp

${OBJECTDIR}/_ext/1ad155ca/TestNative.o: ../../../Source/TestNative.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1ad155ca
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1ad155ca/TestNative.o ../../../Source/TestNative.cpp

${OBJECTDIR}/_ext/1ad155ca/TestStatements.o: ../../../Source/TestStatements.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1ad155ca
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1ad155ca/TestStatements.o ../../../Source/TestStatements.cpp

${OBJECTDIR}/_ext/1ad155ca/TestUnicode.o: ../../../Source/TestUnicode.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1ad155ca
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1ad155ca/TestUnicode.o ../../../Source/TestUnicode.cpp

${OBJECTDIR}/_ext/1ad155ca/UnitTest.o: ../../../Source/UnitTest.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1ad155ca
	${RM} "$@.d"
	$(COMPILE.cc) -g -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1ad155ca/UnitTest.o ../../../Source/UnitTest.cpp

# Subprojects
.build-subprojects:
	cd ../../../../Jinx/Linux/Jinx && ${MAKE}  -f Makefile CONF=Debug

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/unittests

# Subprojects
.clean-subprojects:
	cd ../../../../Jinx/Linux/Jinx && ${MAKE}  -f Makefile CONF=Debug clean

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
