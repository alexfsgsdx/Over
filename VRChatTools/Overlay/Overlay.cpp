#include "../pch.h"
#include "Overlay.h"

// ImGui headers — user must add imgui to their project
// #include <imgui.h>

// Stub ImGui wrappers so this compiles without imgui present.
// Replace with real imgui calls once you add imgui to the project.
namespace ImGui {
	inline bool Begin(const char* name, bool* open = nullptr, int flags = 0) { return false; }
	inline void End() {}
	inline bool BeginTabBar(const char* id, int flags = 0) { return false; }
	inline bool BeginTabItem(const char* label) { return false; }
	inline void EndTabItem() {}
	inline void EndTabBar() {}
	inline void Text(const char* fmt, ...) {}
	inline void TextColored(float r, float g, float b, float a, const char* fmt, ...) {}
	inline void Separator() {}
	inline void SameLine(float x = 0, float spacing = -1) {}
	inline void Spacing() {}
	inline bool Button(const char* label, float w = 0, float h = 0) { return false; }
	inline bool Checkbox(const char* label, bool* v) { return false; }
	inline bool SliderFloat(const char* label, float* v, float min, float max) { return false; }
	inline void Columns(int count = 1, const char* id = nullptr, bool border = true) {}
	inline void NextColumn() {}
	inline void SetColumnWidth(int idx, float width) {}
	inline bool Selectable(const char* label, bool selected = false) { return false; }
	inline void ProgressBar(float fraction, float w = -1, float h = 0, const char* overlay = nullptr) {}
	inline bool CollapsingHeader(const char* label, int flags = 0) { return false; }
	inline float GetTime() { return 0.0f; }
}

void Overlay::Init() {
	show_menu = false;
	cached_stats.clear();
}

void Overlay::Shutdown() {
	cached_stats.clear();
}

void Overlay::RefreshStats() {
	float now = ImGui::GetTime();
	if (auto_refresh && (now - last_refresh) >= refresh_interval) {
		cached_stats = AvatarAnalyzer::ScanAllAvatars();
		last_refresh = now;
	}
}

void Overlay::Render() {
	if (!show_menu) return;

	RefreshStats();

	if (!ImGui::Begin("VRChatTools - Avatar Stats", &show_menu, 0)) {
		ImGui::End();
		return;
	}

	RenderMenuBar();

	if (ImGui::BeginTabBar("MainTabs")) {
		if (ImGui::BeginTabItem("Summary")) {
			show_summary = true;
			show_detailed = false;
			RenderSummaryTab();
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Detailed")) {
			show_summary = false;
			show_detailed = true;
			RenderDetailedTab();
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}

	ImGui::End();
}

void Overlay::RenderMenuBar() {
	ImGui::Checkbox("Auto Refresh", &auto_refresh);
	ImGui::SameLine();
	ImGui::SliderFloat("Interval", &refresh_interval, 0.5f, 10.0f);
	ImGui::SameLine();
	if (ImGui::Button("Refresh Now")) {
		cached_stats = AvatarAnalyzer::ScanAllAvatars();
		last_refresh = ImGui::GetTime();
	}
	ImGui::Separator();
}

void Overlay::RenderSummaryTab() {
	ImGui::Text("Players in Instance: %d", (int)cached_stats.size());
	ImGui::Spacing();

	ImGui::Columns(7, "PlayerColumns", true);
	ImGui::SetColumnWidth(0, 150.0f);
	ImGui::SetColumnWidth(1, 90.0f);
	ImGui::SetColumnWidth(2, 80.0f);
	ImGui::SetColumnWidth(3, 80.0f);
	ImGui::SetColumnWidth(4, 60.0f);
	ImGui::SetColumnWidth(5, 60.0f);
	ImGui::SetColumnWidth(6, 80.0f);

	ImGui::Text("Player"); ImGui::NextColumn();
	ImGui::Text("Trust"); ImGui::NextColumn();
	ImGui::Text("Triangles"); ImGui::NextColumn();
	ImGui::Text("Tex Mem"); ImGui::NextColumn();
	ImGui::Text("Lights"); ImGui::NextColumn();
	ImGui::Text("Bones"); ImGui::NextColumn();
	ImGui::Text("Rating"); ImGui::NextColumn();
	ImGui::Separator();

	for (int i = 0; i < (int)cached_stats.size(); i++) {
		const auto& s = cached_stats[i];

		bool is_selected = (selected_player == i);
		if (ImGui::Selectable(s.player.display_name.c_str(), is_selected)) {
			selected_player = i;
		}
		ImGui::NextColumn();

		ImGui::Text("%s", s.player.trust_rank.c_str()); ImGui::NextColumn();
		ImGui::Text("%u", s.total_triangles); ImGui::NextColumn();

		float tex_mb = s.total_texture_memory / (1024.0f * 1024.0f);
		ImGui::Text("%.1f MB", tex_mb); ImGui::NextColumn();

		ImGui::Text("%d", (int)s.lights.size()); ImGui::NextColumn();
		ImGui::Text("%u", s.total_bones); ImGui::NextColumn();

		RenderPerformanceBadge(s.metrics.performance_rating);
		ImGui::NextColumn();
	}

	ImGui::Columns(1);

	if (selected_player >= 0 && selected_player < (int)cached_stats.size()) {
		ImGui::Spacing();
		ImGui::Separator();
		RenderPlayerDetail(cached_stats[selected_player]);
	}
}

void Overlay::RenderDetailedTab() {
	for (int i = 0; i < (int)cached_stats.size(); i++) {
		const auto& s = cached_stats[i];
		std::string header = s.player.display_name;
		if (s.player.is_local) header += " (YOU)";
		header += " [" + s.metrics.performance_rating + "]";

		if (ImGui::CollapsingHeader(header.c_str())) {
			RenderPlayerDetail(s);
		}
	}
}

void Overlay::RenderPlayerDetail(const AvatarStats& stats) {
	ImGui::Text("Avatar: %s", stats.avatar_name.c_str());
	ImGui::Text("Creator: %s", stats.creator.c_str());
	ImGui::Text("Avatar ID: %s", stats.avatar_id.c_str());
	ImGui::Spacing();

	// Mesh info
	if (ImGui::CollapsingHeader("Meshes")) {
		ImGui::Text("Total Vertices: %u", stats.total_vertices);
		ImGui::Text("Total Triangles: %u", stats.total_triangles);
		for (const auto& mesh : stats.meshes) {
			ImGui::Text("  %s: %u verts, %u tris, %u mats",
				mesh.name.c_str(), mesh.vertex_count,
				mesh.triangle_count, mesh.material_count);
		}
	}

	// Textures
	if (ImGui::CollapsingHeader("Textures")) {
		float total_mb = stats.total_texture_memory / (1024.0f * 1024.0f);
		ImGui::Text("Total Texture Memory: %.2f MB", total_mb);

		float budget_fraction = total_mb / 40.0f;
		if (budget_fraction > 1.0f) budget_fraction = 1.0f;
		ImGui::ProgressBar(budget_fraction, -1, 0, "Tex Budget");

		for (const auto& tex : stats.textures) {
			ImGui::Text("  %s: %ux%u (%s) %.1f KB",
				tex.name.c_str(), tex.width, tex.height,
				tex.format.c_str(), tex.memory_size / 1024.0f);
		}
	}

	// Lights
	if (ImGui::CollapsingHeader("Lights")) {
		ImGui::Text("Total Lights: %d", (int)stats.lights.size());
		for (const auto& light : stats.lights) {
			ImGui::Text("  %s (%s): intensity=%.1f range=%.1f",
				light.name.c_str(), light.type.c_str(),
				light.intensity, light.range);
		}
	}

	// Materials
	if (ImGui::CollapsingHeader("Materials")) {
		ImGui::Text("Total Materials: %d", (int)stats.materials.size());
		for (const auto& mat : stats.materials) {
			ImGui::Text("  %s (Shader: %s) %u textures",
				mat.name.c_str(), mat.shader.c_str(), mat.texture_count);
		}
	}

	// Bones
	if (ImGui::CollapsingHeader("Bones & Physics")) {
		ImGui::Text("Total Bones: %u", stats.total_bones);
		ImGui::Text("Dynamic Bones: %s (%u)",
			stats.assets.has_dynamic_bones ? "Yes" : "No",
			stats.assets.dynamic_bone_count);
		ImGui::Text("Physics: %s (%u colliders)",
			stats.assets.has_physics ? "Yes" : "No",
			stats.assets.collider_count);
		ImGui::Text("Blend Shapes: %s (%u)",
			stats.assets.has_blend_shapes ? "Yes" : "No",
			stats.assets.blend_shape_count);
	}

	// Animations
	if (ImGui::CollapsingHeader("Animations")) {
		ImGui::Text("Total Animations: %d", (int)stats.animations.size());
		for (const auto& anim : stats.animations) {
			ImGui::Text("  %s: %.1fs (%u frames @ %.0f FPS)",
				anim.name.c_str(), anim.length,
				anim.frame_count, anim.fps);
		}
	}

	// Performance
	if (ImGui::CollapsingHeader("Performance")) {
		ImGui::Text("Rating: ");
		ImGui::SameLine();
		RenderPerformanceBadge(stats.metrics.performance_rating);
		ImGui::Text("Est. FPS Impact: %.1f", stats.metrics.estimated_fps_impact);
		ImGui::Text("Memory Usage: %.1f MB", stats.metrics.memory_usage_mb);

		float poly_fraction = stats.total_triangles / 70000.0f;
		if (poly_fraction > 1.0f) poly_fraction = 1.0f;
		ImGui::ProgressBar(poly_fraction, -1, 0, "Poly Budget");
	}
}

void Overlay::RenderPerformanceBadge(const std::string& rating) {
	if (rating == "Excellent")
		ImGui::TextColored(0.0f, 1.0f, 0.0f, 1.0f, "%s", rating.c_str());
	else if (rating == "Good")
		ImGui::TextColored(0.5f, 1.0f, 0.0f, 1.0f, "%s", rating.c_str());
	else if (rating == "Medium")
		ImGui::TextColored(1.0f, 1.0f, 0.0f, 1.0f, "%s", rating.c_str());
	else if (rating == "Poor")
		ImGui::TextColored(1.0f, 0.5f, 0.0f, 1.0f, "%s", rating.c_str());
	else
		ImGui::TextColored(1.0f, 0.0f, 0.0f, 1.0f, "%s", rating.c_str());
}
