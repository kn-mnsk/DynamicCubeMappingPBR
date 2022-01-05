cmake_minimum_required (VERSION 3.1)
cmake_policy(VERSION 3.1)

get_filename_component(ProjectID ${CMAKE_CURRENT_LIST_DIR} NAME)
string(REPLACE " " "_" ProjectID ${ProjectID})
project (${ProjectID})

##### From CmakeLists of freeglut
# for multiarch LIBDIR support (requires cmake>=2.8.8)
INCLUDE(GNUInstallDirs)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}" CACHE PATH "..." FORCE)
endif()

### Folder Structures
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

### Define directories -Configuration types are either Debug or Release only!
set(ConfigDir $<$<CONFIG:Release>:Release>$<$<CONFIG:Debug>:Debug>)
set(COMMONSDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../.commonSDK")
set(CURRENT_DIR "${CMAKE_CURRENT_SOURCE_DIR}")
# NOTE: On Windows and Cygwin, the dll's are placed in the
# CMAKE_RUNTIME_OUTPUT_DIRECTORY, while their corresponding import
# libraries end up in CMAKE_ARCHIVE_OUTPUT_DIRECTORY. On other
# platforms, such as Linux, the shared libraries are put in
# CMAKE_ARCHIVE_OUTPUT_DIRECTORY instead.
# Static libraries end up in CMAKE_ARCHIVE_OUTPUT_DIRECTORY on all
# platforms.
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
##### From CmakeLists of freeglut


IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build (Debug or Release)" FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CURRENT_DIR}/cmake/modules/")

include_directories(
    "${COMMONSDK_DIR}/assimp/assimp-5.0.1/include" 
    "${COMMONSDK_DIR}/glad/glad-4.3-compatibility/include" 
    #"${COMMONSDK_DIR}/glew/glew-2.2.0/include" 
    "${COMMONSDK_DIR}/glfw/glfw-3.3.2/include" 
    "${COMMONSDK_DIR}/glm/glm-0.9.9.7" 
    #"${COMMONSDK_DIR}/Eigen/eigen-3.3.9" 
    "${COMMONSDK_DIR}/stb/stb_image.h_2.26/include" 
    "${COMMONSDK_DIR}/glimg/include" 
    "${COMMONSDK_DIR}/glimg/source" 
    "${COMMONSDK_DIR}/boost/boost_1_72_0" 
    "${COMMONSDK_DIR}/FreeType/FreeType-2.10.4/FreeType/include" 
    "${COMMONSDK_DIR}/.Tutorial/LearnOpenGL/include" 
    #"${COMMONSDK_DIR}/.Tutorial/Khongton/include" 
    "${CURRENT_DIR}/.localSDK/custom" 
    "${CURRENT_DIR}/.localSDK/mydevelop" )

add_definitions(-D_CRT_SECURE_NO_WARNINGS)  # to disable deprication

configure_file("${CURRENT_DIR}/configuration/root_directory.h.in" "${CURRENT_DIR}/.localSDK/configuration/local_configuration/root_directory.h")
include_directories("${CURRENT_DIR}/.localSDK/configuration")


## libraries organization
# OpenGL
find_package(OpenGL REQUIRED)
set(LIBS ${OPENGL_LIBRARIES})

#My Custom  Header File Only library
file(GLOB_RECURSE LOGLCustomHeaderFile
"${CURRENT_DIR}/.localSDK/custom/custom/learnopengl/*.h"
)
file(GLOB_RECURSE LOGLCustomSourceFile
"${CURRENT_DIR}/.localSDK/custom/custom/learnopengl/*.cpp"
)

#MyDevelop  Header File Only library
file(GLOB_RECURSE MyDevelopHeaderFile
"${CURRENT_DIR}/.localSDK/mydevelop/mydevelop/*.h"
)
file(GLOB_RECURSE MyDevelopSourceFile
"${CURRENT_DIR}/.localSDK/mydevelop/mydevelop/*.cpp"
)

