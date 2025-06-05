#include "VulkanRenderer.h"
#include <stdexcept>
#include <iostream>
#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>
#include <backends/imgui_impl_sdl2.h>


namespace REngine {
    VulkanRenderer::VulkanRenderer()
        : m_instance(VK_NULL_HANDLE)
        , m_physicalDevice(VK_NULL_HANDLE)
        , m_device(VK_NULL_HANDLE)
        , m_surface(VK_NULL_HANDLE)
        , m_swapchain(VK_NULL_HANDLE)
        , m_renderPass(VK_NULL_HANDLE)
        , m_commandPool(VK_NULL_HANDLE)
        , m_currentFrame(0)
        , m_imageIndex(0)
        , m_window(nullptr) {}

    VulkanRenderer::~VulkanRenderer() {
        Shutdown();
    }

    bool VulkanRenderer::Initialize(SDL_Window* window) {

        m_window = window;

        m_initialized = false;

        try {
            if (!CreateInstance()) {
                Shutdown();
                return false;
            }
            if (!CreateSurface()) {
                Shutdown();
                return false;
            }
            if (!SelectPhysicalDevice()) {
                Shutdown();
                return false;
            }
            if (!CreateLogicalDevice()) {
                Shutdown();
                return false;
            }
            if (!CreateSwapchain()) {
                Shutdown();
                return false;
            }
            if (!CreateImageViews()) {
                Shutdown();
                return false;
            }
            if (!CreateRenderPass()) {
                Shutdown();
                return false;
            }
            if (!CreateFramebuffers()) {
                Shutdown();
                return false;
            }
            if (!CreateCommandPool()) {
                Shutdown();
                return false;
            }
            if (!CreateCommandBuffers()) {
                Shutdown();
                return false;
            }
            if (!CreateSyncObjects()) {
                Shutdown();
                return false;
            }
            m_initialized = true;
            return true;
        } catch (const std::exception& e) {
            m_initialized = false;
            std::cerr << "Vulkan initialization error: " << e.what() << std::endl;
            Shutdown();
            return false;
        }
    }

    void VulkanRenderer::Shutdown() {
        // 1. Wait for device to finish all operations
        if (m_device != VK_NULL_HANDLE) {
            vkDeviceWaitIdle(m_device);
        }

        // 2. Cleanup swapchain resources (framebuffers, image views, swapchain)
        CleanupSwapchain();

        // 3. Destroy synchronization objects
        if (m_device != VK_NULL_HANDLE) {
            for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
                if (m_imageAvailableSemaphores[i] != VK_NULL_HANDLE) {
                    vkDestroySemaphore(m_device, m_imageAvailableSemaphores[i], nullptr);
                    m_imageAvailableSemaphores[i] = VK_NULL_HANDLE;
                }
                if (m_renderFinishedSemaphores[i] != VK_NULL_HANDLE) {
                    vkDestroySemaphore(m_device, m_renderFinishedSemaphores[i], nullptr);
                    m_renderFinishedSemaphores[i] = VK_NULL_HANDLE;
                }
                if (m_inFlightFences[i] != VK_NULL_HANDLE) {
                    vkDestroyFence(m_device, m_inFlightFences[i], nullptr);
                    m_inFlightFences[i] = VK_NULL_HANDLE;
                }
            }
        }

        // 4. Destroy command pool
        if (m_commandPool != VK_NULL_HANDLE && m_device != VK_NULL_HANDLE) {
            vkDestroyCommandPool(m_device, m_commandPool, nullptr);
            m_commandPool = VK_NULL_HANDLE;
        }

        // 5. Destroy render pass
        if (m_renderPass != VK_NULL_HANDLE && m_device != VK_NULL_HANDLE) {
            vkDestroyRenderPass(m_device, m_renderPass, nullptr);
            m_renderPass = VK_NULL_HANDLE;
        }

        // 6. Destroy device
        if (m_device != VK_NULL_HANDLE) {
            vkDestroyDevice(m_device, nullptr);
            m_device = VK_NULL_HANDLE;
        }

        // 7. Destroy surface
        if (m_surface != VK_NULL_HANDLE && m_instance != VK_NULL_HANDLE) {
            vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
            m_surface = VK_NULL_HANDLE;
        }

