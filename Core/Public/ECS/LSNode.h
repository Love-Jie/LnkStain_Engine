#pragma once

#include"ECS/LSUUID.h"

namespace lse{
    class LSNode{
    public:
        LSNode() = default;
        virtual ~LSNode() = default;

        LSUUID GetId() const;
        void SetId(const LSUUID &nodeId);
        const std::string &GetName() const;
        void SetName(const std::string &name);

        const std::vector<LSNode *> &GetChildren() const;
        bool HasParent();
        bool HasChildren();
        void SetParent(LSNode *node);
        LSNode *GetParent() const;
        void AddChild(LSNode *node);
        void RemoveChild(LSNode *node);

    private:
        LSUUID mId;
        std::string mName;
        LSNode* mParent = nullptr;
        std::vector<LSNode*> mChildren;
    };

}