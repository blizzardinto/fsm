# SimpleSoccer - 基于有限状态机 (FSM) 的足球 AI 仿真系统

本项目是基于 C++ 实现的二维自主智能体足球比赛模拟系统（源自 Mat Buckland 经典著作《Programming Game AI by Example》）。系统融合了**球队与球员协作的有限状态机 (FSM)**、**自主智能体操纵行为 (Steering Behaviors)**、**事件驱动的消息通信体系 (Telegram & MessageDispatcher)** 以及**战术支援点计算算法**，完整模拟了足球比赛中的带球、传球、射门、守门、防守盯人与无球跑动支援等群体协作行为。

---

## 目录

- [核心特性](#核心特性)
- [目录与文件结构](#目录与文件结构)
- [编译与运行指南](#编译与运行指南)
- [操作与快捷键](#操作与快捷键)
- [配置参数说明 (`Params.ini`)](#配置参数说明-paramsini)
- [系统架构简述](#系统架构简述)
- [架构设计文档](#架构设计文档)
- [当前实现边界与维护事项](#当前实现边界与维护事项)
- [命名约定](#命名约定)

---

## 核心特性

1. **球队与球员协作的有限状态机 (FSM)**
   - **球队层级**：负责红蓝两队宏观战术状态切换（开球准备 `PrepareForKickOff`、防守 `Defending`、进攻 `Attacking`）。
   - **场上球员层级**：独立管理球员个体行为（等待 `Wait`、追球 `ChaseBall`、盘带 `Dribble`、踢球 `KickBall`、接球 `ReceiveBall`、跑位支援 `SupportAttacker`、回位 `ReturnToHomeRegion` 等）。
   - **门将层级**：专注于守门员专有战术（守门 `TendGoal`、出击拦截 `InterceptBall`、门前重置 `ReturnHome`、开球门球 `PutBallBackInPlay`）。
2. **自主智能体操纵行为 (Steering Behaviors)**
   - 采用 Reynolds 操纵行为模型，提供精准平滑的运动模拟：寻找 (`seek`)、到达 (`arrive`)、拦截追击 (`pursuit`)、队员分离防挤压 (`separation`)、插足卡位 (`interpose`)。
3. **消息驱动通信体系 (Message-Driven Architecture)**
   - 实体间通过轻量级 `Telegram` 通信，当前比赛使用同步即时发送；延迟队列代码尚未接入更新循环，用于集中管理智能体间的消息投递（如传球呼叫 `msgPassToMe`、支援通知 `msgSupportAttacker`、回防指令 `msgGoHome`）。
4. **战术决策与几何计算**
   - **支援点计算器 (`SupportSpotCalculator`)**：在球场网格内实时评估无球跑动支援点得分（结合传球安全度与射门开阔度）。
   - **传球安全检测**：通过切线和线段相交几何算法，智能计算对手拦截风险并选出最佳传球目标。
   - **足球物理与轨迹预估**：模拟球体滑动摩擦、边界反弹碰撞以及根据踢球力度预测未来到达时间与拦截点。

---

## 目录与文件结构

项目按主要职责划分目录，头文件声明接口，源文件实现行为。目录划分不代表严格的依赖分层，业务、状态与渲染之间仍有直接依赖：

```text
./
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
│   ├── SteeringBehaviors.h / .cpp      # Reynolds 操纵行为力学计算类（移动、拦截、分离等）
│   ├── SupportSpotCalculator.h / .cpp  # 进攻跑位支援点评估计算器（评分网格）
│   └── ParamLoader.h / ParamLoader.cpp # 参数配置加载器（单例 prm，解析 Params.ini）
├── graph/                              # 导航图与路径搜索算法
│   ├── SparseGraph.h                   # 2D/3D 稀疏图数据结构
│   ├── Pathfinder.h / Pathfinder.cpp   # 独立寻路演示类（未接入足球比赛）
│   ├── PriorityQueue.h                 # 优先队列模板（支持索引优先队列）
│   ├── GraphAlgorithms.h               # 图搜索算法库（A*、Dijkstra、bfs、dfs）
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
├── Params.ini                          # 游戏模拟与 AI 启动配置文件
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

在项目根目录使用 GNU Make 和 MinGW UCRT64 工具链执行以下命令。当前 Makefile 的建目录与清理命令使用 Windows cmd 语法；从 MSYS2 shell 执行时需指定兼容的命令解释器，或调整相关规则：

```bash
# 1. 编译生成 SimpleSoccer.exe
make all

# 2. 编译并直接启动运行
make run

# 3. 清理构建生成的中间对象与目标程序
make clean
```

> **注意**：Makefile 默认探测 c 盘或 D 盘的标准 MSYS2 UCRT64 安装目录。若安装在其他位置，可通过 `make MINGW_BIN=相对于项目目录的工具链路径 all` 覆盖默认值，或修改 Makefile。仅修改 PATH 不会覆盖 Makefile 中的编译器路径。

---

## 操作与快捷键

程序运行于 Windows 窗口模式，支持以下快捷键与菜单控制：

| 按键 / 菜单项 | 功能描述 |
| :--- | :--- |
| <kbd>P</kbd> | **暂停 / 继续** (Toggle Pause) 模拟运行 |
| <kbd>R</kbd> | **重置比赛** (reset pitch)，重新生成赛场和球员初始位置 |
| <kbd>Esc</kbd> | **退出程序** |
| **Menu -> AI Aids -> Show IDs** | 开启 / 关闭球员实体 id 显示 |
| **Menu -> AI Aids -> Show States** | 开启 / 关闭球员头上当前 FSM 状态名称文本显示 |
| **Menu -> AI Aids -> Show Regions** | 开启 / 关闭球场网格分区（Regions）边框显示 |
| **Menu -> AI Aids -> Show Support Spots** | 开启 / 关闭最佳支援跑位点及所有候选点评分圆圈显示 |
| **Menu -> AI Aids -> Show Targets** | 开启 / 关闭球员操纵行为目标位路线指引线 |
| **Menu -> AI Aids -> Highlight If Threatened** | 开启 / 关闭当球员处于对方防守威胁半径内时的红色高亮 |
| **Menu -> AI Aids -> clear All** | 一键关闭所有调试可视化辅助图元 |

---

## 配置参数说明 (`Params.ini`)

主要 AI、物理和调试参数来自 [Params.ini](./Params.ini)，无需重新编译，但修改文件后需要重启程序。`ParamLoader` 单例只在首次访问时读取配置；按 `R` 重置比赛不会重新加载。文件按固定顺序读取数值，而非按参数名查找，请保留条目顺序，并从包含 `Params.ini` 的项目根目录启动程序。

### 1. 核心物理与动作参数
- `ballSize` / `ballMass` / `friction`: 足球尺寸、质量与草地摩擦系数（`-0.015`）。
- `playerKickingDistance`: 球员可以起脚踢球的最大判定距离（数值越大抢断越容易）。
- `playerKickFrequency`: 球员每秒最大射门/传球频次（通过 `Regulator` 控制）。
- `playerMaxSpeedWithBall`: 球员带球时的最大移动速度（通常低于无球速度）。
- `playerMaxSpeedWithoutBall`: 球员无球跑动与冲刺最大速度。
- `playerComfortZone`: 球员舒适区半径（若对手进入此区域，持球球员将倾向于寻找传球路线）。

### 2. 射门与传球参数
- `maxShootingForce` / `maxPassingForce` / `maxDribbleForce`: 射门、长短传球和盘带时的最大踢球冲量。
- `minPassDist`: 传球接收者的最小安全距离判定阈值。
- `numAttemptsToFindValidStrike`: 每次判断射门时随机尝试的目标角度次数。
- `playerKickingAccuracy`: 踢球精度控制（`0.0 ~ 1.0`，越小踢出的球偏角散布越大）。

### 3. 门将参数
- `keeperInBallRange`: 守门员可以拾起/控制皮球的距离。
- `entityPlayerGoalKeeperInterceptRange`: 守门员决定出击扑球/拦截的球距警戒线。
- `entityPlayerGoalKeeperTendingDistance`: 守门员门前站位距离球门底线的间距。

### 4. 战术支援点权重
- `numSweetSpotsX` / `numSweetSpotsY`: 赛场划分计算支援点的网格分辨率。
- `spotCanPassScore`: 能够形成安全传球的评分权重。
- `spotCanScoreFromPositionScore`: 该支援点具备直接起脚打门角度时的加分权重。
- `spotDistFromControllingPlayerScore`: 与持球人保持适中距离的评分权重。

---

## 系统架构简述

项目采用经典面向对象游戏 AI 结构：
- **`SoccerPitch`** 作为世界主控，维持比赛推进、物理碰撞更新和渲染驱动。
- **`SoccerTeam`** 统一指挥红队与蓝队，管理传球路由策略与战术状态。
- **`StateMachine<T>`** 作为通用的 FSM 调度核心，由 `EntityPlayerGoalKeeper`、`EntityPlayerOnField` 及 `SoccerTeam` 各自持有。
- **`MessageDispatcher`** 集中投递即时消息，通过实体 id 查找接收者。延迟调度和实体销毁后的清理仍需完善。

关于详细的架构设计与 UML 图解，请参阅 [`DESIGN.md`](./DESIGN.md)。
## 当前实现边界与维护事项

- 球队、场上球员和门将各自持有 FSM；没有嵌套状态或父子状态的层次状态机语义。
- `graph/` 是独立的图搜索与寻路演示代码。目前 Makefile 将其编入程序，但比赛不调用它；球员移动使用 steering。
- 领域对象同时负责更新和 GDI 绘图，`math/` 中部分类型也依赖 Win32 或绘图工具；当前实现面向 Windows。
- 球员注册到 `EntityManager` 后，析构时没有注销。按 `R` 重建比赛会在注册表中留下旧对象指针，需要补全生命周期清理。
- 延迟消息没有接入主循环，依赖的帧计数也没有推进；队列比较规则还可能丢弃同一派发时间的不同消息。
- Makefile 未跟踪头文件依赖。修改头文件后应完整重建；对象文件按文件名展平，不支持不同目录中的同名源文件。
- 仓库目前没有自动化测试或 CI 配置。上述结构说明来自静态代码核对，不代表已完成运行验证。

具体依赖、对象所有权和改进顺序见 [DESIGN.md](./DESIGN.md#9-实际依赖对象生命周期与维护建议)。

## 命名约定

- 函数、变量、参数、常量和枚举值使用 camelCase，例如 `update()`、`dispatchMsg()`、`frameRate`、`msgReceiveBall`。
- 成员变量使用 `m` 前缀，例如 `mPosition`、`mCurrentState`；全局变量使用 `g` 前缀，例如 `gSoccerPitch`。不再使用下划线或匈牙利类型前缀。
- 类、结构、枚举类型与类型别名使用 PascalCase，例如 `StateMachine`、`PlayerRole`、`IniFileLoaderBase`。模板参数可使用 camelCase；可能与成员名称冲突时使用明确的类型名，如 `ExtraInfoType`。
- 预处理宏、头文件保护宏和 Windows 资源 ID 保留现有约定；Win32 API、系统结构字段和 `WinMain` 入口保留系统规定的名称。
- 配置标签也使用 camelCase，读取顺序和值保持不变。源码文件名保持现有名称，文档中的链接使用相对路径。
