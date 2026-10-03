// VRChatTools.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include "Avatar/Avatar.h"

extern "C" AvatarStats GetAvatarStats(size_t avatar_ptr) {
	return AvatarAnalyzer::AnalyzeAvatar(avatar_ptr);
}

extern "C" void DisplayAvatarStats(const AvatarStats& stats) {
	AvatarAnalyzer::PrintAvatarStats(stats);
}

extern "C" void ScanAndPrintAllPlayers() {
	AvatarAnalyzer::PrintAllPlayerStats();
}

extern "C" void PrintPlayerSummaryTable() {
	AvatarAnalyzer::PrintPlayerSummary();
}

extern "C" AvatarStats GetPlayerAvatarStats(const char* display_name) {
	return AvatarAnalyzer::GetAvatarByPlayerName(display_name);
}
