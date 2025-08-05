#pragma once

#include <Engine/Utility/Template/SingletonInterface.h>

class Configuration final : SingletonInterface<Configuration> {
	__SINGLETON_INTERFACE(Configuration)

public:
	enum class Language : bool {
		Japanese,
		English,
	};

public:
	static Language GetLanguage();
	static void SetLanguage(Language lang);

private:
	Language language{ Language::Japanese };
};
