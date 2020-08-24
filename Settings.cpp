#include "Settings.h"

#include "cocos2d_x/DS_Dictionary.h"

ModuleSettings moduleSettings;

ModuleSettings::ModuleSettings() {
	_settingsLoaded = false;

	_show = _collapsed = false;

	_pos = cocos2d::CCPoint(0, 0);

	_size = cocos2d::CCPoint(0, 0);

	_chatUrl = "WebSocket link";

	this->load();
}

ModuleSettings::~ModuleSettings() {
	this->save();
}

void ModuleSettings::fixSettings() { }

void ModuleSettings::save() {
	DS_Dictionary _dict;
	this->fixSettings();
	_dict.setBoolForKey("show", _show);
	_dict.setBoolForKey("collapsed", _collapsed);
	_dict.setVec2ForKey("pos", _pos);
	_dict.setVec2ForKey("size", _size);
	_dict.setStringForKey("chatUrl", _chatUrl);

	_dict.saveRootSubDictToCompressedFile("GDChat.dat");
}

void ModuleSettings::load() {
	DS_Dictionary _dict;
	_dict.loadRootSubDictFromCompressedFile("GDChat.dat");

	_show = _dict.getBoolForKey("show");
	_collapsed = _dict.getBoolForKey("collapsed");
	_pos = _dict.getVec2ForKey("pos");
	_size = _dict.getVec2ForKey("size");
	_chatUrl = _dict.getStringForKey("chatUrl");
	this->fixSettings();
}