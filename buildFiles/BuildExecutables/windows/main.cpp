#include <windows.h>
#include <jni.h>
#include <iostream>
#include <filesystem>
// #include <format>
#include <chrono>

typedef jint (JNICALL *CreateJavaVM_t)(JavaVM**, void**, void*);

// void PrintLastError(DWORD errorCode, const std::string& prefix) {
//     LPSTR messageBuffer = nullptr;

//     DWORD size = FormatMessageA(
//         FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
//         nullptr,
//         errorCode,
//         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//         (LPSTR)&messageBuffer,
//         0,
//         nullptr
//     );

//     if (size && messageBuffer) {
//         std::cerr << prefix << " Error " << errorCode << ": " << messageBuffer << "\n";
//         LocalFree(messageBuffer);
//     } else {
//         std::cerr << prefix << " Unknown error code: " << errorCode << "\n";
//     }
// }

int main(int argc, char** argv) {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::filesystem::path exePath(buffer);
    std::string DIRNAME = exePath.parent_path().string();

    // Construct full path to jvm.dll
    std::string jvmDir = DIRNAME + "\\jre\\bin";
    std::string jvmPath = DIRNAME + "\\jre\\bin\\server\\jvm.dll";

    SetDllDirectoryA(jvmDir.c_str());
    // Load jvm.dll manually
    HMODULE jvmLib = LoadLibraryA(jvmPath.c_str());
    if (!jvmLib) {
        DWORD errCode = GetLastError();
        std::cerr << "Failed to load jvm.dll from: " << jvmPath << "\n";
        // PrintLastError(errCode, "LoadLibraryA failed.");
        // std::cerr<<"Done printing error."<<std::endl;
        return 1;
    }

    // Get JNI_CreateJavaVM symbol
    CreateJavaVM_t createJVM = (CreateJavaVM_t)GetProcAddress(jvmLib, "JNI_CreateJavaVM");
    if (!createJVM) {
        std::cerr << "Failed to find JNI_CreateJavaVM in jvm.dll\n";
        return 1;
    }

    // Now you're free to set PATH for dependent DLLs
    std::string jreBinPath = DIRNAME + "\\jre\\bin";
    std::string newPath = "PATH=" + jreBinPath + ";" + std::getenv("PATH");
    _putenv(newPath.c_str());

    // Prepare Java options
    std::string path = DIRNAME + "\\bin"; // current directory
    std::filesystem::path latestJar;
    std::cout<<"search path: " << path << std::endl;
    std::filesystem::file_time_type latestTime;
    std::string jarPath = DIRNAME + "\\bin\\triplea-game-headed-2.7+14904.jar";

    try {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            std::cout << entry.path() << std::endl;
            if (entry.is_regular_file() && entry.path().extension() == ".jar") {
                // std::cout << "JarFile: " << entry.path() << std::endl;
                auto writeTime = std::filesystem::last_write_time(entry);
                // std::cout << std::format("File write time is {}\n", writeTime) << std::endl;
                if (latestJar.empty() || writeTime > latestTime) {
                    latestTime = writeTime;
                    latestJar = entry.path();
                }
            }
        }

        if (!latestJar.empty()) {
            jarPath = latestJar.string();
            std::cout << "Latest .jar file: " << latestJar << std::endl;
        } else {
            std::cout << "No .jar files found in directory, trying base 14904.\n";
        }
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    std::string classPathOpt = "-Djava.class.path=" + jarPath;
    JavaVMOption options[2];
    options[0].optionString = const_cast<char*>(classPathOpt.c_str());
    options[1].optionString = const_cast<char*>("-Dsun.java2d.dpiaware=true");

    JavaVMInitArgs vm_args;
    vm_args.version = JNI_VERSION_21;
    vm_args.nOptions = 2;
    vm_args.options = options;
    vm_args.ignoreUnrecognized = JNI_FALSE;

    JavaVM* jvm;
    JNIEnv* env;

    std::cout << "Creating JVM..." << std::endl;
    jint res = createJVM(&jvm, reinterpret_cast<void**>(&env), &vm_args);
    if (res != JNI_OK) {
        std::cerr << "Failed to create JVM\n";
        return 1;
    }

    std::cout << "JVM created successfully.\n";
    // Find the main class
    jclass cls = env->FindClass("org/triplea/game/client/HeadedGameRunner");
    if (!cls) {
        std::cerr << "Failed to find main class\n";
        jvm->DestroyJavaVM();
        return 1;
    }

    // Find the main method
    jmethodID mainMethod = env->GetStaticMethodID(cls, "main", "([Ljava/lang/String;)V");
    if (!mainMethod) {
        std::cerr << "Failed to find main method\n";
        jvm->DestroyJavaVM();
        return 1;
    }

    // Convert C++ args to Java String array
    jobjectArray jargs = env->NewObjectArray(argc - 1, env->FindClass("java/lang/String"), nullptr);
    for (int i = 1; i < argc; i++) {
        env->SetObjectArrayElement(jargs, i - 1, env->NewStringUTF(argv[i]));
    }
    
    env->CallStaticVoidMethod(cls, mainMethod, jargs);
    std::cout<<"Called main method!"<<std::endl;
    std::cin.get();
    // Continue with FindClass, etc.

    jvm->DestroyJavaVM();
    return 0;
}
