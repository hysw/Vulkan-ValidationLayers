
// This file is ***GENERATED***.  Do Not Edit.
// See thread_safety_generator.py for modifications.

/* Copyright (c) 2015-2019 The Khronos Group Inc.
 * Copyright (c) 2015-2019 Valve Corporation
 * Copyright (c) 2015-2019 LunarG, Inc.
 * Copyright (c) 2015-2019 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: Mark Lobodzinski <mark@lunarg.com>
 */
#include "chassis.h"
#include "thread_safety.h"


void ThreadSafety::PreCallRecordAllocateCommandBuffers(VkDevice device, const VkCommandBufferAllocateInfo *pAllocateInfo,
                                                       VkCommandBuffer *pCommandBuffers) {
    StartReadObjectParentInstance(device);
    StartWriteObject(pAllocateInfo->commandPool);
}

void ThreadSafety::PostCallRecordAllocateCommandBuffers(VkDevice device, const VkCommandBufferAllocateInfo *pAllocateInfo,
                                                        VkCommandBuffer *pCommandBuffers, VkResult result) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(pAllocateInfo->commandPool);

    // Record mapping from command buffer to command pool
    if(pCommandBuffers) {
        auto lock = write_lock_guard_t(thread_safety_lock);
        auto &pool_command_buffers = pool_command_buffers_map[pAllocateInfo->commandPool];
        for (uint32_t index = 0; index < pAllocateInfo->commandBufferCount; index++) {
            command_pool_map.insert_or_assign(pCommandBuffers[index], pAllocateInfo->commandPool);
            CreateObject(pCommandBuffers[index]);
            pool_command_buffers.insert(pCommandBuffers[index]);
        }
    }
}

void ThreadSafety::PreCallRecordAllocateDescriptorSets(VkDevice device, const VkDescriptorSetAllocateInfo *pAllocateInfo,
                                                       VkDescriptorSet *pDescriptorSets) {
    StartReadObjectParentInstance(device);
    StartWriteObject(pAllocateInfo->descriptorPool);
    // Host access to pAllocateInfo::descriptorPool must be externally synchronized
}

void ThreadSafety::PostCallRecordAllocateDescriptorSets(VkDevice device, const VkDescriptorSetAllocateInfo *pAllocateInfo,
                                                        VkDescriptorSet *pDescriptorSets, VkResult result) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(pAllocateInfo->descriptorPool);
    // Host access to pAllocateInfo::descriptorPool must be externally synchronized
    if (VK_SUCCESS == result) {
        auto lock = write_lock_guard_t(thread_safety_lock);
        auto &pool_descriptor_sets = pool_descriptor_sets_map[pAllocateInfo->descriptorPool];
        for (uint32_t index0 = 0; index0 < pAllocateInfo->descriptorSetCount; index0++) {
            CreateObject(pDescriptorSets[index0]);
            pool_descriptor_sets.insert(pDescriptorSets[index0]);
        }
    }
}

void ThreadSafety::PreCallRecordFreeDescriptorSets(
    VkDevice                                    device,
    VkDescriptorPool                            descriptorPool,
    uint32_t                                    descriptorSetCount,
    const VkDescriptorSet*                      pDescriptorSets) {
    StartReadObjectParentInstance(device);
    StartWriteObject(descriptorPool);
    if (pDescriptorSets) {
        for (uint32_t index=0; index < descriptorSetCount; index++) {
            StartWriteObject(pDescriptorSets[index]);
        }
    }
    // Host access to descriptorPool must be externally synchronized
    // Host access to each member of pDescriptorSets must be externally synchronized
}

void ThreadSafety::PostCallRecordFreeDescriptorSets(
    VkDevice                                    device,
    VkDescriptorPool                            descriptorPool,
    uint32_t                                    descriptorSetCount,
    const VkDescriptorSet*                      pDescriptorSets,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(descriptorPool);
    if (pDescriptorSets) {
        for (uint32_t index=0; index < descriptorSetCount; index++) {
            FinishWriteObject(pDescriptorSets[index]);
        }
    }
    // Host access to descriptorPool must be externally synchronized
    // Host access to each member of pDescriptorSets must be externally synchronized
    // Host access to pAllocateInfo::descriptorPool must be externally synchronized
    if (VK_SUCCESS == result) {
        auto lock = write_lock_guard_t(thread_safety_lock);
        auto &pool_descriptor_sets = pool_descriptor_sets_map[descriptorPool];
        for (uint32_t index0 = 0; index0 < descriptorSetCount; index0++) {
            DestroyObject(pDescriptorSets[index0]);
            pool_descriptor_sets.erase(pDescriptorSets[index0]);
        }
    }
}

void ThreadSafety::PreCallRecordDestroyDescriptorPool(
    VkDevice                                    device,
    VkDescriptorPool                            descriptorPool,
    const VkAllocationCallbacks*                pAllocator) {
    StartReadObjectParentInstance(device);
    StartWriteObject(descriptorPool);
    // Host access to descriptorPool must be externally synchronized
    auto lock = read_lock_guard_t(thread_safety_lock);
    for(auto descriptor_set : pool_descriptor_sets_map[descriptorPool]) {
        StartWriteObject(descriptor_set);
    }
}

void ThreadSafety::PostCallRecordDestroyDescriptorPool(
    VkDevice                                    device,
    VkDescriptorPool                            descriptorPool,
    const VkAllocationCallbacks*                pAllocator) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(descriptorPool);
    DestroyObject(descriptorPool);
    // Host access to descriptorPool must be externally synchronized
    {
        auto lock = read_lock_guard_t(thread_safety_lock);
        // remove references to implicitly freed descriptor sets
        for(auto descriptor_set : pool_descriptor_sets_map[descriptorPool]) {
            FinishWriteObject(descriptor_set);
            DestroyObject(descriptor_set);
        }
        pool_descriptor_sets_map[descriptorPool].clear();
        pool_descriptor_sets_map.erase(descriptorPool);
    }
}

void ThreadSafety::PreCallRecordResetDescriptorPool(
    VkDevice                                    device,
    VkDescriptorPool                            descriptorPool,
    VkDescriptorPoolResetFlags                  flags) {
    StartReadObjectParentInstance(device);
    StartWriteObject(descriptorPool);
    // Host access to descriptorPool must be externally synchronized
    // any sname:VkDescriptorSet objects allocated from pname:descriptorPool must be externally synchronized between host accesses
    auto lock = read_lock_guard_t(thread_safety_lock);
    for(auto descriptor_set : pool_descriptor_sets_map[descriptorPool]) {
        StartWriteObject(descriptor_set);
    }
}

void ThreadSafety::PostCallRecordResetDescriptorPool(
    VkDevice                                    device,
    VkDescriptorPool                            descriptorPool,
    VkDescriptorPoolResetFlags                  flags,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(descriptorPool);
    // Host access to descriptorPool must be externally synchronized
    // any sname:VkDescriptorSet objects allocated from pname:descriptorPool must be externally synchronized between host accesses
    if (VK_SUCCESS == result) {
        // remove references to implicitly freed descriptor sets
        auto lock = write_lock_guard_t(thread_safety_lock);
        for(auto descriptor_set : pool_descriptor_sets_map[descriptorPool]) {
            FinishWriteObject(descriptor_set);
            DestroyObject(descriptor_set);
        }
        pool_descriptor_sets_map[descriptorPool].clear();
    }
}

void ThreadSafety::PreCallRecordFreeCommandBuffers(VkDevice device, VkCommandPool commandPool, uint32_t commandBufferCount,
                                                   const VkCommandBuffer *pCommandBuffers) {
    const bool lockCommandPool = false;  // pool is already directly locked
    StartReadObjectParentInstance(device);
    StartWriteObject(commandPool);
    if(pCommandBuffers) {
        // Even though we're immediately "finishing" below, we still are testing for concurrency with any call in process
        // so this isn't a no-op
        // The driver may immediately reuse command buffers in another thread.
        // These updates need to be done before calling down to the driver.
        auto lock = write_lock_guard_t(thread_safety_lock);
        auto &pool_command_buffers = pool_command_buffers_map[commandPool];
        for (uint32_t index = 0; index < commandBufferCount; index++) {
            StartWriteObject(pCommandBuffers[index], lockCommandPool);
            FinishWriteObject(pCommandBuffers[index], lockCommandPool);
            DestroyObject(pCommandBuffers[index]);
            pool_command_buffers.erase(pCommandBuffers[index]);
            command_pool_map.erase(pCommandBuffers[index]);
        }
    }
}

void ThreadSafety::PostCallRecordFreeCommandBuffers(VkDevice device, VkCommandPool commandPool, uint32_t commandBufferCount,
                                                    const VkCommandBuffer *pCommandBuffers) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(commandPool);
}

void ThreadSafety::PreCallRecordCreateCommandPool(
    VkDevice                                    device,
    const VkCommandPoolCreateInfo*              pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkCommandPool*                              pCommandPool) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordCreateCommandPool(
    VkDevice                                    device,
    const VkCommandPoolCreateInfo*              pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkCommandPool*                              pCommandPool,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    if (result == VK_SUCCESS) {
        CreateObject(*pCommandPool);
        c_VkCommandPoolContents.CreateObject(*pCommandPool);
    }
}

void ThreadSafety::PreCallRecordResetCommandPool(VkDevice device, VkCommandPool commandPool, VkCommandPoolResetFlags flags) {
    StartReadObjectParentInstance(device);
    StartWriteObject(commandPool);
    // Check for any uses of non-externally sync'd command buffers (for example from vkCmdExecuteCommands)
    c_VkCommandPoolContents.StartWrite(commandPool);
    // Host access to commandPool must be externally synchronized
}

void ThreadSafety::PostCallRecordResetCommandPool(VkDevice device, VkCommandPool commandPool, VkCommandPoolResetFlags flags, VkResult result) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(commandPool);
    c_VkCommandPoolContents.FinishWrite(commandPool);
    // Host access to commandPool must be externally synchronized
}

void ThreadSafety::PreCallRecordDestroyCommandPool(VkDevice device, VkCommandPool commandPool, const VkAllocationCallbacks *pAllocator) {
    StartReadObjectParentInstance(device);
    StartWriteObject(commandPool);
    // Check for any uses of non-externally sync'd command buffers (for example from vkCmdExecuteCommands)
    c_VkCommandPoolContents.StartWrite(commandPool);
    // Host access to commandPool must be externally synchronized

    auto lock = write_lock_guard_t(thread_safety_lock);
    // The driver may immediately reuse command buffers in another thread.
    // These updates need to be done before calling down to the driver.
    // remove references to implicitly freed command pools
    for(auto command_buffer : pool_command_buffers_map[commandPool]) {
        DestroyObject(command_buffer);
    }
    pool_command_buffers_map[commandPool].clear();
    pool_command_buffers_map.erase(commandPool);
}

void ThreadSafety::PostCallRecordDestroyCommandPool(VkDevice device, VkCommandPool commandPool, const VkAllocationCallbacks *pAllocator) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(commandPool);
    DestroyObject(commandPool);
    c_VkCommandPoolContents.FinishWrite(commandPool);
    c_VkCommandPoolContents.DestroyObject(commandPool);
}

// GetSwapchainImages can return a non-zero count with a NULL pSwapchainImages pointer.  Let's avoid crashes by ignoring
// pSwapchainImages.
void ThreadSafety::PreCallRecordGetSwapchainImagesKHR(VkDevice device, VkSwapchainKHR swapchain, uint32_t *pSwapchainImageCount,
                                                      VkImage *pSwapchainImages) {
    StartReadObjectParentInstance(device);
    StartReadObject(swapchain);
}

void ThreadSafety::PostCallRecordGetSwapchainImagesKHR(VkDevice device, VkSwapchainKHR swapchain, uint32_t *pSwapchainImageCount,
                                                       VkImage *pSwapchainImages, VkResult result) {
    FinishReadObjectParentInstance(device);
    FinishReadObject(swapchain);
    if (pSwapchainImages != NULL) {
        auto lock = write_lock_guard_t(thread_safety_lock);
        auto &wrapped_swapchain_image_handles = swapchain_wrapped_image_handle_map[swapchain];
        for (uint32_t i = static_cast<uint32_t>(wrapped_swapchain_image_handles.size()); i < *pSwapchainImageCount; i++) {
            CreateObject(pSwapchainImages[i]);
            wrapped_swapchain_image_handles.emplace_back(pSwapchainImages[i]);
        }
    }
}

void ThreadSafety::PreCallRecordDestroySwapchainKHR(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    const VkAllocationCallbacks*                pAllocator) {
    StartReadObjectParentInstance(device);
    StartWriteObject(swapchain);
    // Host access to swapchain must be externally synchronized
    auto lock = read_lock_guard_t(thread_safety_lock);
    for (auto &image_handle : swapchain_wrapped_image_handle_map[swapchain]) {
        StartWriteObject(image_handle);
    }
}

void ThreadSafety::PostCallRecordDestroySwapchainKHR(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    const VkAllocationCallbacks*                pAllocator) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(swapchain);
    DestroyObject(swapchain);
    // Host access to swapchain must be externally synchronized
    auto lock = write_lock_guard_t(thread_safety_lock);
    for (auto &image_handle : swapchain_wrapped_image_handle_map[swapchain]) {
        FinishWriteObject(image_handle);
        DestroyObject(image_handle);
    }
    swapchain_wrapped_image_handle_map.erase(swapchain);
}

void ThreadSafety::PreCallRecordDestroyDevice(
    VkDevice                                    device,
    const VkAllocationCallbacks*                pAllocator) {
    StartWriteObjectParentInstance(device);
    // Host access to device must be externally synchronized
}

void ThreadSafety::PostCallRecordDestroyDevice(
    VkDevice                                    device,
    const VkAllocationCallbacks*                pAllocator) {
    FinishWriteObjectParentInstance(device);
    DestroyObjectParentInstance(device);
    // Host access to device must be externally synchronized
    auto lock = write_lock_guard_t(thread_safety_lock);
    for (auto &queue : device_queues_map[device]) {
        DestroyObject(queue);
    }
    device_queues_map[device].clear();
}

void ThreadSafety::PreCallRecordGetDeviceQueue(
    VkDevice                                    device,
    uint32_t                                    queueFamilyIndex,
    uint32_t                                    queueIndex,
    VkQueue*                                    pQueue) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetDeviceQueue(
    VkDevice                                    device,
    uint32_t                                    queueFamilyIndex,
    uint32_t                                    queueIndex,
    VkQueue*                                    pQueue) {
    FinishReadObjectParentInstance(device);
    CreateObject(*pQueue);
    auto lock = write_lock_guard_t(thread_safety_lock);
    device_queues_map[device].insert(*pQueue);
}

void ThreadSafety::PreCallRecordGetDeviceQueue2(
    VkDevice                                    device,
    const VkDeviceQueueInfo2*                   pQueueInfo,
    VkQueue*                                    pQueue) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetDeviceQueue2(
    VkDevice                                    device,
    const VkDeviceQueueInfo2*                   pQueueInfo,
    VkQueue*                                    pQueue) {
    FinishReadObjectParentInstance(device);
    CreateObject(*pQueue);
    auto lock = write_lock_guard_t(thread_safety_lock);
    device_queues_map[device].insert(*pQueue);
}



void ThreadSafety::PreCallRecordCreateInstance(
    const VkInstanceCreateInfo*                 pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkInstance*                                 pInstance) {
    
}

void ThreadSafety::PostCallRecordCreateInstance(
    const VkInstanceCreateInfo*                 pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkInstance*                                 pInstance,
    VkResult                                    result) {
    if (result == VK_SUCCESS) {
        CreateObjectParentInstance(*pInstance);
    }
}

void ThreadSafety::PreCallRecordDestroyInstance(
    VkInstance                                  instance,
    const VkAllocationCallbacks*                pAllocator) {
    StartWriteObjectParentInstance(instance);
    // Host access to instance must be externally synchronized
}

void ThreadSafety::PostCallRecordDestroyInstance(
    VkInstance                                  instance,
    const VkAllocationCallbacks*                pAllocator) {
    FinishWriteObjectParentInstance(instance);
    DestroyObjectParentInstance(instance);
    // Host access to instance must be externally synchronized
}

void ThreadSafety::PreCallRecordEnumeratePhysicalDevices(
    VkInstance                                  instance,
    uint32_t*                                   pPhysicalDeviceCount,
    VkPhysicalDevice*                           pPhysicalDevices) {
    StartReadObjectParentInstance(instance);
}

void ThreadSafety::PostCallRecordEnumeratePhysicalDevices(
    VkInstance                                  instance,
    uint32_t*                                   pPhysicalDeviceCount,
    VkPhysicalDevice*                           pPhysicalDevices,
    VkResult                                    result) {
    FinishReadObjectParentInstance(instance);
}

void ThreadSafety::PreCallRecordGetInstanceProcAddr(
    VkInstance                                  instance,
    const char*                                 pName) {
    StartReadObjectParentInstance(instance);
}

void ThreadSafety::PostCallRecordGetInstanceProcAddr(
    VkInstance                                  instance,
    const char*                                 pName) {
    FinishReadObjectParentInstance(instance);
}

void ThreadSafety::PreCallRecordGetDeviceProcAddr(
    VkDevice                                    device,
    const char*                                 pName) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetDeviceProcAddr(
    VkDevice                                    device,
    const char*                                 pName) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordCreateDevice(
    VkPhysicalDevice                            physicalDevice,
    const VkDeviceCreateInfo*                   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDevice*                                   pDevice) {
    
}

void ThreadSafety::PostCallRecordCreateDevice(
    VkPhysicalDevice                            physicalDevice,
    const VkDeviceCreateInfo*                   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDevice*                                   pDevice,
    VkResult                                    result) {
    if (result == VK_SUCCESS) {
        CreateObjectParentInstance(*pDevice);
    }
}

void ThreadSafety::PreCallRecordQueueSubmit(
    VkQueue                                     queue,
    uint32_t                                    submitCount,
    const VkSubmitInfo*                         pSubmits,
    VkFence                                     fence) {
    StartWriteObject(queue);
    StartWriteObject(fence);
    // Host access to queue must be externally synchronized
    // Host access to fence must be externally synchronized
}

void ThreadSafety::PostCallRecordQueueSubmit(
    VkQueue                                     queue,
    uint32_t                                    submitCount,
    const VkSubmitInfo*                         pSubmits,
    VkFence                                     fence,
    VkResult                                    result) {
    FinishWriteObject(queue);
    FinishWriteObject(fence);
    // Host access to queue must be externally synchronized
    // Host access to fence must be externally synchronized
}

void ThreadSafety::PreCallRecordQueueWaitIdle(
    VkQueue                                     queue) {
    StartWriteObject(queue);
    // Host access to queue must be externally synchronized
}

void ThreadSafety::PostCallRecordQueueWaitIdle(
    VkQueue                                     queue,
    VkResult                                    result) {
    FinishWriteObject(queue);
    // Host access to queue must be externally synchronized
}

void ThreadSafety::PreCallRecordDeviceWaitIdle(
    VkDevice                                    device) {
    StartReadObjectParentInstance(device);
    // all sname:VkQueue objects created from pname:device must be externally synchronized between host accesses
}

void ThreadSafety::PostCallRecordDeviceWaitIdle(
    VkDevice                                    device,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    // all sname:VkQueue objects created from pname:device must be externally synchronized between host accesses
}

void ThreadSafety::PreCallRecordAllocateMemory(
    VkDevice                                    device,
    const VkMemoryAllocateInfo*                 pAllocateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDeviceMemory*                             pMemory) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordAllocateMemory(
    VkDevice                                    device,
    const VkMemoryAllocateInfo*                 pAllocateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDeviceMemory*                             pMemory,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    if (result == VK_SUCCESS) {
        CreateObject(*pMemory);
    }
}

void ThreadSafety::PreCallRecordFreeMemory(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    const VkAllocationCallbacks*                pAllocator) {
    StartReadObjectParentInstance(device);
    StartWriteObject(memory);
    // Host access to memory must be externally synchronized
}

void ThreadSafety::PostCallRecordFreeMemory(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    const VkAllocationCallbacks*                pAllocator) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(memory);
    DestroyObject(memory);
    // Host access to memory must be externally synchronized
}

void ThreadSafety::PreCallRecordMapMemory(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    VkDeviceSize                                offset,
    VkDeviceSize                                size,
    VkMemoryMapFlags                            flags,
    void**                                      ppData) {
    StartReadObjectParentInstance(device);
    StartWriteObject(memory);
    // Host access to memory must be externally synchronized
}

void ThreadSafety::PostCallRecordMapMemory(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    VkDeviceSize                                offset,
    VkDeviceSize                                size,
    VkMemoryMapFlags                            flags,
    void**                                      ppData,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(memory);
    // Host access to memory must be externally synchronized
}

void ThreadSafety::PreCallRecordUnmapMemory(
    VkDevice                                    device,
    VkDeviceMemory                              memory) {
    StartReadObjectParentInstance(device);
    StartWriteObject(memory);
    // Host access to memory must be externally synchronized
}

void ThreadSafety::PostCallRecordUnmapMemory(
    VkDevice                                    device,
    VkDeviceMemory                              memory) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(memory);
    // Host access to memory must be externally synchronized
}

