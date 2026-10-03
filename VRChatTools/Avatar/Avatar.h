#pragma once
#include "../pch.h"

struct MeshData {
	std::string name;
	uint32_t vertex_count = 0;
	uint32_t triangle_count = 0;
	uint32_t material_count = 0;
	float bounds_size = 0.0f;
};

struct TextureData {
	std::string name;
	uint32_t width = 0;
	uint32_t height = 0;
	std::string format;
	uint32_t memory_size = 0;
};

struct LightData {
	std::string name;
	std::string type; // Directional, Point, Spot
	float intensity = 0.0f;
	float range = 0.0f;
	std::array<float, 3> color = { 1.0f, 1.0f, 1.0f };
};

struct MaterialData {
	std::string name;
	std::string shader;
	uint32_t texture_count = 0;
	std::vector<std::string> textures;
};

struct BoneData {
	std::string name;
	uint32_t index = 0;
	std::array<float, 3> position = { 0.0f, 0.0f, 0.0f };
};

struct AnimationData {
	std::string name;
	float length = 0.0f;
	uint32_t frame_count = 0;
	float fps = 24.0f;
};

struct PlayerData {
	std::string display_name;
	std::string user_id;
	size_t player_ptr = 0;
	size_t avatar_ptr = 0;
	bool is_local = false;
	std::string trust_rank; // Visitor, New User, User, Known User, Trusted
	std::string status; // Online, Away, Busy
};

struct AvatarStats {
	// Player who owns this avatar
	PlayerData player;

	// Basic Info
	std::string avatar_name;
	std::string creator;
	std::string avatar_id;

	// Mesh Data
	std::vector<MeshData> meshes;
	uint32_t total_vertices = 0;
	uint32_t total_triangles = 0;

	// Texture Data
	std::vector<TextureData> textures;
	uint32_t total_texture_memory = 0;

	// Light Data
	std::vector<LightData> lights;

	// Material Data
	std::vector<MaterialData> materials;

	// Bone Data
	std::vector<BoneData> bones;
	uint32_t total_bones = 0;

	// Animation Data
	std::vector<AnimationData> animations;

	// Performance Metrics
	struct PerformanceMetrics {
		float estimated_fps_impact = 0.0f;
		float memory_usage_mb = 0.0f;
		std::string performance_rating; // Excellent, Good, Medium, Poor, Very Poor
		uint32_t polygon_budget = 0;
		uint32_t texture_memory_budget = 0;
	} metrics;

	// Asset Details
	struct AssetDetails {
		bool has_rigged_mesh = false;
		bool has_blend_shapes = false;
		uint32_t blend_shape_count = 0;
		bool has_physics = false;
		uint32_t collider_count = 0;
		bool has_dynamic_bones = false;
		uint32_t dynamic_bone_count = 0;
	} assets;
};

class AvatarAnalyzer {
public:
	static AvatarStats AnalyzeAvatar(size_t avatar_ptr);
	static void PrintAvatarStats(const AvatarStats& stats);
	static std::string GetPerformanceRating(const AvatarStats& stats);

	// Player scanning — iterates over every player in the instance
	static std::vector<PlayerData> GetAllPlayers();
	static std::vector<AvatarStats> ScanAllAvatars();
	static AvatarStats GetAvatarByPlayerName(const std::string& display_name);
	static void PrintAllPlayerStats();
	static void PrintPlayerSummary();

private:
	static std::vector<MeshData> ReadMeshes(size_t avatar_ptr);
	static std::vector<TextureData> ReadTextures(size_t avatar_ptr);
	static std::vector<LightData> ReadLights(size_t avatar_ptr);
	static std::vector<MaterialData> ReadMaterials(size_t avatar_ptr);
	static std::vector<BoneData> ReadBones(size_t avatar_ptr);
	static std::vector<AnimationData> ReadAnimations(size_t avatar_ptr);
	static AvatarStats::PerformanceMetrics CalculateMetrics(const AvatarStats& stats);

	// Internal player list helpers
	static size_t GetPlayerManagerPtr();
	static size_t GetPlayerListPtr(size_t manager_ptr);
	static int GetPlayerCount(size_t manager_ptr);
	static PlayerData ReadPlayerData(size_t player_ptr);
	static size_t GetAvatarPtrFromPlayer(size_t player_ptr);
};
