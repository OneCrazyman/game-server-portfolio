#include "stdafx.h"
#include "ContentsProcess.h"
#include "NetLib/SessionManager.h"
#include "Lobby.h"
#include "NetLib/Session.h"

void ContentsProcess::OnEcho(uint32_t sessionId, int64_t msg)
{
	auto session = SessionManager::Instance().GetSessionPointer(sessionId);
	if (session == nullptr) {
		return;
	}

	Lobby::Instance().ChatUnicast(session->GetUserId(), msg);
}
