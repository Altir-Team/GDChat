#include "GDAModule.h"
#include "ws.hpp"
#include "Settings.h"
#include "imgui.h"
#include "imguiex.h"
#include "Util.h"

using easywsclient::WebSocket;
using namespace std;

static char message[2048];
static int online = 0;
static vector<string> messages;
static bool _connection = false;
static WebSocket* ws = nullptr;

void pingServer() {
	if (ws && ws->getReadyState() == WebSocket::OPEN) {
		ws->sendPing();
	}
}

CALLBACK_F(void, handle_message, const string& msg) {
	string str = msg.c_str();
	if (str.find("online::") == 0) {
		str.erase(0, 8);
		online = std::stoi(str);
		return;
	}
	else if (str == "clearClient") {
		messages.clear();
		return;
	}
	messages.push_back(msg);
}

void connectToChat() {
	if (_connection) {
		if (!ws || ws->getReadyState() != WebSocket::OPEN) {
			messages.push_back("[GDChat] Connecting..");
			ws = WebSocket::from_url(moduleSettings._chatUrl);
		}
		else if (ws->getReadyState() != WebSocket::CLOSED) {
			ws->poll();
			ws->dispatch(handle_message);
		}
	}
}

CALLBACK_F(void, update_ws) {
	Util::timer_start(pingServer, 20000);
	_connection = true;
	connectToChat();
	Util::timer_start(connectToChat, 5000);
}

CALLBACK_F(void, OnModuleDraw, GDA_MODULE* pModule) {
	if (moduleSettings._show) {
		if (!moduleSettings._settingsLoaded) {
			moduleSettings._settingsLoaded = true;
			ImGui::SetNextWindowCollapsed(moduleSettings._collapsed, ImGuiCond_Once);
			ImGui::SetNextWindowPos(ImVec2(moduleSettings._pos.x, moduleSettings._pos.y), ImGuiCond_Once);
			ImGui::SetNextWindowSize(ImVec2(moduleSettings._size.x, moduleSettings._size.y), ImGuiCond_Once);
		}
		ImGui::SetNextWindowSizeConstraints(ImVec2(400, 300), ImVec2(400, 600));

		if (ImGuiEx::BeginWithAStyle(moduleSettings._collapsed, pModule->getName(), &moduleSettings._show)) {
			ImGui::SetNextWindowContentWidth(moduleSettings._size.x / 0.9);
			ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
			if (ImGui::BeginChild("chat", ImVec2(0, moduleSettings._size.y - 60), true, ImGuiWindowFlags_None)) {
				if (messages.size() == 0)
					ImGui::TextDisabled("Chat is empty");
				else for (int i = 0; i < messages.size(); ++i) {
					ImGuiEx::TextMulticolor(messages[i].c_str());
					if (i != messages.size()) ImGui::Separator();
				}
			}
			ImGui::EndChild();
			ImGui::InputText("##inputchat", message, 2048);
			ImGui::SameLine();
			if (ImGui::Button("     >     ") || ImGuiEx::IsKeysReleased(VK_RETURN)) {
				if (ws != nullptr && ws->getReadyState() == WebSocket::OPEN) {
					if (strlen(message) > 0) {
						ws->send(message);
						memset(message, 0, 2048);
					}
				}
			}
			ImGui::SameLine();
			if (ws != nullptr && ws->getReadyState() == WebSocket::OPEN)
				ImGuiEx::TextMulticolor("{32a852}Online: %d{}", online);
			else
				ImGuiEx::TextMulticolor("{cc0000}Offline{}");
		}

		moduleSettings._collapsed = ImGui::IsWindowCollapsed();
		auto& pos = ImGui::GetWindowPos();
		moduleSettings._pos.x = pos.x;
		moduleSettings._pos.y = pos.y;
		auto& size = ImGui::GetWindowSize();
		moduleSettings._size.x = size.x;
		moduleSettings._size.y = size.y;

		ImGui::End();
	}
}

GDA_MODULE_CALLBACK(GDA_MODULE* pModule) {
	memset(message, 0, 2048);
	thread(&update_ws).detach();

	pModule->setName("GDChat");
	pModule->setAuthor("bit0r1n");
	pModule->registerClickCallback(CALLBACK_L(void, GDA_MODULE * pModule) {
		if (moduleSettings._show ^= true)
			ImGui::SetNextWindowFocus();
	});
	pModule->registerDrawCallback(OnModuleDraw);
	pModule->setClickName("  open  ");
	return true;
}