void ThreadSafety::PreCallRecordFlushMappedMemoryRanges(
    VkDevice                                    device,
    uint32_t                                    memoryRangeCount,
    const VkMappedMemoryRange*                  pMemoryRanges) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordFlushMappedMemoryRanges(
    VkDevice                                    device,
    uint32_t                                    memoryRangeCount,
    const VkMappedMemoryRange*                  pMemoryRanges,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordInvalidateMappedMemoryRanges(
    VkDevice                                    device,
    uint32_t                                    memoryRangeCount,
    const VkMappedMemoryRange*                  pMemoryRanges) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordInvalidateMappedMemoryRanges(
    VkDevice                                    device,
    uint32_t                                    memoryRangeCount,
    const VkMappedMemoryRange*                  pMemoryRanges,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordGetDeviceMemoryCommitment(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    VkDeviceSize*                               pCommittedMemoryInBytes) {
    StartReadObjectParentInstance(device);
    StartReadObject(memory);
}

void ThreadSafety::PostCallRecordGetDeviceMemoryCommitment(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    VkDeviceSize*                               pCommittedMemoryInBytes) {
    FinishReadObjectParentInstance(device);
    FinishReadObject(memory);
}

void ThreadSafety::PreCallRecordBindBufferMemory(
    VkDevice                                    device,
    VkBuffer                                    buffer,
    VkDeviceMemory                              memory,
    VkDeviceSize                                memoryOffset) {
    StartReadObjectParentInstance(device);
    StartWriteObject(buffer);
    StartReadObject(memory);
    // Host access to buffer must be externally synchronized
}

void ThreadSafety::PostCallRecordBindBufferMemory(
    VkDevice                                    device,
    VkBuffer                                    buffer,
    VkDeviceMemory                              memory,
    VkDeviceSize                                memoryOffset,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(buffer);
    FinishReadObject(memory);
    // Host access to buffer must be externally synchronized
}

void ThreadSafety::PreCallRecordBindImageMemory(
    VkDevice                                    device,
    VkImage                                     image,
    VkDeviceMemory                              memory,
    VkDeviceSize                                memoryOffset) {
    StartReadObjectParentInstance(device);
    StartWriteObject(image);
    StartReadObject(memory);
    // Host access to image must be externally synchronized
}

void ThreadSafety::PostCallRecordBindImageMemory(
    VkDevice                                    device,
    VkImage                                     image,
    VkDeviceMemory                              memory,
    VkDeviceSize                                memoryOffset,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(image);
    FinishReadObject(memory);
    // Host access to image must be externally synchronized
}

void ThreadSafety::PreCallRecordGetBufferMemoryRequirements(
    VkDevice                                    device,
    VkBuffer                                    buffer,
    VkMemoryRequirements*                       pMemoryRequirements) {
    StartReadObjectParentInstance(device);
    StartReadObject(buffer);
}

void ThreadSafety::PostCallRecordGetBufferMemoryRequirements(
    VkDevice                                    device,
    VkBuffer                                    buffer,
    VkMemoryRequirements*                       pMemoryRequirements) {
    FinishReadObjectParentInstance(device);
    FinishReadObject(buffer);
}

void ThreadSafety::PreCallRecordGetImageMemoryRequirements(
    VkDevice                                    device,
    VkImage                                     image,
    VkMemoryRequirements*                       pMemoryRequirements) {
    StartReadObjectParentInstance(device);
    StartReadObject(image);
}

void ThreadSafety::PostCallRecordGetImageMemoryRequirements(
    VkDevice                                    device,
    VkImage                                     image,
    VkMemoryRequirements*                       pMemoryRequirements) {
    FinishReadObjectParentInstance(device);
    FinishReadObject(image);
}

void ThreadSafety::PreCallRecordGetImageSparseMemoryRequirements(
    VkDevice                                    device,
    VkImage                                     image,
    uint32_t*                                   pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements*            pSparseMemoryRequirements) {
    StartReadObjectParentInstance(device);
    StartReadObject(image);
}

void ThreadSafety::PostCallRecordGetImageSparseMemoryRequirements(
    VkDevice                                    device,
    VkImage                                     image,
    uint32_t*                                   pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements*            pSparseMemoryRequirements) {
    FinishReadObjectParentInstance(device);
    FinishReadObject(image);
}

void ThreadSafety::PreCallRecordQueueBindSparse(
    VkQueue                                     queue,
    uint32_t                                    bindInfoCount,
    const VkBindSparseInfo*                     pBindInfo,
    VkFence                                     fence) {
    StartWriteObject(queue);
    if (pBindInfo) {
        for (uint32_t index=0; index < bindInfoCount; index++) {
            if (pBindInfo[index].pBufferBinds) {
                for (uint32_t index2=0; index2 < pBindInfo[index].bufferBindCount; index2++) {
                    StartWriteObject(pBindInfo[index].pBufferBinds[index2].buffer);
                }
            }
            if (pBindInfo[index].pImageOpaqueBinds) {
                for (uint32_t index2=0; index2 < pBindInfo[index].imageOpaqueBindCount; index2++) {
                    StartWriteObject(pBindInfo[index].pImageOpaqueBinds[index2].image);
                }
            }
            if (pBindInfo[index].pImageBinds) {
                for (uint32_t index2=0; index2 < pBindInfo[index].imageBindCount; index2++) {
                    StartWriteObject(pBindInfo[index].pImageBinds[index2].image);
                }
            }
        }
    }
    StartWriteObject(fence);
    // Host access to queue must be externally synchronized
    // Host access to pBindInfo[].pBufferBinds[].buffer,pBindInfo[].pImageOpaqueBinds[].image,pBindInfo[].pImageBinds[].image must be externally synchronized
    // Host access to fence must be externally synchronized
}

void ThreadSafety::PostCallRecordQueueBindSparse(
    VkQueue                                     queue,
    uint32_t                                    bindInfoCount,
    const VkBindSparseInfo*                     pBindInfo,
    VkFence                                     fence,
    VkResult                                    result) {
    FinishWriteObject(queue);
    if (pBindInfo) {
        for (uint32_t index=0; index < bindInfoCount; index++) {
            if (pBindInfo[index].pBufferBinds) {
                for (uint32_t index2=0; index2 < pBindInfo[index].bufferBindCount; index2++) {
                    FinishWriteObject(pBindInfo[index].pBufferBinds[index2].buffer);
                }
            }
            if (pBindInfo[index].pImageOpaqueBinds) {
                for (uint32_t index2=0; index2 < pBindInfo[index].imageOpaqueBindCount; index2++) {
                    FinishWriteObject(pBindInfo[index].pImageOpaqueBinds[index2].image);
                }
            }
            if (pBindInfo[index].pImageBinds) {
                for (uint32_t index2=0; index2 < pBindInfo[index].imageBindCount; index2++) {
                    FinishWriteObject(pBindInfo[index].pImageBinds[index2].image);
                }
            }
        }
    }
    FinishWriteObject(fence);
    // Host access to queue must be externally synchronized
    // Host access to pBindInfo[].pBufferBinds[].buffer,pBindInfo[].pImageOpaqueBinds[].image,pBindInfo[].pImageBinds[].image must be externally synchronized
    // Host access to fence must be externally synchronized
}

void ThreadSafety::PreCallRecordCreateFence(
    VkDevice                                    device,
    const VkFenceCreateInfo*                    pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkFence*                                    pFence) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordCreateFence(
    VkDevice                                    device,
    const VkFenceCreateInfo*                    pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkFence*                                    pFence,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    if (result == VK_SUCCESS) {
        CreateObject(*pFence);
    }
}

void ThreadSafety::PreCallRecordDestroyFence(
    VkDevice                                    device,
    VkFence                                     fence,
    const VkAllocationCallbacks*                pAllocator) {
    StartReadObjectParentInstance(device);
    StartWriteObject(fence);
    // Host access to fence must be externally synchronized
}

void ThreadSafety::PostCallRecordDestroyFence(
    VkDevice                                    device,
    VkFence                                     fence,
    const VkAllocationCallbacks*                pAllocator) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(fence);
    DestroyObject(fence);
    // Host access to fence must be externally synchronized
}

void ThreadSafety::PreCallRecordResetFences(
    VkDevice                                    device,
    uint32_t                                    fenceCount,
    const VkFence*                              pFences) {
    StartReadObjectParentInstance(device);
    if (pFences) {
        for (uint32_t index=0; index < fenceCount; index++) {
            StartWriteObject(pFences[index]);
        }
    }
    // Host access to each member of pFences must be externally synchronized
}

void ThreadSafety::PostCallRecordResetFences(
    VkDevice                                    device,
    uint32_t                                    fenceCount,
    const VkFence*                              pFences,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    if (pFences) {
        for (uint32_t index=0; index < fenceCount; index++) {
            FinishWriteObject(pFences[index]);
        }
    }
    // Host access to each member of pFences must be externally synchronized
}

void ThreadSafety::PreCallRecordGetFenceStatus(
    VkDevice                                    device,
    VkFence                                     fence) {
    StartReadObjectParentInstance(device);
    StartReadObject(fence);
}

void ThreadSafety::PostCallRecordGetFenceStatus(
    VkDevice                                    device,
    VkFence                                     fence,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishReadObject(fence);
}

void ThreadSafety::PreCallRecordWaitForFences(
    VkDevice                                    device,
    uint32_t                                    fenceCount,
    const VkFence*                              pFences,
    VkBool32                                    waitAll,
    uint64_t                                    timeout) {
    StartReadObjectParentInstance(device);
    if (pFences) {
        for (uint32_t index = 0; index < fenceCount; index++) {
            StartReadObject(pFences[index]);
        }
    }
}

void ThreadSafety::PostCallRecordWaitForFences(
    VkDevice                                    device,
    uint32_t                                    fenceCount,
    const VkFence*                              pFences,
    VkBool32                                    waitAll,
    uint64_t                                    timeout,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    if (pFences) {
        for (uint32_t index = 0; index < fenceCount; index++) {
            FinishReadObject(pFences[index]);
        }
    }
}

void ThreadSafety::PreCallRecordCreateSemaphore(
    VkDevice                                    device,
    const VkSemaphoreCreateInfo*                pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSemaphore*                                pSemaphore) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordCreateSemaphore(
    VkDevice                                    device,
    const VkSemaphoreCreateInfo*                pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSemaphore*                                pSemaphore,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    if (result == VK_SUCCESS) {
        CreateObject(*pSemaphore);
    }
}

void ThreadSafety::PreCallRecordDestroySemaphore(
    VkDevice                                    device,
    VkSemaphore                                 semaphore,
    const VkAllocationCallbacks*                pAllocator) {
    StartReadObjectParentInstance(device);
    StartWriteObject(semaphore);
    // Host access to semaphore must be externally synchronized
}

void ThreadSafety::PostCallRecordDestroySemaphore(
    VkDevice                                    device,
    VkSemaphore                                 semaphore,
    const VkAllocationCallbacks*                pAllocator) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(semaphore);
    DestroyObject(semaphore);
    // Host access to semaphore must be externally synchronized
}

void ThreadSafety::PreCallRecordCreateEvent(
    VkDevice                                    device,
    const VkEventCreateInfo*                    pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkEvent*                                    pEvent) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordCreateEvent(
    VkDevice                                    device,
    const VkEventCreateInfo*                    pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkEvent*                                    pEvent,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    if (result == VK_SUCCESS) {
        CreateObject(*pEvent);
    }
}

void ThreadSafety::PreCallRecordDestroyEvent(
    VkDevice                                    device,
    VkEvent                                     event,
    const VkAllocationCallbacks*                pAllocator) {
    StartReadObjectParentInstance(device);
    StartWriteObject(event);
    // Host access to event must be externally synchronized
}

void ThreadSafety::PostCallRecordDestroyEvent(
    VkDevice                                    device,
    VkEvent                                     event,
    const VkAllocationCallbacks*                pAllocator) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(event);
    DestroyObject(event);
    // Host access to event must be externally synchronized
}

void ThreadSafety::PreCallRecordGetEventStatus(
    VkDevice                                    device,
    VkEvent                                     event) {
    StartReadObjectParentInstance(device);
    StartReadObject(event);
}

void ThreadSafety::PostCallRecordGetEventStatus(
    VkDevice                                    device,
    VkEvent                                     event,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishReadObject(event);
}

void ThreadSafety::PreCallRecordSetEvent(
    VkDevice                                    device,
    VkEvent                                     event) {
    StartReadObjectParentInstance(device);
    StartWriteObject(event);
    // Host access to event must be externally synchronized
}

void ThreadSafety::PostCallRecordSetEvent(
    VkDevice                                    device,
    VkEvent                                     event,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(event);
    // Host access to event must be externally synchronized
}

void ThreadSafety::PreCallRecordResetEvent(
    VkDevice                                    device,
    VkEvent                                     event) {
    StartReadObjectParentInstance(device);
    StartWriteObject(event);
    // Host access to event must be externally synchronized
}

void ThreadSafety::PostCallRecordResetEvent(
    VkDevice                                    device,
    VkEvent                                     event,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(event);
    // Host access to event must be externally synchronized
}

void ThreadSafety::PreCallRecordCreateQueryPool(
    VkDevice                                    device,
    const VkQueryPoolCreateInfo*                pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkQueryPool*                                pQueryPool) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordCreateQueryPool(
    VkDevice                                    device,
    const VkQueryPoolCreateInfo*                pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkQueryPool*                                pQueryPool,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    if (result == VK_SUCCESS) {
        CreateObject(*pQueryPool);
    }
}

void ThreadSafety::PreCallRecordDestroyQueryPool(
    VkDevice                                    device,
    VkQueryPool                                 queryPool,
    const VkAllocationCallbacks*                pAllocator) {
    StartReadObjectParentInstance(device);
    StartWriteObject(queryPool);
    // Host access to queryPool must be externally synchronized
}

void ThreadSafety::PostCallRecordDestroyQueryPool(
    VkDevice                                    device,
    VkQueryPool                                 queryPool,
    const VkAllocationCallbacks*                pAllocator) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(queryPool);
    DestroyObject(queryPool);
    // Host access to queryPool must be externally synchronized
}

void ThreadSafety::PreCallRecordGetQueryPoolResults(
    VkDevice                                    device,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery,
    uint32_t                                    queryCount,
    size_t                                      dataSize,
    void*                                       pData,
    VkDeviceSize                                stride,
    VkQueryResultFlags                          flags) {
    StartReadObjectParentInstance(device);
    StartReadObject(queryPool);
}

void ThreadSafety::PostCallRecordGetQueryPoolResults(
    VkDevice                                    device,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery,
    uint32_t                                    queryCount,
    size_t                                      dataSize,
    void*                                       pData,
    VkDeviceSize                                stride,
    VkQueryResultFlags                          flags,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishReadObject(queryPool);
}

void ThreadSafety::PreCallRecordCreateBuffer(
    VkDevice                                    device,
    const VkBufferCreateInfo*                   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkBuffer*                                   pBuffer) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordCreateBuffer(
    VkDevice                                    device,
    const VkBufferCreateInfo*                   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkBuffer*                                   pBuffer,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    if (result == VK_SUCCESS) {
        CreateObject(*pBuffer);
    }
}

void ThreadSafety::PreCallRecordDestroyBuffer(
    VkDevice                                    device,
    VkBuffer                                    buffer,
    const VkAllocationCallbacks*                pAllocator) {
    StartReadObjectParentInstance(device);
    StartWriteObject(buffer);
    // Host access to buffer must be externally synchronized
}

void ThreadSafety::PostCallRecordDestroyBuffer(
    VkDevice                                    device,
    VkBuffer                                    buffer,
    const VkAllocationCallbacks*                pAllocator) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(buffer);
    DestroyObject(buffer);
    // Host access to buffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCreateBufferView(
    VkDevice                                    device,
    const VkBufferViewCreateInfo*               pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkBufferView*                               pView) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordCreateBufferView(
    VkDevice                                    device,
    const VkBufferViewCreateInfo*               pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkBufferView*                               pView,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    if (result == VK_SUCCESS) {
        CreateObject(*pView);
    }
}

void ThreadSafety::PreCallRecordDestroyBufferView(
    VkDevice                                    device,
    VkBufferView                                bufferView,
    const VkAllocationCallbacks*                pAllocator) {
    StartReadObjectParentInstance(device);
    StartWriteObject(bufferView);
    // Host access to bufferView must be externally synchronized
}

void ThreadSafety::PostCallRecordDestroyBufferView(
    VkDevice                                    device,
    VkBufferView                                bufferView,
    const VkAllocationCallbacks*                pAllocator) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(bufferView);
    DestroyObject(bufferView);
    // Host access to bufferView must be externally synchronized
}

void ThreadSafety::PreCallRecordCreateImage(
    VkDevice                                    device,
    const VkImageCreateInfo*                    pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkImage*                                    pImage) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordCreateImage(
    VkDevice                                    device,
    const VkImageCreateInfo*                    pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkImage*                                    pImage,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    if (result == VK_SUCCESS) {
        CreateObject(*pImage);
    }
}

void ThreadSafety::PreCallRecordDestroyImage(
    VkDevice                                    device,
    VkImage                                     image,
    const VkAllocationCallbacks*                pAllocator) {
    StartReadObjectParentInstance(device);
    StartWriteObject(image);
    // Host access to image must be externally synchronized
}

void ThreadSafety::PostCallRecordDestroyImage(
    VkDevice                                    device,
    VkImage                                     image,
    const VkAllocationCallbacks*                pAllocator) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(image);
    DestroyObject(image);
    // Host access to image must be externally synchronized
}

void ThreadSafety::PreCallRecordGetImageSubresourceLayout(
    VkDevice                                    device,
    VkImage                                     image,
    const VkImageSubresource*                   pSubresource,
    VkSubresourceLayout*                        pLayout) {
    StartReadObjectParentInstance(device);
    StartReadObject(image);
}

void ThreadSafety::PostCallRecordGetImageSubresourceLayout(
    VkDevice                                    device,
    VkImage                                     image,
    const VkImageSubresource*                   pSubresource,
    VkSubresourceLayout*                        pLayout) {
    FinishReadObjectParentInstance(device);
    FinishReadObject(image);
}

void ThreadSafety::PreCallRecordCreateImageView(
    VkDevice                                    device,
    const VkImageViewCreateInfo*                pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkImageView*                                pView) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordCreateImageView(
    VkDevice                                    device,
    const VkImageViewCreateInfo*                pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkImageView*                                pView,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    if (result == VK_SUCCESS) {
        CreateObject(*pView);
    }
}

void ThreadSafety::PreCallRecordDestroyImageView(
    VkDevice                                    device,
    VkImageView                                 imageView,
    const VkAllocationCallbacks*                pAllocator) {
    StartReadObjectParentInstance(device);
    StartWriteObject(imageView);
    // Host access to imageView must be externally synchronized
}

void ThreadSafety::PostCallRecordDestroyImageView(
    VkDevice                                    device,
    VkImageView                                 imageView,
    const VkAllocationCallbacks*                pAllocator) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(imageView);
    DestroyObject(imageView);
    // Host access to imageView must be externally synchronized
}

void ThreadSafety::PreCallRecordCreateShaderModule(
    VkDevice                                    device,
    const VkShaderModuleCreateInfo*             pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkShaderModule*                             pShaderModule) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordCreateShaderModule(
    VkDevice                                    device,
    const VkShaderModuleCreateInfo*             pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkShaderModule*                             pShaderModule,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    if (result == VK_SUCCESS) {
        CreateObject(*pShaderModule);
    }
}

void ThreadSafety::PreCallRecordDestroyShaderModule(
    VkDevice                                    device,
    VkShaderModule                              shaderModule,
    const VkAllocationCallbacks*                pAllocator) {
    StartReadObjectParentInstance(device);
    StartWriteObject(shaderModule);
    // Host access to shaderModule must be externally synchronized
}

void ThreadSafety::PostCallRecordDestroyShaderModule(
    VkDevice                                    device,
    VkShaderModule                              shaderModule,
    const VkAllocationCallbacks*                pAllocator) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(shaderModule);
    DestroyObject(shaderModule);
    // Host access to shaderModule must be externally synchronized
}

void ThreadSafety::PreCallRecordCreatePipelineCache(
    VkDevice                                    device,
    const VkPipelineCacheCreateInfo*            pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkPipelineCache*                            pPipelineCache) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordCreatePipelineCache(
    VkDevice                                    device,
    const VkPipelineCacheCreateInfo*            pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkPipelineCache*                            pPipelineCache,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    if (result == VK_SUCCESS) {
        CreateObject(*pPipelineCache);
    }
}

void ThreadSafety::PreCallRecordDestroyPipelineCache(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    const VkAllocationCallbacks*                pAllocator) {
    StartReadObjectParentInstance(device);
    StartWriteObject(pipelineCache);
    // Host access to pipelineCache must be externally synchronized
}

void ThreadSafety::PostCallRecordDestroyPipelineCache(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    const VkAllocationCallbacks*                pAllocator) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(pipelineCache);
    DestroyObject(pipelineCache);
    // Host access to pipelineCache must be externally synchronized
}

void ThreadSafety::PreCallRecordGetPipelineCacheData(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    size_t*                                     pDataSize,
    void*                                       pData) {
    StartReadObjectParentInstance(device);
    StartReadObject(pipelineCache);
}

void ThreadSafety::PostCallRecordGetPipelineCacheData(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    size_t*                                     pDataSize,
    void*                                       pData,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishReadObject(pipelineCache);
}

void ThreadSafety::PreCallRecordMergePipelineCaches(
    VkDevice                                    device,
    VkPipelineCache                             dstCache,
    uint32_t                                    srcCacheCount,
    const VkPipelineCache*                      pSrcCaches) {
    StartReadObjectParentInstance(device);
    StartWriteObject(dstCache);
    if (pSrcCaches) {
        for (uint32_t index = 0; index < srcCacheCount; index++) {
            StartReadObject(pSrcCaches[index]);
        }
    }
    // Host access to dstCache must be externally synchronized
}

void ThreadSafety::PostCallRecordMergePipelineCaches(
    VkDevice                                    device,
    VkPipelineCache                             dstCache,
    uint32_t                                    srcCacheCount,
    const VkPipelineCache*                      pSrcCaches,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(dstCache);
    if (pSrcCaches) {
        for (uint32_t index = 0; index < srcCacheCount; index++) {
            FinishReadObject(pSrcCaches[index]);
        }
    }
    // Host access to dstCache must be externally synchronized
}

void ThreadSafety::PreCallRecordCreateGraphicsPipelines(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    uint32_t                                    createInfoCount,
    const VkGraphicsPipelineCreateInfo*         pCreateInfos,
    const VkAllocationCallbacks*                pAllocator,
    VkPipeline*                                 pPipelines) {
    StartReadObjectParentInstance(device);
    StartReadObject(pipelineCache);
}

void ThreadSafety::PostCallRecordCreateGraphicsPipelines(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    uint32_t                                    createInfoCount,
    const VkGraphicsPipelineCreateInfo*         pCreateInfos,
    const VkAllocationCallbacks*                pAllocator,
    VkPipeline*                                 pPipelines,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishReadObject(pipelineCache);
    if (pPipelines) {
        for (uint32_t index = 0; index < createInfoCount; index++) {
            if (!pPipelines[index]) continue;
            CreateObject(pPipelines[index]);
        }
    }
}

