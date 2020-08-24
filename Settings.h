#pragma once

#include "cocos2d_x/CCGeometry.h"
#include <string>

using namespace std;

struct ModuleSettings
{
	bool _settingsLoaded;
	string _chatUrl;
	bool _show, _collapsed;
	cocos2d::CCPoint _pos, _size;
	ModuleSettings();
	~ModuleSettings();

private:
	void fixSettings();
	void save();
	void load();
};

extern ModuleSettings moduleSettings;