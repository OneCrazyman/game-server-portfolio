#include "stdafx.h"
#include "Lobby.h"
#include "Util/CPacket.h"
#include "NetLib/SessionManager.h"
#include "Util/SystemLogger.h"
#include "Packet/PacketMaker_Auto.h"

using enum SystemLogger::LOG_LEVEL;

void Lobby::OnAccept(uint32_t sessionId)
{
    this->CreateUser(sessionId);
}

void Lobby::OnRelease(uint32_t sessionId)
{
    this->DeleteUser(sessionId);
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

void Lobby::CreateUser(uint32_t sessionId)
{
    SLog(DEBUG_LEVEL, L"Create User");
    auto session = SessionManager::Instance().GetSessionPointer(sessionId);

    std::lock_guard<std::mutex> lock(mutex_);
    session->SetUserIdLock(userIdCount_);
    users_.emplace(userIdCount_, std::make_shared<User>(userIdCount_, sessionId));
    userIdCount_++;
}

void Lobby::DeleteUser(uint32_t sessionId)
{
    SLog(DEBUG_LEVEL, L"Delete User");
    auto session = SessionManager::Instance().GetSessionPointer(sessionId);
    uint32_t userId = session->GetUserIdLock();

    std::lock_guard<std::mutex> lock(mutex_);

    users_.erase(userId);
}
