#include "../pch.h"
#include "Avatar.h"

AvatarStats AvatarAnalyzer::AnalyzeAvatar(size_t avatar_ptr) {
	AvatarStats stats;

	// Read basic avatar information
	stats.avatar_name = "Avatar";
	stats.creator = "Unknown";
	stats.avatar_id = "";

	// Read mesh data
	stats.meshes = ReadMeshes(avatar_ptr);
	for (const auto& mesh : stats.meshes) {
		stats.total_vertices += mesh.vertex_count;
		stats.total_triangles += mesh.triangle_count;
	}

	// Read texture data
	stats.textures = ReadTextures(avatar_ptr);
	for (const auto& texture : stats.textures) {
		stats.total_texture_memory += texture.memory_size;
	}

	// Read lights
	stats.lights = ReadLights(avatar_ptr);

	// Read materials
	stats.materials = ReadMaterials(avatar_ptr);

	// Read bones
	stats.bones = ReadBones(avatar_ptr);
	stats.total_bones = static_cast<uint32_t>(stats.bones.size());

	// Read animations
	stats.animations = ReadAnimations(avatar_ptr);

	// Calculate performance metrics
	stats.metrics = CalculateMetrics(stats);
	stats.metrics.performance_rating = GetPerformanceRating(stats);

	return stats;
}

void AvatarAnalyzer::PrintAvatarStats(const AvatarStats& stats) {
	std::cout << "\n=== AVATAR STATISTICS ===" << std::endl;
	std::cout << "Avatar Name: " << stats.avatar_name << std::endl;
	std::cout << "Creator: " << stats.creator << std::endl;
	std::cout << "Avatar ID: " << stats.avatar_id << std::endl;

	std::cout << "\n--- MESH DATA ---" << std::endl;
	std::cout << "Total Meshes: " << stats.meshes.size() << std::endl;
	std::cout << "Total Vertices: " << stats.total_vertices << std::endl;
	std::cout << "Total Triangles: " << stats.total_triangles << std::endl;
	std::cout << "\nMesh Details:" << std::endl;
	for (const auto& mesh : stats.meshes) {
		std::cout << "  - " << mesh.name << ": " << mesh.vertex_count << " vertices, "
			<< mesh.triangle_count << " triangles, " << mesh.material_count << " materials" << std::endl;
	}

	std::cout << "\n--- TEXTURE DATA ---" << std::endl;
	std::cout << "Total Textures: " << stats.textures.size() << std::endl;
	std::cout << "Total Texture Memory: " << (stats.total_texture_memory / 1024.0f / 1024.0f) << " MB" << std::endl;
	std::cout << "\nTexture Details:" << std::endl;
	for (const auto& tex : stats.textures) {
		std::cout << "  - " << tex.name << ": " << tex.width << "x" << tex.height
			<< " (" << tex.format << ") " << (tex.memory_size / 1024.0f) << " KB" << std::endl;
	}

	std::cout << "\n--- LIGHT DATA ---" << std::endl;
	std::cout << "Total Lights: " << stats.lights.size() << std::endl;
	for (const auto& light : stats.lights) {
		std::cout << "  - " << light.name << " (" << light.type << "): "
			<< "Intensity=" << light.intensity << ", Range=" << light.range << std::endl;
	}

	std::cout << "\n--- MATERIAL DATA ---" << std::endl;
	std::cout << "Total Materials: " << stats.materials.size() << std::endl;
	for (const auto& mat : stats.materials) {
		std::cout << "  - " << mat.name << " (Shader: " << mat.shader << "): "
			<< mat.texture_count << " textures" << std::endl;
	}

	std::cout << "\n--- BONE DATA ---" << std::endl;
	std::cout << "Total Bones: " << stats.total_bones << std::endl;
	if (stats.total_bones <= 50) {
		std::cout << "Bone List:" << std::endl;
		for (const auto& bone : stats.bones) {
			std::cout << "  - " << bone.name << " (Index: " << bone.index << ")" << std::endl;
		}
	}

	std::cout << "\n--- ANIMATION DATA ---" << std::endl;
	std::cout << "Total Animations: " << stats.animations.size() << std::endl;
	for (const auto& anim : stats.animations) {
		std::cout << "  - " << anim.name << ": " << anim.length << "s ("
			<< anim.frame_count << " frames @ " << anim.fps << " FPS)" << std::endl;
	}

	std::cout << "\n--- ASSET DETAILS ---" << std::endl;
	std::cout << "Rigged Mesh: " << (stats.assets.has_rigged_mesh ? "Yes" : "No") << std::endl;
	std::cout << "Blend Shapes: " << (stats.assets.has_blend_shapes ? "Yes" : "No");
	if (stats.assets.has_blend_shapes) {
		std::cout << " (" << stats.assets.blend_shape_count << " shapes)";
	}
	std::cout << std::endl;
	std::cout << "Physics: " << (stats.assets.has_physics ? "Yes" : "No");
	if (stats.assets.has_physics) {
		std::cout << " (" << stats.assets.collider_count << " colliders)";
	}
	std::cout << std::endl;
	std::cout << "Dynamic Bones: " << (stats.assets.has_dynamic_bones ? "Yes" : "No");
	if (stats.assets.has_dynamic_bones) {
		std::cout << " (" << stats.assets.dynamic_bone_count << " bones)";
	}
	std::cout << std::endl;

	std::cout << "\n--- PERFORMANCE METRICS ---" << std::endl;
	std::cout << "Performance Rating: " << stats.metrics.performance_rating << std::endl;
	std::cout << "Estimated FPS Impact: " << stats.metrics.estimated_fps_impact << " FPS" << std::endl;
	std::cout << "Memory Usage: " << stats.metrics.memory_usage_mb << " MB" << std::endl;
	std::cout << "Polygon Budget Used: " << stats.metrics.polygon_budget << std::endl;
	std::cout << "Texture Memory Budget Used: " << stats.metrics.texture_memory_budget << std::endl;
	std::cout << "\n========================\n" << std::endl;
}

std::string AvatarAnalyzer::GetPerformanceRating(const AvatarStats& stats) {
	uint32_t triangle_count = stats.total_triangles;
	uint32_t texture_memory = stats.total_texture_memory;

	// VRChat performance budgets
	// Poor polygons: > 150k, Good: < 50k
	// Poor textures: > 100MB, Good: < 20MB
	int rating = 0;

	if (triangle_count > 150000) rating -= 2;
	else if (triangle_count > 100000) rating -= 1;
	else if (triangle_count < 50000) rating += 1;

	if (texture_memory > 100 * 1024 * 1024) rating -= 2;
	else if (texture_memory > 50 * 1024 * 1024) rating -= 1;
	else if (texture_memory < 20 * 1024 * 1024) rating += 1;

	if (stats.lights.size() > 5) rating -= 1;
	if (stats.total_bones > 200) rating -= 1;

	if (rating >= 2) return "Excellent";
	if (rating >= 1) return "Good";
	if (rating >= 0) return "Medium";
	if (rating >= -1) return "Poor";
	return "Very Poor";
}

std::vector<PlayerData> AvatarAnalyzer::GetAllPlayers() {
	std::vector<PlayerData> players;

	size_t manager = GetPlayerManagerPtr();
	if (!manager) return players;

	int count = GetPlayerCount(manager);
	size_t list_ptr = GetPlayerListPtr(manager);
	if (!list_ptr || count <= 0) return players;

	for (int i = 0; i < count; i++) {
		// Each entry in the player list is a pointer-sized element
		size_t player_ptr = 0; // TODO: mem.Read<size_t>(list_ptr + i * sizeof(size_t))
		if (!player_ptr) continue;

		PlayerData pd = ReadPlayerData(player_ptr);
		pd.player_ptr = player_ptr;
		pd.avatar_ptr = GetAvatarPtrFromPlayer(player_ptr);
		players.push_back(pd);
	}

	return players;
}

std::vector<AvatarStats> AvatarAnalyzer::ScanAllAvatars() {
	std::vector<AvatarStats> all_stats;

	auto players = GetAllPlayers();
	for (const auto& player : players) {
		if (!player.avatar_ptr) continue;

		AvatarStats stats = AnalyzeAvatar(player.avatar_ptr);
		stats.player = player;
		all_stats.push_back(stats);
	}

	return all_stats;
}

AvatarStats AvatarAnalyzer::GetAvatarByPlayerName(const std::string& display_name) {
	auto players = GetAllPlayers();
	for (const auto& player : players) {
		if (player.display_name == display_name && player.avatar_ptr) {
			AvatarStats stats = AnalyzeAvatar(player.avatar_ptr);
			stats.player = player;
			return stats;
		}
	}
	return AvatarStats{};
}

void AvatarAnalyzer::PrintAllPlayerStats() {
	auto all_stats = ScanAllAvatars();

	std::cout << "\n========== ALL PLAYERS IN INSTANCE ==========" << std::endl;
	std::cout << "Players Found: " << all_stats.size() << "\n" << std::endl;

	for (const auto& stats : all_stats) {
		std::cout << ">> Player: " << stats.player.display_name
			<< " [" << stats.player.trust_rank << "]"
			<< (stats.player.is_local ? " (YOU)" : "")
			<< std::endl;
		PrintAvatarStats(stats);
	}

	std::cout << "=============================================\n" << std::endl;
}

