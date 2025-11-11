#pragma once
#include "stdafx.h"
#include "NetLib/NetworkEvents.h"
#include "Util/Singleton.h"
#include "User.h"

struct SessionInfo;
class Lobby : public Singleton<Lobby> ,public NetworkEvents
{
	friend class Singleton<Lobby>;
public:
	uint32_t OnConnect(uint32_t sessionId) override;
	void OnRelease(uint32_t sessionId, SessionInfo& sessionInfo) override;

	void ChatUnicast(uint32_t userId, int64_t msg);
private:
	Lobby() = default;

	uint32_t CreateUser(uint32_t sessionId);
	void DeleteUser(uint32_t sessionId);

	// user managing
	std::unordered_map<uint32_t, std::shared_ptr<User>> users_;
	uint32_t userIdCount_ = 1;
	std::mutex mutex_;
};