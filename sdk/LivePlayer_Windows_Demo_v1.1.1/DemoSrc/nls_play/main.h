#pragma once

#include "shared/threads.h"

class MiscThread;

/** @class MainThread
  * @brief ���̣߳�UI�̣߳��࣬�̳�nbase::FrameworkThread
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
	* �麯������ʼ�����߳�
	* @return void	�޷���ֵ
	*/
	virtual void Init() override;

	/**
	* �麯�������߳��˳�ʱ����һЩ������
	* @return void	�޷���ֵ
	*/
	virtual void Cleanup() override;

	/**
	* ���߳̿�ʼѭ��ǰ��misc�̺߳�db�߳��ȿ�ʼѭ��
	* @return void	�޷���ֵ
	*/
	void PreMessageLoop();

	/**
	* ���߳̽���ѭ��ǰ��misc�̺߳�db�߳��Ƚ���ѭ��
	* @return void	�޷���ֵ
	*/
	void PostMessageLoop();
private:
	std::unique_ptr<MiscThread>	misc_thread_;
	std::unique_ptr<MiscThread>	play_thread_;
};


/** @class MiscThread
* @brief �����̣߳�����һЩ����
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


