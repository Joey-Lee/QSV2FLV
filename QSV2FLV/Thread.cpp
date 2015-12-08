#include "stdafx.h"
#include "Thread.h"

// -----------------------------------------------------------------------------------------------------

CThread::CThread()
{
	m_hThread		= NULL;
	m_dwThreadID	= 0;
}

// -----------------------------------------------------------------------------------------------------

CThread::~CThread()
{
	try 
	{
		Terminate();
	}
	catch (...)
	{}
}

// -----------------------------------------------------------------------------------------------------

BOOL CThread::Start()
{
	if (IsRuning())
		return FALSE;

	m_hThread = (HANDLE)CreateThread(NULL, 0, Runner, (void *)this, 0, &m_dwThreadID);
	return m_hThread ? TRUE : FALSE;
}

// -----------------------------------------------------------------------------------------------------

void CThread::Terminate()
{
	if (m_hThread != NULL)
	{	
		TerminateThread(m_hThread, 0);
		CloseHandle(m_hThread);
		OnTerminate();
		m_hThread = NULL;
		m_dwThreadID = 0;
	}
}

// -----------------------------------------------------------------------------------------------------

DWORD CThread::Self() 
{
	return ::GetCurrentThreadId();
}

// -----------------------------------------------------------------------------------------------------

DWORD WINAPI Runner(void* arg)
{
	DWORD			dwResult;
	CThread*		t;

	t = (CThread*)arg;
	dwResult = t->Run();

	t->m_hThread	= NULL;
	t->m_dwThreadID	= 0;

	t->OnTerminate();
	return dwResult;
}
