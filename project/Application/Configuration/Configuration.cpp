#include "Configuration.h"

Configuration::Language Configuration::GetLanguage() {
	return GetInstance().language;
}

void Configuration::SetLanguage(Language lang) {
	GetInstance().language = lang;
}
