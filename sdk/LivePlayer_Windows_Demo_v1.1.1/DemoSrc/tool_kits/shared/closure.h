#pragma once
#include "base/callback/callback.h"

//投递任务到UI线程
void Post2UI(const StdClosure &closure);

//投递任务到全局Misc线程
void Post2GlobalMisc(const StdClosure &closure);
