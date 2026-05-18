#ifdef __linux__

#include<dlfcn.h>

#ifndef CMAKE_ROOT_DIR
#define CMAKE_ROOT_DIR "."
#endif

auto libPath = CMAKE_ROOT_DIR "/teacherSolutionBinary/linux/lib/libteacherSolution.so";
void*library = nullptr;

void loadLibrary(char const*libPath){
  library = dlopen(libPath,RTLD_LAZY);
}

void unloadLibrary(){
  if(library)dlclose(library);
  library = nullptr;
}

auto loadFunction(char const*fceName){
  return dlsym(library,fceName);
}

void System::generalSystemSettings(){}

#endif
