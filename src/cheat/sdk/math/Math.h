#pragma once

#include <numbers>
#include <cmath>
#include <dwmapi.h>

#define SCREEN_WIDTH GetSystemMetrics(SM_CXSCREEN)
#define SCREEN_HEIGHT GetSystemMetrics(SM_CYSCREEN)

struct ViewMatrixT
{
	float matrix[4][4];

	float* operator[ ](int index) {
		return matrix[index];
	}
};

struct Vec3
{
	float x, y, z;

	constexpr Vec3(
		const float x = 0.f,
		const float y = 0.f,
		const float z = 0.f) noexcept :
		x(x), y(y), z(z) {
	}

	constexpr const Vec3& operator-(const Vec3& other) const noexcept {
		return Vec3{ x - other.x, y - other.y, z - other.z};
	}

	constexpr const Vec3& operator+(const Vec3& other) const noexcept {
		return Vec3{ x + other.x, y + other.y, z + other.z };
	}

	constexpr const Vec3& operator/(const float factor) const noexcept {
		return Vec3{ x / factor, y / factor, z / factor };
	}

	constexpr const Vec3& operator*(const float factor) const noexcept {
		return Vec3{ x * factor, y * factor, z * factor };
	}
};

struct Vec2
{
	float x, y;
	float Magnitude() const {
		return std::sqrt(x * x + y * y);
	}

	Vec2 operator-(const Vec2& other) const
	{
		return Vec2{ x - other.x, y - other.y };
	}

	Vec2 operator+(const Vec2& other) const
	{
		return Vec2{ x + other.x, y + other.y };
	}

	Vec2(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
};


inline bool WorldToScreen(const Vec3& worldPos, Vec2& screenPos, const ViewMatrixT& viewMatrix)
{
	float clip_x = worldPos.x * viewMatrix.matrix[0][0] + worldPos.y * viewMatrix.matrix[0][1] + worldPos.z * viewMatrix.matrix[0][2] + viewMatrix.matrix[0][3];
	float clip_y = worldPos.x * viewMatrix.matrix[1][0] + worldPos.y * viewMatrix.matrix[1][1] + worldPos.z * viewMatrix.matrix[1][2] + viewMatrix.matrix[1][3];
	float clip_z = worldPos.x * viewMatrix.matrix[2][0] + worldPos.y * viewMatrix.matrix[2][1] + worldPos.z * viewMatrix.matrix[2][2] + viewMatrix.matrix[2][3];
	float clip_w = worldPos.x * viewMatrix.matrix[3][0] + worldPos.y * viewMatrix.matrix[3][1] + worldPos.z * viewMatrix.matrix[3][2] + viewMatrix.matrix[3][3];

	if (clip_w < 0.1f)
		return false;

	float ndc_x = clip_x / clip_w;
	float ndc_y = clip_y / clip_w;

	screenPos.x = (SCREEN_WIDTH / 2 * ndc_x) + (ndc_x + SCREEN_WIDTH / 2);
	screenPos.y = -(SCREEN_HEIGHT / 2 * ndc_y) + (ndc_y + SCREEN_HEIGHT / 2);

	return true;
}