void ThreadSafety::PreCallRecordCreateComputePipelines(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    uint32_t                                    createInfoCount,
    const VkComputePipelineCreateInfo*          pCreateInfos,
    const VkAllocationCallbacks*                pAllocator,
    VkPipeline*                                 pPipelines) {
    StartReadObjectParentInstance(device);
    StartReadObject(pipelineCache);
}

void ThreadSafety::PostCallRecordCreateComputePipelines(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    uint32_t                                    createInfoCount,
    const VkComputePipelineCreateInfo*          pCreateInfos,
    const VkAllocationCallbacks*                pAllocator,
    VkPipeline*                                 pPipelines,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishReadObject(pipelineCache);
    if (pPipelines) {
        for (uint32_t index = 0; index < createInfoCount; index++) {
            if (!pPipelines[index]) continue;
            CreateObject(pPipelines[index]);
        }
    }
}

void ThreadSafety::PreCallRecordDestroyPipeline(
    VkDevice                                    device,
    VkPipeline                                  pipeline,
    const VkAllocationCallbacks*                pAllocator) {
    StartReadObjectParentInstance(device);
    StartWriteObject(pipeline);
    // Host access to pipeline must be externally synchronized
}

void ThreadSafety::PostCallRecordDestroyPipeline(
    VkDevice                                    device,
    VkPipeline                                  pipeline,
    const VkAllocationCallbacks*                pAllocator) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(pipeline);
    DestroyObject(pipeline);
    // Host access to pipeline must be externally synchronized
}

void ThreadSafety::PreCallRecordCreatePipelineLayout(
    VkDevice                                    device,
    const VkPipelineLayoutCreateInfo*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkPipelineLayout*                           pPipelineLayout) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordCreatePipelineLayout(
    VkDevice                                    device,
    const VkPipelineLayoutCreateInfo*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkPipelineLayout*                           pPipelineLayout,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    if (result == VK_SUCCESS) {
        CreateObject(*pPipelineLayout);
    }
}

void ThreadSafety::PreCallRecordDestroyPipelineLayout(
    VkDevice                                    device,
    VkPipelineLayout                            pipelineLayout,
    const VkAllocationCallbacks*                pAllocator) {
    StartReadObjectParentInstance(device);
    StartWriteObject(pipelineLayout);
    // Host access to pipelineLayout must be externally synchronized
}

void ThreadSafety::PostCallRecordDestroyPipelineLayout(
    VkDevice                                    device,
    VkPipelineLayout                            pipelineLayout,
    const VkAllocationCallbacks*                pAllocator) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(pipelineLayout);
    DestroyObject(pipelineLayout);
    // Host access to pipelineLayout must be externally synchronized
}

void ThreadSafety::PreCallRecordCreateSampler(
    VkDevice                                    device,
    const VkSamplerCreateInfo*                  pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSampler*                                  pSampler) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordCreateSampler(
    VkDevice                                    device,
    const VkSamplerCreateInfo*                  pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSampler*                                  pSampler,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    if (result == VK_SUCCESS) {
        CreateObject(*pSampler);
    }
}

void ThreadSafety::PreCallRecordDestroySampler(
    VkDevice                                    device,
    VkSampler                                   sampler,
    const VkAllocationCallbacks*                pAllocator) {
    StartReadObjectParentInstance(device);
    StartWriteObject(sampler);
    // Host access to sampler must be externally synchronized
}

void ThreadSafety::PostCallRecordDestroySampler(
    VkDevice                                    device,
    VkSampler                                   sampler,
    const VkAllocationCallbacks*                pAllocator) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(sampler);
    DestroyObject(sampler);
    // Host access to sampler must be externally synchronized
}

void ThreadSafety::PreCallRecordCreateDescriptorSetLayout(
    VkDevice                                    device,
    const VkDescriptorSetLayoutCreateInfo*      pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDescriptorSetLayout*                      pSetLayout) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordCreateDescriptorSetLayout(
    VkDevice                                    device,
    const VkDescriptorSetLayoutCreateInfo*      pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDescriptorSetLayout*                      pSetLayout,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    if (result == VK_SUCCESS) {
        CreateObject(*pSetLayout);
    }
}

void ThreadSafety::PreCallRecordDestroyDescriptorSetLayout(
    VkDevice                                    device,
    VkDescriptorSetLayout                       descriptorSetLayout,
    const VkAllocationCallbacks*                pAllocator) {
    StartReadObjectParentInstance(device);
    StartWriteObject(descriptorSetLayout);
    // Host access to descriptorSetLayout must be externally synchronized
}

void ThreadSafety::PostCallRecordDestroyDescriptorSetLayout(
    VkDevice                                    device,
    VkDescriptorSetLayout                       descriptorSetLayout,
    const VkAllocationCallbacks*                pAllocator) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(descriptorSetLayout);
    DestroyObject(descriptorSetLayout);
    // Host access to descriptorSetLayout must be externally synchronized
}

void ThreadSafety::PreCallRecordCreateDescriptorPool(
    VkDevice                                    device,
    const VkDescriptorPoolCreateInfo*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDescriptorPool*                           pDescriptorPool) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordCreateDescriptorPool(
    VkDevice                                    device,
    const VkDescriptorPoolCreateInfo*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDescriptorPool*                           pDescriptorPool,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    if (result == VK_SUCCESS) {
        CreateObject(*pDescriptorPool);
    }
}

void ThreadSafety::PreCallRecordUpdateDescriptorSets(
    VkDevice                                    device,
    uint32_t                                    descriptorWriteCount,
    const VkWriteDescriptorSet*                 pDescriptorWrites,
    uint32_t                                    descriptorCopyCount,
    const VkCopyDescriptorSet*                  pDescriptorCopies) {
    StartReadObjectParentInstance(device);
    if (pDescriptorWrites) {
        for (uint32_t index=0; index < descriptorWriteCount; index++) {
            StartWriteObject(pDescriptorWrites[index].dstSet);
        }
    }
    if (pDescriptorCopies) {
        for (uint32_t index=0; index < descriptorCopyCount; index++) {
            StartWriteObject(pDescriptorCopies[index].dstSet);
        }
    }
    // Host access to pDescriptorWrites[].dstSet must be externally synchronized
    // Host access to pDescriptorCopies[].dstSet must be externally synchronized
}

void ThreadSafety::PostCallRecordUpdateDescriptorSets(
    VkDevice                                    device,
    uint32_t                                    descriptorWriteCount,
    const VkWriteDescriptorSet*                 pDescriptorWrites,
    uint32_t                                    descriptorCopyCount,
    const VkCopyDescriptorSet*                  pDescriptorCopies) {
    FinishReadObjectParentInstance(device);
    if (pDescriptorWrites) {
        for (uint32_t index=0; index < descriptorWriteCount; index++) {
            FinishWriteObject(pDescriptorWrites[index].dstSet);
        }
    }
    if (pDescriptorCopies) {
        for (uint32_t index=0; index < descriptorCopyCount; index++) {
            FinishWriteObject(pDescriptorCopies[index].dstSet);
        }
    }
    // Host access to pDescriptorWrites[].dstSet must be externally synchronized
    // Host access to pDescriptorCopies[].dstSet must be externally synchronized
}

void ThreadSafety::PreCallRecordCreateFramebuffer(
    VkDevice                                    device,
    const VkFramebufferCreateInfo*              pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkFramebuffer*                              pFramebuffer) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordCreateFramebuffer(
    VkDevice                                    device,
    const VkFramebufferCreateInfo*              pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkFramebuffer*                              pFramebuffer,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    if (result == VK_SUCCESS) {
        CreateObject(*pFramebuffer);
    }
}

void ThreadSafety::PreCallRecordDestroyFramebuffer(
    VkDevice                                    device,
    VkFramebuffer                               framebuffer,
    const VkAllocationCallbacks*                pAllocator) {
    StartReadObjectParentInstance(device);
    StartWriteObject(framebuffer);
    // Host access to framebuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordDestroyFramebuffer(
    VkDevice                                    device,
    VkFramebuffer                               framebuffer,
    const VkAllocationCallbacks*                pAllocator) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(framebuffer);
    DestroyObject(framebuffer);
    // Host access to framebuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCreateRenderPass(
    VkDevice                                    device,
    const VkRenderPassCreateInfo*               pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkRenderPass*                               pRenderPass) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordCreateRenderPass(
    VkDevice                                    device,
    const VkRenderPassCreateInfo*               pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkRenderPass*                               pRenderPass,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    if (result == VK_SUCCESS) {
        CreateObject(*pRenderPass);
    }
}

void ThreadSafety::PreCallRecordDestroyRenderPass(
    VkDevice                                    device,
    VkRenderPass                                renderPass,
    const VkAllocationCallbacks*                pAllocator) {
    StartReadObjectParentInstance(device);
    StartWriteObject(renderPass);
    // Host access to renderPass must be externally synchronized
}

void ThreadSafety::PostCallRecordDestroyRenderPass(
    VkDevice                                    device,
    VkRenderPass                                renderPass,
    const VkAllocationCallbacks*                pAllocator) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(renderPass);
    DestroyObject(renderPass);
    // Host access to renderPass must be externally synchronized
}

void ThreadSafety::PreCallRecordGetRenderAreaGranularity(
    VkDevice                                    device,
    VkRenderPass                                renderPass,
    VkExtent2D*                                 pGranularity) {
    StartReadObjectParentInstance(device);
    StartReadObject(renderPass);
}

void ThreadSafety::PostCallRecordGetRenderAreaGranularity(
    VkDevice                                    device,
    VkRenderPass                                renderPass,
    VkExtent2D*                                 pGranularity) {
    FinishReadObjectParentInstance(device);
    FinishReadObject(renderPass);
}

void ThreadSafety::PreCallRecordBeginCommandBuffer(
    VkCommandBuffer                             commandBuffer,
    const VkCommandBufferBeginInfo*             pBeginInfo) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
    // the sname:VkCommandPool that pname:commandBuffer was allocated from must be externally synchronized between host accesses
}

void ThreadSafety::PostCallRecordBeginCommandBuffer(
    VkCommandBuffer                             commandBuffer,
    const VkCommandBufferBeginInfo*             pBeginInfo,
    VkResult                                    result) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
    // the sname:VkCommandPool that pname:commandBuffer was allocated from must be externally synchronized between host accesses
}

void ThreadSafety::PreCallRecordEndCommandBuffer(
    VkCommandBuffer                             commandBuffer) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
    // the sname:VkCommandPool that pname:commandBuffer was allocated from must be externally synchronized between host accesses
}

void ThreadSafety::PostCallRecordEndCommandBuffer(
    VkCommandBuffer                             commandBuffer,
    VkResult                                    result) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
    // the sname:VkCommandPool that pname:commandBuffer was allocated from must be externally synchronized between host accesses
}

