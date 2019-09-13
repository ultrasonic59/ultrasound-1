
#pragma once

#include "base/synchronization/lock.h"
#include "base/base_types.h"
#include <string>
#include <map>

namespace nim_comp
{
/** @struct PicRegion
  * @brief һ����ɫ�����������������ڲ�������
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @date 2016/09/19
  */
struct PicRegion
{
	PicRegion()
	{
		pdata_ = NULL;
		//subtype_ = nim::kNIMVideoSubTypeARGB;
		size_max_ = 0;
		size_ = 0;
	}

	~PicRegion()
	{
		Clear();
	}

	/**
	* ���������ɫ����
	* @return void	�޷���ֵ
	*/
	void Clear()
	{
		if (pdata_)
		{
			delete[] pdata_;
			pdata_ = NULL;
		}
		size_max_ = 0;
		size_ = 0;
	}

	/**
	* ������ɫ����
	* @param[in] time ʱ���
	* @param[in] data ֡����
	* @param[in] size ֡���ݴ�С
	* @param[in] width ��Ƶ���
	* @param[in] height ��Ƶ�߶�
	* @return int ���ش����sizeֵ
	*/
	int ResetData(uint64_t time, const char* data, int size, unsigned int width, unsigned int height/*, nim::NIMVideoSubType subtype*/)
	{
		if (size > size_max_)
		{
			if (pdata_)
			{
				delete[] pdata_;
				pdata_ = NULL;
			}
			pdata_ = new char[size];
			size_max_ = size;
		}
		width_ = width;
		height_ = height;
		timestamp_ = time;
		//subtype_ = subtype;
		size_ = size;
		memcpy(pdata_, data, size);
		return size;
	}
	
	//nim::NIMVideoSubType subtype_;
	char*		pdata_;         //��ɫ�����׵�ַ
	int			size_max_;
	int			size_;
	long        width_;         //���ؿ��
	long        height_;        //���ظ߶�
	int64_t	timestamp_;     //ʱ��������룩
};

/** @class VideoFrameMng
  * @brief ��Ƶ֡������
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @date 2016/09/19
  */
class VideoFrameMng
{
public:
	enum FrameType
	{
		Ft_I420 = 0,
		Ft_ARGB,
		Ft_ARGB_r,
	};
	VideoFrameMng();
	~VideoFrameMng();

	/**
	* ���������Ƶ֡����
	* @return void	�޷���ֵ
	*/
	void Clear();


	/**
	* ���һ����Ƶ֡����
	* @param[in] capture �Ƿ�Ϊ¼����Ƶ��֡
	* @param[in] time ʱ���
	* @param[in] data ֡����
	* @param[in] size ֡���ݴ�С
	* @param[in] width ��Ƶ���
	* @param[in] height ��Ƶ�߶�
	* @param[in] json �����������û�id��json��
	* @param[in] frame_type ֡����
	* @return void	�޷���ֵ
	*/
	void AddVideoFrame(bool capture, int64_t time, const char* data, int size, int width, int height, const std::string& json, FrameType frame_type = Ft_I420);
	
	/**
	* ��ȡĳ���û�������һ����Ƶ֡����
	* @param[in] uid �û�id
	* @param[out] time ʱ���
	* @param[out] out_data ֡����
	* @param[out] width ��Ƶ���
	* @param[out] height ��Ƶ�߶�
	* @param[in] mirror �Ƿ���Ҫ��תͼ��
	* @param[in] argb_or_yuv �Ƿ���ҪARGB��ʽ����
	* @return void	�޷���ֵ
	*/
	bool GetVideoFrame(uint64_t uid, int64_t& time, char* out_data, int& width, int& height, bool mirror = false, bool argb_or_yuv = true);
private:
	nbase::NLock  lock_;
	PicRegion capture_video_pic_;
	std::map<uint64_t, PicRegion*> recv_video_pic_list_;
};

}