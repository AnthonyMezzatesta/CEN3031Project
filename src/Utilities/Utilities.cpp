#include <SFML/Graphics.hpp>
#include <string>
#include "Utilities.h"

// redeclaring static variable
std::string Utilities::fontPath = "../../resources/aovel-sans-rounded-font/AovelSansRounded-rdDL.ttf";

float Utilities::LerpClamped(float a, float b, float t) {
    t = std::clamp(t, 0.f, 1.f);
    return (1 - t) * a + t * b;
}

float Utilities::InvLerpClamped(float a, float b, float v) {
    v = std::clamp(v, a, b);
    return (v - a) / (b - a);
}

float Utilities::RemapClamped(float in1, float in2, float out1, float out2, float v) {
    float t = InvLerpClamped(in1, in2, v);
    return LerpClamped(out1, out2, t);
}