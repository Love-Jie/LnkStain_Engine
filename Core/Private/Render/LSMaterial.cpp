#include"Render/LSMaterial.h"

namespace lse{
    LSMaterialFactory LSMaterialFactory::s_MaterialFactory{};

    bool LSMaterial::HasTexture(uint32_t id) const {
        if(mTextures.find(id) != mTextures.end()) {
            return true;
        }
        return false;
    }

    const TextureView* LSMaterial::GetTextureView(uint32_t id) const {
        if(HasTexture(id)) {
            return &mTextures.at(id);
        }
        return nullptr;
    }

    void LSMaterial::SetTextureView(uint32_t id,LSTexture *texture, LSSampler *sampler) {
        if(HasTexture(id)) {
            mTextures[id].texture = texture;
            mTextures[id].sampler = sampler;
        }else {
            mTextures[id] = { texture, sampler };
        }
        bShouldFlushResource = true;
    }
    void LSMaterial::UpdateTextureViewEnable(uint32_t id, bool enable) {
        if(HasTexture(id)){
            mTextures[id].bEnable = enable;
            bShouldFlushParams = true;
        }
    }

    void LSMaterial::UpdateTextureViewUVTranslation(uint32_t id, const glm::vec2 &uvTranslation) {
        if(HasTexture(id)){
            mTextures[id].uvTranslation = uvTranslation;
            bShouldFlushParams = true;
        }
    }

    void LSMaterial::UpdateTextureViewUVRotation(uint32_t id, float uvRotation) {
        if(HasTexture(id)){
            mTextures[id].uvRotation = uvRotation;
            bShouldFlushParams = true;
        }
    }

    void LSMaterial::UpdateTextureViewUVScale(uint32_t id, const glm::vec2 &uvScale) {
        if(HasTexture(id)){
            mTextures[id].uvScale = uvScale;
            bShouldFlushParams = true;
        }
    }

}