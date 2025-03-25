#include"LSVKDescriptorSet.h"
#include"LSVKDevice.h"

namespace lse{
    LSVKDescriptorSetLayout::LSVKDescriptorSetLayout(LSVKDevice *device, const std::vector<VkDescriptorSetLayoutBinding>& bindings) : mDevice(device) {
        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo = {};
        descriptorSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutInfo.pNext = nullptr;
        descriptorSetLayoutInfo.flags = 0;
        descriptorSetLayoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
        descriptorSetLayoutInfo.pBindings = bindings.data();

        CALL_VK(vkCreateDescriptorSetLayout(device->GetHandle(),&descriptorSetLayoutInfo,nullptr,&mHandle));
    }

    LSVKDescriptorSetLayout::~LSVKDescriptorSetLayout() {
        VK_D(DescriptorSetLayout, mDevice->GetHandle(), mHandle);
    }

    LSVKDescriptorPool::LSVKDescriptorPool(LSVKDevice *device,uint32_t maxSets, const std::vector<VkDescriptorPoolSize> &poolSizes) : mDevice(device) {
        VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
        descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        descriptorPoolInfo.pNext = nullptr;
        descriptorPoolInfo.flags = 0;
        descriptorPoolInfo.maxSets = maxSets;
        descriptorPoolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
        descriptorPoolInfo.pPoolSizes = poolSizes.data();

        CALL_VK(vkCreateDescriptorPool(mDevice->GetHandle(), &descriptorPoolInfo, nullptr, &mHandle));
    }

    LSVKDescriptorPool::~LSVKDescriptorPool() {
        VK_D(DescriptorPool, mDevice->GetHandle(), mHandle);
    }

    std::vector<VkDescriptorSet> LSVKDescriptorPool::AllocateDescriptorSet(LSVKDescriptorSetLayout *setLayout,uint32_t count) {
        std::vector<VkDescriptorSet> descriptorSets(count);
        std::vector<VkDescriptorSetLayout> setLayouts(count);

        for(int i = 0; i < count; i++){
            setLayouts[i] = setLayout->GetHandle();
        }

        VkDescriptorSetAllocateInfo allocateInfo = {};
        allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocateInfo.pNext = nullptr;
        allocateInfo.descriptorPool = mHandle;
        allocateInfo.descriptorSetCount = count;
        allocateInfo.pSetLayouts = setLayouts.data();

        VkResult ret = vkAllocateDescriptorSets(mDevice->GetHandle(), &allocateInfo, descriptorSets.data());
        CALL_VK(ret);
        if(ret != VK_SUCCESS){
            descriptorSets.clear();
        }
        return descriptorSets;
    }

}