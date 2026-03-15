#include "Math/Utility.h"

#include <cmath>

namespace Rynox::Math
{
    bool IsZero(float f, float epsilon)
    {
        return std::abs(f) < epsilon;
    }

    bool IsEqual(float a, float b, float epsilon)
    {
        return std::abs(a - b) < epsilon;
    }

    float Clamp(float value, float min, float max)
    {
        if (value < min) return min;
        if (value > max) return max;
        return value;
    }

    float Min(float a, float b) { return a < b ? a : b; }

    float Max(float a, float b) { return a > b ? a : b; }

    float NormalizeAngle(float angle)
    {
        angle = std::fmod(angle, TWO_PI);
        if (angle < 0) angle += TWO_PI;
        return angle;
    }

    float NormalizeAngleSymmetrical(float angle)
    {
        angle = std::fmod(angle, TWO_PI);
        if (angle > PI) angle -= TWO_PI;
        if (angle < -PI) angle += TWO_PI;
        return angle;
    }

    float ToRadians(float degrees)
    {
        return degrees * PI / 180.0f;
    }

    float ToDegrees(float radians)
    {
        return radians * 180.0f / PI;
    }

    float Lerp(float a, float b, float t)
    {
        return a + (b - a) * t;
    }

    float SmoothStep(float edge0, float edge1, float x)
    {
        float t = Clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    }
}