#pragma once
#include "Graphic/LSVKBuffer.h"
#include "LSGeometryUtil.h"

namespace lse{
    class LSMesh{
    public:
        LSMesh(const std::vector<lse::LSVertex> &vertices, const std::vector<uint32_t> &indices = {});
        ~LSMesh();

        void Draw(VkCommandBuffer cmdBuffer);

    private:
        std::shared_ptr<LSVKBuffer> mVertexBuffer;
        std::shared_ptr<LSVKBuffer> mIndexBuffer;
        uint32_t mVertexCount;
        uint32_t mIndexCount;
    };
}