#Then, other libs
set(LIBS "${LIBS}" 
    "${COMMONSDK_DIR}/.Tutorial/LearnOpenGL/libDynamic/.build/lib/${ConfigDir}/dynamicLEARNOPENGL.lib" 
    #"${COMMONSDK_DIR}/.Tutorial/Khongton/libStatic/.build/lib/${ConfigDir}/staticKHONGTON.lib" 
    "${COMMONSDK_DIR}/glad/glad-4.3-compatibility/libStatic/.build/lib/${ConfigDir}/staticGLAD.lib" 
    "${COMMONSDK_DIR}/stb/stb_image.h_2.26/libStatic/.build/lib/${ConfigDir}/staticSTBIMAGE.lib" 
    "${COMMONSDK_DIR}/assimp/assimp-5.0.1/.build/code/${ConfigDir}/assimp-vc142-mt$<$<CONFIG:Debug>:d>.lib"
    "${COMMONSDK_DIR}/glfw/glfw-3.3.2/.build/src/${ConfigDir}/glfw3dll.lib" 
    "${COMMONSDK_DIR}/glimg/.build/lib/${ConfigDir}/GLIMG.lib" 
    "${COMMONSDK_DIR}/FreeType/FreeType-2.10.4/FreeType/release dll/win64/freetype.lib" 
     )

### then create a project file
file(GLOB_RECURSE Source 
    "${CURRENT_DIR}/source/*.cpp" )
file(GLOB_RECURSE Shader 
    "${CURRENT_DIR}/shader/*.vert"
    "${CURRENT_DIR}/shader/*.frag"
    "${CURRENT_DIR}/shader/*.geom"
    "${CURRENT_DIR}/shader/*.tesc"
    "${CURRENT_DIR}/shader/*.tese"
    "${CURRENT_DIR}/shader/*.glsl")

add_executable(${ProjectID} ${Source} ${Shader} 
${LOGLCustomHeaderFile}
${MyDevelopHeaderFile}
)
source_group(Shader FILES ${Shader})
source_group(MyLibrary/MyDevelop FILES ${MyDevelopHeaderFile})
source_group(MyLibrary/LearnOPGLCustom FILES ${LOGLCustomHeaderFile})
target_link_libraries(${ProjectID} PUBLIC "${LIBS}" )
#target_link_libraries(${ProjectID} "${LIBS}" )

unset(LOGLCustomHeaderFile)
unset(LOGLCustomSourceFile)
unset(MyDevelopHeaderFile)
unset(MyDevelopSourceFile)
unset(Source)
unset(Shader)

### copy dlls    
add_custom_command(TARGET ${ProjectID} POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy "${COMMONSDK_DIR}/.Tutorial/LearnOpenGL/libDynamic/.build/bin/${ConfigDir}/dynamicLEARNOPENGL.dll" $<TARGET_FILE_DIR:${ProjectID}> 
    COMMAND ${CMAKE_COMMAND} -E copy "${COMMONSDK_DIR}/assimp/assimp-5.0.1/.build/code/${ConfigDir}/assimp-vc142-mt$<$<CONFIG:Debug>:d>.dll" $<TARGET_FILE_DIR:${ProjectID}> 
    COMMAND ${CMAKE_COMMAND} -E copy "${COMMONSDK_DIR}/glfw/glfw-3.3.2/.build/src/${ConfigDir}/glfw3.dll" $<TARGET_FILE_DIR:${ProjectID}> 
    COMMAND ${CMAKE_COMMAND} -E copy "${COMMONSDK_DIR}/FreeType/FreeType-2.10.4/FreeType/release dll/win64/freetype.dll" $<TARGET_FILE_DIR:${ProjectID}> 
    )

### if compiling for visual studio, also use configure file for each project (specifically to set up working directory)
if(MSVC)
    configure_file(${CMAKE_SOURCE_DIR}/configuration/visualstudio.vcxproj.user.in ${CMAKE_CURRENT_BINARY_DIR}/${ProjectID}.vcxproj.user @ONLY)
endif(MSVC)
