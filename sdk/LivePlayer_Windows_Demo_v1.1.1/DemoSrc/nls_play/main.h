#pragma once

#include "shared/threads.h"

class MiscThread;

/** @class MainThread
  * @brief 主线程（UI线程）类，继承nbase::FrameworkThread
  * @copyright (c) 2015, NetEase Inc. All rights reserved
  * @author towik
  * @date 2015/1/1
  */
class MainThread :
	public nbase::FrameworkThread
{
public:
	MainThread() : nbase::FrameworkThread("MainThread") {}
	virtual ~MainThread() {}
private:

	/**
	* 虚函数，初始化主线程
	* @return void	无返回值
	*/
	virtual void Init() override;

	/**
	* 虚函数，主线程退出时，做一些清理工作
	* @return void	无返回值
	*/
	virtual void Cleanup() override;

	/**
	* 主线程开始循环前，misc线程和db线程先开始循环
	* @return void	无返回值
	*/
	void PreMessageLoop();

	/**
	* 主线程结束循环前，misc线程和db线程先结束循环
	* @return void	无返回值
	*/
	void PostMessageLoop();
private:
	std::unique_ptr<MiscThread>	misc_thread_;
	std::unique_ptr<MiscThread>	play_thread_;
};


/** @class MiscThread
* @brief 杂项线程，处理一些琐事
* @copyright (c) 2015, NetEase Inc. All rights reserved
* @author towik
* @date 2015/1/1
*/
class MiscThread : public nbase::FrameworkThread
{
public:
	MiscThread(EnThreadId thread_id, const char *name);
	~MiscThread(void);

private:
	virtual void Init() override;
	virtual void Cleanup() override;

private:
	EnThreadId thread_id_;
};