        // 8. Destroy instance
        if (m_instance != VK_NULL_HANDLE) {
            vkDestroyInstance(m_instance, nullptr);
            m_instance = VK_NULL_HANDLE;
        }
    }

    bool VulkanRenderer::BeginFrame() {
        vkWaitForFences(m_device, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);

        VkResult result = vkAcquireNextImageKHR(
            m_device, m_swapchain, UINT64_MAX,
            m_imageAvailableSemaphores[m_currentFrame],
            VK_NULL_HANDLE, &m_imageIndex
        );

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            RecreateSwapchain();
            return false;
        } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Failed to acquire swapchain image!");
        }

        vkResetFences(m_device, 1, &m_inFlightFences[m_currentFrame]);
        vkResetCommandBuffer(m_commandBuffers[m_currentFrame], 0);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        if (vkBeginCommandBuffer(m_commandBuffers[m_currentFrame], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Failed to begin command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m_renderPass;
        renderPassInfo.framebuffer = m_framebuffers[m_imageIndex];
        renderPassInfo.renderArea.extent = m_swapchainExtent;

        constexpr VkClearValue clearColor = {{{0.2f, 0.3f, 0.4f, 1.0f}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        vkCmdBeginRenderPass(m_commandBuffers[m_currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        return true;
    }

    void VulkanRenderer::EndFrame() {
        vkCmdEndRenderPass(m_commandBuffers[m_currentFrame]);

        if (vkEndCommandBuffer(m_commandBuffers[m_currentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to record command buffer!");
        }

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = {m_imageAvailableSemaphores[m_currentFrame]};
        VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_commandBuffers[m_currentFrame];

        VkSemaphore signalSemaphores[] = {m_renderFinishedSemaphores[m_currentFrame]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFences[m_currentFrame]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to submit command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = {m_swapchain};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &m_imageIndex;

        const VkResult result = vkQueuePresentKHR(m_presentQueue, &presentInfo);

        CheckVkResult(result);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
            RecreateSwapchain();
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to present swapchain image!");
        }

        m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    // Helper method to check for device loss from Vulkan results
    void VulkanRenderer::CheckVkResult(const VkResult result) {
        switch (result) {
            case VK_SUCCESS:
                break;
            case VK_ERROR_DEVICE_LOST:
                m_deviceLost = true;
                // You might want to log this or take additional action
                break;
            case VK_ERROR_OUT_OF_DATE_KHR:
            case VK_SUBOPTIMAL_KHR:
                m_deviceLost = true;
                break;
            default:
                // For other errors, you might want to assert or log
                break;
        }
    }

    void VulkanRenderer::SetVsync(const bool enabled) {
        m_Vsync = enabled;

        // Recreate swapchain with new present mode
        RecreateSwapchain();
    }

    void VulkanRenderer::CleanupSwapchain() {
        // Destroy framebuffers first (before render pass and image views)
        if (m_device != VK_NULL_HANDLE) {
            for (auto framebuffer : m_framebuffers) {
                if (framebuffer != VK_NULL_HANDLE) {
                    vkDestroyFramebuffer(m_device, framebuffer, nullptr);
                }
            }
            m_framebuffers.clear();

            // Destroy image views
            for (auto imageView : m_swapchainImageViews) {
                if (imageView != VK_NULL_HANDLE) {
                    vkDestroyImageView(m_device, imageView, nullptr);
                }
            }
            m_swapchainImageViews.clear();

            // Destroy swapchain
            if (m_swapchain != VK_NULL_HANDLE) {
                vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
                m_swapchain = VK_NULL_HANDLE;
            }
        }
    }

    void VulkanRenderer::RecreateSwapchain() {

        if (!m_initialized) {
            return;
        }

        int width = 0, height = 0;

        SDL_GetWindowSize(m_window, &width, &height);

        while (width == 0 || height == 0) {
            SDL_GetWindowSize(m_window, &width, &height);

            SDL_WaitEvent(nullptr);
        }

        vkDeviceWaitIdle(m_device);

        CleanupSwapchain();

        CreateSwapchain();

        CreateImageViews();

        CreateFramebuffers();
    }

    bool VulkanRenderer::CreateSwapchain() {
        VkSurfaceCapabilitiesKHR capabilities;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice, m_surface, &capabilities);

        m_swapchainExtent = capabilities.currentExtent;

        if (m_swapchainExtent.width == UINT32_MAX) {
            int width, height;
            SDL_GetWindowSize(m_window, &width, &height);
            m_swapchainExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
        }

        VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = m_surface;
        createInfo.minImageCount = capabilities.minImageCount + 1;
        createInfo.imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
        createInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        createInfo.imageExtent = m_swapchainExtent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        createInfo.preTransform = capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = m_Vsync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR;
        createInfo.clipped = VK_TRUE;

        if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapchain) != VK_SUCCESS) {
            return false;
        }

        uint32_t imageCount;

        vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, nullptr);

        m_swapchainImages.resize(imageCount);

        vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, m_swapchainImages.data());

        m_swapchainImageFormat = VK_FORMAT_B8G8R8A8_SRGB;

        return true;
    }

    bool VulkanRenderer::CreateImageViews() {
        m_swapchainImageViews.resize(m_swapchainImages.size());
        for (size_t i = 0; i < m_swapchainImages.size(); i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = m_swapchainImages[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = m_swapchainImageFormat;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(m_device, &createInfo, nullptr, &m_swapchainImageViews[i]) != VK_SUCCESS) {
                return false;
            }
        }
        return true;
    }

    bool VulkanRenderer::CreateRenderPass() {
        VkAttachmentDescription colorAttachment{};
        colorAttachment.format = m_swapchainImageFormat;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;

        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments = &colorAttachment;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;

        if (vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
            return false;
        }
        return true;
    }

    bool VulkanRenderer::CreateFramebuffers() {
        m_framebuffers.resize(m_swapchainImageViews.size());
        for (size_t i = 0; i < m_swapchainImageViews.size(); i++) {
            VkImageView attachments[] = {m_swapchainImageViews[i]};

            VkFramebufferCreateInfo framebufferInfo{};
            framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass = m_renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments = attachments;
            framebufferInfo.width = m_swapchainExtent.width;
            framebufferInfo.height = m_swapchainExtent.height;
            framebufferInfo.layers = 1;

            if (vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &m_framebuffers[i]) != VK_SUCCESS) {
                return false;
            }
        }
        return true;
    }

    bool VulkanRenderer::CreateCommandPool() {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = m_graphicsQueueFamilyIndex;

        if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS) {
            return false;
        }
        return true;
    }

    bool VulkanRenderer::CreateCommandBuffers() {
        m_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = m_commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

        if (vkAllocateCommandBuffers(m_device, &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
            return false;
        }
        return true;
    }

    bool VulkanRenderer::CreateSyncObjects() {
        m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
        m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

        VkSemaphoreCreateInfo semaphoreInfo{};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Start signaled so first frame doesn't wait

        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
                vkCreateFence(m_device, &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS) {
                return false;
                }
        }

        return true;
    }

    bool VulkanRenderer::CreateLogicalDevice() {
        // Queue creation
        float queuePriority = 1.0f;
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = m_graphicsQueueFamilyIndex;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        // Device features
        VkPhysicalDeviceFeatures deviceFeatures{};

        // Device creation
        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.pQueueCreateInfos = &queueCreateInfo;
        createInfo.queueCreateInfoCount = 1;
        createInfo.pEnabledFeatures = &deviceFeatures;

        // Extensions
        const std::vector<const char*> deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };
        createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
        createInfo.ppEnabledExtensionNames = deviceExtensions.data();

        if (vkCreateDevice(m_physicalDevice, &createInfo, nullptr, &m_device) != VK_SUCCESS) {
            return false;
        }

        // Get queues
        vkGetDeviceQueue(m_device, m_graphicsQueueFamilyIndex, 0, &m_graphicsQueue);
        vkGetDeviceQueue(m_device, m_presentQueueFamilyIndex, 0, &m_presentQueue);

        return true;
    }

    bool VulkanRenderer::SelectPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

        if (deviceCount == 0) {
            return false;
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

        for (const auto& device : devices) {
            uint32_t queueFamilyCount = 0;
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

            std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

            // Find graphics and present queue families
            for (uint32_t i = 0; i < queueFamilyCount; i++) {
                VkBool32 presentSupport = false;
                vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_surface, &presentSupport);

                if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && presentSupport) {
                    m_physicalDevice = device;
                    m_graphicsQueueFamilyIndex = i;
                    m_presentQueueFamilyIndex = i;
                    return true;
                }
            }
        }

        return false;
    }

    bool VulkanRenderer::CreateSurface() {
        if (!SDL_Vulkan_CreateSurface(m_window, m_instance, &m_surface)) {
            return false;
        }
        return true;
    }

    bool VulkanRenderer::CreateInstance() {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "REngine";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "REngine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        // Get required extensions from SDL
        uint32_t extensionCount = 0;
        SDL_Vulkan_GetInstanceExtensions(m_window, &extensionCount, nullptr);
        std::vector<const char*> extensions(extensionCount);
        SDL_Vulkan_GetInstanceExtensions(m_window, &extensionCount, extensions.data());

        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        if (vkCreateInstance(&createInfo, nullptr, &m_instance) != VK_SUCCESS) {
            return false;
        }

        return true;
    }

    void VulkanRenderer::HandleDeviceLost() {
        m_deviceLost = true;

        try {
            if (!RecreateVulkanDevice()) {
                throw std::runtime_error("Failed to recover from device loss");
            }
            m_deviceLost = false;
        } catch (...) {

        }
    }

    bool VulkanRenderer::RecreateVulkanDevice() {
        Shutdown();
        return Initialize(m_window); // Reinitialize with existing window
    }

    void VulkanRenderer::InitImGui(SDL_Window* window) {
        VkDescriptorPoolSize pool_sizes[] = {{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },};
        vkCreateDescriptorPool(m_device, &m_poolInfo, nullptr, &m_imguiDescriptorPool);

        // 1. Setup ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();

        // 2. Setup Platform/Renderer backends
        ImGui_ImplSDL2_InitForVulkan(window);
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = m_instance;
        init_info.PhysicalDevice = m_physicalDevice;
        init_info.Device = m_device;
        init_info.QueueFamily = m_graphicsQueueFamilyIndex;
        init_info.Queue = m_graphicsQueue;
        init_info.PipelineCache = VK_NULL_HANDLE;
        init_info.DescriptorPool = m_imguiDescriptorPool; // Create this earlier!
        init_info.MinImageCount = m_swapchainImages.size();
        init_info.ImageCount = m_swapchainImages.size();
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        init_info.RenderPass = m_renderPass;
        ImGui_ImplVulkan_Init(&init_info); // Use your main render pass

        // 3. Create default fonts texture.
        ImGui_ImplVulkan_CreateFontsTexture();

    }

    void VulkanRenderer::RenderImGui() const {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL2_NewFrame(); // Pass SDL_Window*
        ImGui::NewFrame();

        // Draw your debug text
        ImGui::Begin("STATS",0,ImGuiWindowFlags_NoMove);
        ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
        ImGui::End();

        // Render
        ImGui::Render();
        ImDrawData* draw_data = ImGui::GetDrawData();
        ImGui_ImplVulkan_RenderDrawData(draw_data, m_commandBuffers[m_currentFrame]);
    }

    void VulkanRenderer::ProcessImGuiEvents(const SDL_Event* event) const {

        ImGuiIO& io = ImGui::GetIO();

        switch (event->type) {
            case SDL_MOUSEMOTION: {
                io.AddMousePosEvent(
                    event->motion.x,
                    event->motion.y
                );
                break;
            }

            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP: {
                int button = -1;
                if (event->button.button == SDL_BUTTON_LEFT) button = 0;
                if (event->button.button == SDL_BUTTON_RIGHT) button = 1;
                if (event->button.button == SDL_BUTTON_MIDDLE) button = 2;
                if (button != -1)
                    io.AddMouseButtonEvent(button, event->type == SDL_MOUSEBUTTONDOWN);
                break;
            }
            case SDL_MOUSEWHEEL:
                io.AddMouseWheelEvent((float)event->wheel.x, (float)event->wheel.y);
                break;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                // Handle keyboard events
                break;
            case SDL_TEXTINPUT:
                io.AddInputCharactersUTF8(event->text.text);
                break;
            default: break;
        }

    }

    void VulkanRenderer::ShutdownImGui() {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
    }

} // namespace REngine