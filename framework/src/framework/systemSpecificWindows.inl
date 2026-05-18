#if  defined(_MSC_VER) || defined(__MINGW64__)

#include <windows.h>
#include <system_error>

#ifndef CMAKE_ROOT_DIR
#define CMAKE_ROOT_DIR "."
#endif

auto libPath = CMAKE_ROOT_DIR "/teacherSolutionBinary/windows/bin/libteacherSolution.dll";
HMODULE library = nullptr;

void loadLibrary(char const*libPath){
  library = LoadLibrary(libPath);
  if(!library){
    DWORD error = ::GetLastError();
    auto message = std::system_category().message(error);
    std::cerr << message << std::endl;
  }
}

void unloadLibrary(){
  if(library)FreeLibrary(library);
  library = nullptr;
}

auto loadFunction(char const*fceName){
  return GetProcAddress(library,fceName);
}

void System::generalSystemSettings(){
  SetConsoleOutputCP(CP_UTF8);
  setvbuf(stdout, nullptr, _IOFBF, 1000);
}

#endif
