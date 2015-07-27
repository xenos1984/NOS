// EventManager.cpp - Basic event management.

#include <EventManager.h>
#include <StdDef.h>

using namespace Kernel;

EventManager::EventManager(void)
{
}

static unsigned int EventManager::NameToId(char* name)
{
	int i;
	unsigned int n = 0;

	if(name == nullptr)
		return 0;

	for(i = 0; i < 6; i++)
	{
		if('a' <= name[i] && name[i] <= 'z')
			n = 37 * n + name[i] - 'a' + 1;
		else if('A' <= name[i] && name[i] <= 'Z')
			n = 37 * n + name[i] - 'A' + 1;
		else if('0' <= name[i] && name[i] <= '9')
			n = 37 * n + name[i] - '0' + 27;
		else
			break;
	}

	return n;
}

bool EventManager::Unregister(char* name)
{
	return Unregister(NameToId(name));
}

int EventManager::Listen(char* name, bool exclusive)
{
	return Listen(NameToId(name), exclusive);
}

bool EventManager::Unlisten(char* name)
{
	return Unlisten(NameToId(name));
}

bool EventManager::Send(char* name, ...)
{
	return false;
}

bool EventManager::Send(unsigned int id, ...)
{
	return false;
}

bool EventManager::Retrieve(char* name, int* param)
{
	return Retrieve(NameToId(name), param);
}
