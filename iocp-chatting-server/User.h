#include "stdafx.h"

class User
{
public:
	User(uint32_t userId, uint32_t sessionId)
		: id_(userId), sessionId_(sessionId)
	{}

	uint32_t Id() const { return id_; }
	uint32_t SessionId() const { return sessionId_; }

private:
	uint32_t id_;
	uint32_t sessionId_;
};