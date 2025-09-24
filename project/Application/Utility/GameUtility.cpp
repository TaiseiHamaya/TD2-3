#include "GameUtility.h"
#include <cmath>
#include <Library/Math/Basis.h>
#include <numbers>
#include <algorithm>


bool GameUtility::approximately_equal(const Vector3& a, const Vector3& b, float epsilon)
{
    return std::fabs(a.x - b.x) < epsilon &&
        std::fabs(a.y - b.y) < epsilon &&
        std::fabs(a.z - b.z) < epsilon;
}

Vector3 GameUtility::rotate_direction_90_left(const Vector3& direction)
{
    return { -direction.z, direction.y, direction.x }; // 左回転
}

Vector3 GameUtility::rotate_direction_90_right(const Vector3& direction)
{
    return { direction.z, direction.y, -direction.x }; // 右回転
}

bool GameUtility::IsRotation90Degrees(const Quaternion& q1, const Quaternion& q2) {
    constexpr float DEGREE_TOLERANCE = 5.0f;
    constexpr float RAD_TO_DEG = 180.0f / static_cast<float>(std::numbers::pi);

    // 相対回転
    Quaternion delta = q2 * q1.inverse();
    float w = std::clamp(std::fabs(delta.real()), -1.0f, 1.0f);

    float angleDeg = 2.0f * std::acos(w) * RAD_TO_DEG;

    return std::fabs(angleDeg - 90.0f) <= DEGREE_TOLERANCE;
}

bool GameUtility::IsRotation180Degrees(const Quaternion& q1, const Quaternion& q2)
{
    constexpr float DEGREE_TOLERANCE = 5.0f;
    constexpr float RAD_TO_DEG = 180.0f / static_cast<float>(std::numbers::pi);

    // 相対回転
    Quaternion delta = q2 * q1.inverse();
    float w = std::clamp(std::fabs(delta.real()), -1.0f, 1.0f);

    float angleDeg = 2.0f * std::acos(w) * RAD_TO_DEG;

    return std::fabs(angleDeg - 180.0f) <= DEGREE_TOLERANCE;
}
