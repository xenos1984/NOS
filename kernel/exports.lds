EXTERN(CreateThread);
EXTERN(ExitThread);
EXTERN(ExitProcess);

VERSION
{
	{
	global:
		CreateThread;
		ExitThread;
		ExitProcess;

	local: *;
	};
}
