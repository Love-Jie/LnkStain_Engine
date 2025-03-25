#pragma once

#include"LSEngine.h"

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS   //该宏表示强制用弧度而不是角度
#define GLM_FORCE_DEPTH_ZERO_TO_ONE   //定义深度取值0~1
 #include"glm/glm.hpp"
#include"glm/gtc/matrix_transform.hpp"   //矩阵变换
#include"glm/gtx/quaternion.hpp"    //四元数
#include"glm/gtc/random.hpp"    //用于生成随机数
#include"glm/gtc/type_ptr.hpp"   //将矩阵向量转化为指针

namespace lse{
    class LSWindow;
    class LSGraphicContext{
    public:
        LSGraphicContext(const LSGraphicContext&) = delete;
        LSGraphicContext& operator=(const LSGraphicContext&) = delete;
        virtual ~LSGraphicContext() = default;

        static std::unique_ptr<LSGraphicContext>Create(LSWindow* window);

    protected:
        LSGraphicContext() = default;

    };

}
