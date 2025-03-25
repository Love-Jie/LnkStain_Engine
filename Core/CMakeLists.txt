cmake_minimum_required(VERSION 3.29)

add_library(lnkstain_core
    Private/LSApplication.cpp
    Private/Render/LSRenderContext.cpp
    Private/Render/LSRenderTarget.cpp
    Private/Render/LSMesh.cpp
    Private/Render/LSTexture.cpp
    Private/Render/LSRenderer.cpp
    Private/Render/LSSampler.cpp
    Private/Render/LSMaterial.cpp

    Private/ECS/LSUUID.cpp
    Private/ECS/LSNode.cpp
    Private/ECS/LSScene.cpp
    Private/ECS/LSEntity.cpp
    Private/ECS/System/LSBaseMaterialSystem.cpp
    Private/ECS/System/LSUnlitMaterialSystem.cpp
    Private/ECS/System/LSMaterialSystem.cpp
    Private/ECS/Component/LSCameraComponent.cpp
)

target_link_libraries(lnkstain_core PUBLIC lnkstain_platform)