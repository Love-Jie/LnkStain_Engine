#include"LSNode.h"

namespace lse{
    LSUUID LSNode::GetId() const {
        return mId;
    }

    void LSNode::SetId(const LSUUID &nodeId) {
        mId = nodeId;
    }

    const std::string &LSNode::GetName() const {
        return mName;
    }

    void LSNode::SetName(const std::string &name) {
        mName = name;
    }

    const std::vector<LSNode *> &LSNode::GetChildren() const {
        return mChildren;
    }

    bool LSNode::HasParent() {
        return mParent != nullptr;
    }

    bool LSNode::HasChildren() {
        return !mChildren.empty();
    }

    void LSNode::SetParent(LSNode *node) {
        node->AddChild(this);
    }

    LSNode *LSNode::GetParent() const {
        return mParent;
    }

    void LSNode::AddChild(LSNode *node) {
        if(node->HasParent()){
            node->GetParent()->RemoveChild(node);
        }
        node->mParent = this;
        mChildren.push_back(node);
    }

    void LSNode::RemoveChild(LSNode *node) {
        if(!HasChildren()){
            return;
        }
        for(auto it = mChildren.begin(); it != mChildren.end(); ++it){
            if(node == *it){
                mChildren.erase(it);
                break;
            }
        }
    }

}