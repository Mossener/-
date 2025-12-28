cmake_minimum_required(VERSION 3.10)
project(opengl_demo)

set(CMAKE_CXX_STANDARD 17)

# 1. 查找 Assimp 库（Linux 下通常通过 apt 安装 libassimp-dev）
find_package(assimp REQUIRED)

# 2. 将所有源文件加入编译列表
add_executable(opengl_demo
    src/main.cpp
    src/glad.c
    src/SkinnedModel.cpp       # 新增：处理模型加载
    src/SkeletonHandler.cpp    # 新增：处理骨骼逻辑
)

# 3. 包含头文件路径
target_include_directories(opengl_demo PRIVATE include src)

# 4. 链接所有必要的库
# assimp::assimp 会自动处理库路径和头文件
# 错误写法：
# target_link_libraries(opengl_demo glfw GL assimp::assimp X11 lpthread lXrandr lXi ldl)

# 正确写法：
target_link_libraries(opengl_demo 
    glfw 
    GL 
    assimp::assimp 
    X11 
    pthread    # 注意：这里是 pthread，不是 lpthread
    Xrandr     # 注意：这里是 Xrandr，不是 lXrandr
    Xi         # 注意：这里是 Xi，不是 lXi
    dl         # 注意：这里是 dl，不是 ldl
)