void AvatarAnalyzer::PrintPlayerSummary() {
	auto all_stats = ScanAllAvatars();

	std::cout << "\n===== PLAYER SUMMARY =====" << std::endl;
	std::cout << std::left;

	printf("%-20s %-12s %-10s %-10s %-8s %-8s %-12s\n",
		"Player", "Trust", "Triangles", "TexMem", "Lights", "Bones", "Rating");
	printf("%-20s %-12s %-10s %-10s %-8s %-8s %-12s\n",
		"------", "-----", "---------", "------", "------", "-----", "------");

	for (const auto& stats : all_stats) {
		std::string name = stats.player.display_name;
		if (name.length() > 18) name = name.substr(0, 18) + "..";

		char tex_buf[16];
		snprintf(tex_buf, sizeof(tex_buf), "%.1fMB",
			stats.total_texture_memory / (1024.0f * 1024.0f));

		printf("%-20s %-12s %-10u %-10s %-8zu %-8u %-12s\n",
			name.c_str(),
			stats.player.trust_rank.c_str(),
			stats.total_triangles,
			tex_buf,
			stats.lights.size(),
			stats.total_bones,
			stats.metrics.performance_rating.c_str());
	}

	std::cout << "\n=========================\n" << std::endl;
}

std::vector<MeshData> AvatarAnalyzer::ReadMeshes(size_t avatar_ptr) {
	std::vector<MeshData> meshes;
	// TODO: Implement mesh reading from memory
	return meshes;
}

std::vector<TextureData> AvatarAnalyzer::ReadTextures(size_t avatar_ptr) {
	std::vector<TextureData> textures;
	// TODO: Implement texture reading from memory
	return textures;
}

std::vector<LightData> AvatarAnalyzer::ReadLights(size_t avatar_ptr) {
	std::vector<LightData> lights;
	// TODO: Implement light reading from memory
	return lights;
}

std::vector<MaterialData> AvatarAnalyzer::ReadMaterials(size_t avatar_ptr) {
	std::vector<MaterialData> materials;
	// TODO: Implement material reading from memory
	return materials;
}

std::vector<BoneData> AvatarAnalyzer::ReadBones(size_t avatar_ptr) {
	std::vector<BoneData> bones;
	// TODO: Implement bone reading from memory
	return bones;
}

std::vector<AnimationData> AvatarAnalyzer::ReadAnimations(size_t avatar_ptr) {
	std::vector<AnimationData> animations;
	// TODO: Implement animation reading from memory
	return animations;
}

size_t AvatarAnalyzer::GetPlayerManagerPtr() {
	// TODO: Read the PlayerManager singleton pointer from VRChat's memory
	// Typically found via sig scan or static offset from base
	return 0;
}

size_t AvatarAnalyzer::GetPlayerListPtr(size_t manager_ptr) {
	// TODO: Read the internal player list array from the manager
	return 0;
}

int AvatarAnalyzer::GetPlayerCount(size_t manager_ptr) {
	// TODO: Read player count from the manager struct
	return 0;
}

PlayerData AvatarAnalyzer::ReadPlayerData(size_t player_ptr) {
	PlayerData pd;
	// TODO: Read display name, user ID, trust rank, status from player object
	return pd;
}

size_t AvatarAnalyzer::GetAvatarPtrFromPlayer(size_t player_ptr) {
	// TODO: Follow the pointer chain from player -> avatar gameobject
	return 0;
}

AvatarStats::PerformanceMetrics AvatarAnalyzer::CalculateMetrics(const AvatarStats& stats) {
	AvatarStats::PerformanceMetrics metrics;

	metrics.polygon_budget = stats.total_triangles;
	metrics.texture_memory_budget = stats.total_texture_memory / (1024 * 1024);
	metrics.memory_usage_mb = metrics.texture_memory_budget + (stats.total_vertices * 12) / (1024 * 1024);

	// Rough FPS impact estimation
	// ~1 FPS per 50k triangles
	metrics.estimated_fps_impact = stats.total_triangles / 50000.0f;

	// Add impact from lights
	metrics.estimated_fps_impact += stats.lights.size() * 0.5f;

	// Add impact from bones
	metrics.estimated_fps_impact += (stats.total_bones / 100.0f) * 0.5f;

	return metrics;
}
