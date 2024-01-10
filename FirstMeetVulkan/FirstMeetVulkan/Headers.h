#pragma once
#ifndef HEADERS_H
#define HEADERS_H
#define VK_USE_PLATFORM_WIN32_KHR
#define APP_NAME_STR_LEN 80

/* Vulkan */
#include <vulkan/vulkan.h>

#ifdef AUTO_COMPILE_GLSL_TO_SPV
#include <glslang/SPIRV/GlslangToSpv.h>
#endif // AUTO_COMPILE_GLSL_TO_SPV

/* Standard Library */
#include <vector>
#include <iostream>
#include <cassert>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <iomanip>

/* Platform */
#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32


#endif /* !HEADERS_H */
