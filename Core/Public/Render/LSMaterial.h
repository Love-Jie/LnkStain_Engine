#pragma once

#include "LSTexture.h"
#include "LSSampler.h"
#include "LSGraphicContext.h"
#include "entt/core/type_info.hpp"

namespace lse{
    //shader params
    struct TextureParam {
        bool enable;
        alignas(4) float uvRotation{ 0.0f };        //alignas(4)是内存对齐的偏移
        alignas(16) glm::vec4 uvTransform { 1.0f, 1.0f, 0.0f, 0.0f}; //x,y -> scale, z,w -> translation
    };

    //片元着色器输入的PushContants
    struct ModelPC {
        alignas(16) glm::mat4 modelMat;
        alignas(16) glm::mat3 normalMat;
    };

    struct TextureView{
        LSTexture *texture = nullptr;        //纹理
        LSSampler *sampler = nullptr;        //采样方式
        bool bEnable = true;                 //是否启用
        glm::vec2 uvTranslation{ 0.f, 0.f }; //UV位移
        float uvRotation{ 0.f };             //UV旋转
        glm::vec2 uvScale { 1.f, 1.f };      //UV缩放

        bool IsValid() const {
            return bEnable && texture != nullptr && sampler != nullptr;
        }
    };


    class LSMaterial{
    public:
        LSMaterial(const LSMaterial&) = delete;
        LSMaterial &operator=(const LSMaterial&) = delete;
        static void UpdateTextureParams(const TextureView *textureView, TextureParam *param){
            param->enable = textureView->IsValid() && textureView->bEnable;
            param->uvRotation = textureView->uvRotation;
            param->uvTransform = { textureView->uvScale.x, textureView->uvScale.y, textureView->uvTranslation.x, textureView->uvTranslation.y };
        }

        int32_t GetIndex() const {
            return mIndex;
        }
        bool ShouldFlushParams() const { return bShouldFlushParams; }
        bool ShouldFlushResource() const { return bShouldFlushResource; }
        void FinishFlushParams() { bShouldFlushParams = false; }
        void FinishFlushResource() { bShouldFlushResource = false; }

        bool HasTexture(uint32_t id) const;
        const TextureView* GetTextureView(uint32_t id) const;
        void SetTextureView(uint32_t id, LSTexture *texture, LSSampler *sampler);
        void UpdateTextureViewEnable(uint32_t id, bool enable);
        void UpdateTextureViewUVTranslation(uint32_t id, const glm::vec2 &uvTranslation);
        void UpdateTextureViewUVRotation(uint32_t id, float uvRotation);
        void UpdateTextureViewUVScale(uint32_t id, const glm::vec2 &uvScale);

    protected:
        LSMaterial() = default;

        bool bShouldFlushParams = false;    //params是否发生变更，material内部设置
        bool bShouldFlushResource = false;  //texture是否发生替换，material内部设置

    private:
        int32_t mIndex = -1;
        std::unordered_map<uint32_t,TextureView> mTextures;
        friend class LSMaterialFactory;
    };

    //定义一个材质工厂，材质的维护统一交给材质工厂来做，不提供外界实例化材质，重点需要实现的函数是创建材质
    class LSMaterialFactory{
    public:
        LSMaterialFactory(const LSMaterialFactory&) = delete;
        LSMaterialFactory &operator=(const LSMaterialFactory&) = delete;

        //采用单例模式
        static LSMaterialFactory* GetInstance(){
            return &s_MaterialFactory;
        }

        ~LSMaterialFactory() {
            mMaterials.clear();
        }

        template<typename T>
        size_t GetMaterialSize(){
            uint32_t typeId = entt::type_id<T>().hash();
            if(mMaterials.find(typeId) == mMaterials.end()){
                return 0;
            }
            return mMaterials[typeId].size();
        }

        template<typename T>
        T* CreateMaterial(){
            auto mat = std::make_shared<T>();
            uint32_t typeId = entt::type_id<T>().hash();

            uint32_t index = 0;
            if(mMaterials.find(typeId) == mMaterials.end()){
                mMaterials.insert({ typeId, { mat }});
            } else {
                index = mMaterials[typeId].size();
                mMaterials[typeId].push_back(mat);
            }
            mat->mIndex = index;
            return mat.get();
        }
    private:
        LSMaterialFactory() = default;

        static LSMaterialFactory s_MaterialFactory;

        std::unordered_map<uint32_t, std::vector<std::shared_ptr<LSMaterial>>> mMaterials;
    };

}