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
CND_PLATFORM=GNU-Linux-x86
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
	${OBJECTDIR}/ComparisonPredicates.o \
	${OBJECTDIR}/EliminateDuplicatesImpl.o \
	${OBJECTDIR}/HashJoinImpl.o \
	${OBJECTDIR}/MergeJoinImpl.o \
	${OBJECTDIR}/MergeSortImpl.o \
	${OBJECTDIR}/Tests.o \
	${OBJECTDIR}/dbtproj.o \
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-std=c++0x
CXXFLAGS=-std=c++0x

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/database-storage-implementations

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/database-storage-implementations: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/database-storage-implementations ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/ComparisonPredicates.o: ComparisonPredicates.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ComparisonPredicates.o ComparisonPredicates.cpp

${OBJECTDIR}/EliminateDuplicatesImpl.o: EliminateDuplicatesImpl.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/EliminateDuplicatesImpl.o EliminateDuplicatesImpl.cpp

${OBJECTDIR}/HashJoinImpl.o: HashJoinImpl.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/HashJoinImpl.o HashJoinImpl.cpp

${OBJECTDIR}/MergeJoinImpl.o: MergeJoinImpl.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MergeJoinImpl.o MergeJoinImpl.cpp

${OBJECTDIR}/MergeSortImpl.o: MergeSortImpl.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/MergeSortImpl.o MergeSortImpl.cpp

${OBJECTDIR}/Tests.o: Tests.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Tests.o Tests.cpp

${OBJECTDIR}/dbtproj.o: dbtproj.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/dbtproj.o dbtproj.cpp

${OBJECTDIR}/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/database-storage-implementations

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc