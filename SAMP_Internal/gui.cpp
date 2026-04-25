#include "gui.h"
#include <algorithm>

// تعريف القائمة التي سنستخدمها (تأكد أنها نفس الموجودة في aimbot.cpp)
namespace GlobalFriends {
	extern std::vector<int> list;
	extern bool ignoreSameColor;
}

tWndProc oWndProc;

LRESULT CALLBACK hWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (G::menuVisible) {
		ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);
		return 1;
	}
	else {
		return CallWindowProc(oWndProc, hwnd, uMsg, wParam, lParam);
	}
}

void GUI::init() {
	D3DDEVICE_CREATION_PARAMETERS cp;
	G::devicePtr->GetCreationParameters(&cp);
	hWindow = cp.hFocusWindow;

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplWin32_Init(hWindow);
	ImGui_ImplDX9_Init(G::devicePtr);
	oWndProc = (WNDPROC)SetWindowLongPtr(hWindow, GWL_WNDPROC, (LONG_PTR)hWndProc);

	// الستايل الخاص بك (اللون الأخضر)
// --- [ WTLS 2 Style: Orange & White & Dark Grey ] ---
	ImGuiStyle& style = ImGui::GetStyle();

	style.WindowRounding = 6.0f;
	style.FrameRounding = 4.0f;
	style.WindowBorderSize = 1.0f;

	// نصوص بيضاء واضحة
	ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));

	// خلفية النافذة (رمادي غامق جداً قريب للأسود)
	ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32(15, 15, 15, 245));

	// شريط العنوان (برتقالي WTLS)
	ImGui::PushStyleColor(ImGuiCol_TitleBgActive, IM_COL32(255, 120, 0, 255));
	ImGui::PushStyleColor(ImGuiCol_TitleBg, IM_COL32(180, 80, 0, 255));

	// خانات الإدخال والخلفيات الداخلية
	ImGui::PushStyleColor(ImGuiCol_FrameBg, IM_COL32(35, 35, 35, 255));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, IM_COL32(50, 50, 50, 255));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, IM_COL32(255, 120, 0, 50));

	// الأزرار
	ImGui::PushStyleColor(ImGuiCol_Button, IM_COL32(45, 45, 45, 255));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, IM_COL32(255, 120, 0, 200));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, IM_COL32(255, 150, 20, 255));

	// التبويبات (Tabs) - لتظهر بشكل عصري
	ImGui::PushStyleColor(ImGuiCol_Tab, IM_COL32(25, 25, 25, 255));
	ImGui::PushStyleColor(ImGuiCol_TabHovered, IM_COL32(255, 120, 0, 180));
	ImGui::PushStyleColor(ImGuiCol_TabActive, IM_COL32(255, 120, 0, 255));
	ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, IM_COL32(200, 90, 0, 255));

	// علامة الصح والسلايدر
	ImGui::PushStyleColor(ImGuiCol_CheckMark, IM_COL32(255, 120, 0, 255));
	ImGui::PushStyleColor(ImGuiCol_SliderGrab, IM_COL32(255, 120, 0, 255));
	ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, IM_COL32(255, 160, 50, 255));

	// القوائم المنسدلة والهيدر
	ImGui::PushStyleColor(ImGuiCol_Header, IM_COL32(255, 120, 0, 80));
	ImGui::PushStyleColor(ImGuiCol_HeaderHovered, IM_COL32(255, 120, 0, 150));
	ImGui::PushStyleColor(ImGuiCol_HeaderActive, IM_COL32(255, 120, 0, 255));

	// الحدود والزوايا
	ImGui::PushStyleColor(ImGuiCol_Border, IM_COL32(60, 60, 60, 255));
	ImGui::PushStyleColor(ImGuiCol_Separator, IM_COL32(50, 50, 50, 255));

	G::guiInitialized = true;
}

std::vector<std::string> options;
char buf[200];
unsigned int i = 0;

