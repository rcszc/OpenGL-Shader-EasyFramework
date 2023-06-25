// ShaderFramework. windows. msvc 170.
// rcsz. version 1.5.0
 
//[2023-5-15] maths[弃用](配合glm). 增加classDelete(用于统一释放). 增加FrameBufferObject.
//[2023-5-17] Exten增加RTX检测. 可启用RT.Core光追加速.
//[2023-5-22] 转为采用三角面渲染 Model,弃用 RectModel.

#ifndef _SHADERFRAMEWORK_H
#define _SHADERFRAMEWORK_H
#include <GL/GL.h>
#include "ShaderFramework_define.hpp"
#include "framework_log/Framework_ConsoleLog.hpp"

/* 快速查看所有类:
- GCS_LoadShader, GCS_LoadTexture, GCS_LoadModelData, GCS_LoadShaderUniform, GCS_LoadFrameBuffer.
- GC_LoadInformation.
*/
#define VIEW_FRAME_CLASS 0

/* 快速查看所有函数:
- GC_ShaderTexture, GC_ShaderDrawMD, GC_ShaderEnable.
- GC_FrameBuffer_Above, GC_FrameBuffer_Below.
*/
#define VIEW_FRAME_FUNCTION 0

// 开启MSAA抗锯齿 "glEnable(GL_MULTISAMPLE)".
void PGC_MSAA(); 
//******************************** load shader ********************************
// opengl glew init shader,program.
GCUI32 _GC_ShaderGLEWinit();
GCUI32 _GC_ProgramGLEWcreate();

class GCS_LoadShader {
protected:
	// shader str data.
	GC_TEXT source_vertexShader = nullptr;
	GC_TEXT source_fragmentShader = nullptr;
	// shader index.
	GLuint shader_ver = NULL, shader_frg = NULL;
	GLuint SHADER_PROGRAM = _GC_ShaderGLEWinit();
	GCBIT repeat_compile;

	GC_CHARPTR _load_shader_file(GC_TEXT _file);                  // load file.
	GCBIT _shader_compile(GLuint _shaderhandle, GC_TEXT _shader); // compile, print errorlog.
public:
	// 重复编译: 如果shader编译失败会再编译一次.[垃圾AMD毛病]
	GCS_LoadShader(GCBIT RepeatCompile = false) {
		repeat_compile = RepeatCompile;
		// init glewshader.
		shader_ver = _GC_ShaderGLEWinit();
		shader_frg = _GC_ShaderGLEWinit();
		SHADER_PROGRAM = _GC_ProgramGLEWcreate();
	}
	~GCS_LoadShader() {
		// free.
		if (source_vertexShader != nullptr) delete[] source_vertexShader;
		if (source_fragmentShader != nullptr) delete[] source_fragmentShader;
		source_vertexShader = nullptr;
		source_fragmentShader = nullptr;
	};

	// Load_shader.
	void load_shader_vs(GC_TEXT File_vs);
	void load_shader_fs(GC_TEXT File_fs);
	GCUI32 Link_shader_program();            // Linck.shader return program_handle.
	GCUI32* GET_SHADER_INDEX(GCSIZE& datlen); // shader index 0:vertex 1:fragment.

	// delete vertex fragment shader.
	void delete_shader();
};

//******************************** load texture ********************************
// opengl gl glu, stbimg => load image_texture. 
#define GC_CHANNEL_RGB  3 // texture channels BYTE_3.
#define GC_CHANNEL_RGBA 4 // texture channels BYTE_4.

void __GLEW_TEXTUREUSE(GCUI32 number, GC_TEXINDEX& index, GC_TEXT name);
void __GLEW_TEXTUREUNBIND();

// Create Texture + Loadimage.
// Load image .jpg .png [RGB][RGBA].
class GCS_LoadTexture {
protected:
	static GCUI32 texture_limit[2];
	GCUI32 texture_index = NULL;
public:
	GCS_LoadTexture(GC_TEXT LoadTexture);
	~GCS_LoadTexture() {};

	void setload_texturelimit(GC_VEC2f setloadlimit);
	// LoadTexture => GetTexture.
	GC_TEXINDEX get_textureindex(GCUI32 program);
};

#define GC_TEXTURE_RGBA    0xF00A // Texture Color RGBA. 24bit [8*4]
#define GC_TEXTURE_HDRRGBA 0xF00B // Texture Color RGBA. 64bit [16*4]
// Create Texture 2D.
class GCS_CreateTexture {
protected:
	GCUI32 texture_index = NULL;
public:
	GCS_CreateTexture(GCUI32 TexType, GC_VEC2f TextureSize);
	~GCS_CreateTexture() {};

	// return Texture.
	GCUI32 get_textureindex();
	void unbind_texture();
};

