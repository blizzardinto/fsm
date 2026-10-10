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
INCLUDES := -I. -Icommon -Ientity -Ifsm -Igame -Imath -Imessaging
CXXFLAGS := -std=c++17 -O2 -Wall -Wno-sign-compare -Wno-unused-variable -fexec-charset=GBK $(INCLUDES)
LDFLAGS  := -mwindows -lgdi32 -luser32 -lwinmm -static

# Build output directory (.o, .res, .exe)
OBJ_DIR  := obj

SRC := \
  common/Cgdi.cpp \
  common/DebugConsole.cpp \
  common/FrameCounter.cpp \
  common/iniFileLoaderBase.cpp \
  common/PrecisionTimer.cpp \
  common/WindowUtils.cpp \
  entity/EntityBase.cpp \
  entity/EntityManager.cpp \
  entity/EntityPlayer.cpp \
  entity/EntityPlayerGoalkeeper.cpp \
  entity/EntityPlayerOnField.cpp \
  fsm/StatesPlayerGoalKeeper.cpp \
  fsm/StatesPlayerOnField.cpp \
  fsm/StatesTeam.cpp \
  game/Goal.cpp \
  game/ParamLoader.cpp \
  game/SoccerBall.cpp \
  game/SoccerPitch.cpp \
  game/SoccerTeam.cpp \
  game/SteeringBehaviors.cpp \
  game/SupportSpotCalculator.cpp \
  math/C2DMatrix.cpp \
  math/geometry.cpp \
  math/Transformations.cpp \
  math/Vector2d.cpp \
  messaging/MessageDispatcher.cpp \
  messaging/SoccerMessages.cpp \
  main.cpp

OBJ := $(addprefix $(OBJ_DIR)/, $(notdir $(SRC:.cpp=.o)))
RES := $(OBJ_DIR)/Script1.res

TARGET := $(OBJ_DIR)/SimpleSoccer.exe

vpath %.cpp common entity fsm game math messaging .

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJ) $(RES)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ) $(RES) $(LDFLAGS)

$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(RES): res/Script1.rc res/resource.h res/icon1.ico | $(OBJ_DIR)
	$(WINDRES) -Ires -i res/Script1.rc -o $@ -O coff

$(OBJ_DIR):
	@if not exist "$(OBJ_DIR)" mkdir "$(OBJ_DIR)"

clean:
	-if exist "$(OBJ_DIR)" rmdir /s /q "$(OBJ_DIR)"

run: $(TARGET)
	"$(subst /,\,$(TARGET))"
