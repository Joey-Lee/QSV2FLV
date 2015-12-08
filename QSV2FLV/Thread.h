#ifndef _THREAD_H_
#define _THREAD_H_

/**
*@class CThread
*@brief 线程
*
*主要用于简化开线程的复杂度。
*/
class CThread
{
public:
	CThread();
	virtual ~CThread();

public:
	HANDLE		m_hThread;

public:
	/**
	*@brief	开启线程
	*@return	
	*- TRUE	线程开启成功
	*- FALSE线程开启失败
	*/
	BOOL Start();

	/**
	*@brief	线程对象所封装的线程是否和调用该方法的线程为同一线程。
	*@return	
	*- TRUE	是
	*- FALSE否
	*/
	BOOL IsCurrent() { return (m_dwThreadID == Self()); }

	/** 
	*@brief	线程对象所封装的线程是否在运行中
	*@return	
	*- TRUE	在运行中
	*- FALSE已经停止运行
	*/
	BOOL IsRuning() { return (NULL != m_hThread); }

	/** 
	*@brief	终止一个线程
	*/
	void Terminate();

	/** 
	*@brief	得到调用线程ID
	*@return 线程ID
	*/
	static DWORD Self();

	/**
	*@brief	线程执行体，是一个抽象方法，由派生类实现
	*@return 线程的返回值
	*/
	virtual DWORD Run() = 0;

	/**
	*@brief	线程结束的时候被调用的方法, 由具体的派生类决定怎么处理
	*@return 无返回值
	*/
	virtual void OnTerminate(){}

private:
	typedef unsigned (WINAPI *PTHREAD_START) (void *);

private:
	friend DWORD WINAPI Runner(void* arg);

private:
	DWORD 		m_dwThreadID;
};

#endif 
