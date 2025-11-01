#pragma once
#include "stdafx.h"
#include "NetLib/Server.h"

class ConsoleHandler
{
public:
	ConsoleHandler(std::shared_ptr<Server>& server) : server_(server) {}

	void Run();

private:
	std::shared_ptr<Server> server_;
};