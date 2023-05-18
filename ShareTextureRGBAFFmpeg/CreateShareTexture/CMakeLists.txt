#Cmake 最低版本号要求
cmake_minimum_required(VERSION 2.8)

# 项目信息
project (Demo) # 项目名后面可以使用变量${PROJECT_NAME}进行引用。

# 查找目录下的所有源文件
# 并将名称保存到DIR_SRCS变量
aux_source_directory(. DIR_SRCS)
list(APPEND DIR_SRCS ${PROJECT_SOURCE_DIR}/Resource.rc)
#aux_source_directory(${PROJECT_SOURCE_DIR}/../include direct3dinclude)
include_directories(../../include)
include_directories(../../ffmpeg-5.1.2/include)

# 设置子系统为窗口程序
set ( CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS" ) 

#项目使用UNICODE版本。win中的各种API将使用UNICODE版本。
add_definitions("-DUNICODE" "-D_UNICODE")

# 选择x64还是x86
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	link_directories(${PROJECT_SOURCE_DIR}/../../Lib/x64)
else()
	link_directories(${PROJECT_SOURCE_DIR}/../../Lib/x86)
endif()
link_directories(${PROJECT_SOURCE_DIR}/../../ffmpeg-5.1.2/lib)


# 指定生成目标
add_executable(${PROJECT_NAME} ${DIR_SRCS})

# 添加链接库
target_link_libraries(${PROJECT_NAME} d3d11.lib d3dcompiler.lib d3dx11d.lib d3dx9d.lib dxerr.lib dxguid.lib winmm.lib comctl32.lib
	avcodec.lib avdevice.lib avfilter.lib avformat.lib avutil.lib postproc.lib swresample.lib swscale.lib
)

add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/guilin_640x360_rgba.rgb ${PROJECT_BINARY_DIR}/$<CONFIGURATION>
  )

add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/RectTexture.fx ${PROJECT_BINARY_DIR}/$<CONFIGURATION>
  )
  
add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/guilin_640x360_rgba.rgb ${CMAKE_CURRENT_BINARY_DIR}
  ) 

add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/RectTexture.fx ${CMAKE_CURRENT_BINARY_DIR}
  )
  

add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/../../ffmpeg-5.1.2/bin/avcodec-59.dll ${PROJECT_BINARY_DIR}/$<CONFIGURATION>
  )
add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/../../ffmpeg-5.1.2/bin/avdevice-59.dll ${PROJECT_BINARY_DIR}/$<CONFIGURATION>
  )
add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/../../ffmpeg-5.1.2/bin/avfilter-8.dll ${PROJECT_BINARY_DIR}/$<CONFIGURATION>
  )  
add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/../../ffmpeg-5.1.2/bin/avformat-59.dll ${PROJECT_BINARY_DIR}/$<CONFIGURATION>
  )
add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/../../ffmpeg-5.1.2/bin/avutil-57.dll ${PROJECT_BINARY_DIR}/$<CONFIGURATION>
  )
add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/../../ffmpeg-5.1.2/bin/postproc-56.dll ${PROJECT_BINARY_DIR}/$<CONFIGURATION>
  )
add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/../../ffmpeg-5.1.2/bin/swresample-4.dll ${PROJECT_BINARY_DIR}/$<CONFIGURATION>
  )
add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/../../ffmpeg-5.1.2/bin/swscale-6.dll ${PROJECT_BINARY_DIR}/$<CONFIGURATION>
  )


add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/../../ffmpeg-5.1.2/bin/avcodec-59.dll ${PROJECT_BINARY_DIR}
  )
add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/../../ffmpeg-5.1.2/bin/avdevice-59.dll ${PROJECT_BINARY_DIR}
  )
add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/../../ffmpeg-5.1.2/bin/avfilter-8.dll ${PROJECT_BINARY_DIR}
  )  
add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/../../ffmpeg-5.1.2/bin/avformat-59.dll ${PROJECT_BINARY_DIR}
  )
add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/../../ffmpeg-5.1.2/bin/avutil-57.dll ${PROJECT_BINARY_DIR}
  )
add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/../../ffmpeg-5.1.2/bin/postproc-56.dll ${PROJECT_BINARY_DIR}
  )
add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/../../ffmpeg-5.1.2/bin/swresample-4.dll ${PROJECT_BINARY_DIR}
  )
add_custom_command(TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/../../ffmpeg-5.1.2/bin/swscale-6.dll ${PROJECT_BINARY_DIR}
  )