// EventManager.h - Basic event handling.

#ifndef __EVENTMANAGER_H__
#define __EVENTMANAGER_H__

namespace Kernel
{
	class EventManager
	{
	private:
		static unsigned int NameToId(char* name);

	public:
		EventManager(void);

		unsigned int Register(char* name, int nargs); /**< Registers a new event. */
		bool Unregister(unsigned int id); /**< Unregisters a previously registered event. */
		bool Unregister(char* name); /**< Unregisters a previously registered event. */

		int Listen(unsigned int id, bool exclusive); /**< Listens to a previously registered event. */
		int Listen(char* name, bool exclusive); /**< Listens to a previously registered event. */
		bool Unlisten(unsigned int id);
		bool Unlisten(char* name);

		bool Send(unsigned int id, ...);
		bool Send(char* name, ...);
		bool Retrieve(unsigned int id, int* param);
		bool Retrieve(char* name, int* param);
	};
}

#endif
