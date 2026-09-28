# Makefile for SimpleSoccer (FSM football simulation)
# Built with MSYS2 MinGW-w64 (UCRT toolchain). Fully static-linked, so the
# produced exe has no MinGW runtime DLL dependencies.

# Auto-detect MSYS2 UCRT64 toolchain path (D:/msys64 or C:/msys64)
ifeq ($(wildcard D:/msys64/ucrt64/bin/g++.exe),)
  MINGW_BIN := C:/msys64/ucrt64/bin
else
  MINGW_BIN := D:/msys64/ucrt64/bin
endif

# Put the toolchain dir on PATH so g++ can find its sub-tools (cc1plus, as, ld).
export PATH := $(MINGW_BIN);$(PATH)

CXX      := $(MINGW_BIN)/g++
WINDRES  := $(MINGW_BIN)/windres
CXXFLAGS := -std=c++17 -O2 -Wall -Wno-sign-compare -Wno-unused-variable -fexec-charset=GBK
LDFLAGS  := -mwindows -lgdi32 -luser32 -lwinmm -static

# Intermediate build objects directory (.o, .res)
OBJ_DIR  := obj

SRC := \
  Cgdi.cpp \
  DebugConsole.cpp \
  EntityBase.cpp \
  EntityManager.cpp \
  EntityPlayer.cpp \
  EntityPlayerGoalkeeper.cpp \
  EntityPlayerOnField.cpp \
  FrameCounter.cpp \
  Goal.cpp \
  iniFileLoaderBase.cpp \
  MessageDispatcher.cpp \
  ParamLoader.cpp \
  Pathfinder.cpp \
  PrecisionTimer.cpp \
  SoccerBall.cpp \
  SoccerMessages.cpp \
  SoccerPitch.cpp \
  SoccerTeam.cpp \
  StatesPlayerGoalKeeper.cpp \
  StatesPlayerOnField.cpp \
  StatesTeam.cpp \
  SteeringBehaviors.cpp \
  SupportSpotCalculator.cpp \
  Vector2d.cpp \
  WindowUtils.cpp \
  main.cpp

OBJ := $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRC))
RES := $(OBJ_DIR)/Script1.res

TARGET := SimpleSoccer.exe

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJ) $(RES)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) $(RES) $(LDFLAGS)

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(RES): Script1.rc resource.h icon1.ico | $(OBJ_DIR)
	$(WINDRES) -i Script1.rc -o $@ -O coff

$(OBJ_DIR):
	@if not exist "$(OBJ_DIR)" mkdir "$(OBJ_DIR)"

clean:
	-if exist "$(OBJ_DIR)" rmdir /s /q "$(OBJ_DIR)"
	-if exist "$(TARGET)" del /q "$(TARGET)" 2>nul

run: $(TARGET)
	$(TARGET)
