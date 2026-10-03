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

struct AvatarStats {
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
	/**
	* @brief Analyzes an avatar and returns statistics
	* @param avatar_ptr pointer to avatar object in memory
	* @return AvatarStats structure with avatar information
	*/
	static AvatarStats AnalyzeAvatar(size_t avatar_ptr);

	/**
	* @brief Prints formatted avatar stats to console
	* @param stats the avatar stats to display
	*/
	static void PrintAvatarStats(const AvatarStats& stats);

	/**
	* @brief Gets performance rating based on metrics
	* @param stats the avatar stats
	* @return performance rating string
	*/
	static std::string GetPerformanceRating(const AvatarStats& stats);

	/**
	* @brief Scans for all avatars in the scene
	* @return vector of AvatarStats for all found avatars
	*/
	static std::vector<AvatarStats> ScanAllAvatars();

	/**
	* @brief Gets a specific avatar's stats by name
	* @param avatar_name the name of the avatar to find
	* @return AvatarStats if found, empty stats if not
	*/
	static AvatarStats GetAvatarByName(const std::string& avatar_name);

private:
	/**
	* @brief Reads mesh data from memory
	*/
	static std::vector<MeshData> ReadMeshes(size_t avatar_ptr);

	/**
	* @brief Reads texture data from memory
	*/
	static std::vector<TextureData> ReadTextures(size_t avatar_ptr);

	/**
	* @brief Reads light data from memory
	*/
	static std::vector<LightData> ReadLights(size_t avatar_ptr);

	/**
	* @brief Reads material data from memory
	*/
	static std::vector<MaterialData> ReadMaterials(size_t avatar_ptr);

	/**
	* @brief Reads bone data from memory
	*/
	static std::vector<BoneData> ReadBones(size_t avatar_ptr);

	/**
	* @brief Reads animation data from memory
	*/
	static std::vector<AnimationData> ReadAnimations(size_t avatar_ptr);

	/**
	* @brief Calculates performance metrics
	*/
	static AvatarStats::PerformanceMetrics CalculateMetrics(const AvatarStats& stats);
};
