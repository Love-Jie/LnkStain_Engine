#include "ECS/LSScene.h"
#include "ECS/LSEntity.h"
#include "ECS/Component/LSTransformComponent.h"

namespace lse{
    LSScene::LSScene() {
        mRootNode = std::make_shared<LSNode>();
    }

    LSScene::~LSScene() {
        mRootNode.reset();
        DestroyAllEntity();
        mEntities.clear();
    }

    LSEntity *LSScene::CreateEntity(const std::string &name) {
        return CreateEntityWithUUID(LSUUID(), name);
    }

    LSEntity *LSScene::CreateEntityWithUUID(const LSUUID &id, const std::string &name) {
        auto enttEntity = mEcsRegistry.create();
        mEntities.insert({ enttEntity, std::make_shared<LSEntity>(enttEntity, this) });
        mEntities[enttEntity]->SetParent(mRootNode.get());
        mEntities[enttEntity]->SetId(id);
        mEntities[enttEntity]->SetName(name.empty() ? "Entity" : name);

        // add default components
        mEntities[enttEntity]->AddComponent<LSTransformComponent>();

        return mEntities[enttEntity].get();
    }

    void LSScene::DestroyEntity(const LSEntity *entity) {
        if(entity && entity->IsValid()){
            mEcsRegistry.destroy(entity->GetEcsEntity());
        }

        auto it = mEntities.find(entity->GetEcsEntity());
        if(it != mEntities.end()){
            LSNode *parent = it->second->GetParent();
            if(parent){
                parent->RemoveChild(it->second.get());
            }
            mEntities.erase(it);
        }
    }

    void LSScene::DestroyAllEntity() {
        mEcsRegistry.clear();
        mEntities.clear();
    }

    LSEntity *LSScene::GetEntity(entt::entity enttEntity) {
        if(mEntities.find(enttEntity) != mEntities.end()){
            return mEntities.at(enttEntity).get();
        }
        return nullptr;
    }
}