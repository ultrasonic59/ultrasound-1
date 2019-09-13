#include "nim_service/sdk/sdk_instance.h"

#include "base/strings/string_util.h"

namespace nim {

SdkInstance::SdkInstance() {
    m_instance_nim = NULL;
}

SdkInstance::~SdkInstance() {
    m_function_map.clear();
}

bool SdkInstance::LoadSdkDll(const char *cur_module_dir,
  const char *sdk_dll_file_name) {
    std::string dir(cur_module_dir);
    dir.append(sdk_dll_file_name);


    std::wstring utf16_dir = base::UTF8ToUTF16(dir);
    m_instance_nim = ::LoadLibraryW(utf16_dir.c_str());

    if (m_instance_nim == NULL) {
        return false;
    }

    return true;
}

void SdkInstance::UnLoadSdkDll() {
    assert(m_instance_nim);
    if (m_instance_nim) {
        ::FreeLibrary(m_instance_nim);
        m_instance_nim = NULL;
    }
}

} // namespace nim