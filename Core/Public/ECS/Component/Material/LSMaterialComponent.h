#pragma once

#include"Render/LSMesh.h"
#include"Render/LSMaterial.h"
#include "ECS/LSComponent.h"

namespace lse{

    template<typename T>
    class LSMaterialComponent : public LSComponent {
    public:
        void AddMesh(LSMesh *mesh,T *material = nullptr) {
            if(!mesh){
                return;
            }
            uint32_t meshIndex = mMeshList.size();
            mMeshList.push_back(mesh);

            if(mMeshMaterials.find(material) != mMeshMaterials.end()){
                mMeshMaterials[material].push_back(meshIndex);
            }else{
                mMeshMaterials.insert({material,{meshIndex}});
            }
        }

        //获取材质的总数量
        uint32_t GetMaterialCount() const {
            return mMeshMaterials.size();
        }

        //返回材质的map
        const std::unordered_map<T*,std::vector<uint32_t>> &GetMeshMaterials() const {
            return mMeshMaterials;
        }

        //根据索引获取网格体的指针
        LSMesh *GetMesh(uint32_t index) const {
          	if(index < mMeshList.size()){
                  return mMeshList[index];
          	}
          	return nullptr;
        }

    private:
        std::vector<LSMesh*>mMeshList;
        std::unordered_map<T*, std::vector<uint32_t>> mMeshMaterials;
    };

}
