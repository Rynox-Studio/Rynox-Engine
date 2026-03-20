#pragma once

namespace Rynox::Math
{
    constexpr float PI = 3.14159265358979323846f;
    constexpr float TWO_PI = 2.0f * PI;
    constexpr float HALF_PI = PI / 2.0f;
    constexpr float FLOAT_EPS = 1e-6f;
    constexpr float DOUBLE_EPS = 1e-12;

    bool IsZero(float f, float epsilon = FLOAT_EPS);

    bool IsEqual(float a, float b, float epsilon = FLOAT_EPS);

    float Clamp(float value, float min, float max);

    float Min(float a, float b);

    float Max(float a, float b);

    float ToRadians(float degrees);

    float ToDegrees(float radians);

    float Lerp(float a, float b, float t);

    float SmoothStep(float edge0, float edge1, float x);
}