void GUI::frame() {
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("My private rapist wtls 2  -  Mr Tomito");

	ImGui::BeginTabBar("##mainTabs", ImGuiTabBarFlags_Reorderable);

	// --- Tab: Aimbot ---
	if (ImGui::BeginTabItem("Aimbot")) {
		ImGui::Checkbox("Enabled", &G::config.aimbot.enabled);

		ImGui::Separator();
		ImGui::Text("Aimbot key");

		// --- [ إضافة الزر الأيمن للماوس هنا ] ---
		// نستخدم متغير مؤقت لضمان تحديث الواجهة فوراً (Sync)
		int currentKey = G::config.aimbot.aimKey;

		if (ImGui::RadioButton("Right Click", currentKey == VK_RBUTTON)) {
			G::config.aimbot.aimKey = VK_RBUTTON;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Mouse 4", currentKey == VK_XBUTTON1)) {
			G::config.aimbot.aimKey = VK_XBUTTON1;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Mouse 5", currentKey == VK_XBUTTON2)) {
			G::config.aimbot.aimKey = VK_XBUTTON2;
		}
		ImGui::SameLine();
		if (ImGui::RadioButton("Left Alt", currentKey == VK_LMENU)) {
			G::config.aimbot.aimKey = VK_LMENU;
		}

		ImGui::Separator();

		ImGui::SliderFloat("Virtual height modifier", &G::config.aimbot.heightChange, -1.0f, 1.0f, "%+.3f");
		ImGui::SliderFloat("FOV", &G::config.aimbot.fov, 0.0f, 180.0f, "%.1f");
		ImGui::SliderFloat("Max Range", &G::config.aimbot.maxRange, 10.0f, 250.0f, "%.0f m");

		ImGui::Checkbox("Prediction", &G::config.aimbot.prediction);
		if (G::config.aimbot.prediction) {
			ImGui::SliderInt("Prediction level", (int*)&G::config.aimbot.predictionLvl, 0, 10);
		}

		ImGui::Checkbox("Smooth aim", &G::config.aimbot.smoothAim);
		if (G::config.aimbot.smoothAim) {
			// تنبيه: تأكد أن smoothSpeed في ملف aimbot.cpp لا تُطرح من 101 لكي يعمل السلايدر بشكل منطقي
			ImGui::SliderFloat("Smooth aim speed", &G::config.aimbot.smoothSpeed, 1.0f, 100.0f, "%.1f");
		}

		ImGui::EndTabItem();
	}

	// --- Tab: Friends (التبويب الجديد) ---
	if (ImGui::BeginTabItem("Friends")) {
		ImGui::Separator();

		static int idInput = 0;
		ImGui::InputInt("Player ID", &idInput);

		if (ImGui::Button("Add Friend ID", ImVec2(120, 0))) {
			// إضافة إذا لم يكن موجوداً مسبقاً
			if (std::find(GlobalFriends::list.begin(), GlobalFriends::list.end(), idInput) == GlobalFriends::list.end()) {
				GlobalFriends::list.push_back(idInput);
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Clear All List", ImVec2(120, 0))) {
			GlobalFriends::list.clear();
		}

		ImGui::Text("Current Friends List:");
		ImGui::BeginChild("FriendsListChild", ImVec2(0, 150), true);
		for (size_t n = 0; n < GlobalFriends::list.size(); n++) {
			ImGui::Text("ID: %d", GlobalFriends::list[n]);
			ImGui::SameLine(ImGui::GetWindowWidth() - 50);
			char label[32];
			sprintf(label, "X##%d", (int)n);
			if (ImGui::SmallButton(label)) {
				GlobalFriends::list.erase(GlobalFriends::list.begin() + n);
			}
		}
		ImGui::EndChild();

		ImGui::EndTabItem();
	}

	// --- Tab: ESP ---
	if (ImGui::BeginTabItem("ESP")) {
		ImGui::Checkbox("ESP box", &G::config.esp.ESPBox);
		ImGui::SameLine();
		if (ImGui::ColorEdit4("##ESPBox", G::config.esp.ESPBoxColor, ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
			G::config.esp.calcColors.ESPBoxColor = FLOATARR_2_D3DCOLOR(G::config.esp.ESPBoxColor);
		}
		// ... بقية كود الـ ESP ...
		ImGui::Checkbox("Check teammate", &G::config.esp.mateBox);
		ImGui::SameLine();
		if (ImGui::ColorEdit4("##mateBox", G::config.esp.mateBoxColor, ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
			G::config.esp.calcColors.mateBoxColor = FLOATARR_2_D3DCOLOR(G::config.esp.mateBoxColor);
		}

		ImGui::Checkbox("Snap lines", &G::config.esp.snapLines);
		ImGui::SameLine();
		if (ImGui::ColorEdit4("##snapLines", G::config.esp.snapLinesColor, ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
			G::config.esp.calcColors.snapLinesColor = FLOATARR_2_D3DCOLOR(G::config.esp.snapLinesColor);
		}

		ImGui::Checkbox("Health ESP", &G::config.esp.healthESP);
		ImGui::SameLine();
		if (ImGui::ColorEdit4("##healthESP", G::config.esp.healthESPColor, ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
			G::config.esp.calcColors.healthESPColor = FLOATARR_2_D3DCOLOR(G::config.esp.healthESPColor);
		}

		ImGui::Checkbox("Armor ESP", &G::config.esp.armorESP);
		ImGui::SameLine();
		if (ImGui::ColorEdit4("##armorESP", G::config.esp.armorESPColor, ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
			G::config.esp.calcColors.armorESPColor = FLOATARR_2_D3DCOLOR(G::config.esp.armorESPColor);
		}

		ImGui::Checkbox("Name ESP", &G::config.esp.nameESP);
		ImGui::SameLine();
		if (ImGui::ColorEdit4("##nameESP", G::config.esp.nameESPColor, ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
			G::config.esp.calcColors.nameESPColor = FLOATARR_2_D3DCOLOR(G::config.esp.nameESPColor);
		}

		ImGui::Checkbox("Weapon ESP", &G::config.esp.weaponESP);
		ImGui::SameLine();
		if (ImGui::ColorEdit4("##weaponESP", G::config.esp.weaponESPColor, ImGuiColorEditFlags_InputRGB | ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_AlphaBar)) {
			G::config.esp.calcColors.weaponESPColor = FLOATARR_2_D3DCOLOR(G::config.esp.weaponESPColor);
		}
		ImGui::EndTabItem();
	}

	// --- Tab: Misc & Config ---
	if (ImGui::BeginTabItem("Misc")) {
		ImGui::Checkbox("Freecam", &G::config.misc.freecam);
		ImGui::SliderFloat("Freecam speed", &G::config.misc.freecamSpeed, 0, 6);
		ImGui::Checkbox("Auto turn off radio", &G::config.misc.autoOffRadio);
		ImGui::Checkbox("Auto hold horn", &G::config.misc.holdHorn);
		ImGui::EndTabItem();
	}

	if (ImGui::BeginTabItem("Config")) {
		ImGui::InputText("##cfgname", buf, 200);
		ImGui::SameLine();
		if (ImGui::Button("Create")) {
			CONFIG::save(std::string(buf));
		}

		options = CONFIG::list();
		if (options.size()) {
			for (i = 0; i < options.size(); ++i) {
				ImGui::Text("%s", options[i].c_str());
				ImGui::SameLine();
				if (ImGui::Button("Save")) { CONFIG::save(options[i]); }
				ImGui::SameLine();
				if (ImGui::Button("Load")) { CONFIG::load(options[i]); }
				ImGui::SameLine();
				if (ImGui::Button("Delete")) { CONFIG::remove(options[i]); }
			}
		}
		ImGui::EndTabItem();
	}

	ImGui::EndTabBar();

	ImGui::End();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void GUI::shutdown() {
	SetWindowLongPtr(hWindow, GWL_WNDPROC, (LONG_PTR)oWndProc);
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}