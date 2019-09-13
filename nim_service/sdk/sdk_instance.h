#ifndef NIM_SERVICE_SDK_SDK_INSTANCE_H_
#define NIM_SERVICE_SDK_SDK_INSTANCE_H_

#include <assert.h>
#include <map>
#include <string>
#include <windows.h>

namespace nim {

class SdkInstance
{
public:
    SdkInstance();
    virtual ~SdkInstance();

    bool LoadSdkDll(const char *cur_module_dir, const char *sdk_dll_file_name);

    void UnLoadSdkDll();

    void* GetFunction(const std::string& function_name)
    {
        auto it = m_function_map.find(function_name);
        if (it != m_function_map.end()) {
            return it->second;
        }

        void* function_ptr = ::GetProcAddress(m_instance_nim, function_name.c_str());

        assert(function_ptr);
        m_function_map[function_name] = function_ptr;
        return function_ptr;
    }

private:
    HINSTANCE m_instance_nim;
    std::map<std::string, void*> m_function_map;
};

static void nim_print_unfound_func_name(char* name)
{
    printf("function [ %s ] not found \n", name);
}

static void unfound_function_holder()
{

}

extern SdkInstance *g_nim_sdk_instance;

#define NIM_SDK_GET_FUNC(function_ptr)	\
						((function_ptr)g_nim_sdk_instance->GetFunction(#function_ptr) != NULL ? ((function_ptr)g_nim_sdk_instance->GetFunction(#function_ptr)) : (nim_print_unfound_func_name(#function_ptr),(function_ptr)unfound_function_holder))

} // namespace nim

#endif // NIM_SERVICE_SDK_SDK_INSTANCE_H_
