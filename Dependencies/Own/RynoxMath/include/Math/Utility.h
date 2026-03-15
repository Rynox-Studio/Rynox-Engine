#pragma once

namespace Rynox::Math
{
    // ================ CONSTANTS ================

    inline constexpr float PI = 3.14159265358979323846f;
    inline constexpr float TWO_PI = 2.0f * PI;
    inline constexpr float HALF_PI = PI / 2.0f;
    inline constexpr float EPSILON = 1e-6f;

    // ================ CHECKS ================

    // Checks if a value is approximately zero (with tolerance)
    inline bool IsZero(float f, float epsilon = EPSILON);

    // Checks if two values are approximately equal (with tolerance)
    inline bool IsEqual(float a, float b, float epsilon = EPSILON);

    // ================ CLAMPING ================

    // Clamps a value between min and max
    inline float Clamp(float value, float min, float max);

    inline float Min(float a, float b);

    inline float Max(float a, float b);

    // ================ ANGLE UTILITIES ================

    // Normalizes an angle in radians to the range [0, 2PI]
    inline float NormalizeAngle(float angle);

    // Normalizes an angle in radians to the range [-PI, PI]
    inline float NormalizeAngleSymmetrical(float angle);

    // Converts degrees to radians
    inline float ToRadians(float degrees);

    // Converts radians to degrees
    inline float ToDegrees(float radians);

    // ================ INTERPOLATION ================

    // Linear interpolation for float values
    inline float Lerp(float a, float b, float t);

    // Smooth step interpolation (ease-in, ease-out)
    inline float SmoothStep(float edge0, float edge1, float x);
}