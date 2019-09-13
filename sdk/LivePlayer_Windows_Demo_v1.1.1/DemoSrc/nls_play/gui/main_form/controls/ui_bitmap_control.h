﻿#ifndef _UIBITMAPCONTROL_H_
#define _UIBITMAPCONTROL_H_
#pragma once

#include <map>

namespace ui
{
/** @class BitmapControl
  * @brief 支持贴hbitmap的图片
  * @copyright (c) 2016, NetEase Inc. All rights reserved
  * @author gaoqi
  * @date 2014/3/10
  */
class BitmapControl : public ui::Box
{
public:
	/**
	* 构造函数
	* @param[in] video_frame_mng 视频帧管理器
	* @return void	无返回值
	*/
	BitmapControl();
	~BitmapControl();

	/**
	* 重写父类绘制函数
	* @param[in] hDC 目标DC
	* @param[in] rcPaint 可绘制区域
	* @return void	无返回值
	*/
	void Paint(HDC hDC, const UiRect& rcPaint) override;

	/**
	* 清理数据
	* @return void	无返回值
	*/
	void Clear();

	/**
	* 更新图像
	* @return bool true 成功，false 失败
	*/
	bool Update(const char* argb_data, int width, int height);

	//刷新图象
	void Refresh();

	/**
	* 是否已经刷新超时
	* @return bool true 是，false 否
	*/
	bool IsRefreshTimeout();

protected:
	int64_t timestamp_;
	std::string image_data_;
	int image_width_ = 0;
	int image_height_ = 0;
	bool updated_ = false;
	std::string data_;
	int width_ = 0;
	int height_ = 0;
	nbase::NLock lock_;
};

}

#endif // _UIBITMAPCONTROL_H_