void ThreadSafety::PreCallRecordResetCommandBuffer(
    VkCommandBuffer                             commandBuffer,
    VkCommandBufferResetFlags                   flags) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordResetCommandBuffer(
    VkCommandBuffer                             commandBuffer,
    VkCommandBufferResetFlags                   flags,
    VkResult                                    result) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdBindPipeline(
    VkCommandBuffer                             commandBuffer,
    VkPipelineBindPoint                         pipelineBindPoint,
    VkPipeline                                  pipeline) {
    StartWriteObject(commandBuffer);
    StartReadObject(pipeline);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdBindPipeline(
    VkCommandBuffer                             commandBuffer,
    VkPipelineBindPoint                         pipelineBindPoint,
    VkPipeline                                  pipeline) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(pipeline);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdSetViewport(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstViewport,
    uint32_t                                    viewportCount,
    const VkViewport*                           pViewports) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdSetViewport(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstViewport,
    uint32_t                                    viewportCount,
    const VkViewport*                           pViewports) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdSetScissor(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstScissor,
    uint32_t                                    scissorCount,
    const VkRect2D*                             pScissors) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdSetScissor(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstScissor,
    uint32_t                                    scissorCount,
    const VkRect2D*                             pScissors) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdSetLineWidth(
    VkCommandBuffer                             commandBuffer,
    float                                       lineWidth) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdSetLineWidth(
    VkCommandBuffer                             commandBuffer,
    float                                       lineWidth) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdSetDepthBias(
    VkCommandBuffer                             commandBuffer,
    float                                       depthBiasConstantFactor,
    float                                       depthBiasClamp,
    float                                       depthBiasSlopeFactor) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdSetDepthBias(
    VkCommandBuffer                             commandBuffer,
    float                                       depthBiasConstantFactor,
    float                                       depthBiasClamp,
    float                                       depthBiasSlopeFactor) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdSetBlendConstants(
    VkCommandBuffer                             commandBuffer,
    const float                                 blendConstants[4]) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdSetBlendConstants(
    VkCommandBuffer                             commandBuffer,
    const float                                 blendConstants[4]) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdSetDepthBounds(
    VkCommandBuffer                             commandBuffer,
    float                                       minDepthBounds,
    float                                       maxDepthBounds) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdSetDepthBounds(
    VkCommandBuffer                             commandBuffer,
    float                                       minDepthBounds,
    float                                       maxDepthBounds) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdSetStencilCompareMask(
    VkCommandBuffer                             commandBuffer,
    VkStencilFaceFlags                          faceMask,
    uint32_t                                    compareMask) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdSetStencilCompareMask(
    VkCommandBuffer                             commandBuffer,
    VkStencilFaceFlags                          faceMask,
    uint32_t                                    compareMask) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdSetStencilWriteMask(
    VkCommandBuffer                             commandBuffer,
    VkStencilFaceFlags                          faceMask,
    uint32_t                                    writeMask) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdSetStencilWriteMask(
    VkCommandBuffer                             commandBuffer,
    VkStencilFaceFlags                          faceMask,
    uint32_t                                    writeMask) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdSetStencilReference(
    VkCommandBuffer                             commandBuffer,
    VkStencilFaceFlags                          faceMask,
    uint32_t                                    reference) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdSetStencilReference(
    VkCommandBuffer                             commandBuffer,
    VkStencilFaceFlags                          faceMask,
    uint32_t                                    reference) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdBindDescriptorSets(
    VkCommandBuffer                             commandBuffer,
    VkPipelineBindPoint                         pipelineBindPoint,
    VkPipelineLayout                            layout,
    uint32_t                                    firstSet,
    uint32_t                                    descriptorSetCount,
    const VkDescriptorSet*                      pDescriptorSets,
    uint32_t                                    dynamicOffsetCount,
    const uint32_t*                             pDynamicOffsets) {
    StartWriteObject(commandBuffer);
    StartReadObject(layout);
    if (pDescriptorSets) {
        for (uint32_t index = 0; index < descriptorSetCount; index++) {
            StartReadObject(pDescriptorSets[index]);
        }
    }
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdBindDescriptorSets(
    VkCommandBuffer                             commandBuffer,
    VkPipelineBindPoint                         pipelineBindPoint,
    VkPipelineLayout                            layout,
    uint32_t                                    firstSet,
    uint32_t                                    descriptorSetCount,
    const VkDescriptorSet*                      pDescriptorSets,
    uint32_t                                    dynamicOffsetCount,
    const uint32_t*                             pDynamicOffsets) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(layout);
    if (pDescriptorSets) {
        for (uint32_t index = 0; index < descriptorSetCount; index++) {
            FinishReadObject(pDescriptorSets[index]);
        }
    }
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdBindIndexBuffer(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkIndexType                                 indexType) {
    StartWriteObject(commandBuffer);
    StartReadObject(buffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdBindIndexBuffer(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkIndexType                                 indexType) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(buffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdBindVertexBuffers(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstBinding,
    uint32_t                                    bindingCount,
    const VkBuffer*                             pBuffers,
    const VkDeviceSize*                         pOffsets) {
    StartWriteObject(commandBuffer);
    if (pBuffers) {
        for (uint32_t index = 0; index < bindingCount; index++) {
            StartReadObject(pBuffers[index]);
        }
    }
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdBindVertexBuffers(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstBinding,
    uint32_t                                    bindingCount,
    const VkBuffer*                             pBuffers,
    const VkDeviceSize*                         pOffsets) {
    FinishWriteObject(commandBuffer);
    if (pBuffers) {
        for (uint32_t index = 0; index < bindingCount; index++) {
            FinishReadObject(pBuffers[index]);
        }
    }
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdDraw(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    vertexCount,
    uint32_t                                    instanceCount,
    uint32_t                                    firstVertex,
    uint32_t                                    firstInstance) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdDraw(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    vertexCount,
    uint32_t                                    instanceCount,
    uint32_t                                    firstVertex,
    uint32_t                                    firstInstance) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdDrawIndexed(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    indexCount,
    uint32_t                                    instanceCount,
    uint32_t                                    firstIndex,
    int32_t                                     vertexOffset,
    uint32_t                                    firstInstance) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdDrawIndexed(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    indexCount,
    uint32_t                                    instanceCount,
    uint32_t                                    firstIndex,
    int32_t                                     vertexOffset,
    uint32_t                                    firstInstance) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdDrawIndirect(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    uint32_t                                    drawCount,
    uint32_t                                    stride) {
    StartWriteObject(commandBuffer);
    StartReadObject(buffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdDrawIndirect(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    uint32_t                                    drawCount,
    uint32_t                                    stride) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(buffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdDrawIndexedIndirect(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    uint32_t                                    drawCount,
    uint32_t                                    stride) {
    StartWriteObject(commandBuffer);
    StartReadObject(buffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdDrawIndexedIndirect(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    uint32_t                                    drawCount,
    uint32_t                                    stride) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(buffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdDispatch(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    groupCountX,
    uint32_t                                    groupCountY,
    uint32_t                                    groupCountZ) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdDispatch(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    groupCountX,
    uint32_t                                    groupCountY,
    uint32_t                                    groupCountZ) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdDispatchIndirect(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset) {
    StartWriteObject(commandBuffer);
    StartReadObject(buffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdDispatchIndirect(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(buffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdCopyBuffer(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    srcBuffer,
    VkBuffer                                    dstBuffer,
    uint32_t                                    regionCount,
    const VkBufferCopy*                         pRegions) {
    StartWriteObject(commandBuffer);
    StartReadObject(srcBuffer);
    StartReadObject(dstBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdCopyBuffer(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    srcBuffer,
    VkBuffer                                    dstBuffer,
    uint32_t                                    regionCount,
    const VkBufferCopy*                         pRegions) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(srcBuffer);
    FinishReadObject(dstBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdCopyImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     srcImage,
    VkImageLayout                               srcImageLayout,
    VkImage                                     dstImage,
    VkImageLayout                               dstImageLayout,
    uint32_t                                    regionCount,
    const VkImageCopy*                          pRegions) {
    StartWriteObject(commandBuffer);
    StartReadObject(srcImage);
    StartReadObject(dstImage);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdCopyImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     srcImage,
    VkImageLayout                               srcImageLayout,
    VkImage                                     dstImage,
    VkImageLayout                               dstImageLayout,
    uint32_t                                    regionCount,
    const VkImageCopy*                          pRegions) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(srcImage);
    FinishReadObject(dstImage);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdBlitImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     srcImage,
    VkImageLayout                               srcImageLayout,
    VkImage                                     dstImage,
    VkImageLayout                               dstImageLayout,
    uint32_t                                    regionCount,
    const VkImageBlit*                          pRegions,
    VkFilter                                    filter) {
    StartWriteObject(commandBuffer);
    StartReadObject(srcImage);
    StartReadObject(dstImage);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdBlitImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     srcImage,
    VkImageLayout                               srcImageLayout,
    VkImage                                     dstImage,
    VkImageLayout                               dstImageLayout,
    uint32_t                                    regionCount,
    const VkImageBlit*                          pRegions,
    VkFilter                                    filter) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(srcImage);
    FinishReadObject(dstImage);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdCopyBufferToImage(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    srcBuffer,
    VkImage                                     dstImage,
    VkImageLayout                               dstImageLayout,
    uint32_t                                    regionCount,
    const VkBufferImageCopy*                    pRegions) {
    StartWriteObject(commandBuffer);
    StartReadObject(srcBuffer);
    StartReadObject(dstImage);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdCopyBufferToImage(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    srcBuffer,
    VkImage                                     dstImage,
    VkImageLayout                               dstImageLayout,
    uint32_t                                    regionCount,
    const VkBufferImageCopy*                    pRegions) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(srcBuffer);
    FinishReadObject(dstImage);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdCopyImageToBuffer(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     srcImage,
    VkImageLayout                               srcImageLayout,
    VkBuffer                                    dstBuffer,
    uint32_t                                    regionCount,
    const VkBufferImageCopy*                    pRegions) {
    StartWriteObject(commandBuffer);
    StartReadObject(srcImage);
    StartReadObject(dstBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdCopyImageToBuffer(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     srcImage,
    VkImageLayout                               srcImageLayout,
    VkBuffer                                    dstBuffer,
    uint32_t                                    regionCount,
    const VkBufferImageCopy*                    pRegions) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(srcImage);
    FinishReadObject(dstBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdUpdateBuffer(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    dstBuffer,
    VkDeviceSize                                dstOffset,
    VkDeviceSize                                dataSize,
    const void*                                 pData) {
    StartWriteObject(commandBuffer);
    StartReadObject(dstBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdUpdateBuffer(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    dstBuffer,
    VkDeviceSize                                dstOffset,
    VkDeviceSize                                dataSize,
    const void*                                 pData) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(dstBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdFillBuffer(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    dstBuffer,
    VkDeviceSize                                dstOffset,
    VkDeviceSize                                size,
    uint32_t                                    data) {
    StartWriteObject(commandBuffer);
    StartReadObject(dstBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdFillBuffer(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    dstBuffer,
    VkDeviceSize                                dstOffset,
    VkDeviceSize                                size,
    uint32_t                                    data) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(dstBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdClearColorImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     image,
    VkImageLayout                               imageLayout,
    const VkClearColorValue*                    pColor,
    uint32_t                                    rangeCount,
    const VkImageSubresourceRange*              pRanges) {
    StartWriteObject(commandBuffer);
    StartReadObject(image);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdClearColorImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     image,
    VkImageLayout                               imageLayout,
    const VkClearColorValue*                    pColor,
    uint32_t                                    rangeCount,
    const VkImageSubresourceRange*              pRanges) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(image);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdClearDepthStencilImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     image,
    VkImageLayout                               imageLayout,
    const VkClearDepthStencilValue*             pDepthStencil,
    uint32_t                                    rangeCount,
    const VkImageSubresourceRange*              pRanges) {
    StartWriteObject(commandBuffer);
    StartReadObject(image);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdClearDepthStencilImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     image,
    VkImageLayout                               imageLayout,
    const VkClearDepthStencilValue*             pDepthStencil,
    uint32_t                                    rangeCount,
    const VkImageSubresourceRange*              pRanges) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(image);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdClearAttachments(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    attachmentCount,
    const VkClearAttachment*                    pAttachments,
    uint32_t                                    rectCount,
    const VkClearRect*                          pRects) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdClearAttachments(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    attachmentCount,
    const VkClearAttachment*                    pAttachments,
    uint32_t                                    rectCount,
    const VkClearRect*                          pRects) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdResolveImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     srcImage,
    VkImageLayout                               srcImageLayout,
    VkImage                                     dstImage,
    VkImageLayout                               dstImageLayout,
    uint32_t                                    regionCount,
    const VkImageResolve*                       pRegions) {
    StartWriteObject(commandBuffer);
    StartReadObject(srcImage);
    StartReadObject(dstImage);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdResolveImage(
    VkCommandBuffer                             commandBuffer,
    VkImage                                     srcImage,
    VkImageLayout                               srcImageLayout,
    VkImage                                     dstImage,
    VkImageLayout                               dstImageLayout,
    uint32_t                                    regionCount,
    const VkImageResolve*                       pRegions) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(srcImage);
    FinishReadObject(dstImage);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdSetEvent(
    VkCommandBuffer                             commandBuffer,
    VkEvent                                     event,
    VkPipelineStageFlags                        stageMask) {
    StartWriteObject(commandBuffer);
    StartReadObject(event);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdSetEvent(
    VkCommandBuffer                             commandBuffer,
    VkEvent                                     event,
    VkPipelineStageFlags                        stageMask) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(event);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdResetEvent(
    VkCommandBuffer                             commandBuffer,
    VkEvent                                     event,
    VkPipelineStageFlags                        stageMask) {
    StartWriteObject(commandBuffer);
    StartReadObject(event);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdResetEvent(
    VkCommandBuffer                             commandBuffer,
    VkEvent                                     event,
    VkPipelineStageFlags                        stageMask) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(event);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdWaitEvents(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    eventCount,
    const VkEvent*                              pEvents,
    VkPipelineStageFlags                        srcStageMask,
    VkPipelineStageFlags                        dstStageMask,
    uint32_t                                    memoryBarrierCount,
    const VkMemoryBarrier*                      pMemoryBarriers,
    uint32_t                                    bufferMemoryBarrierCount,
    const VkBufferMemoryBarrier*                pBufferMemoryBarriers,
    uint32_t                                    imageMemoryBarrierCount,
    const VkImageMemoryBarrier*                 pImageMemoryBarriers) {
    StartWriteObject(commandBuffer);
    if (pEvents) {
        for (uint32_t index = 0; index < eventCount; index++) {
            StartReadObject(pEvents[index]);
        }
    }
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdWaitEvents(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    eventCount,
    const VkEvent*                              pEvents,
    VkPipelineStageFlags                        srcStageMask,
    VkPipelineStageFlags                        dstStageMask,
    uint32_t                                    memoryBarrierCount,
    const VkMemoryBarrier*                      pMemoryBarriers,
    uint32_t                                    bufferMemoryBarrierCount,
    const VkBufferMemoryBarrier*                pBufferMemoryBarriers,
    uint32_t                                    imageMemoryBarrierCount,
    const VkImageMemoryBarrier*                 pImageMemoryBarriers) {
    FinishWriteObject(commandBuffer);
    if (pEvents) {
        for (uint32_t index = 0; index < eventCount; index++) {
            FinishReadObject(pEvents[index]);
        }
    }
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdPipelineBarrier(
    VkCommandBuffer                             commandBuffer,
    VkPipelineStageFlags                        srcStageMask,
    VkPipelineStageFlags                        dstStageMask,
    VkDependencyFlags                           dependencyFlags,
    uint32_t                                    memoryBarrierCount,
    const VkMemoryBarrier*                      pMemoryBarriers,
    uint32_t                                    bufferMemoryBarrierCount,
    const VkBufferMemoryBarrier*                pBufferMemoryBarriers,
    uint32_t                                    imageMemoryBarrierCount,
    const VkImageMemoryBarrier*                 pImageMemoryBarriers) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdPipelineBarrier(
    VkCommandBuffer                             commandBuffer,
    VkPipelineStageFlags                        srcStageMask,
    VkPipelineStageFlags                        dstStageMask,
    VkDependencyFlags                           dependencyFlags,
    uint32_t                                    memoryBarrierCount,
    const VkMemoryBarrier*                      pMemoryBarriers,
    uint32_t                                    bufferMemoryBarrierCount,
    const VkBufferMemoryBarrier*                pBufferMemoryBarriers,
    uint32_t                                    imageMemoryBarrierCount,
    const VkImageMemoryBarrier*                 pImageMemoryBarriers) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdBeginQuery(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    query,
    VkQueryControlFlags                         flags) {
    StartWriteObject(commandBuffer);
    StartReadObject(queryPool);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdBeginQuery(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    query,
    VkQueryControlFlags                         flags) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(queryPool);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdEndQuery(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    query) {
    StartWriteObject(commandBuffer);
    StartReadObject(queryPool);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdEndQuery(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    query) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(queryPool);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdResetQueryPool(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery,
    uint32_t                                    queryCount) {
    StartWriteObject(commandBuffer);
    StartReadObject(queryPool);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdResetQueryPool(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery,
    uint32_t                                    queryCount) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(queryPool);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdWriteTimestamp(
    VkCommandBuffer                             commandBuffer,
    VkPipelineStageFlagBits                     pipelineStage,
    VkQueryPool                                 queryPool,
    uint32_t                                    query) {
    StartWriteObject(commandBuffer);
    StartReadObject(queryPool);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdWriteTimestamp(
    VkCommandBuffer                             commandBuffer,
    VkPipelineStageFlagBits                     pipelineStage,
    VkQueryPool                                 queryPool,
    uint32_t                                    query) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(queryPool);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdCopyQueryPoolResults(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery,
    uint32_t                                    queryCount,
    VkBuffer                                    dstBuffer,
    VkDeviceSize                                dstOffset,
    VkDeviceSize                                stride,
    VkQueryResultFlags                          flags) {
    StartWriteObject(commandBuffer);
    StartReadObject(queryPool);
    StartReadObject(dstBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdCopyQueryPoolResults(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery,
    uint32_t                                    queryCount,
    VkBuffer                                    dstBuffer,
    VkDeviceSize                                dstOffset,
    VkDeviceSize                                stride,
    VkQueryResultFlags                          flags) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(queryPool);
    FinishReadObject(dstBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdPushConstants(
    VkCommandBuffer                             commandBuffer,
    VkPipelineLayout                            layout,
    VkShaderStageFlags                          stageFlags,
    uint32_t                                    offset,
    uint32_t                                    size,
    const void*                                 pValues) {
    StartWriteObject(commandBuffer);
    StartReadObject(layout);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdPushConstants(
    VkCommandBuffer                             commandBuffer,
    VkPipelineLayout                            layout,
    VkShaderStageFlags                          stageFlags,
    uint32_t                                    offset,
    uint32_t                                    size,
    const void*                                 pValues) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(layout);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdBeginRenderPass(
    VkCommandBuffer                             commandBuffer,
    const VkRenderPassBeginInfo*                pRenderPassBegin,
    VkSubpassContents                           contents) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdBeginRenderPass(
    VkCommandBuffer                             commandBuffer,
    const VkRenderPassBeginInfo*                pRenderPassBegin,
    VkSubpassContents                           contents) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdNextSubpass(
    VkCommandBuffer                             commandBuffer,
    VkSubpassContents                           contents) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdNextSubpass(
    VkCommandBuffer                             commandBuffer,
    VkSubpassContents                           contents) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdEndRenderPass(
    VkCommandBuffer                             commandBuffer) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdEndRenderPass(
    VkCommandBuffer                             commandBuffer) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdExecuteCommands(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    commandBufferCount,
    const VkCommandBuffer*                      pCommandBuffers) {
    StartWriteObject(commandBuffer);
    if (pCommandBuffers) {
        for (uint32_t index = 0; index < commandBufferCount; index++) {
            StartReadObject(pCommandBuffers[index]);
        }
    }
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdExecuteCommands(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    commandBufferCount,
    const VkCommandBuffer*                      pCommandBuffers) {
    FinishWriteObject(commandBuffer);
    if (pCommandBuffers) {
        for (uint32_t index = 0; index < commandBufferCount; index++) {
            FinishReadObject(pCommandBuffers[index]);
        }
    }
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordBindBufferMemory2(
    VkDevice                                    device,
    uint32_t                                    bindInfoCount,
    const VkBindBufferMemoryInfo*               pBindInfos) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordBindBufferMemory2(
    VkDevice                                    device,
    uint32_t                                    bindInfoCount,
    const VkBindBufferMemoryInfo*               pBindInfos,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordBindImageMemory2(
    VkDevice                                    device,
    uint32_t                                    bindInfoCount,
    const VkBindImageMemoryInfo*                pBindInfos) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordBindImageMemory2(
    VkDevice                                    device,
    uint32_t                                    bindInfoCount,
    const VkBindImageMemoryInfo*                pBindInfos,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordGetDeviceGroupPeerMemoryFeatures(
    VkDevice                                    device,
    uint32_t                                    heapIndex,
    uint32_t                                    localDeviceIndex,
    uint32_t                                    remoteDeviceIndex,
    VkPeerMemoryFeatureFlags*                   pPeerMemoryFeatures) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetDeviceGroupPeerMemoryFeatures(
    VkDevice                                    device,
    uint32_t                                    heapIndex,
    uint32_t                                    localDeviceIndex,
    uint32_t                                    remoteDeviceIndex,
    VkPeerMemoryFeatureFlags*                   pPeerMemoryFeatures) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordCmdSetDeviceMask(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    deviceMask) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdSetDeviceMask(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    deviceMask) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdDispatchBase(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    baseGroupX,
    uint32_t                                    baseGroupY,
    uint32_t                                    baseGroupZ,
    uint32_t                                    groupCountX,
    uint32_t                                    groupCountY,
    uint32_t                                    groupCountZ) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdDispatchBase(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    baseGroupX,
    uint32_t                                    baseGroupY,
    uint32_t                                    baseGroupZ,
    uint32_t                                    groupCountX,
    uint32_t                                    groupCountY,
    uint32_t                                    groupCountZ) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordEnumeratePhysicalDeviceGroups(
    VkInstance                                  instance,
    uint32_t*                                   pPhysicalDeviceGroupCount,
    VkPhysicalDeviceGroupProperties*            pPhysicalDeviceGroupProperties) {
    StartReadObjectParentInstance(instance);
}

void ThreadSafety::PostCallRecordEnumeratePhysicalDeviceGroups(
    VkInstance                                  instance,
    uint32_t*                                   pPhysicalDeviceGroupCount,
    VkPhysicalDeviceGroupProperties*            pPhysicalDeviceGroupProperties,
    VkResult                                    result) {
    FinishReadObjectParentInstance(instance);
}

void ThreadSafety::PreCallRecordGetImageMemoryRequirements2(
    VkDevice                                    device,
    const VkImageMemoryRequirementsInfo2*       pInfo,
    VkMemoryRequirements2*                      pMemoryRequirements) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetImageMemoryRequirements2(
    VkDevice                                    device,
    const VkImageMemoryRequirementsInfo2*       pInfo,
    VkMemoryRequirements2*                      pMemoryRequirements) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordGetBufferMemoryRequirements2(
    VkDevice                                    device,
    const VkBufferMemoryRequirementsInfo2*      pInfo,
    VkMemoryRequirements2*                      pMemoryRequirements) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetBufferMemoryRequirements2(
    VkDevice                                    device,
    const VkBufferMemoryRequirementsInfo2*      pInfo,
    VkMemoryRequirements2*                      pMemoryRequirements) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordGetImageSparseMemoryRequirements2(
    VkDevice                                    device,
    const VkImageSparseMemoryRequirementsInfo2* pInfo,
    uint32_t*                                   pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements2*           pSparseMemoryRequirements) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetImageSparseMemoryRequirements2(
    VkDevice                                    device,
    const VkImageSparseMemoryRequirementsInfo2* pInfo,
    uint32_t*                                   pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements2*           pSparseMemoryRequirements) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordTrimCommandPool(
    VkDevice                                    device,
    VkCommandPool                               commandPool,
    VkCommandPoolTrimFlags                      flags) {
    StartReadObjectParentInstance(device);
    StartWriteObject(commandPool);
    // Host access to commandPool must be externally synchronized
}

void ThreadSafety::PostCallRecordTrimCommandPool(
    VkDevice                                    device,
    VkCommandPool                               commandPool,
    VkCommandPoolTrimFlags                      flags) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(commandPool);
    // Host access to commandPool must be externally synchronized
}

void ThreadSafety::PreCallRecordCreateSamplerYcbcrConversion(
    VkDevice                                    device,
    const VkSamplerYcbcrConversionCreateInfo*   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSamplerYcbcrConversion*                   pYcbcrConversion) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordCreateSamplerYcbcrConversion(
    VkDevice                                    device,
    const VkSamplerYcbcrConversionCreateInfo*   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSamplerYcbcrConversion*                   pYcbcrConversion,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    if (result == VK_SUCCESS) {
        CreateObject(*pYcbcrConversion);
    }
}

void ThreadSafety::PreCallRecordDestroySamplerYcbcrConversion(
    VkDevice                                    device,
    VkSamplerYcbcrConversion                    ycbcrConversion,
    const VkAllocationCallbacks*                pAllocator) {
    StartReadObjectParentInstance(device);
    StartWriteObject(ycbcrConversion);
    // Host access to ycbcrConversion must be externally synchronized
}

void ThreadSafety::PostCallRecordDestroySamplerYcbcrConversion(
    VkDevice                                    device,
    VkSamplerYcbcrConversion                    ycbcrConversion,
    const VkAllocationCallbacks*                pAllocator) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(ycbcrConversion);
    DestroyObject(ycbcrConversion);
    // Host access to ycbcrConversion must be externally synchronized
}

void ThreadSafety::PreCallRecordCreateDescriptorUpdateTemplate(
    VkDevice                                    device,
    const VkDescriptorUpdateTemplateCreateInfo* pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDescriptorUpdateTemplate*                 pDescriptorUpdateTemplate) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordCreateDescriptorUpdateTemplate(
    VkDevice                                    device,
    const VkDescriptorUpdateTemplateCreateInfo* pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDescriptorUpdateTemplate*                 pDescriptorUpdateTemplate,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    if (result == VK_SUCCESS) {
        CreateObject(*pDescriptorUpdateTemplate);
    }
}

void ThreadSafety::PreCallRecordDestroyDescriptorUpdateTemplate(
    VkDevice                                    device,
    VkDescriptorUpdateTemplate                  descriptorUpdateTemplate,
    const VkAllocationCallbacks*                pAllocator) {
    StartReadObjectParentInstance(device);
    StartWriteObject(descriptorUpdateTemplate);
    // Host access to descriptorUpdateTemplate must be externally synchronized
}

void ThreadSafety::PostCallRecordDestroyDescriptorUpdateTemplate(
    VkDevice                                    device,
    VkDescriptorUpdateTemplate                  descriptorUpdateTemplate,
    const VkAllocationCallbacks*                pAllocator) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(descriptorUpdateTemplate);
    DestroyObject(descriptorUpdateTemplate);
    // Host access to descriptorUpdateTemplate must be externally synchronized
}

void ThreadSafety::PreCallRecordUpdateDescriptorSetWithTemplate(
    VkDevice                                    device,
    VkDescriptorSet                             descriptorSet,
    VkDescriptorUpdateTemplate                  descriptorUpdateTemplate,
    const void*                                 pData) {
    StartReadObjectParentInstance(device);
    StartWriteObject(descriptorSet);
    StartReadObject(descriptorUpdateTemplate);
    // Host access to descriptorSet must be externally synchronized
}

void ThreadSafety::PostCallRecordUpdateDescriptorSetWithTemplate(
    VkDevice                                    device,
    VkDescriptorSet                             descriptorSet,
    VkDescriptorUpdateTemplate                  descriptorUpdateTemplate,
    const void*                                 pData) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(descriptorSet);
    FinishReadObject(descriptorUpdateTemplate);
    // Host access to descriptorSet must be externally synchronized
}

void ThreadSafety::PreCallRecordGetDescriptorSetLayoutSupport(
    VkDevice                                    device,
    const VkDescriptorSetLayoutCreateInfo*      pCreateInfo,
    VkDescriptorSetLayoutSupport*               pSupport) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetDescriptorSetLayoutSupport(
    VkDevice                                    device,
    const VkDescriptorSetLayoutCreateInfo*      pCreateInfo,
    VkDescriptorSetLayoutSupport*               pSupport) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordDestroySurfaceKHR(
    VkInstance                                  instance,
    VkSurfaceKHR                                surface,
    const VkAllocationCallbacks*                pAllocator) {
    StartReadObjectParentInstance(instance);
    StartWriteObjectParentInstance(surface);
    // Host access to surface must be externally synchronized
}

void ThreadSafety::PostCallRecordDestroySurfaceKHR(
    VkInstance                                  instance,
    VkSurfaceKHR                                surface,
    const VkAllocationCallbacks*                pAllocator) {
    FinishReadObjectParentInstance(instance);
    FinishWriteObjectParentInstance(surface);
    DestroyObjectParentInstance(surface);
    // Host access to surface must be externally synchronized
}

void ThreadSafety::PreCallRecordGetPhysicalDeviceSurfaceSupportKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    queueFamilyIndex,
    VkSurfaceKHR                                surface,
    VkBool32*                                   pSupported) {
    StartReadObjectParentInstance(surface);
}

void ThreadSafety::PostCallRecordGetPhysicalDeviceSurfaceSupportKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    queueFamilyIndex,
    VkSurfaceKHR                                surface,
    VkBool32*                                   pSupported,
    VkResult                                    result) {
    FinishReadObjectParentInstance(surface);
}

void ThreadSafety::PreCallRecordGetPhysicalDeviceSurfaceCapabilitiesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    VkSurfaceCapabilitiesKHR*                   pSurfaceCapabilities) {
    StartReadObjectParentInstance(surface);
}

void ThreadSafety::PostCallRecordGetPhysicalDeviceSurfaceCapabilitiesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    VkSurfaceCapabilitiesKHR*                   pSurfaceCapabilities,
    VkResult                                    result) {
    FinishReadObjectParentInstance(surface);
}

void ThreadSafety::PreCallRecordGetPhysicalDeviceSurfaceFormatsKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    uint32_t*                                   pSurfaceFormatCount,
    VkSurfaceFormatKHR*                         pSurfaceFormats) {
    StartReadObjectParentInstance(surface);
}

void ThreadSafety::PostCallRecordGetPhysicalDeviceSurfaceFormatsKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    uint32_t*                                   pSurfaceFormatCount,
    VkSurfaceFormatKHR*                         pSurfaceFormats,
    VkResult                                    result) {
    FinishReadObjectParentInstance(surface);
}

void ThreadSafety::PreCallRecordGetPhysicalDeviceSurfacePresentModesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    uint32_t*                                   pPresentModeCount,
    VkPresentModeKHR*                           pPresentModes) {
    StartReadObjectParentInstance(surface);
}

void ThreadSafety::PostCallRecordGetPhysicalDeviceSurfacePresentModesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    uint32_t*                                   pPresentModeCount,
    VkPresentModeKHR*                           pPresentModes,
    VkResult                                    result) {
    FinishReadObjectParentInstance(surface);
}

void ThreadSafety::PreCallRecordCreateSwapchainKHR(
    VkDevice                                    device,
    const VkSwapchainCreateInfoKHR*             pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSwapchainKHR*                             pSwapchain) {
    StartReadObjectParentInstance(device);
        StartWriteObjectParentInstance(pCreateInfo->surface);
        StartWriteObject(pCreateInfo->oldSwapchain);
    // Host access to pCreateInfo.surface,pCreateInfo.oldSwapchain must be externally synchronized
}

void ThreadSafety::PostCallRecordCreateSwapchainKHR(
    VkDevice                                    device,
    const VkSwapchainCreateInfoKHR*             pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSwapchainKHR*                             pSwapchain,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
        FinishWriteObjectParentInstance(pCreateInfo->surface);
        FinishWriteObject(pCreateInfo->oldSwapchain);
    if (result == VK_SUCCESS) {
        CreateObject(*pSwapchain);
    }
    // Host access to pCreateInfo.surface,pCreateInfo.oldSwapchain must be externally synchronized
}

void ThreadSafety::PreCallRecordAcquireNextImageKHR(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    uint64_t                                    timeout,
    VkSemaphore                                 semaphore,
    VkFence                                     fence,
    uint32_t*                                   pImageIndex) {
    StartReadObjectParentInstance(device);
    StartWriteObject(swapchain);
    StartWriteObject(semaphore);
    StartWriteObject(fence);
    // Host access to swapchain must be externally synchronized
    // Host access to semaphore must be externally synchronized
    // Host access to fence must be externally synchronized
}

void ThreadSafety::PostCallRecordAcquireNextImageKHR(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    uint64_t                                    timeout,
    VkSemaphore                                 semaphore,
    VkFence                                     fence,
    uint32_t*                                   pImageIndex,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(swapchain);
    FinishWriteObject(semaphore);
    FinishWriteObject(fence);
    // Host access to swapchain must be externally synchronized
    // Host access to semaphore must be externally synchronized
    // Host access to fence must be externally synchronized
}

void ThreadSafety::PreCallRecordGetDeviceGroupPresentCapabilitiesKHR(
    VkDevice                                    device,
    VkDeviceGroupPresentCapabilitiesKHR*        pDeviceGroupPresentCapabilities) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetDeviceGroupPresentCapabilitiesKHR(
    VkDevice                                    device,
    VkDeviceGroupPresentCapabilitiesKHR*        pDeviceGroupPresentCapabilities,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordGetDeviceGroupSurfacePresentModesKHR(
    VkDevice                                    device,
    VkSurfaceKHR                                surface,
    VkDeviceGroupPresentModeFlagsKHR*           pModes) {
    StartReadObjectParentInstance(device);
    StartWriteObjectParentInstance(surface);
    // Host access to surface must be externally synchronized
}

void ThreadSafety::PostCallRecordGetDeviceGroupSurfacePresentModesKHR(
    VkDevice                                    device,
    VkSurfaceKHR                                surface,
    VkDeviceGroupPresentModeFlagsKHR*           pModes,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishWriteObjectParentInstance(surface);
    // Host access to surface must be externally synchronized
}

void ThreadSafety::PreCallRecordGetPhysicalDevicePresentRectanglesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    uint32_t*                                   pRectCount,
    VkRect2D*                                   pRects) {
    StartWriteObjectParentInstance(surface);
    // Host access to surface must be externally synchronized
}

void ThreadSafety::PostCallRecordGetPhysicalDevicePresentRectanglesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    uint32_t*                                   pRectCount,
    VkRect2D*                                   pRects,
    VkResult                                    result) {
    FinishWriteObjectParentInstance(surface);
    // Host access to surface must be externally synchronized
}

void ThreadSafety::PreCallRecordAcquireNextImage2KHR(
    VkDevice                                    device,
    const VkAcquireNextImageInfoKHR*            pAcquireInfo,
    uint32_t*                                   pImageIndex) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordAcquireNextImage2KHR(
    VkDevice                                    device,
    const VkAcquireNextImageInfoKHR*            pAcquireInfo,
    uint32_t*                                   pImageIndex,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordGetDisplayPlaneSupportedDisplaysKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    planeIndex,
    uint32_t*                                   pDisplayCount,
    VkDisplayKHR*                               pDisplays) {
    if (pDisplays) {
        for (uint32_t index = 0; index < *pDisplayCount; index++) {
            StartReadObject(pDisplays[index]);
        }
    }
}

void ThreadSafety::PostCallRecordGetDisplayPlaneSupportedDisplaysKHR(
    VkPhysicalDevice                            physicalDevice,
    uint32_t                                    planeIndex,
    uint32_t*                                   pDisplayCount,
    VkDisplayKHR*                               pDisplays,
    VkResult                                    result) {
    if (pDisplays) {
        for (uint32_t index = 0; index < *pDisplayCount; index++) {
            FinishReadObject(pDisplays[index]);
        }
    }
}

void ThreadSafety::PreCallRecordGetDisplayModePropertiesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayKHR                                display,
    uint32_t*                                   pPropertyCount,
    VkDisplayModePropertiesKHR*                 pProperties) {
    StartReadObject(display);
}

void ThreadSafety::PostCallRecordGetDisplayModePropertiesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayKHR                                display,
    uint32_t*                                   pPropertyCount,
    VkDisplayModePropertiesKHR*                 pProperties,
    VkResult                                    result) {
    FinishReadObject(display);
}

void ThreadSafety::PreCallRecordCreateDisplayModeKHR(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayKHR                                display,
    const VkDisplayModeCreateInfoKHR*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDisplayModeKHR*                           pMode) {
    StartWriteObject(display);
    // Host access to display must be externally synchronized
}

void ThreadSafety::PostCallRecordCreateDisplayModeKHR(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayKHR                                display,
    const VkDisplayModeCreateInfoKHR*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDisplayModeKHR*                           pMode,
    VkResult                                    result) {
    FinishWriteObject(display);
    if (result == VK_SUCCESS) {
        CreateObject(*pMode);
    }
    // Host access to display must be externally synchronized
}

void ThreadSafety::PreCallRecordGetDisplayPlaneCapabilitiesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayModeKHR                            mode,
    uint32_t                                    planeIndex,
    VkDisplayPlaneCapabilitiesKHR*              pCapabilities) {
    StartWriteObject(mode);
    // Host access to mode must be externally synchronized
}

void ThreadSafety::PostCallRecordGetDisplayPlaneCapabilitiesKHR(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayModeKHR                            mode,
    uint32_t                                    planeIndex,
    VkDisplayPlaneCapabilitiesKHR*              pCapabilities,
    VkResult                                    result) {
    FinishWriteObject(mode);
    // Host access to mode must be externally synchronized
}

void ThreadSafety::PreCallRecordCreateDisplayPlaneSurfaceKHR(
    VkInstance                                  instance,
    const VkDisplaySurfaceCreateInfoKHR*        pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    StartReadObjectParentInstance(instance);
}

void ThreadSafety::PostCallRecordCreateDisplayPlaneSurfaceKHR(
    VkInstance                                  instance,
    const VkDisplaySurfaceCreateInfoKHR*        pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface,
    VkResult                                    result) {
    FinishReadObjectParentInstance(instance);
    if (result == VK_SUCCESS) {
        CreateObjectParentInstance(*pSurface);
    }
}

void ThreadSafety::PreCallRecordCreateSharedSwapchainsKHR(
    VkDevice                                    device,
    uint32_t                                    swapchainCount,
    const VkSwapchainCreateInfoKHR*             pCreateInfos,
    const VkAllocationCallbacks*                pAllocator,
    VkSwapchainKHR*                             pSwapchains) {
    StartReadObjectParentInstance(device);
    if (pCreateInfos) {
        for (uint32_t index=0; index < swapchainCount; index++) {
            StartWriteObjectParentInstance(pCreateInfos[index].surface);
            StartWriteObject(pCreateInfos[index].oldSwapchain);
        }
    }
    if (pSwapchains) {
        for (uint32_t index = 0; index < swapchainCount; index++) {
            StartReadObject(pSwapchains[index]);
        }
    }
    // Host access to pCreateInfos[].surface,pCreateInfos[].oldSwapchain must be externally synchronized
}

void ThreadSafety::PostCallRecordCreateSharedSwapchainsKHR(
    VkDevice                                    device,
    uint32_t                                    swapchainCount,
    const VkSwapchainCreateInfoKHR*             pCreateInfos,
    const VkAllocationCallbacks*                pAllocator,
    VkSwapchainKHR*                             pSwapchains,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    if (pCreateInfos) {
        for (uint32_t index=0; index < swapchainCount; index++) {
            FinishWriteObjectParentInstance(pCreateInfos[index].surface);
            FinishWriteObject(pCreateInfos[index].oldSwapchain);
        }
    }
    if (result == VK_SUCCESS) {
        if (pSwapchains) {
            for (uint32_t index = 0; index < swapchainCount; index++) {
                CreateObject(pSwapchains[index]);
            }
        }
    }
    // Host access to pCreateInfos[].surface,pCreateInfos[].oldSwapchain must be externally synchronized
}

#ifdef VK_USE_PLATFORM_XLIB_KHR

void ThreadSafety::PreCallRecordCreateXlibSurfaceKHR(
    VkInstance                                  instance,
    const VkXlibSurfaceCreateInfoKHR*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    StartReadObjectParentInstance(instance);
}

void ThreadSafety::PostCallRecordCreateXlibSurfaceKHR(
    VkInstance                                  instance,
    const VkXlibSurfaceCreateInfoKHR*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface,
    VkResult                                    result) {
    FinishReadObjectParentInstance(instance);
    if (result == VK_SUCCESS) {
        CreateObjectParentInstance(*pSurface);
    }
}
#endif // VK_USE_PLATFORM_XLIB_KHR

#ifdef VK_USE_PLATFORM_XCB_KHR

void ThreadSafety::PreCallRecordCreateXcbSurfaceKHR(
    VkInstance                                  instance,
    const VkXcbSurfaceCreateInfoKHR*            pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    StartReadObjectParentInstance(instance);
}

void ThreadSafety::PostCallRecordCreateXcbSurfaceKHR(
    VkInstance                                  instance,
    const VkXcbSurfaceCreateInfoKHR*            pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface,
    VkResult                                    result) {
    FinishReadObjectParentInstance(instance);
    if (result == VK_SUCCESS) {
        CreateObjectParentInstance(*pSurface);
    }
}
#endif // VK_USE_PLATFORM_XCB_KHR

#ifdef VK_USE_PLATFORM_WAYLAND_KHR

void ThreadSafety::PreCallRecordCreateWaylandSurfaceKHR(
    VkInstance                                  instance,
    const VkWaylandSurfaceCreateInfoKHR*        pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    StartReadObjectParentInstance(instance);
}

void ThreadSafety::PostCallRecordCreateWaylandSurfaceKHR(
    VkInstance                                  instance,
    const VkWaylandSurfaceCreateInfoKHR*        pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface,
    VkResult                                    result) {
    FinishReadObjectParentInstance(instance);
    if (result == VK_SUCCESS) {
        CreateObjectParentInstance(*pSurface);
    }
}
#endif // VK_USE_PLATFORM_WAYLAND_KHR

#ifdef VK_USE_PLATFORM_ANDROID_KHR

void ThreadSafety::PreCallRecordCreateAndroidSurfaceKHR(
    VkInstance                                  instance,
    const VkAndroidSurfaceCreateInfoKHR*        pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    StartReadObjectParentInstance(instance);
}

void ThreadSafety::PostCallRecordCreateAndroidSurfaceKHR(
    VkInstance                                  instance,
    const VkAndroidSurfaceCreateInfoKHR*        pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface,
    VkResult                                    result) {
    FinishReadObjectParentInstance(instance);
    if (result == VK_SUCCESS) {
        CreateObjectParentInstance(*pSurface);
    }
}
#endif // VK_USE_PLATFORM_ANDROID_KHR

#ifdef VK_USE_PLATFORM_WIN32_KHR

void ThreadSafety::PreCallRecordCreateWin32SurfaceKHR(
    VkInstance                                  instance,
    const VkWin32SurfaceCreateInfoKHR*          pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    StartReadObjectParentInstance(instance);
}

void ThreadSafety::PostCallRecordCreateWin32SurfaceKHR(
    VkInstance                                  instance,
    const VkWin32SurfaceCreateInfoKHR*          pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface,
    VkResult                                    result) {
    FinishReadObjectParentInstance(instance);
    if (result == VK_SUCCESS) {
        CreateObjectParentInstance(*pSurface);
    }
}
#endif // VK_USE_PLATFORM_WIN32_KHR

void ThreadSafety::PreCallRecordGetDeviceGroupPeerMemoryFeaturesKHR(
    VkDevice                                    device,
    uint32_t                                    heapIndex,
    uint32_t                                    localDeviceIndex,
    uint32_t                                    remoteDeviceIndex,
    VkPeerMemoryFeatureFlags*                   pPeerMemoryFeatures) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetDeviceGroupPeerMemoryFeaturesKHR(
    VkDevice                                    device,
    uint32_t                                    heapIndex,
    uint32_t                                    localDeviceIndex,
    uint32_t                                    remoteDeviceIndex,
    VkPeerMemoryFeatureFlags*                   pPeerMemoryFeatures) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordCmdSetDeviceMaskKHR(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    deviceMask) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdSetDeviceMaskKHR(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    deviceMask) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdDispatchBaseKHR(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    baseGroupX,
    uint32_t                                    baseGroupY,
    uint32_t                                    baseGroupZ,
    uint32_t                                    groupCountX,
    uint32_t                                    groupCountY,
    uint32_t                                    groupCountZ) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdDispatchBaseKHR(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    baseGroupX,
    uint32_t                                    baseGroupY,
    uint32_t                                    baseGroupZ,
    uint32_t                                    groupCountX,
    uint32_t                                    groupCountY,
    uint32_t                                    groupCountZ) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordTrimCommandPoolKHR(
    VkDevice                                    device,
    VkCommandPool                               commandPool,
    VkCommandPoolTrimFlags                      flags) {
    StartReadObjectParentInstance(device);
    StartWriteObject(commandPool);
    // Host access to commandPool must be externally synchronized
}

void ThreadSafety::PostCallRecordTrimCommandPoolKHR(
    VkDevice                                    device,
    VkCommandPool                               commandPool,
    VkCommandPoolTrimFlags                      flags) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(commandPool);
    // Host access to commandPool must be externally synchronized
}

void ThreadSafety::PreCallRecordEnumeratePhysicalDeviceGroupsKHR(
    VkInstance                                  instance,
    uint32_t*                                   pPhysicalDeviceGroupCount,
    VkPhysicalDeviceGroupProperties*            pPhysicalDeviceGroupProperties) {
    StartReadObjectParentInstance(instance);
}

void ThreadSafety::PostCallRecordEnumeratePhysicalDeviceGroupsKHR(
    VkInstance                                  instance,
    uint32_t*                                   pPhysicalDeviceGroupCount,
    VkPhysicalDeviceGroupProperties*            pPhysicalDeviceGroupProperties,
    VkResult                                    result) {
    FinishReadObjectParentInstance(instance);
}

#ifdef VK_USE_PLATFORM_WIN32_KHR

void ThreadSafety::PreCallRecordGetMemoryWin32HandleKHR(
    VkDevice                                    device,
    const VkMemoryGetWin32HandleInfoKHR*        pGetWin32HandleInfo,
    HANDLE*                                     pHandle) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetMemoryWin32HandleKHR(
    VkDevice                                    device,
    const VkMemoryGetWin32HandleInfoKHR*        pGetWin32HandleInfo,
    HANDLE*                                     pHandle,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordGetMemoryWin32HandlePropertiesKHR(
    VkDevice                                    device,
    VkExternalMemoryHandleTypeFlagBits          handleType,
    HANDLE                                      handle,
    VkMemoryWin32HandlePropertiesKHR*           pMemoryWin32HandleProperties) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetMemoryWin32HandlePropertiesKHR(
    VkDevice                                    device,
    VkExternalMemoryHandleTypeFlagBits          handleType,
    HANDLE                                      handle,
    VkMemoryWin32HandlePropertiesKHR*           pMemoryWin32HandleProperties,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}
#endif // VK_USE_PLATFORM_WIN32_KHR

void ThreadSafety::PreCallRecordGetMemoryFdKHR(
    VkDevice                                    device,
    const VkMemoryGetFdInfoKHR*                 pGetFdInfo,
    int*                                        pFd) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetMemoryFdKHR(
    VkDevice                                    device,
    const VkMemoryGetFdInfoKHR*                 pGetFdInfo,
    int*                                        pFd,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordGetMemoryFdPropertiesKHR(
    VkDevice                                    device,
    VkExternalMemoryHandleTypeFlagBits          handleType,
    int                                         fd,
    VkMemoryFdPropertiesKHR*                    pMemoryFdProperties) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetMemoryFdPropertiesKHR(
    VkDevice                                    device,
    VkExternalMemoryHandleTypeFlagBits          handleType,
    int                                         fd,
    VkMemoryFdPropertiesKHR*                    pMemoryFdProperties,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}

#ifdef VK_USE_PLATFORM_WIN32_KHR
#endif // VK_USE_PLATFORM_WIN32_KHR

#ifdef VK_USE_PLATFORM_WIN32_KHR

void ThreadSafety::PreCallRecordImportSemaphoreWin32HandleKHR(
    VkDevice                                    device,
    const VkImportSemaphoreWin32HandleInfoKHR*  pImportSemaphoreWin32HandleInfo) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordImportSemaphoreWin32HandleKHR(
    VkDevice                                    device,
    const VkImportSemaphoreWin32HandleInfoKHR*  pImportSemaphoreWin32HandleInfo,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordGetSemaphoreWin32HandleKHR(
    VkDevice                                    device,
    const VkSemaphoreGetWin32HandleInfoKHR*     pGetWin32HandleInfo,
    HANDLE*                                     pHandle) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetSemaphoreWin32HandleKHR(
    VkDevice                                    device,
    const VkSemaphoreGetWin32HandleInfoKHR*     pGetWin32HandleInfo,
    HANDLE*                                     pHandle,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}
#endif // VK_USE_PLATFORM_WIN32_KHR

void ThreadSafety::PreCallRecordImportSemaphoreFdKHR(
    VkDevice                                    device,
    const VkImportSemaphoreFdInfoKHR*           pImportSemaphoreFdInfo) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordImportSemaphoreFdKHR(
    VkDevice                                    device,
    const VkImportSemaphoreFdInfoKHR*           pImportSemaphoreFdInfo,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordGetSemaphoreFdKHR(
    VkDevice                                    device,
    const VkSemaphoreGetFdInfoKHR*              pGetFdInfo,
    int*                                        pFd) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetSemaphoreFdKHR(
    VkDevice                                    device,
    const VkSemaphoreGetFdInfoKHR*              pGetFdInfo,
    int*                                        pFd,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordCmdPushDescriptorSetKHR(
    VkCommandBuffer                             commandBuffer,
    VkPipelineBindPoint                         pipelineBindPoint,
    VkPipelineLayout                            layout,
    uint32_t                                    set,
    uint32_t                                    descriptorWriteCount,
    const VkWriteDescriptorSet*                 pDescriptorWrites) {
    StartWriteObject(commandBuffer);
    StartReadObject(layout);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdPushDescriptorSetKHR(
    VkCommandBuffer                             commandBuffer,
    VkPipelineBindPoint                         pipelineBindPoint,
    VkPipelineLayout                            layout,
    uint32_t                                    set,
    uint32_t                                    descriptorWriteCount,
    const VkWriteDescriptorSet*                 pDescriptorWrites) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(layout);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdPushDescriptorSetWithTemplateKHR(
    VkCommandBuffer                             commandBuffer,
    VkDescriptorUpdateTemplate                  descriptorUpdateTemplate,
    VkPipelineLayout                            layout,
    uint32_t                                    set,
    const void*                                 pData) {
    StartWriteObject(commandBuffer);
    StartReadObject(descriptorUpdateTemplate);
    StartReadObject(layout);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdPushDescriptorSetWithTemplateKHR(
    VkCommandBuffer                             commandBuffer,
    VkDescriptorUpdateTemplate                  descriptorUpdateTemplate,
    VkPipelineLayout                            layout,
    uint32_t                                    set,
    const void*                                 pData) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(descriptorUpdateTemplate);
    FinishReadObject(layout);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCreateDescriptorUpdateTemplateKHR(
    VkDevice                                    device,
    const VkDescriptorUpdateTemplateCreateInfo* pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDescriptorUpdateTemplate*                 pDescriptorUpdateTemplate) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordCreateDescriptorUpdateTemplateKHR(
    VkDevice                                    device,
    const VkDescriptorUpdateTemplateCreateInfo* pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDescriptorUpdateTemplate*                 pDescriptorUpdateTemplate,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    if (result == VK_SUCCESS) {
        CreateObject(*pDescriptorUpdateTemplate);
    }
}

void ThreadSafety::PreCallRecordDestroyDescriptorUpdateTemplateKHR(
    VkDevice                                    device,
    VkDescriptorUpdateTemplate                  descriptorUpdateTemplate,
    const VkAllocationCallbacks*                pAllocator) {
    StartReadObjectParentInstance(device);
    StartWriteObject(descriptorUpdateTemplate);
    // Host access to descriptorUpdateTemplate must be externally synchronized
}

void ThreadSafety::PostCallRecordDestroyDescriptorUpdateTemplateKHR(
    VkDevice                                    device,
    VkDescriptorUpdateTemplate                  descriptorUpdateTemplate,
    const VkAllocationCallbacks*                pAllocator) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(descriptorUpdateTemplate);
    DestroyObject(descriptorUpdateTemplate);
    // Host access to descriptorUpdateTemplate must be externally synchronized
}

void ThreadSafety::PreCallRecordUpdateDescriptorSetWithTemplateKHR(
    VkDevice                                    device,
    VkDescriptorSet                             descriptorSet,
    VkDescriptorUpdateTemplate                  descriptorUpdateTemplate,
    const void*                                 pData) {
    StartReadObjectParentInstance(device);
    StartWriteObject(descriptorSet);
    StartReadObject(descriptorUpdateTemplate);
    // Host access to descriptorSet must be externally synchronized
}

void ThreadSafety::PostCallRecordUpdateDescriptorSetWithTemplateKHR(
    VkDevice                                    device,
    VkDescriptorSet                             descriptorSet,
    VkDescriptorUpdateTemplate                  descriptorUpdateTemplate,
    const void*                                 pData) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(descriptorSet);
    FinishReadObject(descriptorUpdateTemplate);
    // Host access to descriptorSet must be externally synchronized
}

void ThreadSafety::PreCallRecordCreateRenderPass2KHR(
    VkDevice                                    device,
    const VkRenderPassCreateInfo2KHR*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkRenderPass*                               pRenderPass) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordCreateRenderPass2KHR(
    VkDevice                                    device,
    const VkRenderPassCreateInfo2KHR*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkRenderPass*                               pRenderPass,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    if (result == VK_SUCCESS) {
        CreateObject(*pRenderPass);
    }
}

void ThreadSafety::PreCallRecordCmdBeginRenderPass2KHR(
    VkCommandBuffer                             commandBuffer,
    const VkRenderPassBeginInfo*                pRenderPassBegin,
    const VkSubpassBeginInfoKHR*                pSubpassBeginInfo) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdBeginRenderPass2KHR(
    VkCommandBuffer                             commandBuffer,
    const VkRenderPassBeginInfo*                pRenderPassBegin,
    const VkSubpassBeginInfoKHR*                pSubpassBeginInfo) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdNextSubpass2KHR(
    VkCommandBuffer                             commandBuffer,
    const VkSubpassBeginInfoKHR*                pSubpassBeginInfo,
    const VkSubpassEndInfoKHR*                  pSubpassEndInfo) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdNextSubpass2KHR(
    VkCommandBuffer                             commandBuffer,
    const VkSubpassBeginInfoKHR*                pSubpassBeginInfo,
    const VkSubpassEndInfoKHR*                  pSubpassEndInfo) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdEndRenderPass2KHR(
    VkCommandBuffer                             commandBuffer,
    const VkSubpassEndInfoKHR*                  pSubpassEndInfo) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdEndRenderPass2KHR(
    VkCommandBuffer                             commandBuffer,
    const VkSubpassEndInfoKHR*                  pSubpassEndInfo) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordGetSwapchainStatusKHR(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain) {
    StartReadObjectParentInstance(device);
    StartWriteObject(swapchain);
    // Host access to swapchain must be externally synchronized
}

void ThreadSafety::PostCallRecordGetSwapchainStatusKHR(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(swapchain);
    // Host access to swapchain must be externally synchronized
}

#ifdef VK_USE_PLATFORM_WIN32_KHR

void ThreadSafety::PreCallRecordImportFenceWin32HandleKHR(
    VkDevice                                    device,
    const VkImportFenceWin32HandleInfoKHR*      pImportFenceWin32HandleInfo) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordImportFenceWin32HandleKHR(
    VkDevice                                    device,
    const VkImportFenceWin32HandleInfoKHR*      pImportFenceWin32HandleInfo,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordGetFenceWin32HandleKHR(
    VkDevice                                    device,
    const VkFenceGetWin32HandleInfoKHR*         pGetWin32HandleInfo,
    HANDLE*                                     pHandle) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetFenceWin32HandleKHR(
    VkDevice                                    device,
    const VkFenceGetWin32HandleInfoKHR*         pGetWin32HandleInfo,
    HANDLE*                                     pHandle,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}
#endif // VK_USE_PLATFORM_WIN32_KHR

void ThreadSafety::PreCallRecordImportFenceFdKHR(
    VkDevice                                    device,
    const VkImportFenceFdInfoKHR*               pImportFenceFdInfo) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordImportFenceFdKHR(
    VkDevice                                    device,
    const VkImportFenceFdInfoKHR*               pImportFenceFdInfo,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordGetFenceFdKHR(
    VkDevice                                    device,
    const VkFenceGetFdInfoKHR*                  pGetFdInfo,
    int*                                        pFd) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetFenceFdKHR(
    VkDevice                                    device,
    const VkFenceGetFdInfoKHR*                  pGetFdInfo,
    int*                                        pFd,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordGetDisplayModeProperties2KHR(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayKHR                                display,
    uint32_t*                                   pPropertyCount,
    VkDisplayModeProperties2KHR*                pProperties) {
    StartReadObject(display);
}

void ThreadSafety::PostCallRecordGetDisplayModeProperties2KHR(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayKHR                                display,
    uint32_t*                                   pPropertyCount,
    VkDisplayModeProperties2KHR*                pProperties,
    VkResult                                    result) {
    FinishReadObject(display);
}

void ThreadSafety::PreCallRecordGetImageMemoryRequirements2KHR(
    VkDevice                                    device,
    const VkImageMemoryRequirementsInfo2*       pInfo,
    VkMemoryRequirements2*                      pMemoryRequirements) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetImageMemoryRequirements2KHR(
    VkDevice                                    device,
    const VkImageMemoryRequirementsInfo2*       pInfo,
    VkMemoryRequirements2*                      pMemoryRequirements) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordGetBufferMemoryRequirements2KHR(
    VkDevice                                    device,
    const VkBufferMemoryRequirementsInfo2*      pInfo,
    VkMemoryRequirements2*                      pMemoryRequirements) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetBufferMemoryRequirements2KHR(
    VkDevice                                    device,
    const VkBufferMemoryRequirementsInfo2*      pInfo,
    VkMemoryRequirements2*                      pMemoryRequirements) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordGetImageSparseMemoryRequirements2KHR(
    VkDevice                                    device,
    const VkImageSparseMemoryRequirementsInfo2* pInfo,
    uint32_t*                                   pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements2*           pSparseMemoryRequirements) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetImageSparseMemoryRequirements2KHR(
    VkDevice                                    device,
    const VkImageSparseMemoryRequirementsInfo2* pInfo,
    uint32_t*                                   pSparseMemoryRequirementCount,
    VkSparseImageMemoryRequirements2*           pSparseMemoryRequirements) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordCreateSamplerYcbcrConversionKHR(
    VkDevice                                    device,
    const VkSamplerYcbcrConversionCreateInfo*   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSamplerYcbcrConversion*                   pYcbcrConversion) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordCreateSamplerYcbcrConversionKHR(
    VkDevice                                    device,
    const VkSamplerYcbcrConversionCreateInfo*   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSamplerYcbcrConversion*                   pYcbcrConversion,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    if (result == VK_SUCCESS) {
        CreateObject(*pYcbcrConversion);
    }
}

void ThreadSafety::PreCallRecordDestroySamplerYcbcrConversionKHR(
    VkDevice                                    device,
    VkSamplerYcbcrConversion                    ycbcrConversion,
    const VkAllocationCallbacks*                pAllocator) {
    StartReadObjectParentInstance(device);
    StartWriteObject(ycbcrConversion);
    // Host access to ycbcrConversion must be externally synchronized
}

void ThreadSafety::PostCallRecordDestroySamplerYcbcrConversionKHR(
    VkDevice                                    device,
    VkSamplerYcbcrConversion                    ycbcrConversion,
    const VkAllocationCallbacks*                pAllocator) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(ycbcrConversion);
    DestroyObject(ycbcrConversion);
    // Host access to ycbcrConversion must be externally synchronized
}

void ThreadSafety::PreCallRecordBindBufferMemory2KHR(
    VkDevice                                    device,
    uint32_t                                    bindInfoCount,
    const VkBindBufferMemoryInfo*               pBindInfos) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordBindBufferMemory2KHR(
    VkDevice                                    device,
    uint32_t                                    bindInfoCount,
    const VkBindBufferMemoryInfo*               pBindInfos,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordBindImageMemory2KHR(
    VkDevice                                    device,
    uint32_t                                    bindInfoCount,
    const VkBindImageMemoryInfo*                pBindInfos) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordBindImageMemory2KHR(
    VkDevice                                    device,
    uint32_t                                    bindInfoCount,
    const VkBindImageMemoryInfo*                pBindInfos,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordGetDescriptorSetLayoutSupportKHR(
    VkDevice                                    device,
    const VkDescriptorSetLayoutCreateInfo*      pCreateInfo,
    VkDescriptorSetLayoutSupport*               pSupport) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetDescriptorSetLayoutSupportKHR(
    VkDevice                                    device,
    const VkDescriptorSetLayoutCreateInfo*      pCreateInfo,
    VkDescriptorSetLayoutSupport*               pSupport) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordCmdDrawIndirectCountKHR(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkBuffer                                    countBuffer,
    VkDeviceSize                                countBufferOffset,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride) {
    StartWriteObject(commandBuffer);
    StartReadObject(buffer);
    StartReadObject(countBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdDrawIndirectCountKHR(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkBuffer                                    countBuffer,
    VkDeviceSize                                countBufferOffset,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(buffer);
    FinishReadObject(countBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdDrawIndexedIndirectCountKHR(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkBuffer                                    countBuffer,
    VkDeviceSize                                countBufferOffset,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride) {
    StartWriteObject(commandBuffer);
    StartReadObject(buffer);
    StartReadObject(countBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdDrawIndexedIndirectCountKHR(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkBuffer                                    countBuffer,
    VkDeviceSize                                countBufferOffset,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(buffer);
    FinishReadObject(countBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordGetPipelineExecutablePropertiesKHR(
    VkDevice                                    device,
    const VkPipelineInfoKHR*                    pPipelineInfo,
    uint32_t*                                   pExecutableCount,
    VkPipelineExecutablePropertiesKHR*          pProperties) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetPipelineExecutablePropertiesKHR(
    VkDevice                                    device,
    const VkPipelineInfoKHR*                    pPipelineInfo,
    uint32_t*                                   pExecutableCount,
    VkPipelineExecutablePropertiesKHR*          pProperties,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordGetPipelineExecutableStatisticsKHR(
    VkDevice                                    device,
    const VkPipelineExecutableInfoKHR*          pExecutableInfo,
    uint32_t*                                   pStatisticCount,
    VkPipelineExecutableStatisticKHR*           pStatistics) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetPipelineExecutableStatisticsKHR(
    VkDevice                                    device,
    const VkPipelineExecutableInfoKHR*          pExecutableInfo,
    uint32_t*                                   pStatisticCount,
    VkPipelineExecutableStatisticKHR*           pStatistics,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordGetPipelineExecutableInternalRepresentationsKHR(
    VkDevice                                    device,
    const VkPipelineExecutableInfoKHR*          pExecutableInfo,
    uint32_t*                                   pInternalRepresentationCount,
    VkPipelineExecutableInternalRepresentationKHR* pInternalRepresentations) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetPipelineExecutableInternalRepresentationsKHR(
    VkDevice                                    device,
    const VkPipelineExecutableInfoKHR*          pExecutableInfo,
    uint32_t*                                   pInternalRepresentationCount,
    VkPipelineExecutableInternalRepresentationKHR* pInternalRepresentations,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordCreateDebugReportCallbackEXT(
    VkInstance                                  instance,
    const VkDebugReportCallbackCreateInfoEXT*   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDebugReportCallbackEXT*                   pCallback) {
    StartReadObjectParentInstance(instance);
}

void ThreadSafety::PostCallRecordCreateDebugReportCallbackEXT(
    VkInstance                                  instance,
    const VkDebugReportCallbackCreateInfoEXT*   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDebugReportCallbackEXT*                   pCallback,
    VkResult                                    result) {
    FinishReadObjectParentInstance(instance);
    if (result == VK_SUCCESS) {
        CreateObjectParentInstance(*pCallback);
    }
}

void ThreadSafety::PreCallRecordDestroyDebugReportCallbackEXT(
    VkInstance                                  instance,
    VkDebugReportCallbackEXT                    callback,
    const VkAllocationCallbacks*                pAllocator) {
    StartReadObjectParentInstance(instance);
    StartWriteObjectParentInstance(callback);
    // Host access to callback must be externally synchronized
}

void ThreadSafety::PostCallRecordDestroyDebugReportCallbackEXT(
    VkInstance                                  instance,
    VkDebugReportCallbackEXT                    callback,
    const VkAllocationCallbacks*                pAllocator) {
    FinishReadObjectParentInstance(instance);
    FinishWriteObjectParentInstance(callback);
    DestroyObjectParentInstance(callback);
    // Host access to callback must be externally synchronized
}

void ThreadSafety::PreCallRecordDebugReportMessageEXT(
    VkInstance                                  instance,
    VkDebugReportFlagsEXT                       flags,
    VkDebugReportObjectTypeEXT                  objectType,
    uint64_t                                    object,
    size_t                                      location,
    int32_t                                     messageCode,
    const char*                                 pLayerPrefix,
    const char*                                 pMessage) {
    StartReadObjectParentInstance(instance);
}

void ThreadSafety::PostCallRecordDebugReportMessageEXT(
    VkInstance                                  instance,
    VkDebugReportFlagsEXT                       flags,
    VkDebugReportObjectTypeEXT                  objectType,
    uint64_t                                    object,
    size_t                                      location,
    int32_t                                     messageCode,
    const char*                                 pLayerPrefix,
    const char*                                 pMessage) {
    FinishReadObjectParentInstance(instance);
}
// TODO - not wrapping EXT function vkDebugMarkerSetObjectTagEXT
// TODO - not wrapping EXT function vkDebugMarkerSetObjectNameEXT
// TODO - not wrapping EXT function vkCmdDebugMarkerBeginEXT
// TODO - not wrapping EXT function vkCmdDebugMarkerEndEXT
// TODO - not wrapping EXT function vkCmdDebugMarkerInsertEXT

void ThreadSafety::PreCallRecordCmdBindTransformFeedbackBuffersEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstBinding,
    uint32_t                                    bindingCount,
    const VkBuffer*                             pBuffers,
    const VkDeviceSize*                         pOffsets,
    const VkDeviceSize*                         pSizes) {
    StartWriteObject(commandBuffer);
    if (pBuffers) {
        for (uint32_t index = 0; index < bindingCount; index++) {
            StartReadObject(pBuffers[index]);
        }
    }
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdBindTransformFeedbackBuffersEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstBinding,
    uint32_t                                    bindingCount,
    const VkBuffer*                             pBuffers,
    const VkDeviceSize*                         pOffsets,
    const VkDeviceSize*                         pSizes) {
    FinishWriteObject(commandBuffer);
    if (pBuffers) {
        for (uint32_t index = 0; index < bindingCount; index++) {
            FinishReadObject(pBuffers[index]);
        }
    }
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdBeginTransformFeedbackEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstCounterBuffer,
    uint32_t                                    counterBufferCount,
    const VkBuffer*                             pCounterBuffers,
    const VkDeviceSize*                         pCounterBufferOffsets) {
    StartWriteObject(commandBuffer);
    if (pCounterBuffers) {
        for (uint32_t index = 0; index < counterBufferCount; index++) {
            StartReadObject(pCounterBuffers[index]);
        }
    }
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdBeginTransformFeedbackEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstCounterBuffer,
    uint32_t                                    counterBufferCount,
    const VkBuffer*                             pCounterBuffers,
    const VkDeviceSize*                         pCounterBufferOffsets) {
    FinishWriteObject(commandBuffer);
    if (pCounterBuffers) {
        for (uint32_t index = 0; index < counterBufferCount; index++) {
            FinishReadObject(pCounterBuffers[index]);
        }
    }
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdEndTransformFeedbackEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstCounterBuffer,
    uint32_t                                    counterBufferCount,
    const VkBuffer*                             pCounterBuffers,
    const VkDeviceSize*                         pCounterBufferOffsets) {
    StartWriteObject(commandBuffer);
    if (pCounterBuffers) {
        for (uint32_t index = 0; index < counterBufferCount; index++) {
            StartReadObject(pCounterBuffers[index]);
        }
    }
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdEndTransformFeedbackEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstCounterBuffer,
    uint32_t                                    counterBufferCount,
    const VkBuffer*                             pCounterBuffers,
    const VkDeviceSize*                         pCounterBufferOffsets) {
    FinishWriteObject(commandBuffer);
    if (pCounterBuffers) {
        for (uint32_t index = 0; index < counterBufferCount; index++) {
            FinishReadObject(pCounterBuffers[index]);
        }
    }
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdBeginQueryIndexedEXT(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    query,
    VkQueryControlFlags                         flags,
    uint32_t                                    index) {
    StartWriteObject(commandBuffer);
    StartReadObject(queryPool);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdBeginQueryIndexedEXT(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    query,
    VkQueryControlFlags                         flags,
    uint32_t                                    index) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(queryPool);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdEndQueryIndexedEXT(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    query,
    uint32_t                                    index) {
    StartWriteObject(commandBuffer);
    StartReadObject(queryPool);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdEndQueryIndexedEXT(
    VkCommandBuffer                             commandBuffer,
    VkQueryPool                                 queryPool,
    uint32_t                                    query,
    uint32_t                                    index) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(queryPool);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdDrawIndirectByteCountEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    instanceCount,
    uint32_t                                    firstInstance,
    VkBuffer                                    counterBuffer,
    VkDeviceSize                                counterBufferOffset,
    uint32_t                                    counterOffset,
    uint32_t                                    vertexStride) {
    StartWriteObject(commandBuffer);
    StartReadObject(counterBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdDrawIndirectByteCountEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    instanceCount,
    uint32_t                                    firstInstance,
    VkBuffer                                    counterBuffer,
    VkDeviceSize                                counterBufferOffset,
    uint32_t                                    counterOffset,
    uint32_t                                    vertexStride) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(counterBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordGetImageViewHandleNVX(
    VkDevice                                    device,
    const VkImageViewHandleInfoNVX*             pInfo) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetImageViewHandleNVX(
    VkDevice                                    device,
    const VkImageViewHandleInfoNVX*             pInfo) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordCmdDrawIndirectCountAMD(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkBuffer                                    countBuffer,
    VkDeviceSize                                countBufferOffset,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride) {
    StartWriteObject(commandBuffer);
    StartReadObject(buffer);
    StartReadObject(countBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdDrawIndirectCountAMD(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkBuffer                                    countBuffer,
    VkDeviceSize                                countBufferOffset,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(buffer);
    FinishReadObject(countBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdDrawIndexedIndirectCountAMD(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkBuffer                                    countBuffer,
    VkDeviceSize                                countBufferOffset,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride) {
    StartWriteObject(commandBuffer);
    StartReadObject(buffer);
    StartReadObject(countBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdDrawIndexedIndirectCountAMD(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkBuffer                                    countBuffer,
    VkDeviceSize                                countBufferOffset,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(buffer);
    FinishReadObject(countBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordGetShaderInfoAMD(
    VkDevice                                    device,
    VkPipeline                                  pipeline,
    VkShaderStageFlagBits                       shaderStage,
    VkShaderInfoTypeAMD                         infoType,
    size_t*                                     pInfoSize,
    void*                                       pInfo) {
    StartReadObjectParentInstance(device);
    StartReadObject(pipeline);
}

void ThreadSafety::PostCallRecordGetShaderInfoAMD(
    VkDevice                                    device,
    VkPipeline                                  pipeline,
    VkShaderStageFlagBits                       shaderStage,
    VkShaderInfoTypeAMD                         infoType,
    size_t*                                     pInfoSize,
    void*                                       pInfo,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishReadObject(pipeline);
}

#ifdef VK_USE_PLATFORM_GGP

void ThreadSafety::PreCallRecordCreateStreamDescriptorSurfaceGGP(
    VkInstance                                  instance,
    const VkStreamDescriptorSurfaceCreateInfoGGP* pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    StartReadObjectParentInstance(instance);
}

void ThreadSafety::PostCallRecordCreateStreamDescriptorSurfaceGGP(
    VkInstance                                  instance,
    const VkStreamDescriptorSurfaceCreateInfoGGP* pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface,
    VkResult                                    result) {
    FinishReadObjectParentInstance(instance);
    if (result == VK_SUCCESS) {
        CreateObjectParentInstance(*pSurface);
    }
}
#endif // VK_USE_PLATFORM_GGP

#ifdef VK_USE_PLATFORM_WIN32_KHR

void ThreadSafety::PreCallRecordGetMemoryWin32HandleNV(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    VkExternalMemoryHandleTypeFlagsNV           handleType,
    HANDLE*                                     pHandle) {
    StartReadObjectParentInstance(device);
    StartReadObject(memory);
}

void ThreadSafety::PostCallRecordGetMemoryWin32HandleNV(
    VkDevice                                    device,
    VkDeviceMemory                              memory,
    VkExternalMemoryHandleTypeFlagsNV           handleType,
    HANDLE*                                     pHandle,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishReadObject(memory);
}
#endif // VK_USE_PLATFORM_WIN32_KHR

#ifdef VK_USE_PLATFORM_WIN32_KHR
#endif // VK_USE_PLATFORM_WIN32_KHR

#ifdef VK_USE_PLATFORM_VI_NN

void ThreadSafety::PreCallRecordCreateViSurfaceNN(
    VkInstance                                  instance,
    const VkViSurfaceCreateInfoNN*              pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    StartReadObjectParentInstance(instance);
}

void ThreadSafety::PostCallRecordCreateViSurfaceNN(
    VkInstance                                  instance,
    const VkViSurfaceCreateInfoNN*              pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface,
    VkResult                                    result) {
    FinishReadObjectParentInstance(instance);
    if (result == VK_SUCCESS) {
        CreateObjectParentInstance(*pSurface);
    }
}
#endif // VK_USE_PLATFORM_VI_NN

void ThreadSafety::PreCallRecordCmdBeginConditionalRenderingEXT(
    VkCommandBuffer                             commandBuffer,
    const VkConditionalRenderingBeginInfoEXT*   pConditionalRenderingBegin) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdBeginConditionalRenderingEXT(
    VkCommandBuffer                             commandBuffer,
    const VkConditionalRenderingBeginInfoEXT*   pConditionalRenderingBegin) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdEndConditionalRenderingEXT(
    VkCommandBuffer                             commandBuffer) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdEndConditionalRenderingEXT(
    VkCommandBuffer                             commandBuffer) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdProcessCommandsNVX(
    VkCommandBuffer                             commandBuffer,
    const VkCmdProcessCommandsInfoNVX*          pProcessCommandsInfo) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdProcessCommandsNVX(
    VkCommandBuffer                             commandBuffer,
    const VkCmdProcessCommandsInfoNVX*          pProcessCommandsInfo) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdReserveSpaceForCommandsNVX(
    VkCommandBuffer                             commandBuffer,
    const VkCmdReserveSpaceForCommandsInfoNVX*  pReserveSpaceInfo) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdReserveSpaceForCommandsNVX(
    VkCommandBuffer                             commandBuffer,
    const VkCmdReserveSpaceForCommandsInfoNVX*  pReserveSpaceInfo) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCreateIndirectCommandsLayoutNVX(
    VkDevice                                    device,
    const VkIndirectCommandsLayoutCreateInfoNVX* pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkIndirectCommandsLayoutNVX*                pIndirectCommandsLayout) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordCreateIndirectCommandsLayoutNVX(
    VkDevice                                    device,
    const VkIndirectCommandsLayoutCreateInfoNVX* pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkIndirectCommandsLayoutNVX*                pIndirectCommandsLayout,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    if (result == VK_SUCCESS) {
        CreateObject(*pIndirectCommandsLayout);
    }
}

void ThreadSafety::PreCallRecordDestroyIndirectCommandsLayoutNVX(
    VkDevice                                    device,
    VkIndirectCommandsLayoutNVX                 indirectCommandsLayout,
    const VkAllocationCallbacks*                pAllocator) {
    StartReadObjectParentInstance(device);
    StartReadObject(indirectCommandsLayout);
}

void ThreadSafety::PostCallRecordDestroyIndirectCommandsLayoutNVX(
    VkDevice                                    device,
    VkIndirectCommandsLayoutNVX                 indirectCommandsLayout,
    const VkAllocationCallbacks*                pAllocator) {
    FinishReadObjectParentInstance(device);
    FinishReadObject(indirectCommandsLayout);
}

void ThreadSafety::PreCallRecordCreateObjectTableNVX(
    VkDevice                                    device,
    const VkObjectTableCreateInfoNVX*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkObjectTableNVX*                           pObjectTable) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordCreateObjectTableNVX(
    VkDevice                                    device,
    const VkObjectTableCreateInfoNVX*           pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkObjectTableNVX*                           pObjectTable,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    if (result == VK_SUCCESS) {
        CreateObject(*pObjectTable);
    }
}

void ThreadSafety::PreCallRecordDestroyObjectTableNVX(
    VkDevice                                    device,
    VkObjectTableNVX                            objectTable,
    const VkAllocationCallbacks*                pAllocator) {
    StartReadObjectParentInstance(device);
    StartWriteObject(objectTable);
    // Host access to objectTable must be externally synchronized
}

void ThreadSafety::PostCallRecordDestroyObjectTableNVX(
    VkDevice                                    device,
    VkObjectTableNVX                            objectTable,
    const VkAllocationCallbacks*                pAllocator) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(objectTable);
    DestroyObject(objectTable);
    // Host access to objectTable must be externally synchronized
}

void ThreadSafety::PreCallRecordRegisterObjectsNVX(
    VkDevice                                    device,
    VkObjectTableNVX                            objectTable,
    uint32_t                                    objectCount,
    const VkObjectTableEntryNVX* const*         ppObjectTableEntries,
    const uint32_t*                             pObjectIndices) {
    StartReadObjectParentInstance(device);
    StartWriteObject(objectTable);
    // Host access to objectTable must be externally synchronized
}

void ThreadSafety::PostCallRecordRegisterObjectsNVX(
    VkDevice                                    device,
    VkObjectTableNVX                            objectTable,
    uint32_t                                    objectCount,
    const VkObjectTableEntryNVX* const*         ppObjectTableEntries,
    const uint32_t*                             pObjectIndices,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(objectTable);
    // Host access to objectTable must be externally synchronized
}

void ThreadSafety::PreCallRecordUnregisterObjectsNVX(
    VkDevice                                    device,
    VkObjectTableNVX                            objectTable,
    uint32_t                                    objectCount,
    const VkObjectEntryTypeNVX*                 pObjectEntryTypes,
    const uint32_t*                             pObjectIndices) {
    StartReadObjectParentInstance(device);
    StartWriteObject(objectTable);
    // Host access to objectTable must be externally synchronized
}

void ThreadSafety::PostCallRecordUnregisterObjectsNVX(
    VkDevice                                    device,
    VkObjectTableNVX                            objectTable,
    uint32_t                                    objectCount,
    const VkObjectEntryTypeNVX*                 pObjectEntryTypes,
    const uint32_t*                             pObjectIndices,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(objectTable);
    // Host access to objectTable must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdSetViewportWScalingNV(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstViewport,
    uint32_t                                    viewportCount,
    const VkViewportWScalingNV*                 pViewportWScalings) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdSetViewportWScalingNV(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstViewport,
    uint32_t                                    viewportCount,
    const VkViewportWScalingNV*                 pViewportWScalings) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordReleaseDisplayEXT(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayKHR                                display) {
    StartReadObject(display);
}

void ThreadSafety::PostCallRecordReleaseDisplayEXT(
    VkPhysicalDevice                            physicalDevice,
    VkDisplayKHR                                display,
    VkResult                                    result) {
    FinishReadObject(display);
}

#ifdef VK_USE_PLATFORM_XLIB_XRANDR_EXT

void ThreadSafety::PreCallRecordAcquireXlibDisplayEXT(
    VkPhysicalDevice                            physicalDevice,
    Display*                                    dpy,
    VkDisplayKHR                                display) {
    StartReadObject(display);
}

void ThreadSafety::PostCallRecordAcquireXlibDisplayEXT(
    VkPhysicalDevice                            physicalDevice,
    Display*                                    dpy,
    VkDisplayKHR                                display,
    VkResult                                    result) {
    FinishReadObject(display);
}
#endif // VK_USE_PLATFORM_XLIB_XRANDR_EXT

void ThreadSafety::PreCallRecordGetPhysicalDeviceSurfaceCapabilities2EXT(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    VkSurfaceCapabilities2EXT*                  pSurfaceCapabilities) {
    StartReadObjectParentInstance(surface);
}

void ThreadSafety::PostCallRecordGetPhysicalDeviceSurfaceCapabilities2EXT(
    VkPhysicalDevice                            physicalDevice,
    VkSurfaceKHR                                surface,
    VkSurfaceCapabilities2EXT*                  pSurfaceCapabilities,
    VkResult                                    result) {
    FinishReadObjectParentInstance(surface);
}

void ThreadSafety::PreCallRecordDisplayPowerControlEXT(
    VkDevice                                    device,
    VkDisplayKHR                                display,
    const VkDisplayPowerInfoEXT*                pDisplayPowerInfo) {
    StartReadObjectParentInstance(device);
    StartReadObject(display);
}

void ThreadSafety::PostCallRecordDisplayPowerControlEXT(
    VkDevice                                    device,
    VkDisplayKHR                                display,
    const VkDisplayPowerInfoEXT*                pDisplayPowerInfo,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishReadObject(display);
}

void ThreadSafety::PreCallRecordRegisterDeviceEventEXT(
    VkDevice                                    device,
    const VkDeviceEventInfoEXT*                 pDeviceEventInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkFence*                                    pFence) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordRegisterDeviceEventEXT(
    VkDevice                                    device,
    const VkDeviceEventInfoEXT*                 pDeviceEventInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkFence*                                    pFence,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordRegisterDisplayEventEXT(
    VkDevice                                    device,
    VkDisplayKHR                                display,
    const VkDisplayEventInfoEXT*                pDisplayEventInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkFence*                                    pFence) {
    StartReadObjectParentInstance(device);
    StartReadObject(display);
}

void ThreadSafety::PostCallRecordRegisterDisplayEventEXT(
    VkDevice                                    device,
    VkDisplayKHR                                display,
    const VkDisplayEventInfoEXT*                pDisplayEventInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkFence*                                    pFence,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishReadObject(display);
}

void ThreadSafety::PreCallRecordGetSwapchainCounterEXT(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    VkSurfaceCounterFlagBitsEXT                 counter,
    uint64_t*                                   pCounterValue) {
    StartReadObjectParentInstance(device);
    StartReadObject(swapchain);
}

void ThreadSafety::PostCallRecordGetSwapchainCounterEXT(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    VkSurfaceCounterFlagBitsEXT                 counter,
    uint64_t*                                   pCounterValue,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishReadObject(swapchain);
}

void ThreadSafety::PreCallRecordGetRefreshCycleDurationGOOGLE(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    VkRefreshCycleDurationGOOGLE*               pDisplayTimingProperties) {
    StartReadObjectParentInstance(device);
    StartWriteObject(swapchain);
    // Host access to swapchain must be externally synchronized
}

void ThreadSafety::PostCallRecordGetRefreshCycleDurationGOOGLE(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    VkRefreshCycleDurationGOOGLE*               pDisplayTimingProperties,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(swapchain);
    // Host access to swapchain must be externally synchronized
}

void ThreadSafety::PreCallRecordGetPastPresentationTimingGOOGLE(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    uint32_t*                                   pPresentationTimingCount,
    VkPastPresentationTimingGOOGLE*             pPresentationTimings) {
    StartReadObjectParentInstance(device);
    StartWriteObject(swapchain);
    // Host access to swapchain must be externally synchronized
}

void ThreadSafety::PostCallRecordGetPastPresentationTimingGOOGLE(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    uint32_t*                                   pPresentationTimingCount,
    VkPastPresentationTimingGOOGLE*             pPresentationTimings,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(swapchain);
    // Host access to swapchain must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdSetDiscardRectangleEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstDiscardRectangle,
    uint32_t                                    discardRectangleCount,
    const VkRect2D*                             pDiscardRectangles) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdSetDiscardRectangleEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstDiscardRectangle,
    uint32_t                                    discardRectangleCount,
    const VkRect2D*                             pDiscardRectangles) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordSetHdrMetadataEXT(
    VkDevice                                    device,
    uint32_t                                    swapchainCount,
    const VkSwapchainKHR*                       pSwapchains,
    const VkHdrMetadataEXT*                     pMetadata) {
    StartReadObjectParentInstance(device);
    if (pSwapchains) {
        for (uint32_t index = 0; index < swapchainCount; index++) {
            StartReadObject(pSwapchains[index]);
        }
    }
}

void ThreadSafety::PostCallRecordSetHdrMetadataEXT(
    VkDevice                                    device,
    uint32_t                                    swapchainCount,
    const VkSwapchainKHR*                       pSwapchains,
    const VkHdrMetadataEXT*                     pMetadata) {
    FinishReadObjectParentInstance(device);
    if (pSwapchains) {
        for (uint32_t index = 0; index < swapchainCount; index++) {
            FinishReadObject(pSwapchains[index]);
        }
    }
}

#ifdef VK_USE_PLATFORM_IOS_MVK

void ThreadSafety::PreCallRecordCreateIOSSurfaceMVK(
    VkInstance                                  instance,
    const VkIOSSurfaceCreateInfoMVK*            pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    StartReadObjectParentInstance(instance);
}

void ThreadSafety::PostCallRecordCreateIOSSurfaceMVK(
    VkInstance                                  instance,
    const VkIOSSurfaceCreateInfoMVK*            pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface,
    VkResult                                    result) {
    FinishReadObjectParentInstance(instance);
    if (result == VK_SUCCESS) {
        CreateObjectParentInstance(*pSurface);
    }
}
#endif // VK_USE_PLATFORM_IOS_MVK

#ifdef VK_USE_PLATFORM_MACOS_MVK

void ThreadSafety::PreCallRecordCreateMacOSSurfaceMVK(
    VkInstance                                  instance,
    const VkMacOSSurfaceCreateInfoMVK*          pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    StartReadObjectParentInstance(instance);
}

void ThreadSafety::PostCallRecordCreateMacOSSurfaceMVK(
    VkInstance                                  instance,
    const VkMacOSSurfaceCreateInfoMVK*          pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface,
    VkResult                                    result) {
    FinishReadObjectParentInstance(instance);
    if (result == VK_SUCCESS) {
        CreateObjectParentInstance(*pSurface);
    }
}
#endif // VK_USE_PLATFORM_MACOS_MVK
// TODO - not wrapping EXT function vkSetDebugUtilsObjectNameEXT
// TODO - not wrapping EXT function vkSetDebugUtilsObjectTagEXT

void ThreadSafety::PreCallRecordQueueBeginDebugUtilsLabelEXT(
    VkQueue                                     queue,
    const VkDebugUtilsLabelEXT*                 pLabelInfo) {
    StartReadObject(queue);
}

void ThreadSafety::PostCallRecordQueueBeginDebugUtilsLabelEXT(
    VkQueue                                     queue,
    const VkDebugUtilsLabelEXT*                 pLabelInfo) {
    FinishReadObject(queue);
}

void ThreadSafety::PreCallRecordQueueEndDebugUtilsLabelEXT(
    VkQueue                                     queue) {
    StartReadObject(queue);
}

void ThreadSafety::PostCallRecordQueueEndDebugUtilsLabelEXT(
    VkQueue                                     queue) {
    FinishReadObject(queue);
}

void ThreadSafety::PreCallRecordQueueInsertDebugUtilsLabelEXT(
    VkQueue                                     queue,
    const VkDebugUtilsLabelEXT*                 pLabelInfo) {
    StartReadObject(queue);
}

void ThreadSafety::PostCallRecordQueueInsertDebugUtilsLabelEXT(
    VkQueue                                     queue,
    const VkDebugUtilsLabelEXT*                 pLabelInfo) {
    FinishReadObject(queue);
}

void ThreadSafety::PreCallRecordCmdBeginDebugUtilsLabelEXT(
    VkCommandBuffer                             commandBuffer,
    const VkDebugUtilsLabelEXT*                 pLabelInfo) {
    StartReadObject(commandBuffer);
}

void ThreadSafety::PostCallRecordCmdBeginDebugUtilsLabelEXT(
    VkCommandBuffer                             commandBuffer,
    const VkDebugUtilsLabelEXT*                 pLabelInfo) {
    FinishReadObject(commandBuffer);
}

void ThreadSafety::PreCallRecordCmdEndDebugUtilsLabelEXT(
    VkCommandBuffer                             commandBuffer) {
    StartReadObject(commandBuffer);
}

void ThreadSafety::PostCallRecordCmdEndDebugUtilsLabelEXT(
    VkCommandBuffer                             commandBuffer) {
    FinishReadObject(commandBuffer);
}

void ThreadSafety::PreCallRecordCmdInsertDebugUtilsLabelEXT(
    VkCommandBuffer                             commandBuffer,
    const VkDebugUtilsLabelEXT*                 pLabelInfo) {
    StartReadObject(commandBuffer);
}

void ThreadSafety::PostCallRecordCmdInsertDebugUtilsLabelEXT(
    VkCommandBuffer                             commandBuffer,
    const VkDebugUtilsLabelEXT*                 pLabelInfo) {
    FinishReadObject(commandBuffer);
}

void ThreadSafety::PreCallRecordCreateDebugUtilsMessengerEXT(
    VkInstance                                  instance,
    const VkDebugUtilsMessengerCreateInfoEXT*   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDebugUtilsMessengerEXT*                   pMessenger) {
    StartReadObjectParentInstance(instance);
}

void ThreadSafety::PostCallRecordCreateDebugUtilsMessengerEXT(
    VkInstance                                  instance,
    const VkDebugUtilsMessengerCreateInfoEXT*   pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkDebugUtilsMessengerEXT*                   pMessenger,
    VkResult                                    result) {
    FinishReadObjectParentInstance(instance);
    if (result == VK_SUCCESS) {
        CreateObjectParentInstance(*pMessenger);
    }
}

void ThreadSafety::PreCallRecordDestroyDebugUtilsMessengerEXT(
    VkInstance                                  instance,
    VkDebugUtilsMessengerEXT                    messenger,
    const VkAllocationCallbacks*                pAllocator) {
    StartReadObjectParentInstance(instance);
    StartWriteObjectParentInstance(messenger);
    // Host access to messenger must be externally synchronized
}

void ThreadSafety::PostCallRecordDestroyDebugUtilsMessengerEXT(
    VkInstance                                  instance,
    VkDebugUtilsMessengerEXT                    messenger,
    const VkAllocationCallbacks*                pAllocator) {
    FinishReadObjectParentInstance(instance);
    FinishWriteObjectParentInstance(messenger);
    DestroyObjectParentInstance(messenger);
    // Host access to messenger must be externally synchronized
}

void ThreadSafety::PreCallRecordSubmitDebugUtilsMessageEXT(
    VkInstance                                  instance,
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData) {
    StartReadObjectParentInstance(instance);
}

void ThreadSafety::PostCallRecordSubmitDebugUtilsMessageEXT(
    VkInstance                                  instance,
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData) {
    FinishReadObjectParentInstance(instance);
}

#ifdef VK_USE_PLATFORM_ANDROID_KHR

void ThreadSafety::PreCallRecordGetAndroidHardwareBufferPropertiesANDROID(
    VkDevice                                    device,
    const struct AHardwareBuffer*               buffer,
    VkAndroidHardwareBufferPropertiesANDROID*   pProperties) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetAndroidHardwareBufferPropertiesANDROID(
    VkDevice                                    device,
    const struct AHardwareBuffer*               buffer,
    VkAndroidHardwareBufferPropertiesANDROID*   pProperties,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordGetMemoryAndroidHardwareBufferANDROID(
    VkDevice                                    device,
    const VkMemoryGetAndroidHardwareBufferInfoANDROID* pInfo,
    struct AHardwareBuffer**                    pBuffer) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetMemoryAndroidHardwareBufferANDROID(
    VkDevice                                    device,
    const VkMemoryGetAndroidHardwareBufferInfoANDROID* pInfo,
    struct AHardwareBuffer**                    pBuffer,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}
#endif // VK_USE_PLATFORM_ANDROID_KHR

void ThreadSafety::PreCallRecordCmdSetSampleLocationsEXT(
    VkCommandBuffer                             commandBuffer,
    const VkSampleLocationsInfoEXT*             pSampleLocationsInfo) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdSetSampleLocationsEXT(
    VkCommandBuffer                             commandBuffer,
    const VkSampleLocationsInfoEXT*             pSampleLocationsInfo) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordGetImageDrmFormatModifierPropertiesEXT(
    VkDevice                                    device,
    VkImage                                     image,
    VkImageDrmFormatModifierPropertiesEXT*      pProperties) {
    StartReadObjectParentInstance(device);
    StartReadObject(image);
}

void ThreadSafety::PostCallRecordGetImageDrmFormatModifierPropertiesEXT(
    VkDevice                                    device,
    VkImage                                     image,
    VkImageDrmFormatModifierPropertiesEXT*      pProperties,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishReadObject(image);
}

void ThreadSafety::PreCallRecordCreateValidationCacheEXT(
    VkDevice                                    device,
    const VkValidationCacheCreateInfoEXT*       pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkValidationCacheEXT*                       pValidationCache) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordCreateValidationCacheEXT(
    VkDevice                                    device,
    const VkValidationCacheCreateInfoEXT*       pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkValidationCacheEXT*                       pValidationCache,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    if (result == VK_SUCCESS) {
        CreateObject(*pValidationCache);
    }
}

void ThreadSafety::PreCallRecordDestroyValidationCacheEXT(
    VkDevice                                    device,
    VkValidationCacheEXT                        validationCache,
    const VkAllocationCallbacks*                pAllocator) {
    StartReadObjectParentInstance(device);
    StartWriteObject(validationCache);
    // Host access to validationCache must be externally synchronized
}

void ThreadSafety::PostCallRecordDestroyValidationCacheEXT(
    VkDevice                                    device,
    VkValidationCacheEXT                        validationCache,
    const VkAllocationCallbacks*                pAllocator) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(validationCache);
    DestroyObject(validationCache);
    // Host access to validationCache must be externally synchronized
}

void ThreadSafety::PreCallRecordMergeValidationCachesEXT(
    VkDevice                                    device,
    VkValidationCacheEXT                        dstCache,
    uint32_t                                    srcCacheCount,
    const VkValidationCacheEXT*                 pSrcCaches) {
    StartReadObjectParentInstance(device);
    StartWriteObject(dstCache);
    if (pSrcCaches) {
        for (uint32_t index = 0; index < srcCacheCount; index++) {
            StartReadObject(pSrcCaches[index]);
        }
    }
    // Host access to dstCache must be externally synchronized
}

void ThreadSafety::PostCallRecordMergeValidationCachesEXT(
    VkDevice                                    device,
    VkValidationCacheEXT                        dstCache,
    uint32_t                                    srcCacheCount,
    const VkValidationCacheEXT*                 pSrcCaches,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishWriteObject(dstCache);
    if (pSrcCaches) {
        for (uint32_t index = 0; index < srcCacheCount; index++) {
            FinishReadObject(pSrcCaches[index]);
        }
    }
    // Host access to dstCache must be externally synchronized
}

void ThreadSafety::PreCallRecordGetValidationCacheDataEXT(
    VkDevice                                    device,
    VkValidationCacheEXT                        validationCache,
    size_t*                                     pDataSize,
    void*                                       pData) {
    StartReadObjectParentInstance(device);
    StartReadObject(validationCache);
}

void ThreadSafety::PostCallRecordGetValidationCacheDataEXT(
    VkDevice                                    device,
    VkValidationCacheEXT                        validationCache,
    size_t*                                     pDataSize,
    void*                                       pData,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishReadObject(validationCache);
}

void ThreadSafety::PreCallRecordCmdBindShadingRateImageNV(
    VkCommandBuffer                             commandBuffer,
    VkImageView                                 imageView,
    VkImageLayout                               imageLayout) {
    StartWriteObject(commandBuffer);
    StartReadObject(imageView);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdBindShadingRateImageNV(
    VkCommandBuffer                             commandBuffer,
    VkImageView                                 imageView,
    VkImageLayout                               imageLayout) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(imageView);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdSetViewportShadingRatePaletteNV(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstViewport,
    uint32_t                                    viewportCount,
    const VkShadingRatePaletteNV*               pShadingRatePalettes) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdSetViewportShadingRatePaletteNV(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstViewport,
    uint32_t                                    viewportCount,
    const VkShadingRatePaletteNV*               pShadingRatePalettes) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdSetCoarseSampleOrderNV(
    VkCommandBuffer                             commandBuffer,
    VkCoarseSampleOrderTypeNV                   sampleOrderType,
    uint32_t                                    customSampleOrderCount,
    const VkCoarseSampleOrderCustomNV*          pCustomSampleOrders) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdSetCoarseSampleOrderNV(
    VkCommandBuffer                             commandBuffer,
    VkCoarseSampleOrderTypeNV                   sampleOrderType,
    uint32_t                                    customSampleOrderCount,
    const VkCoarseSampleOrderCustomNV*          pCustomSampleOrders) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCreateAccelerationStructureNV(
    VkDevice                                    device,
    const VkAccelerationStructureCreateInfoNV*  pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkAccelerationStructureNV*                  pAccelerationStructure) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordCreateAccelerationStructureNV(
    VkDevice                                    device,
    const VkAccelerationStructureCreateInfoNV*  pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkAccelerationStructureNV*                  pAccelerationStructure,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    if (result == VK_SUCCESS) {
        CreateObject(*pAccelerationStructure);
    }
}

void ThreadSafety::PreCallRecordDestroyAccelerationStructureNV(
    VkDevice                                    device,
    VkAccelerationStructureNV                   accelerationStructure,
    const VkAllocationCallbacks*                pAllocator) {
    StartReadObjectParentInstance(device);
    StartReadObject(accelerationStructure);
}

void ThreadSafety::PostCallRecordDestroyAccelerationStructureNV(
    VkDevice                                    device,
    VkAccelerationStructureNV                   accelerationStructure,
    const VkAllocationCallbacks*                pAllocator) {
    FinishReadObjectParentInstance(device);
    FinishReadObject(accelerationStructure);
}

void ThreadSafety::PreCallRecordGetAccelerationStructureMemoryRequirementsNV(
    VkDevice                                    device,
    const VkAccelerationStructureMemoryRequirementsInfoNV* pInfo,
    VkMemoryRequirements2KHR*                   pMemoryRequirements) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetAccelerationStructureMemoryRequirementsNV(
    VkDevice                                    device,
    const VkAccelerationStructureMemoryRequirementsInfoNV* pInfo,
    VkMemoryRequirements2KHR*                   pMemoryRequirements) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordBindAccelerationStructureMemoryNV(
    VkDevice                                    device,
    uint32_t                                    bindInfoCount,
    const VkBindAccelerationStructureMemoryInfoNV* pBindInfos) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordBindAccelerationStructureMemoryNV(
    VkDevice                                    device,
    uint32_t                                    bindInfoCount,
    const VkBindAccelerationStructureMemoryInfoNV* pBindInfos,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordCmdBuildAccelerationStructureNV(
    VkCommandBuffer                             commandBuffer,
    const VkAccelerationStructureInfoNV*        pInfo,
    VkBuffer                                    instanceData,
    VkDeviceSize                                instanceOffset,
    VkBool32                                    update,
    VkAccelerationStructureNV                   dst,
    VkAccelerationStructureNV                   src,
    VkBuffer                                    scratch,
    VkDeviceSize                                scratchOffset) {
    StartReadObject(commandBuffer);
    StartReadObject(instanceData);
    StartReadObject(dst);
    StartReadObject(src);
    StartReadObject(scratch);
}

void ThreadSafety::PostCallRecordCmdBuildAccelerationStructureNV(
    VkCommandBuffer                             commandBuffer,
    const VkAccelerationStructureInfoNV*        pInfo,
    VkBuffer                                    instanceData,
    VkDeviceSize                                instanceOffset,
    VkBool32                                    update,
    VkAccelerationStructureNV                   dst,
    VkAccelerationStructureNV                   src,
    VkBuffer                                    scratch,
    VkDeviceSize                                scratchOffset) {
    FinishReadObject(commandBuffer);
    FinishReadObject(instanceData);
    FinishReadObject(dst);
    FinishReadObject(src);
    FinishReadObject(scratch);
}

void ThreadSafety::PreCallRecordCmdCopyAccelerationStructureNV(
    VkCommandBuffer                             commandBuffer,
    VkAccelerationStructureNV                   dst,
    VkAccelerationStructureNV                   src,
    VkCopyAccelerationStructureModeNV           mode) {
    StartReadObject(commandBuffer);
    StartReadObject(dst);
    StartReadObject(src);
}

void ThreadSafety::PostCallRecordCmdCopyAccelerationStructureNV(
    VkCommandBuffer                             commandBuffer,
    VkAccelerationStructureNV                   dst,
    VkAccelerationStructureNV                   src,
    VkCopyAccelerationStructureModeNV           mode) {
    FinishReadObject(commandBuffer);
    FinishReadObject(dst);
    FinishReadObject(src);
}

void ThreadSafety::PreCallRecordCmdTraceRaysNV(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    raygenShaderBindingTableBuffer,
    VkDeviceSize                                raygenShaderBindingOffset,
    VkBuffer                                    missShaderBindingTableBuffer,
    VkDeviceSize                                missShaderBindingOffset,
    VkDeviceSize                                missShaderBindingStride,
    VkBuffer                                    hitShaderBindingTableBuffer,
    VkDeviceSize                                hitShaderBindingOffset,
    VkDeviceSize                                hitShaderBindingStride,
    VkBuffer                                    callableShaderBindingTableBuffer,
    VkDeviceSize                                callableShaderBindingOffset,
    VkDeviceSize                                callableShaderBindingStride,
    uint32_t                                    width,
    uint32_t                                    height,
    uint32_t                                    depth) {
    StartReadObject(commandBuffer);
    StartReadObject(raygenShaderBindingTableBuffer);
    StartReadObject(missShaderBindingTableBuffer);
    StartReadObject(hitShaderBindingTableBuffer);
    StartReadObject(callableShaderBindingTableBuffer);
}

void ThreadSafety::PostCallRecordCmdTraceRaysNV(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    raygenShaderBindingTableBuffer,
    VkDeviceSize                                raygenShaderBindingOffset,
    VkBuffer                                    missShaderBindingTableBuffer,
    VkDeviceSize                                missShaderBindingOffset,
    VkDeviceSize                                missShaderBindingStride,
    VkBuffer                                    hitShaderBindingTableBuffer,
    VkDeviceSize                                hitShaderBindingOffset,
    VkDeviceSize                                hitShaderBindingStride,
    VkBuffer                                    callableShaderBindingTableBuffer,
    VkDeviceSize                                callableShaderBindingOffset,
    VkDeviceSize                                callableShaderBindingStride,
    uint32_t                                    width,
    uint32_t                                    height,
    uint32_t                                    depth) {
    FinishReadObject(commandBuffer);
    FinishReadObject(raygenShaderBindingTableBuffer);
    FinishReadObject(missShaderBindingTableBuffer);
    FinishReadObject(hitShaderBindingTableBuffer);
    FinishReadObject(callableShaderBindingTableBuffer);
}

void ThreadSafety::PreCallRecordCreateRayTracingPipelinesNV(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    uint32_t                                    createInfoCount,
    const VkRayTracingPipelineCreateInfoNV*     pCreateInfos,
    const VkAllocationCallbacks*                pAllocator,
    VkPipeline*                                 pPipelines) {
    StartReadObjectParentInstance(device);
    StartReadObject(pipelineCache);
}

void ThreadSafety::PostCallRecordCreateRayTracingPipelinesNV(
    VkDevice                                    device,
    VkPipelineCache                             pipelineCache,
    uint32_t                                    createInfoCount,
    const VkRayTracingPipelineCreateInfoNV*     pCreateInfos,
    const VkAllocationCallbacks*                pAllocator,
    VkPipeline*                                 pPipelines,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishReadObject(pipelineCache);
    if (pPipelines) {
        for (uint32_t index = 0; index < createInfoCount; index++) {
            if (!pPipelines[index]) continue;
            CreateObject(pPipelines[index]);
        }
    }
}

void ThreadSafety::PreCallRecordGetRayTracingShaderGroupHandlesNV(
    VkDevice                                    device,
    VkPipeline                                  pipeline,
    uint32_t                                    firstGroup,
    uint32_t                                    groupCount,
    size_t                                      dataSize,
    void*                                       pData) {
    StartReadObjectParentInstance(device);
    StartReadObject(pipeline);
}

void ThreadSafety::PostCallRecordGetRayTracingShaderGroupHandlesNV(
    VkDevice                                    device,
    VkPipeline                                  pipeline,
    uint32_t                                    firstGroup,
    uint32_t                                    groupCount,
    size_t                                      dataSize,
    void*                                       pData,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishReadObject(pipeline);
}

void ThreadSafety::PreCallRecordGetAccelerationStructureHandleNV(
    VkDevice                                    device,
    VkAccelerationStructureNV                   accelerationStructure,
    size_t                                      dataSize,
    void*                                       pData) {
    StartReadObjectParentInstance(device);
    StartReadObject(accelerationStructure);
}

void ThreadSafety::PostCallRecordGetAccelerationStructureHandleNV(
    VkDevice                                    device,
    VkAccelerationStructureNV                   accelerationStructure,
    size_t                                      dataSize,
    void*                                       pData,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishReadObject(accelerationStructure);
}

void ThreadSafety::PreCallRecordCmdWriteAccelerationStructuresPropertiesNV(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    accelerationStructureCount,
    const VkAccelerationStructureNV*            pAccelerationStructures,
    VkQueryType                                 queryType,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery) {
    StartReadObject(commandBuffer);
    if (pAccelerationStructures) {
        for (uint32_t index = 0; index < accelerationStructureCount; index++) {
            StartReadObject(pAccelerationStructures[index]);
        }
    }
    StartReadObject(queryPool);
}

void ThreadSafety::PostCallRecordCmdWriteAccelerationStructuresPropertiesNV(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    accelerationStructureCount,
    const VkAccelerationStructureNV*            pAccelerationStructures,
    VkQueryType                                 queryType,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery) {
    FinishReadObject(commandBuffer);
    if (pAccelerationStructures) {
        for (uint32_t index = 0; index < accelerationStructureCount; index++) {
            FinishReadObject(pAccelerationStructures[index]);
        }
    }
    FinishReadObject(queryPool);
}

void ThreadSafety::PreCallRecordCompileDeferredNV(
    VkDevice                                    device,
    VkPipeline                                  pipeline,
    uint32_t                                    shader) {
    StartReadObjectParentInstance(device);
    StartReadObject(pipeline);
}

void ThreadSafety::PostCallRecordCompileDeferredNV(
    VkDevice                                    device,
    VkPipeline                                  pipeline,
    uint32_t                                    shader,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishReadObject(pipeline);
}

void ThreadSafety::PreCallRecordGetMemoryHostPointerPropertiesEXT(
    VkDevice                                    device,
    VkExternalMemoryHandleTypeFlagBits          handleType,
    const void*                                 pHostPointer,
    VkMemoryHostPointerPropertiesEXT*           pMemoryHostPointerProperties) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetMemoryHostPointerPropertiesEXT(
    VkDevice                                    device,
    VkExternalMemoryHandleTypeFlagBits          handleType,
    const void*                                 pHostPointer,
    VkMemoryHostPointerPropertiesEXT*           pMemoryHostPointerProperties,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordCmdWriteBufferMarkerAMD(
    VkCommandBuffer                             commandBuffer,
    VkPipelineStageFlagBits                     pipelineStage,
    VkBuffer                                    dstBuffer,
    VkDeviceSize                                dstOffset,
    uint32_t                                    marker) {
    StartWriteObject(commandBuffer);
    StartReadObject(dstBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdWriteBufferMarkerAMD(
    VkCommandBuffer                             commandBuffer,
    VkPipelineStageFlagBits                     pipelineStage,
    VkBuffer                                    dstBuffer,
    VkDeviceSize                                dstOffset,
    uint32_t                                    marker) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(dstBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordGetCalibratedTimestampsEXT(
    VkDevice                                    device,
    uint32_t                                    timestampCount,
    const VkCalibratedTimestampInfoEXT*         pTimestampInfos,
    uint64_t*                                   pTimestamps,
    uint64_t*                                   pMaxDeviation) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetCalibratedTimestampsEXT(
    VkDevice                                    device,
    uint32_t                                    timestampCount,
    const VkCalibratedTimestampInfoEXT*         pTimestampInfos,
    uint64_t*                                   pTimestamps,
    uint64_t*                                   pMaxDeviation,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}

#ifdef VK_USE_PLATFORM_GGP
#endif // VK_USE_PLATFORM_GGP

void ThreadSafety::PreCallRecordCmdDrawMeshTasksNV(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    taskCount,
    uint32_t                                    firstTask) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdDrawMeshTasksNV(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    taskCount,
    uint32_t                                    firstTask) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdDrawMeshTasksIndirectNV(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    uint32_t                                    drawCount,
    uint32_t                                    stride) {
    StartWriteObject(commandBuffer);
    StartReadObject(buffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdDrawMeshTasksIndirectNV(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    uint32_t                                    drawCount,
    uint32_t                                    stride) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(buffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdDrawMeshTasksIndirectCountNV(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkBuffer                                    countBuffer,
    VkDeviceSize                                countBufferOffset,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride) {
    StartWriteObject(commandBuffer);
    StartReadObject(buffer);
    StartReadObject(countBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdDrawMeshTasksIndirectCountNV(
    VkCommandBuffer                             commandBuffer,
    VkBuffer                                    buffer,
    VkDeviceSize                                offset,
    VkBuffer                                    countBuffer,
    VkDeviceSize                                countBufferOffset,
    uint32_t                                    maxDrawCount,
    uint32_t                                    stride) {
    FinishWriteObject(commandBuffer);
    FinishReadObject(buffer);
    FinishReadObject(countBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdSetExclusiveScissorNV(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstExclusiveScissor,
    uint32_t                                    exclusiveScissorCount,
    const VkRect2D*                             pExclusiveScissors) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdSetExclusiveScissorNV(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    firstExclusiveScissor,
    uint32_t                                    exclusiveScissorCount,
    const VkRect2D*                             pExclusiveScissors) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordCmdSetCheckpointNV(
    VkCommandBuffer                             commandBuffer,
    const void*                                 pCheckpointMarker) {
    StartReadObject(commandBuffer);
}

void ThreadSafety::PostCallRecordCmdSetCheckpointNV(
    VkCommandBuffer                             commandBuffer,
    const void*                                 pCheckpointMarker) {
    FinishReadObject(commandBuffer);
}

void ThreadSafety::PreCallRecordGetQueueCheckpointDataNV(
    VkQueue                                     queue,
    uint32_t*                                   pCheckpointDataCount,
    VkCheckpointDataNV*                         pCheckpointData) {
    StartReadObject(queue);
}

void ThreadSafety::PostCallRecordGetQueueCheckpointDataNV(
    VkQueue                                     queue,
    uint32_t*                                   pCheckpointDataCount,
    VkCheckpointDataNV*                         pCheckpointData) {
    FinishReadObject(queue);
}

void ThreadSafety::PreCallRecordInitializePerformanceApiINTEL(
    VkDevice                                    device,
    const VkInitializePerformanceApiInfoINTEL*  pInitializeInfo) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordInitializePerformanceApiINTEL(
    VkDevice                                    device,
    const VkInitializePerformanceApiInfoINTEL*  pInitializeInfo,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordUninitializePerformanceApiINTEL(
    VkDevice                                    device) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordUninitializePerformanceApiINTEL(
    VkDevice                                    device) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordCmdSetPerformanceMarkerINTEL(
    VkCommandBuffer                             commandBuffer,
    const VkPerformanceMarkerInfoINTEL*         pMarkerInfo) {
    StartReadObject(commandBuffer);
}

void ThreadSafety::PostCallRecordCmdSetPerformanceMarkerINTEL(
    VkCommandBuffer                             commandBuffer,
    const VkPerformanceMarkerInfoINTEL*         pMarkerInfo,
    VkResult                                    result) {
    FinishReadObject(commandBuffer);
}

void ThreadSafety::PreCallRecordCmdSetPerformanceStreamMarkerINTEL(
    VkCommandBuffer                             commandBuffer,
    const VkPerformanceStreamMarkerInfoINTEL*   pMarkerInfo) {
    StartReadObject(commandBuffer);
}

void ThreadSafety::PostCallRecordCmdSetPerformanceStreamMarkerINTEL(
    VkCommandBuffer                             commandBuffer,
    const VkPerformanceStreamMarkerInfoINTEL*   pMarkerInfo,
    VkResult                                    result) {
    FinishReadObject(commandBuffer);
}

void ThreadSafety::PreCallRecordCmdSetPerformanceOverrideINTEL(
    VkCommandBuffer                             commandBuffer,
    const VkPerformanceOverrideInfoINTEL*       pOverrideInfo) {
    StartReadObject(commandBuffer);
}

void ThreadSafety::PostCallRecordCmdSetPerformanceOverrideINTEL(
    VkCommandBuffer                             commandBuffer,
    const VkPerformanceOverrideInfoINTEL*       pOverrideInfo,
    VkResult                                    result) {
    FinishReadObject(commandBuffer);
}

void ThreadSafety::PreCallRecordAcquirePerformanceConfigurationINTEL(
    VkDevice                                    device,
    const VkPerformanceConfigurationAcquireInfoINTEL* pAcquireInfo,
    VkPerformanceConfigurationINTEL*            pConfiguration) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordAcquirePerformanceConfigurationINTEL(
    VkDevice                                    device,
    const VkPerformanceConfigurationAcquireInfoINTEL* pAcquireInfo,
    VkPerformanceConfigurationINTEL*            pConfiguration,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordReleasePerformanceConfigurationINTEL(
    VkDevice                                    device,
    VkPerformanceConfigurationINTEL             configuration) {
    StartReadObjectParentInstance(device);
    StartReadObject(configuration);
}

void ThreadSafety::PostCallRecordReleasePerformanceConfigurationINTEL(
    VkDevice                                    device,
    VkPerformanceConfigurationINTEL             configuration,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishReadObject(configuration);
}

void ThreadSafety::PreCallRecordQueueSetPerformanceConfigurationINTEL(
    VkQueue                                     queue,
    VkPerformanceConfigurationINTEL             configuration) {
    StartReadObject(queue);
    StartReadObject(configuration);
}

void ThreadSafety::PostCallRecordQueueSetPerformanceConfigurationINTEL(
    VkQueue                                     queue,
    VkPerformanceConfigurationINTEL             configuration,
    VkResult                                    result) {
    FinishReadObject(queue);
    FinishReadObject(configuration);
}

void ThreadSafety::PreCallRecordGetPerformanceParameterINTEL(
    VkDevice                                    device,
    VkPerformanceParameterTypeINTEL             parameter,
    VkPerformanceValueINTEL*                    pValue) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetPerformanceParameterINTEL(
    VkDevice                                    device,
    VkPerformanceParameterTypeINTEL             parameter,
    VkPerformanceValueINTEL*                    pValue,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}

void ThreadSafety::PreCallRecordSetLocalDimmingAMD(
    VkDevice                                    device,
    VkSwapchainKHR                              swapChain,
    VkBool32                                    localDimmingEnable) {
    StartReadObjectParentInstance(device);
    StartReadObject(swapChain);
}

void ThreadSafety::PostCallRecordSetLocalDimmingAMD(
    VkDevice                                    device,
    VkSwapchainKHR                              swapChain,
    VkBool32                                    localDimmingEnable) {
    FinishReadObjectParentInstance(device);
    FinishReadObject(swapChain);
}

#ifdef VK_USE_PLATFORM_FUCHSIA

void ThreadSafety::PreCallRecordCreateImagePipeSurfaceFUCHSIA(
    VkInstance                                  instance,
    const VkImagePipeSurfaceCreateInfoFUCHSIA*  pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    StartReadObjectParentInstance(instance);
}

void ThreadSafety::PostCallRecordCreateImagePipeSurfaceFUCHSIA(
    VkInstance                                  instance,
    const VkImagePipeSurfaceCreateInfoFUCHSIA*  pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface,
    VkResult                                    result) {
    FinishReadObjectParentInstance(instance);
    if (result == VK_SUCCESS) {
        CreateObjectParentInstance(*pSurface);
    }
}
#endif // VK_USE_PLATFORM_FUCHSIA

#ifdef VK_USE_PLATFORM_METAL_EXT

void ThreadSafety::PreCallRecordCreateMetalSurfaceEXT(
    VkInstance                                  instance,
    const VkMetalSurfaceCreateInfoEXT*          pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    StartReadObjectParentInstance(instance);
}

void ThreadSafety::PostCallRecordCreateMetalSurfaceEXT(
    VkInstance                                  instance,
    const VkMetalSurfaceCreateInfoEXT*          pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface,
    VkResult                                    result) {
    FinishReadObjectParentInstance(instance);
    if (result == VK_SUCCESS) {
        CreateObjectParentInstance(*pSurface);
    }
}
#endif // VK_USE_PLATFORM_METAL_EXT

void ThreadSafety::PreCallRecordGetBufferDeviceAddressEXT(
    VkDevice                                    device,
    const VkBufferDeviceAddressInfoEXT*         pInfo) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetBufferDeviceAddressEXT(
    VkDevice                                    device,
    const VkBufferDeviceAddressInfoEXT*         pInfo,
    VkDeviceAddress                             result) {
    FinishReadObjectParentInstance(device);
}

#ifdef VK_USE_PLATFORM_WIN32_KHR

void ThreadSafety::PreCallRecordAcquireFullScreenExclusiveModeEXT(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain) {
    StartReadObjectParentInstance(device);
    StartReadObject(swapchain);
}

void ThreadSafety::PostCallRecordAcquireFullScreenExclusiveModeEXT(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishReadObject(swapchain);
}

void ThreadSafety::PreCallRecordReleaseFullScreenExclusiveModeEXT(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain) {
    StartReadObjectParentInstance(device);
    StartReadObject(swapchain);
}

void ThreadSafety::PostCallRecordReleaseFullScreenExclusiveModeEXT(
    VkDevice                                    device,
    VkSwapchainKHR                              swapchain,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
    FinishReadObject(swapchain);
}

void ThreadSafety::PreCallRecordGetDeviceGroupSurfacePresentModes2EXT(
    VkDevice                                    device,
    const VkPhysicalDeviceSurfaceInfo2KHR*      pSurfaceInfo,
    VkDeviceGroupPresentModeFlagsKHR*           pModes) {
    StartReadObjectParentInstance(device);
}

void ThreadSafety::PostCallRecordGetDeviceGroupSurfacePresentModes2EXT(
    VkDevice                                    device,
    const VkPhysicalDeviceSurfaceInfo2KHR*      pSurfaceInfo,
    VkDeviceGroupPresentModeFlagsKHR*           pModes,
    VkResult                                    result) {
    FinishReadObjectParentInstance(device);
}
#endif // VK_USE_PLATFORM_WIN32_KHR

void ThreadSafety::PreCallRecordCreateHeadlessSurfaceEXT(
    VkInstance                                  instance,
    const VkHeadlessSurfaceCreateInfoEXT*       pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface) {
    StartReadObjectParentInstance(instance);
}

void ThreadSafety::PostCallRecordCreateHeadlessSurfaceEXT(
    VkInstance                                  instance,
    const VkHeadlessSurfaceCreateInfoEXT*       pCreateInfo,
    const VkAllocationCallbacks*                pAllocator,
    VkSurfaceKHR*                               pSurface,
    VkResult                                    result) {
    FinishReadObjectParentInstance(instance);
    if (result == VK_SUCCESS) {
        CreateObjectParentInstance(*pSurface);
    }
}

void ThreadSafety::PreCallRecordCmdSetLineStippleEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    lineStippleFactor,
    uint16_t                                    lineStipplePattern) {
    StartWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PostCallRecordCmdSetLineStippleEXT(
    VkCommandBuffer                             commandBuffer,
    uint32_t                                    lineStippleFactor,
    uint16_t                                    lineStipplePattern) {
    FinishWriteObject(commandBuffer);
    // Host access to commandBuffer must be externally synchronized
}

void ThreadSafety::PreCallRecordResetQueryPoolEXT(
    VkDevice                                    device,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery,
    uint32_t                                    queryCount) {
    StartReadObjectParentInstance(device);
    StartReadObject(queryPool);
}

void ThreadSafety::PostCallRecordResetQueryPoolEXT(
    VkDevice                                    device,
    VkQueryPool                                 queryPool,
    uint32_t                                    firstQuery,
    uint32_t                                    queryCount) {
    FinishReadObjectParentInstance(device);
    FinishReadObject(queryPool);
}
