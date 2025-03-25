#pragma once

#include"LSGraphicContext.h"

namespace lse {
    //定义定点数据的格式
    struct LSVertex {
        glm::vec3 position;
        glm::vec2 texcoord;
        glm::vec3 normal;
    };
    class LSGeometryUtil {
    public:


        static void CreateCube(float leftPlane,float rightPlane, float bottomPlane,float topPlane,float nearPlane,float farPlane,
                                std::vector<LSVertex>& vertices,std::vector<uint32_t>& indices,const bool bUseTexcoords = true,
                                const bool bUseNormals = true,const glm::mat4 &relativeMat = glm::mat4(1.0f));
    };


}



