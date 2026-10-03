#pragma once
#include "../pch.h"
#include "../Avatar/Avatar.h"

struct ImGuiContext;

class Overlay {
public:
	static void Init();
	static void Shutdown();
	static void Render();
	static void Toggle() { show_menu = !show_menu; }
	static bool IsVisible() { return show_menu; }

private:
	static inline bool show_menu = false;
	static inline bool show_summary = true;
	static inline bool show_detailed = false;
	static inline bool auto_refresh = true;
	static inline float refresh_interval = 2.0f;
	static inline float last_refresh = 0.0f;
	static inline int selected_player = -1;

	static inline std::vector<AvatarStats> cached_stats;

	static void RefreshStats();
	static void RenderMenuBar();
	static void RenderSummaryTab();
	static void RenderDetailedTab();
	static void RenderPlayerDetail(const AvatarStats& stats);
	static void RenderPerformanceBadge(const std::string& rating);
};
