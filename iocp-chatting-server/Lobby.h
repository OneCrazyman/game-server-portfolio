#pragma once
#include "stdafx.h"
#include "NetworkEvents.h"
#include "Singleton.h"
#include "User.h"

class Lobby : public Singleton<Lobby> ,public NetworkEvents
{
	friend class Singleton<Lobby>;
public:
	// NetworkEvents을(를) 통해 상속됨
	void OnAccept(uint32_t sessionId) override;
	void OnRelease(uint32_t sessionId) override;

	void ChatUnicast(uint32_t userId, int64_t msg);
private:
	Lobby() = default;

	void CreateUser(uint32_t sessionId);
	void DeleteUser(uint32_t sessionId);

	// user managing
	std::unordered_map<uint32_t, std::shared_ptr<User>> users_;
	uint32_t userIdCount_ = 1;
	std::mutex mutex_;
};