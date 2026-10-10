# SimpleSoccer 构建规则：编译源码、资源以及状态机测试。
# 使用 MSYS2 的 UCRT 工具链，并采用静态链接。
# 输出程序无需附带 MinGW 运行时动态库。

# 自动选择 MSYS2 UCRT64 工具链位置。
ifeq ($(wildcard D:/msys64/ucrt64/bin/g++.exe),)
  MINGW_BIN := C:/msys64/ucrt64/bin
else
  MINGW_BIN := D:/msys64/ucrt64/bin
endif

# 把工具链目录加入搜索路径，供编译器查找汇编器和链接器等工具。
export PATH := $(MINGW_BIN);$(PATH)

CXX      := $(MINGW_BIN)/g++
WINDRES  := $(MINGW_BIN)/windres
INCLUDES := -I. -Iai -Icommon -Ientity -Ifsm -Igame -Imath -Imessaging
CXXFLAGS := -std=c++17 -O2 -Wall -Wno-sign-compare -Wno-unused-variable -fexec-charset=GBK $(INCLUDES)
LDFLAGS  := -mwindows -lgdi32 -luser32 -lwinmm -static

# 构建产物集中放在 obj 中，包含对象文件、编译后的资源和可执行文件。
OBJ_DIR  := obj

SRC := \
  ai/FieldPlayerAI.cpp \
  ai/GoalkeeperAI.cpp \
  ai/TeamAI.cpp \
  ai/StatesPlayerGoalKeeper.cpp \
  ai/StatesPlayerOnField.cpp \
  ai/StatesTeam.cpp \
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
  fsm/State.cpp \
  fsm/StateMachine.cpp \
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
FSM_TEST := $(OBJ_DIR)/stateMachineTest.exe
AI_TEST := $(OBJ_DIR)/aiIntegrationTest.exe
SIM_OBJ := $(filter-out $(OBJ_DIR)/main.o,$(OBJ))

vpath %.cpp ai common entity fsm game math messaging .

.PHONY: all clean run test

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

$(FSM_TEST): tests/stateMachineTest.cpp fsm/State.cpp fsm/StateMachine.cpp fsm/State.h fsm/StateMachine.h messaging/Telegram.h | $(OBJ_DIR)
	$(CXX) -std=c++17 -Wall -Ifsm -Imessaging tests/stateMachineTest.cpp fsm/State.cpp fsm/StateMachine.cpp -static -o $@

$(AI_TEST): tests/aiIntegrationTest.cpp $(SIM_OBJ) | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) tests/aiIntegrationTest.cpp $(SIM_OBJ) -o $@ $(filter-out -mwindows,$(LDFLAGS)) -lcomdlg32

test: $(FSM_TEST) $(AI_TEST)
	"$(subst /,\,$(FSM_TEST))"
	"$(subst /,\,$(AI_TEST))"
