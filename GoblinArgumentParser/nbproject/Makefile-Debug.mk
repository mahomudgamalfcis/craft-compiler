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
CND_PLATFORM=Cygwin-Windows
CND_DLIB_EXT=dll
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/GoblinArgumentParser.o \
	${OBJECTDIR}/src/Argument.o \
	${OBJECTDIR}/src/ArgumentContainer.o


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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ../bin/GoblinArgumentParser.${CND_DLIB_EXT}

../bin/GoblinArgumentParser.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ../bin
	${LINK.cc} -o ../bin/GoblinArgumentParser.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -shared

${OBJECTDIR}/GoblinArgumentParser.o: GoblinArgumentParser.cpp
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -DDEBUG_MODE -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/GoblinArgumentParser.o GoblinArgumentParser.cpp

${OBJECTDIR}/src/Argument.o: src/Argument.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DDEBUG_MODE -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/Argument.o src/Argument.cpp

${OBJECTDIR}/src/ArgumentContainer.o: src/ArgumentContainer.cpp
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -DDEBUG_MODE -Iinclude -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/ArgumentContainer.o src/ArgumentContainer.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
