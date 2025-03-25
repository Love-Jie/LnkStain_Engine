#include "ECS/Component/LSCameraComponent.h"
#include "ECS/Component/LSTransformComponent.h"

namespace lse{
    const glm::mat4 &LSCameraComponent::GetProjMat() {
        mProjMat = glm::perspective(glm::radians(mFov), mAspect, mNearPlane, mFarPlane);
        return mProjMat;
    }

    const glm::mat4 &LSCameraComponent::GetViewMat() {
        LSEntity *owner = GetOwner();
        if(LSEntity::HasComponent<LSTransformComponent>(owner)){
            auto &transComp = owner->GetComponent<LSTransformComponent>();
            float yaw = transComp.rotation.x;
            float pitch = transComp.rotation.y;

            glm::vec3 direction;
            direction.x = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
            direction.y = sin(glm::radians(pitch));
            direction.z = cos(glm::radians(pitch)) * cos(glm::radians(yaw));

            transComp.position = mTarget + direction * mRadius;

            mViewMat = glm::lookAt(transComp.position, mTarget, mWorldUp);
        }
        return mViewMat;
    }

    void LSCameraComponent::SetViewMat(const glm::mat4 &viewMat) {
        // TODO
    }

}