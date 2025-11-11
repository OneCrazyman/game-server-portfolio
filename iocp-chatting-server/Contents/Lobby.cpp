#include "stdafx.h"
#include "Lobby.h"
#include "Util/CPacket.h"
#include "NetLib/SessionManager.h"
#include "Util/SystemLogger.h"
#include "Packet/PacketMaker_Auto.h"
#include "define.h"

using enum SystemLogger::LOG_LEVEL;

uint32_t Lobby::OnConnect(uint32_t sessionId)
{
    return this->CreateUser(sessionId);
}

void Lobby::OnRelease(uint32_t sessionId, SessionInfo& sessionInfo)
{
    auto userId = sessionInfo.userId;
    this->DeleteUser(userId);
}

void Lobby::ChatUnicast(uint32_t userId, int64_t msg)
{
    std::unique_lock<std::mutex> lock(mutex_);
    auto user = users_.find(userId);
    if (user == users_.end()) {
        return;
    }
    auto sessionId = user->second.get()->SessionId();
    lock.unlock();

	auto session = SessionManager::Instance().GetSessionPointer(sessionId);
    if (session == nullptr) {
        return;
	}

    CPacket* packet = new CPacket; // FIX
    mp_Echo(*packet, msg);
	session->SendPacket(*packet);
}

uint32_t Lobby::CreateUser(uint32_t sessionId)
{
    SLog(DEBUG_LEVEL, L"Create User");
    std::lock_guard<std::mutex> lock(mutex_);
    users_.emplace(userIdCount_, std::make_shared<User>(userIdCount_, sessionId));
    return userIdCount_++;
}

void Lobby::DeleteUser(uint32_t userId)
{
    SLog(DEBUG_LEVEL, L"Delete User");
    std::lock_guard<std::mutex> lock(mutex_);
    users_.erase(userId);
}
