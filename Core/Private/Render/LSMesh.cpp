#include"Render/LSMesh.h"
#include"Render/LSRenderContext.h"
#include"LSApplication.h"

namespace lse {
    LSMesh::LSMesh(const std::vector<lse::LSVertex> &vertices, const std::vector<uint32_t> &indices) {
        if(vertices.empty()){
            return;
        }
        lse::LSRenderContext *renderContext = LSApplication::GetAppContext()->renderContext;
        lse::LSVKDevice *device = renderContext->GetDevice();

        mVertexCount = vertices.size();
        mIndexCount = indices.size();
        mVertexBuffer = std::make_shared<lse::LSVKBuffer>(device,VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,sizeof(vertices[0]) * vertices.size(), (void*)vertices.data());
        if(mIndexCount > 0){
            mIndexBuffer = std::make_shared<lse::LSVKBuffer>(device, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, sizeof(indices[0]) * indices.size(), (void*)indices.data());
        }
    }
    LSMesh::~LSMesh() {

    }

    void LSMesh::Draw(VkCommandBuffer cmdBuffer) {
        VkBuffer vertexBuffers[] = { mVertexBuffer->GetHandle() };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(cmdBuffer, 0, 1, vertexBuffers, offsets);

        if(mIndexCount > 0){
            vkCmdBindIndexBuffer(cmdBuffer, mIndexBuffer->GetHandle(), 0, VK_INDEX_TYPE_UINT32);
            vkCmdDrawIndexed(cmdBuffer, mIndexCount, 1, 0, 0, 0);
        } else {
            vkCmdDraw(cmdBuffer, mVertexCount, 1, 0, 0);
        }
    }

}