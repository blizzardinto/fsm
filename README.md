# SimpleSoccer - 基于有限状态机 (FSM) 的足球 AI 仿真系统

本项目是基于 C++ 实现的二维自主智能体足球比赛模拟系统（源自 Mat Buckland 经典著作《Programming Game AI by Example》）。系统融合了**分层有限状态机 (Hierarchical FSM)**、**自主智能体操纵行为 (Steering Behaviors)**、**事件驱动的消息通信体系 (Telegram & MessageDispatcher)** 以及**战术支援点计算算法**，完整模拟了足球比赛中的带球、传球、射门、守门、防守盯人与无球跑动支援等群体协作行为。

---

## 目录

- [核心特性](#核心特性)
- [目录与文件结构](#目录与文件结构)
- [编译与运行指南](#编译与运行指南)
- [操作与快捷键](#操作与快捷键)
- [配置参数说明 (`Params.ini`)](#配置参数说明-paramsini)
- [系统架构简述](#系统架构简述)
- [架构设计文档](#架构设计文档)

---

## 核心特性

1. **分层有限状态机 (Hierarchical FSM)**
   - **球队层级**：负责红蓝两队宏观战术状态切换（开球准备 `PrepareForKickOff`、防守 `Defending`、进攻 `Attacking`）。
   - **场上球员层级**：独立管理球员个体行为（等待 `Wait`、追球 `ChaseBall`、盘带 `Dribble`、踢球 `KickBall`、接球 `ReceiveBall`、跑位支援 `SupportAttacker`、回位 `ReturnToHomeRegion` 等）。
   - **门将层级**：专注于守门员专有战术（守门 `TendGoal`、出击拦截 `InterceptBall`、门前重置 `ReturnHome`、开球门球 `PutBallBackInPlay`）。
2. **自主智能体操纵行为 (Steering Behaviors)**
   - 采用 Reynolds 操纵行为模型，提供精准平滑的运动模拟：寻找 (`Seek`)、到达 (`Arrive`)、拦截追击 (`Pursuit`)、队员分离防挤压 (`Separation`)、插足卡位 (`Interpose`)。
3. **消息驱动通信体系 (Message-Driven Architecture)**
   - 实体间通过轻量级 `Telegram` 通信，支持即时发送与延迟优先队列调度，解耦智能体之间的交互逻辑（如传球呼叫 `Msg_PassToMe`、支援通知 `Msg_SupportAttacker`、回防指令 `Msg_GoHome`）。
4. **战术决策与几何计算**
   - **支援点计算器 (`SupportSpotCalculator`)**：在球场网格内实时评估无球跑动支援点得分（结合传球安全度与射门开阔度）。
   - **传球安全检测**：通过切线和线段相交几何算法，智能计算对手拦截风险并选出最佳传球目标。
   - **足球物理与轨迹预估**：模拟球体滑动摩擦、边界反弹碰撞以及根据踢球力度预测未来到达时间与拦截点。

---

## 目录与文件结构

项目代码已按照职责驱动的架构规范划分为清晰的子模块目录，实现接口（`.h`）与实现（`.cpp`）的解耦：

```text
d:\Code\fsm\
├── common/                             # 基础工具与运行时支持
│   ├── Cgdi.h / Cgdi.cpp               # Windows GDI 绘图封装与画笔/画刷渲染工具
│   ├── DebugConsole.h / .cpp           # 调试控制台输出窗口
│   ├── FrameCounter.h / .cpp           # 帧率统计器
│   ├── PrecisionTimer.h / .cpp         # 高精度时间计数器
│   ├── WindowUtils.h / .cpp            # Win32 窗口辅助工具
│   ├── iniFileLoaderBase.h / .cpp      # INI 配置文件解析基类
│   ├── utils.h                         # 常用数学函数与随机数生成
│   ├── constants.h                     # 全局常量定义
│   ├── autolist.h                      # 自动链表模板基类
│   ├── Regulator.h                     # 行为更新频率调节器
│   └── Stream_Utility_Functions.h      # 输入输出流辅助函数
├── entity/                             # 游戏实体抽象与足球角色实现
│   ├── EntityBase.h / EntityBase.cpp   # 游戏实体抽象基类
│   ├── EntityMovable.h                 # 移动实体基类（物理属性：质量、速度、推力等）
│   ├── EntityManager.h / .cpp          # 实体注册与全局单例管理器
│   ├── EntityPlayer.h / .cpp           # 球员通用基类
│   ├── EntityPlayerGoalkeeper.h / .cpp # 守门员智能体实现
│   ├── EntityPlayerOnField.h / .cpp    # 场上球员智能体实现
│   └── EntityFunctionTemplates.h       # 实体泛型辅助模板（按距离排序、区域判定等）
├── fsm/                                # 有限状态机引擎与具体行为状态
│   ├── State.h                         # 状态抽象接口模板类
│   ├── StateMachine.h                  # 通用有限状态机调度核心模板类
│   ├── StatesPlayerGoalKeeper.h / .cpp # 守门员具体状态实现（守门、拦截出击、门前归位等）
│   ├── StatesPlayerOnField.h / .cpp    # 场上球员具体状态实现（追球、盘带、接球、支援等）
│   └── StatesTeam.h / StatesTeam.cpp   # 球队宏观战术状态（准备开球、进攻、防守）
├── game/                               # 足球比赛业务领域与战术决策
│   ├── SoccerPitch.h / SoccerPitch.cpp # 足球场地主控类（驱动各系统运转、渲染赛场）
│   ├── SoccerBall.h / SoccerBall.cpp   # 足球实体、物理运动与反弹碰撞检测
│   ├── SoccerTeam.h / SoccerTeam.cpp   # 球队协同管理、传球路由判定与射门策略
│   ├── Goal.h / Goal.cpp               # 球门实体与进球判定
│   ├── SteeringBehaviors.h / .cpp      # Reynolds 操纵行为力学计算类（寻路、拦截、避让等）
│   ├── SupportSpotCalculator.h / .cpp  # 进攻跑位支援点评估计算器（评分网格）
│   └── ParamLoader.h / ParamLoader.cpp # 参数配置加载器（单例 Prm，解析 Params.ini）
├── graph/                              # 导航图与路径搜索算法
│   ├── SparseGraph.h                   # 2D/3D 稀疏图数据结构
│   ├── Pathfinder.h / Pathfinder.cpp   # 路径搜索寻路器（A* 等算法封装）
│   ├── PriorityQueue.h                 # 优先队列模板（支持索引优先队列）
│   ├── GraphAlgorithms.h               # 图搜索算法库（A*、Dijkstra、BFS、DFS）
│   ├── GraphEdgeTypes.h / GraphNodeTypes.h # 图节点与图边数据结构
│   ├── HandyGraphFunctions.h           # 图构建与辅助函数
│   ├── NodeTypeEnumerations.h          # 节点类型枚举
│   └── AStarHeuristicPolicies.h        # A* 启发式估价策略
├── math/                               # 2D 几何与数学基础库
│   ├── Vector2D.h / Vector2d.cpp       # 2D 向量计算类（点积、求模、归一化、操作符重载）
│   ├── geometry.h / geometry.cpp       # 2D 几何相交算法、距离与切线计算
│   ├── C2DMatrix.h / C2DMatrix.cpp     # 2D 仿射变换矩阵（平移、旋转、缩放）
│   ├── Transformations.h / .cpp        # 局部坐标与世界坐标相互转换、触角生成
│   ├── Region.h                        # 赛场矩形区域与分区划分
│   └── Wall2D.h                        # 2D 边界线段墙壁与法线计算
├── messaging/                          # 消息通信与事件调度体系
│   ├── MessageDispatcher.h / .cpp      # 消息分发单例与延迟优先队列调度器
│   ├── Telegram.h                      # 轻量级消息数据包结构体
│   └── SoccerMessages.h / .cpp         # 智能体间传递的消息枚举与转换函数
├── main.cpp                            # Windows 程序入口、消息循环与窗口过程
├── Makefile                            # MinGW/GCC 项目自动化构建规则
├── Params.ini                          # 游戏模拟与 AI 参数动态配置文件
├── resource.h / Script1.rc / icon1.ico # Windows 窗口菜单资源与程序图标
├── DESIGN.md                           # 系统架构与详细设计文档
└── README.md                           # 项目说明文档
```

---

## 编译与运行指南

### 环境要求
- **操作系统**：Windows 10 / 11
- **编译工具链**：MSYS2 MinGW-w64（推荐 UCRT64 工具链 `g++` 11+）
- **依赖库**：纯 Win32 原生 API（GDI、User32、Winmm），已完全静态链接，运行时无需附带第三方 DLL。

### 编译步骤

打开支持 MinGW 工具链的终端（如 MSYS2 UCRT64 终端或配置好 PATH 的 PowerShell），进入项目根目录：

```bash
# 1. 编译生成 SimpleSoccer.exe
make all

# 2. 编译并直接启动运行
make run

# 3. 清理构建生成的中间对象与目标程序
make clean
```

> **注意**：Makefile 中默认自动探测 `D:/msys64/ucrt64/bin` 与 `C:/msys64/ucrt64/bin`。若安装在其他路径，可直接修改 Makefile 开头的 `MINGW_BIN` 或配置系统环境变量。

---

## 操作与快捷键

程序运行于 Windows 窗口模式，支持以下快捷键与菜单控制：

| 按键 / 菜单项 | 功能描述 |
| :--- | :--- |
| <kbd>P</kbd> | **暂停 / 继续** (Toggle Pause) 模拟运行 |
| <kbd>R</kbd> | **重置比赛** (Reset Pitch)，重新生成赛场和球员初始位置 |
| <kbd>Esc</kbd> | **退出程序** |
| **Menu -> AI Aids -> Show IDs** | 开启 / 关闭球员实体 ID 显示 |
| **Menu -> AI Aids -> Show States** | 开启 / 关闭球员头上当前 FSM 状态名称文本显示 |
| **Menu -> AI Aids -> Show Regions** | 开启 / 关闭球场网格分区（Regions）边框显示 |
| **Menu -> AI Aids -> Show Support Spots** | 开启 / 关闭最佳支援跑位点及所有候选点评分圆圈显示 |
| **Menu -> AI Aids -> Show Targets** | 开启 / 关闭球员操纵行为目标位路线指引线 |
| **Menu -> AI Aids -> Highlight If Threatened** | 开启 / 关闭当球员处于对方防守威胁半径内时的红色高亮 |
| **Menu -> AI Aids -> Clear All** | 一键关闭所有调试可视化辅助图元 |

---

## 配置参数说明 (`Params.ini`)

所有 AI 逻辑、物理运动属性与画面调试项均可在 `Params.ini` 中动态调参，无需重新编译即可生效：

### 1. 核心物理与动作参数
- `BallSize` / `BallMass` / `Friction`: 足球尺寸、质量与草地摩擦系数（`-0.015`）。
- `PlayerKickingDistance`: 球员可以起脚踢球的最大判定距离（数值越大抢断越容易）。
- `PlayerKickFrequency`: 球员每秒最大射门/传球频次（通过 `Regulator` 控制）。
- `PlayerMaxSpeedWithBall`: 球员带球时的最大移动速度（通常低于无球速度）。
- `PlayerMaxSpeedWithoutBall`: 球员无球跑动与冲刺最大速度。
- `PlayerComfortZone`: 球员舒适区半径（若对手进入此区域，持球球员将倾向于寻找传球路线）。

### 2. 射门与传球参数
- `MaxShootingForce` / `MaxPassingForce` / `MaxDribbleForce`: 射门、长短传球和盘带时的最大踢球冲量。
- `MinPassDistance`: 传球接收者的最小安全距离判定阈值。
- `NumAttemptsToFindValidStrike`: 每次判断射门时随机尝试的目标角度次数。
- `PlayerKickingAccuracy`: 踢球精度控制（`0.0 ~ 1.0`，越小踢出的球偏角散布越大）。

### 3. 门将参数
- `KeeperInBallRange`: 守门员可以拾起/控制皮球的距离。
- `EntityPlayerGoalKeeperInterceptRange`: 守门员决定出击扑球/拦截的球距警戒线。
- `EntityPlayerGoalKeeperTendingDistance`: 守门员门前站位距离球门底线的间距。

### 4. 战术支援点权重
- `NumSweetSpotsX` / `NumSweetSpotsY`: 赛场划分计算支援点的网格分辨率。
- `Spot_CanPassScore`: 能够形成安全传球的评分权重。
- `Spot_CanScoreFromPositionScore`: 该支援点具备直接起脚打门角度时的加分权重。
- `Spot_DistFromControllingPlayerScore`: 与持球人保持适中距离的评分权重。

---

## 系统架构简述

项目架构遵循高度解耦的经典游戏 AI 模式：
- **`SoccerPitch`** 作为世界主控，维持比赛推进、物理碰撞更新和渲染驱动。
- **`SoccerTeam`** 统一指挥红队与蓝队，管理传球路由策略与战术状态。
- **`StateMachine<T>`** 作为通用的 FSM 调度核心，由 `EntityPlayerGoalKeeper`、`EntityPlayerOnField` 及 `SoccerTeam` 各自持有。
- **`MessageDispatcher`** 充当中介者调度器，负责可靠的消息投递。

关于详细的架构设计与 UML 图解，请参阅 [`DESIGN.md`](./DESIGN.md)。