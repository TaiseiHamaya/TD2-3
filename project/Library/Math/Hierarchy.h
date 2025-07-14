#pragma once

#include <Library/Utility/Template/Reference.h>

class Affine;
class WorldInstance;

class Hierarchy final {
public:
	Hierarchy(Reference<const Affine> selfAffine_);
	~Hierarchy() = default;

public:// コピー禁止、ムーブ許可
	Hierarchy(const Hierarchy&) = delete;
	Hierarchy& operator=(const Hierarchy&) = delete;
	Hierarchy(Hierarchy&&) = default;
	Hierarchy& operator=(Hierarchy&&) = default;

public:
	void set_parent(const WorldInstance& instance) noexcept;
	void reset_parent() noexcept;
	const Reference<const WorldInstance>& get_parent() const noexcept;
	bool has_parent() const noexcept;
	const Affine& parent_affine() const;
	const Affine& parent_matrix_safe() const noexcept;

private:
	Reference<const WorldInstance> parent;
	Reference<const Affine> selfAffine;
};
