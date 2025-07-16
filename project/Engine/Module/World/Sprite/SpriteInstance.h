#pragma once

#include <memory>
#include <optional>
#include <string>

#include <Library/Math/Color4.h>
#include <Library/Math/Matrix4x4.h>
#include <Library/Math/Transform2D.h>
#include <Library/Math/Vector2.h>

class TextureAsset;

struct SpriteMaterial {
	Color4 color;
	std::optional<u32> textureIndex;
	Transform2D uvTransform;

#ifdef DEBUG_FEATURES_ENABLE
	std::shared_ptr<const TextureAsset> texture;
#endif
};

class SpriteInstance {
public:
	SpriteInstance() noexcept(false);
	explicit SpriteInstance(const std::string& textureName, const Vector2& pivot = CVector2::ZERO) noexcept(false);
	~SpriteInstance() noexcept;

	SpriteInstance(SpriteInstance&&) noexcept;
	SpriteInstance& operator=(SpriteInstance&&) noexcept;

private:
	SpriteInstance(const SpriteInstance&) = delete;
	SpriteInstance& operator=(const SpriteInstance&) = delete;

public:
	const Transform2D& get_transform() const noexcept;
	Transform2D& get_transform() noexcept;

	SpriteMaterial& get_material() noexcept;
	const SpriteMaterial& get_material() const noexcept;

	Matrix4x4 create_local_matrix() const noexcept;
	Matrix4x4 create_world_matrix() const noexcept;

	/// <summary>
	/// アクティブフラグの設定
	/// </summary>
	/// <param name="isActive_"></param>
	void set_active(bool isActive_) { isActive = isActive_; };

	/// <summary>
	/// アクティブフラグの取得
	/// </summary>
	/// <returns></returns>
	bool is_active() const { return isActive; };

	//void update();

#ifdef _DEBUG
	void debug_gui();
#endif // _DEBUG

protected:
	bool isActive{ true };

	float priority{ 0.0f };

	Transform2D transform;

	SpriteMaterial material;

	Vector2 pivot{ CVector2::ZERO };
	Vector2 size{ CVector2::ONE };
};
