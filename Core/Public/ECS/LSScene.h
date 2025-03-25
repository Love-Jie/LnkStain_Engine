#pragma once

#include"LSUUID.h"
#include"entt/entity/registry.hpp"

namespace lse{
    class LSNode;
    class LSEntity;
    class LSScene{
    public:
        LSScene();
        ~LSScene();

        LSEntity* CreateEntity(const std::string &name = "");
        LSEntity* CreateEntityWithUUID(const LSUUID &id, const std::string &name = "");
        void DestroyEntity(const LSEntity *entity);
        void DestroyAllEntity();

        entt::registry &GetEcsRegistry() { return mEcsRegistry; }
        LSNode *GetRootNode() const { return mRootNode.get(); }
        LSEntity *GetEntity(entt::entity enttEntity);

    private:
        std::string mName;
        entt::registry mEcsRegistry;

        std::unordered_map<entt::entity,std::shared_ptr<LSEntity>> mEntities;
        std::shared_ptr<LSNode> mRootNode;
        friend class LSEntity;
    };

}