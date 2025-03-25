#include "ECS/System/LSMaterialSystem.h"

#include "LSApplication.h"
#include "Render/LSRenderContext.h"
#include "Render/LSRenderTarget.h"
#include "ECS/Component/LSCameraComponent.h"

namespace lse{
    LSApplication *LSMaterialSystem::GetApp() const {
        LSAppContext *appContext = LSApplication::GetAppContext();
        if(appContext){
            return appContext->app;
        }
        return nullptr;
    }

    LSScene *LSMaterialSystem::GetScene() const {
        LSAppContext *appContext = LSApplication::GetAppContext();
        if(appContext){
            return appContext->scene;
        }
        return nullptr;
    }

    LSVKDevice *LSMaterialSystem::GetDevice() const {
        LSAppContext *appContext = LSApplication::GetAppContext();
        if(appContext){
            if(appContext->renderContext){
                return appContext->renderContext->GetDevice();
            }
        }
        return nullptr;
    }

    const glm::mat4 LSMaterialSystem::GetProjMat(LSRenderTarget *renderTarget) const {
        glm::mat4 projMat{1.f};
        LSEntity *camera = renderTarget->GetCamera();
        if(LSEntity::HasComponent<LSCameraComponent>(camera)){
            auto &cameraComp = camera->GetComponent<LSCameraComponent>();
            projMat = cameraComp.GetProjMat();
        }
        return projMat;
    }

    const glm::mat4 LSMaterialSystem::GetViewMat(LSRenderTarget *renderTarget) const {
        glm::mat4 viewMat{1.f};
        LSEntity *camera = renderTarget->GetCamera();
        if(LSEntity::HasComponent<LSCameraComponent>(camera)){
            auto &cameraComp = camera->GetComponent<LSCameraComponent>();
            viewMat = cameraComp.GetViewMat();
        }
        return viewMat;
    }

}