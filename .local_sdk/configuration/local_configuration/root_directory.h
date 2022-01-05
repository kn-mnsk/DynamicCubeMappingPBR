// root_directory.h.in
#ifndef ROOT_DIRECTORY_H
#define ROOT_DIRECTORY_H

//extern char * project_root = "E:/MyDevelopment/02_OpenGL/01_DynamicCubeMapping/DynamicCubeMappingPBR/";
//extern std::string localdata_root = "E:/MyDevelopment/02_OpenGL/01_DynamicCubeMapping/DynamicCubeMappingPBR/.local_data/";
//extern std::string commondata_root = "E:/MyDevelopment/02_OpenGL/01_DynamicCubeMapping/DynamicCubeMappingPBR/../../../.common_data/";
//extern std::string shader_root = "E:/MyDevelopment/02_OpenGL/01_DynamicCubeMapping/DynamicCubeMappingPBR/shader/";

inline std::string get_localResources(const std::string file){
    std::string localResources = "E:/MyDevelopment/02_OpenGL/01_DynamicCubeMapping/DynamicCubeMappingPBR/.localResources/";
return (localResources + file);
}
inline std::string get_commonResources(const std::string file){
    std::string commonResources = "E:/MyDevelopment/02_OpenGL/01_DynamicCubeMapping/DynamicCubeMappingPBR/../../../.commonResources/";
return (commonResources + file);
}
inline std::string get_shader(const std::string file){
    std::string shader = "E:/MyDevelopment/02_OpenGL/01_DynamicCubeMapping/DynamicCubeMappingPBR/shader/";
return (shader + file);
}
inline std::string get_OGLDEVshader(const std::string file){
    std::string shader = "E:/MyDevelopment/02_OpenGL/01_DynamicCubeMapping/DynamicCubeMappingPBR/../../../.commonSDK/.Tutorial/OGLDEV/Shaders/";
return (shader + file);
}
// ROOT_DIRECTORY_H
#endif
