#pragma once

namespace Rynox::Math
{
    // ================ CONSTANTS ================

    constexpr float PI = 3.14159265358979323846f;
    constexpr float TWO_PI = 2.0f * PI;
    constexpr float HALF_PI = PI / 2.0f;
    constexpr float EPSILON = 1e-6f;

    // ================ CHECKS ================

    // Checks if a value is approximately zero (with tolerance)
    bool IsZero(float f, float epsilon = EPSILON);

    // Checks if two values are approximately equal (with tolerance)
    bool IsEqual(float a, float b, float epsilon = EPSILON);

    // ================ CLAMPING ================

    // Clamps a value between min and max
    float Clamp(float value, float min, float max);

    float Min(float a, float b);

    float Max(float a, float b);

    // ================ ANGLE UTILITIES ================

    // Normalizes an angle in radians to the range [0, 2PI]
    float NormalizeAngle(float angle);

    // Normalizes an angle in radians to the range [-PI, PI]
    float NormalizeAngleSymmetrical(float angle);

    // Converts degrees to radians
    float ToRadians(float degrees);

    // Converts radians to degrees
    float ToDegrees(float radians);

    // ================ INTERPOLATION ================

    // Linear interpolation for float values
    float Lerp(float a, float b, float t);

    // Smooth step interpolation (ease-in, ease-out)
    float SmoothStep(float edge0, float edge1, float x);
}