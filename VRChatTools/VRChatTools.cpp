// VRChatTools.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include "Avatar/Avatar.h"

// Export avatar stats for a given avatar pointer
extern "C" AvatarStats GetAvatarStats(size_t avatar_ptr) {
	return AvatarAnalyzer::AnalyzeAvatar(avatar_ptr);
}

// Print avatar stats to console
extern "C" void DisplayAvatarStats(const AvatarStats& stats) {
	AvatarAnalyzer::PrintAvatarStats(stats);
}
