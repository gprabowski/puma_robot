#pragma once

#include <imgui.h>

#include <scene.h>

namespace update {
void setup_globals(const ImVec2 &s);
void refresh_ubos();
void per_frame_update(puma::scene &s);
} // namespace update
