#pragma once

#include <string>     // Required for std::string
#include <functional> // Required for std::function

// Explicitly use std::function<void()> instead of auto
std::function<void()> drawText(std::string text);
std::function<void()> draw_ortho_compass(int rot_x);

void draw_HUD(const std::function<void()>& drawUI);
