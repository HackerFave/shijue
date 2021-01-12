#include <Windows.h>
#include <DbgHelp.h>
#include <qapplication.h>
#include <qdir.h>
#include <qdatetime.h>
#pragma comment(lib, "Dbghelp.lib")
static LONG WINAPI exceptionCallback(struct _EXCEPTION_POINTERS* exceptionInfo)
{

	QString savePath = qApp->applicationDirPath() + "dump/";
	QDir dir(savePath);
	if (!dir.exists() && !dir.mkpath(savePath)) {
		qApp->exit(E_UNEXPECTED);
		return EXCEPTION_EXECUTE_HANDLER;
	}

	savePath.append("assit_");
	savePath.append(QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz"));
	savePath.append(".dmp");

	HANDLE dump = CreateFileW(savePath.toStdWString().c_str(), GENERIC_WRITE,
		0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == dump) {
		qApp->exit(E_UNEXPECTED);
		return EXCEPTION_EXECUTE_HANDLER;
	}

	MINIDUMP_EXCEPTION_INFORMATION miniDumpExceptionInfo;
	miniDumpExceptionInfo.ExceptionPointers = exceptionInfo;
	miniDumpExceptionInfo.ThreadId = GetCurrentThreadId();
	miniDumpExceptionInfo.ClientPointers = TRUE;
	DWORD idProcess = GetCurrentProcessId();
	MiniDumpWriteDump(GetCurrentProcess(), idProcess, dump,
		MiniDumpNormal, &miniDumpExceptionInfo, NULL, NULL);

	CloseHandle(dump);

	qApp->exit(E_UNEXPECTED);
	return EXCEPTION_EXECUTE_HANDLER;
}