//******************************** load shaderdata ********************************
// TraingleModel. 其他在扩展库中.
void __GLEW_SHADERDRAW_MODEL(GC_MODELINDEX& objectindex);
void __GLEW_USESHADER(GCUI32& index);

// opengl glew => loaddata -> gpu.
class GCS_LoadModelData {
public:
	// Create VA,VB object.
	// Location End = Begin + 4, [Begin, Begin + 4].
	void Load_DModelData(GC_DynamicModel input, GC_MODELINDEX& objectindex, GCUI32 begin);
};
extern GCS_LoadModelData GCSF_TransmitData;

// Persets Data.
extern GCFP32 ShaderTestCube[(GC_VERTEX_LENGTH) * 4];
extern GCSIZE ShaderTestCubeLen;

// 用于FBO渲染到整个屏幕纹理.
GCFP32* FBO_RDwindowCube(GC_VEC2f WindowCoord);
extern GCSIZE FBO_RDwindowCubeLen;

//******************************** load FBO ********************************
// FrameBufferObject.
void __GLEW_FBOBIND(GCUI32& FBO);
void __GLEW_FBOUNBIND();
void __GLEW_CLEARBUFFER();

// FBO 操作较为细化. 
class GCS_LoadFrameBuffer {
protected:
	GCUI32 framebuffer_index = NULL;
public:
	GCS_LoadFrameBuffer();
	~GCS_LoadFrameBuffer() {};

	// 将FBO绑定到Texture.
	void bind_framebuffer(GCUI32 texture);
	// return FBO + UnBind FBO.
	GCUI32 get_framebufferindex();
};

//******************************** shader Uniform ********************************
// Shader GobalValue.
class GCS_LoadShaderUniform {
public:
	void sUniformMat3(GCUI32 program, GC_TEXT name, GC_Matrix3 matrix);
	void sUniformMat4(GCUI32 program, GC_TEXT name, GC_Matrix4 matrix);
	void sUniform1f(GCUI32 program, GC_TEXT name, GCFP32 value);
	void sUniform2f(GCUI32 program, GC_TEXT name, GC_VEC2f value);
	void sUniform3f(GCUI32 program, GC_TEXT name, GC_VEC3f value);
	void sUniform4f(GCUI32 program, GC_TEXT name, GC_VEC4f value);
	void sUniform1i(GCUI32 program, GC_TEXT name, GCI32 value);
};
extern GCS_LoadShaderUniform GCSF_GlobalValue;

//******************************** load information ********************************
// command: sysinfo GPU [获取GPU信息]. NotOut.
// command: sysinfo Attribs [获取"location"最大值].
// command: sysinfo Texture [获取"TxetureUnits"最大值].
// command: sysinfo MEMcpiy [获取显存容量].
// command: sysinfo MEMsize [获取显存余量].
class GC_LoadInformation {
protected:
	GCSIZE _tempvalue = NULL;
	GCBIT _print_switch = false;

	void _info_Command(GC_TEXT _command); // interpret instruction.
	void _info_GetAttribsMax();
	void _info_GetTextureMax();
	void _info_GetGPUMemory(GCBIT _type);
	void _info_GetGPUParameter();
public:
	GC_LoadInformation(GC_TEXT loadcomm, GCBIT printsw = true) {
		_print_switch = printsw;
		_info_Command(loadcomm);
	}
	~GC_LoadInformation() {};

	GCSIZE get_sizevalue(); // get.value GCSIZE.
};

//******************************** delete ********************************
class GC_DeleteGLindex {
public:
	void Delete_ShaderProgram(GCUI32& program);      // free shader program.
	void Delete_ModelObject(GC_MODELINDEX& model);   // free model.
	void Delete_TextureObject(GC_TEXINDEX& texture); // free texture.
	void Delete_FrameBufferObject(GCUI32& fbo);      // free frame buffer object.
};
extern GC_DeleteGLindex GCSF_DeleteIndex;

// Texture Units number, Texture index, Program index, Uniform name [Sample].
#define GC_ShaderTexture       __GLEW_TEXTUREUSE
#define GC_ShaderTextureUnBind __GLEW_TEXTUREUNBIND

#define GC_ShaderDrawMD __GLEW_SHADERDRAW_MODEL // draw model.
#define GC_ShaderEnable __GLEW_USESHADER        // use shader program.

#define GC_FrameBuffer_Above __GLEW_FBOBIND   // 开始FBO操作.
#define GC_FrameBuffer_Below __GLEW_FBOUNBIND // 结束FBO操作(并切换回默认FBO).

#define GC_ClearFrameBuffer __GLEW_CLEARBUFFER // 清理帧缓冲.

#endif