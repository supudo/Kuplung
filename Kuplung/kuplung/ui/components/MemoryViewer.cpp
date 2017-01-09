//
//  MemoryViewer.hpp
//  Kuplung
//
//  Created by Sergey Petrov on 11/17/15.
//  Copyright © 2015 supudo.net. All rights reserved.
//

#include "MemoryViewer.hpp"
#include "kuplung/settings/Settings.h"

#define LMT_CAPTURE_ACTIVATED 1
#define LMT_INSTANCE_COUNT_ACTIVATED 1
#define LMT_x64

#define LMT_ALLOC_NUMBER_PER_CHUNK 512
#define LMT_STACK_SIZE_PER_ALLOC 50
#define LMT_CHUNK_NUMBER_PER_THREAD 6
#define LMT_CACHE_SIZE 8
#define LMT_ALLOC_DICTIONARY_SIZE 1024 * 16
#define LMT_STACK_DICTIONARY_SIZE 1024 * 16
#define LMT_TREE_DICTIONARY_SIZE 1024 * 32
#define LMT_IMGUI 1
#define LMT_PLATFORM_WINDOWS 1
#define LMT_IMGUI_INCLUDE_PATH "kuplung/utilities/imgui/imgui.h"
#define LMT_USE_MALLOC ::malloc
#define LMT_USE_REALLOC ::realloc
#define LMT_USE_FREE ::free
#define LMT_STATS 1
#define LMT_DEBUG_DEV 1

#define LMT_TREAT_CHUNK(chunk) \
SCOPE_profile_cpu_i("Alloc", "Alloc chunk"); \
TMQ::TaskManager::emplaceSharedTask<AGE::Tasks::Basic::VoidFunction>([=](){LiveMemTracer::treatChunk(chunk);}); \

#define LMT_ENABLED 1
#define LMT_IMPL 1
#include "LiveMemTracer.hpp"

inline void *myMalloc(size_t size) {
    return LMT_ALLOC(size);
}

inline void myFree(void *ptr) {
    LMT_DEALLOC(ptr);
}

void *myRealloc(void *ptr, size_t size) {
    return LMT_REALLOC(ptr, size);
}

void* operator new(size_t count) throw(std::bad_alloc) {
    return LMT_ALLOC(count);
}

void* operator new(size_t count, const std::nothrow_t&) throw() {
    return LMT_ALLOC(count);
}

void* operator new(size_t count, size_t alignment) throw(std::bad_alloc) {
    return LMT_ALLOC_ALIGNED(count, alignment);
}

void* operator new(size_t count, size_t alignment, const std::nothrow_t&) throw() {
    return LMT_ALLOC_ALIGNED(count, alignment);
}

void* operator new[](size_t count) throw(std::bad_alloc) {
    return LMT_ALLOC(count);
}

void* operator new[](size_t count, const std::nothrow_t&) throw() {
    return LMT_ALLOC(count);
}

void* operator new[](size_t count, size_t alignment) throw(std::bad_alloc) {
    return LMT_ALLOC_ALIGNED(count, alignment);
}

void* operator new[](size_t count, size_t alignment, const std::nothrow_t&) throw() {
    return LMT_ALLOC_ALIGNED(count, alignment);
}

void operator delete(void* ptr) throw() {
    return LMT_DEALLOC(ptr);
}

void operator delete(void *ptr, const std::nothrow_t&) throw() {
    return LMT_DEALLOC(ptr);
}

void operator delete(void *ptr, size_t alignment) throw()
{
    return LMT_DEALLOC_ALIGNED(ptr);
}

void operator delete(void *ptr, size_t alignment, const std::nothrow_t&) throw() {
    return LMT_DEALLOC_ALIGNED(ptr);
}

void operator delete[](void* ptr) throw() {
    return LMT_DEALLOC(ptr);
}

void operator delete[](void *ptr, const std::nothrow_t&) throw() {
    return LMT_DEALLOC(ptr);
}

void operator delete[](void *ptr, size_t alignment) throw() {
    return LMT_DEALLOC_ALIGNED(ptr);
}

void operator delete[](void *ptr, size_t alignment, const std::nothrow_t&) throw() {
    return LMT_DEALLOC_ALIGNED(ptr);
}

void MemoryViewer::init(int positionX, int positionY, int width, int height) {
    this->positionX = positionX;
    this->positionY = positionY;
    this->width = width;
    this->height = height;
    LMT_INIT();
}

void MemoryViewer::draw(const char* title, bool* p_opened) {
    if (this->width > 0 && this->height > 0)
        ImGui::SetNextWindowSize(ImVec2(this->width, this->height), ImGuiSetCond_FirstUseEver);
    else
        ImGui::SetNextWindowSize(ImVec2(Settings::Instance()->frameLog_Width, Settings::Instance()->frameLog_Height), ImGuiSetCond_FirstUseEver);

    if (this->positionX > 0 && this->positionY > 0)
        ImGui::SetNextWindowPos(ImVec2(this->positionX, this->positionY), ImGuiSetCond_FirstUseEver);

//    ImGui::Begin(title, p_opened, ImGuiWindowFlags_ShowBorders);
//    ImGui::End();

    LMT_DISPLAY(ImGui::GetIO().DeltaTime * 1000.0f);
}
