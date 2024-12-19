#pragma once

#include "vortex_device.h"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace VortexEngine {

    class VortexDescriptorSetLayout {
    public:
        class Builder {
        public:
            Builder(VortexDevice& vortexDevice) : vortexDevice{ vortexDevice } {}

            Builder& addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<VortexDescriptorSetLayout> build() const;

        private:
            VortexDevice& vortexDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        VortexDescriptorSetLayout(
            VortexDevice& vortexDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~VortexDescriptorSetLayout();
        VortexDescriptorSetLayout(const VortexDescriptorSetLayout&) = delete;
        VortexDescriptorSetLayout& operator=(const VortexDescriptorSetLayout&) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        VortexDevice& vortexDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class VortexDescriptorWriter;
    };

    class VortexDescriptorPool {
    public:
        class Builder {
        public:
            Builder(VortexDevice& vortexDevice) : vortexDevice{ vortexDevice } {}

            Builder& addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder& setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder& setMaxSets(uint32_t count);
            std::unique_ptr<VortexDescriptorPool> build() const;

        private:
            VortexDevice& vortexDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        VortexDescriptorPool(
            VortexDevice& vortexDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize>& poolSizes);
        ~VortexDescriptorPool();
        VortexDescriptorPool(const VortexDescriptorPool&) = delete;
        VortexDescriptorPool& operator=(const VortexDescriptorPool&) = delete;

        bool allocateDescriptorSet(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet& descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet>& descriptors) const;

        void resetPool();

    private:
        VortexDevice& vortexDevice;
        VkDescriptorPool descriptorPool;

        friend class VortexDescriptorWriter;
    };

    class VortexDescriptorWriter {
    public:
        VortexDescriptorWriter(VortexDescriptorSetLayout& setLayout, VortexDescriptorPool& pool);

        VortexDescriptorWriter& writeBuffer(uint32_t binding, VkDescriptorBufferInfo* bufferInfo);
        VortexDescriptorWriter& writeImage(uint32_t binding, VkDescriptorImageInfo* imageInfo);

        bool build(VkDescriptorSet& set);
        void overwrite(VkDescriptorSet& set);

    private:
        VortexDescriptorSetLayout& setLayout;
        VortexDescriptorPool& pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

}  // namespace vortex