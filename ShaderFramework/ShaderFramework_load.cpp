// ShaderFramework_load.
#include <fstream>
#include <string>

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include "ShaderFramework.h"

using namespace std;
using namespace LOGCONS;

/*
@ ******************************** load shader ********************************
@ opengl glew shader VersionMax 4.6
*/
#define SHADER_INFOHEAD     "[ShaderProgram]: "
#define SHADER_ERROR        "GLcompiler: "
#define SHADER_LOGLEN       512  // log length [char].
#define SHADER_SOURCE_COUNT 1    // 每次只读1着色器文件.

GCUI32 _GC_ShaderGLEWinit() { return glewInit(); }            // glew.init.
GCUI32 _GC_ProgramGLEWcreate() { return glCreateProgram(); }; // glew.init.

void PGC_MSAA() { glEnable(GL_MULTISAMPLE); }; // 启用多重采样抗锯齿.

GC_CHARPTR GCS_LoadShader::_load_shader_file(GC_TEXT _file) {
	fstream _load_ShaderData(_file);
	string _TextDataTemp = {};
	GC_CHARPTR _ReturnSourceData = {};
	GCCHAR _chardata = {};

	if (_load_ShaderData) {
		Log << Log.SET(CLOG_INFO) << SHADER_INFOHEAD << "load.Shader File: " << _file << LogEnd;
		while (_load_ShaderData.get(_chardata)) { // read_source.
			_TextDataTemp.push_back(_chardata);
		}
		_TextDataTemp.push_back(TEXTSTR_END); // add_endflag.

		GCSIZE SourceLength = _TextDataTemp.size();
		_ReturnSourceData = new GCCHAR[SourceLength];

		for (GCSIZE i = 0; i < SourceLength; i++)
			_ReturnSourceData[i] = _TextDataTemp[i];
	}
	else
		Log << Log.SET(CLOG_ERROR) << SHADER_INFOHEAD << "load.Shader Failed: " << _file << LogEnd;
	return _ReturnSourceData;
}

GCBIT GCS_LoadShader::_shader_compile(GLuint _shaderindex, GC_TEXT _lable) {
	GCBIT compile_state = true;
	GCI32 _compile_success = NULL;
	GCI32 _shaderloglen = NULL;
	GC_CHARPTR _shader_infoLog = {};

	if (_lable != "program") { // shader.
		glGetShaderiv(_shaderindex, GL_COMPILE_STATUS, &_compile_success);
		glGetShaderiv(_shaderindex, GL_INFO_LOG_LENGTH, &_shaderloglen); // log length.
		if (!_compile_success) {
			compile_state = false;

			_shader_infoLog = new GCCHAR[_shaderloglen];
			glGetShaderInfoLog(_shaderindex, _shaderloglen, NULL, _shader_infoLog);

			// print error.
			string error_info = SHADER_ERROR;
			if (_lable != nullptr)
				error_info += _lable;
			Log << Log.SET(CLOG_ERROR) << error_info.c_str() << " " << _shader_infoLog << LogEnd;

			delete[] _shader_infoLog;
		}
		else
			Log << Log.SET(CLOG_INFO) << SHADER_INFOHEAD << _lable << " Compilation Succeeded." << LogEnd;
	}
	else { // shader program.
		glGetProgramiv(_shaderindex, GL_LINK_STATUS, &_compile_success);
		glGetProgramiv(_shaderindex, GL_INFO_LOG_LENGTH, &_shaderloglen);
		if (!_compile_success) {
			compile_state = false;

			_shader_infoLog = new GCCHAR[_shaderloglen];
			glGetProgramInfoLog(_shaderindex, _shaderloglen, NULL, _shader_infoLog);

			// print error.
			string error_info = SHADER_ERROR;
			if (_lable != nullptr)
				error_info += _lable;
			Log << Log.SET(CLOG_ERROR) << error_info.c_str() << " " << _shader_infoLog << LogEnd;

			delete[] _shader_infoLog;
		}
		else
			Log << Log.SET(CLOG_INFO) << SHADER_INFOHEAD << _lable << " CompilationLink Succeeded." << LogEnd;
	}
	return compile_state;
}

void GCS_LoadShader::load_shader_vs(GC_TEXT file_vs) {
	shader_ver = glCreateShader(GL_VERTEX_SHADER);
	{
		source_vertexShader = _load_shader_file(file_vs);
		glShaderSource(shader_ver, SHADER_SOURCE_COUNT, &source_vertexShader, NULL);
		glCompileShader(shader_ver);
	}

	// verts compile loop.
	for (GCI32 i = 0; i < repeat_compile + 1; i++) {
		GCBIT _state = _shader_compile(shader_ver, "vertex"); // vertex shader info.
		if ((repeat_compile + 1 > 1) && _state)
			break;
		else if ((i == NULL) && repeat_compile)
			Log << Log.SET(CLOG_TRACE) << SHADER_INFOHEAD << "vs_RepeatCompile..." << LogEnd;
	}
	glAttachShader(SHADER_PROGRAM, shader_ver);
}

void GCS_LoadShader::load_shader_fs(GC_TEXT File_fs) {
	shader_frg = glCreateShader(GL_FRAGMENT_SHADER);
	{
		source_fragmentShader = _load_shader_file(File_fs);
		glShaderSource(shader_frg, SHADER_SOURCE_COUNT, &source_fragmentShader, NULL);
		glCompileShader(shader_frg);
	}

	// frags compile loop.
	for (GCI32 i = 0; i < repeat_compile + 1; i++) {
		GCBIT _state = _shader_compile(shader_frg, "fragment"); // fragment shader info.
		if ((repeat_compile + 1 > 1) && _state)
			break;
		else if ((i == NULL) && repeat_compile)
			Log << Log.SET(CLOG_TRACE) << SHADER_INFOHEAD << "fs_RepeatCompile..." << LogEnd;
	}
	glAttachShader(SHADER_PROGRAM, shader_frg);
}

GCUI32 GCS_LoadShader::Link_shader_program() {
	// fragment,vertex => program.
	glLinkProgram(SHADER_PROGRAM);
	_shader_compile(SHADER_PROGRAM, "program");
	return SHADER_PROGRAM;
}

GCUI32* GCS_LoadShader::GET_SHADER_INDEX(GCSIZE& datlen) {
	static GLuint retshader[2] = {};
	datlen = 2;
	retshader[0] = shader_ver;
	retshader[1] = shader_frg;
	return retshader;
}

void GCS_LoadShader::delete_shader() {
	glDeleteShader(shader_ver);
	glDeleteShader(shader_frg);
}

/*
@ ******************************** load texture ********************************
@ image texture .jpg RGB .png RGBA
*/
#define TEXTURE_INFOHEAD "[Texture]: "

inline GCBIT _TESTFILE(GC_TEXT _File) {
	GCBIT _returnstate = false;
	fstream _TestFile(_File);
	if (_TestFile) _returnstate = true; // Test file exists.
	_TestFile.close();
	return _returnstate;
}

void __GLEW_TEXTUREUSE(GCUI32 number, GC_TEXINDEX& index, GC_TEXT name) {
	glActiveTexture(GL_TEXTURE0 + number);       // 激活纹理单元.
	glBindTexture(GL_TEXTURE_2D, index.Texture); // 绑定纹理到当前上下文.

	// Texture => Shader TextureUniform.
	GCSF_GlobalValue.sUniform1i(index.Program, name, number);
}
void __GLEW_TEXTUREUNBIND() {
	// opengl state UnBind.Texture
	glBindTexture(GL_TEXTURE_2D, NULL);
}

// opengl,stb_image load texture. [.jpg.png]
#define STB_IMAGE_IMPLEMENTATION
#include "../../graphics_library/stb_image.h"

GCUI32 GCS_LoadTexture::texture_limit[2] = { 1024,1024 };

GCS_LoadTexture::GCS_LoadTexture(GC_TEXT _file) {
	GCUI32 returnTexture = NULL;
	GCI32 _width = NULL, _height = NULL, _nrChannels = NULL;
	// GCI32 _glerr = NULL;
	if (_TESTFILE(_file)) {

		GCBYTE* GetImageData = stbi_load(_file, &_width, &_height, &_nrChannels, NULL);
		Log << Log.SET(CLOG_INFO) << TEXTURE_INFOHEAD << "load.Texture File: " << _file << LogEnd;
		Log << Log.SET(CLOG_INFO) << TEXTURE_INFOHEAD << "Image Width: " << _width << LogEnd;
		Log << Log.SET(CLOG_INFO) << TEXTURE_INFOHEAD << "Image Height: " << _height << LogEnd;
		Log << Log.SET(CLOG_INFO) << TEXTURE_INFOHEAD << "Image ColorChannel: " << _nrChannels << LogEnd;

		if ((GCUI32)_width > texture_limit[0] || (GCUI32)_height > texture_limit[1]) { // LimitSize.
			Log << Log.SET(CLOG_WARRING) << TEXTURE_INFOHEAD << "load.Texture > Limit(" <<
				texture_limit[0] << " x " << texture_limit[1] << ")" << LogEnd;
		}

		glGenTextures(1, &returnTexture);
		glBindTexture(GL_TEXTURE_2D, returnTexture);

		if (returnTexture < 1) // opengl index > 1.
			Log << Log.SET(CLOG_WARRING) << TEXTURE_INFOHEAD << "create.Texture Failed." << LogEnd;

		// [弃用项].
		// gluBuild2DMipmaps(GL_TEXTURE_2D, _nrChannels, _width, _height, GL_RGBA, GL_UNSIGNED_BYTE, GetImageData);
		if (_nrChannels > GC_CHANNEL_RGB)
			glTexImage2D(GL_TEXTURE_2D, NULL, _nrChannels, _width, _height, NULL, GL_RGBA, GL_UNSIGNED_BYTE, GetImageData);
		else glTexImage2D(GL_TEXTURE_2D, NULL, _nrChannels, _width, _height, NULL, GL_RGB, GL_UNSIGNED_BYTE, GetImageData);

		// opengl load texture init.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// 生成mipmap级贴图.
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, NULL); // 解绑纹理.
		stbi_image_free(GetImageData);      // freeimgdata.

		/* gluBuild2DMipmaps, load texture image error.
		if (_glerr != NULL)
			Log << Log.SET(CLOG_ERROR) << TEXTURE_INFOHEAD << "OpenGLglu Code: " << _glerr << LogEnd; */
	}
	else
		Log << Log.SET(CLOG_ERROR) << TEXTURE_INFOHEAD << "load.Texture Failed: " << _file << LogEnd;
	texture_index = returnTexture;
}

GC_TEXINDEX GCS_LoadTexture::get_textureindex(GCUI32 program) {
	GC_TEXINDEX returnindex = GC_TEXINDEX(texture_index, program);
	return returnindex;
}

void GCS_LoadTexture::setload_texturelimit(GC_VEC2f setloadlimit) {
	if (setloadlimit.vector_x > 0.0f && setloadlimit.vector_y > 0.0f) {
		// soft conversion.[warr]
		texture_limit[0] = (GCUI32)setloadlimit.vector_x;
		texture_limit[1] = (GCUI32)setloadlimit.vector_x;
	}
	else
		Log << Log.SET(CLOG_WARRING) << TEXTURE_INFOHEAD << "Set load.TextureLimit value > 0.0f" << LogEnd;
}

// ################ Create ################.
GCS_CreateTexture::GCS_CreateTexture(GCUI32 TexType, GC_VEC2f TextureSize) {
	glGenTextures(1, &texture_index);
	glBindTexture(GL_TEXTURE_2D, texture_index);

	if (TexType == GC_TEXTURE_RGBA) { // RGBA 32bit 1byte * 4.
		glTexImage2D(GL_TEXTURE_2D, NULL, GL_RGBA, 
			(GCSIZE)TextureSize.vector_x, (GCSIZE)TextureSize.vector_y, NULL, GL_RGBA, GL_FLOAT, nullptr);
		Log << Log.SET(CLOG_INFO) << TEXTURE_INFOHEAD << "create.Texture RGBA" << LogEnd;
	}
	if (TexType == GC_TEXTURE_HDRRGBA) { // RGBA 64bit 2byte * 4.
		glTexImage2D(GL_TEXTURE_2D, NULL, GL_RGBA16F, 
			(GCSIZE)TextureSize.vector_x, (GCSIZE)TextureSize.vector_y, NULL, GL_RGBA, GL_FLOAT, nullptr);
		Log << Log.SET(CLOG_INFO) << TEXTURE_INFOHEAD << "create.Texture HDR_RGBA" << LogEnd;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
}

GCUI32 GCS_CreateTexture::get_textureindex() {
	return texture_index;
}
void GCS_CreateTexture::unbind_texture() {
	// opengl state UnBind.Texture
	glBindTexture(GL_TEXTURE_2D, NULL);
}

/*
@ ******************************** load shaderdata ********************************
@ Main: load RECTmodel. Ext: Line, Point.
@ Global Object Class.
*/
#define INFO_SDATAHEAD "[ShaderData]: "
void __GLEW_SHADERDRAW_MODEL(GC_MODELINDEX& objectindex) {
	glBindVertexArray(objectindex.VAOindex);
	glDrawArrays(GL_QUADS, NULL, 4);
};
void __GLEW_USESHADER(GCUI32& index) {
	glUseProgram(index);
}
// vertex data => opengl index.
inline void __GLEW_DataIndex(GCUI32 vertex_length_bytes, GCUI32 LBegin) {
	// vertex_data.poscoord 3 * float, bias = 0, Location = 0.
	// vertex_block.begin = 0, vertex_block.end = 2.
	glVertexAttribPointer(LBegin + 0, GC_VERTEX_COORD, GL_FLOAT, GL_FALSE, vertex_length_bytes,
		(void*)0); // bias_pointer.
	glEnableVertexAttribArray(LBegin + 0);

	// vertex_data.color 4 * float, bias = 0 + 3, Location = 1.
	// vertex_block.begin = 3, vertex_block.end = 6.
	glVertexAttribPointer(LBegin + 1, GC_VERTEX_COLOR, GL_FLOAT, GL_FALSE, vertex_length_bytes,
		(void*)(3 * GC_SIZE_FP32)); // bias_pointer.
	glEnableVertexAttribArray(LBegin + 1);

	// vertex_data.texture 2 * float, bias = 0 + 3 + 4, Location = 2.
	// vertex_block.begin = 7, vertex_block.end = 9.
	glVertexAttribPointer(LBegin + 2, GC_VERTEX_TEXUV, GL_FLOAT, GL_FALSE, vertex_length_bytes,
		(void*)(7 * GC_SIZE_FP32)); // bias_pointer.
	glEnableVertexAttribArray(LBegin + 2);

	// vertex_data.normal 3 * float, bias = 0 + 3 + 4 + 2, Location = 3.
	// vertex_block.begin = 9, vertex_block.end = 11.
	glVertexAttribPointer(LBegin + 3, GC_VERTEX_COORD, GL_FLOAT, GL_FALSE, vertex_length_bytes,
		(void*)(9 * GC_SIZE_FP32)); // bias_pointer.
	glEnableVertexAttribArray(LBegin + 3);
}

// vertex data => opengl. VAO VBO.
inline void __GLEW_DataLoad(GCUI32& VAO, GCUI32& VBO, GCUI32 data_length_byte, GCFP32* vertex_data, GCUI32 lbegin) {
	// Create VAO, VBO.
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);             // bind vertex.array
	glBindBuffer(GL_ARRAY_BUFFER, VBO); // bind vertex.buffer

	glBufferData(GL_ARRAY_BUFFER, data_length_byte, vertex_data, GL_STATIC_DRAW);
	// "Location" 0~3.
	// data struct: { pos, color, textureUV, normal }.
	__GLEW_DataIndex(GC_VERTEX_BYTES, lbegin);

	glBindVertexArray(NULL); // unbind vertex array.
	glBindBuffer(GL_ARRAY_BUFFER, NULL);

	Log << Log.SET(CLOG_INFO) << INFO_SDATAHEAD << "TransmitData VertexGroup: " << data_length_byte << " bytes" << LogEnd;
}
GCS_LoadModelData GCSF_TransmitData;

void GCS_LoadModelData::Load_DModelData(GC_DynamicModel input, GC_MODELINDEX& objectindex, GCUI32 begin) {
	// Load_ModelData => __GLEW_DataLoad -> __GLEW_DataIndex.
	__GLEW_DataLoad(objectindex.VAOindex, objectindex.VBOindex,
		(GCUI32)input.ModelDataLength * GC_SIZE_FP32, input.vertexData, begin);
}

/*
@ ******************************** shader Uniform ********************************
@ Global Object Class.
*/
// glGetUniformLocation => glUniformMatrix3fv.
// GCFP32_MatrixGroup.
void GCS_LoadShaderUniform::sUniformMat3(GCUI32 program, GC_TEXT name, GC_Matrix3 matrix) {
	// Maths - Matrix. GCFP32.[3 x 3]. ptr. 
	GCI32 uniformLocation = glGetUniformLocation(program, name);
	glUniformMatrix3fv(uniformLocation, 1, false, matrix.Matrix);
}

void GCS_LoadShaderUniform::sUniformMat4(GCUI32 program, GC_TEXT name, GC_Matrix4 matrix) {
	// Maths - Matrix. GCFP32.[4 x 4]. ptr. 
	GCI32 uniformLocation = glGetUniformLocation(program, name);
	glUniformMatrix4fv(uniformLocation, 1, false, matrix.Matrix);
}

// GCFP32_VECf32.value.
void GCS_LoadShaderUniform::sUniform1f(GCUI32 program, GC_TEXT name, GCFP32 value) {
	GCI32 uniformLocation = glGetUniformLocation(program, name);
	glUniform1f(uniformLocation, value);
}

void GCS_LoadShaderUniform::sUniform2f(GCUI32 program, GC_TEXT name, GC_VEC2f value) {
	GCI32 uniformLocation = glGetUniformLocation(program, name);
	glUniform2f(uniformLocation, value.vector_x, value.vector_y);
}

void GCS_LoadShaderUniform::sUniform3f(GCUI32 program, GC_TEXT name, GC_VEC3f value) {
	GCI32 uniformLocation = glGetUniformLocation(program, name);
	glUniform3f(uniformLocation, value.vector_x, value.vector_y, value.vector_z);
}

void GCS_LoadShaderUniform::sUniform4f(GCUI32 program, GC_TEXT name, GC_VEC4f value) {
	GCI32 uniformLocation = glGetUniformLocation(program, name);
	glUniform4f(uniformLocation, value.vector_x, value.vector_y, value.vector_z, value.vector_w);
}

// GCFP32_int32.value. 
void GCS_LoadShaderUniform::sUniform1i(GCUI32 program, GC_TEXT name, GCI32 value) {
	GCI32 uniformLocation = glGetUniformLocation(program, name);
	glUniform1i(uniformLocation, value);
}
GCS_LoadShaderUniform GCSF_GlobalValue;

/*
@ ******************************** FrameBufferObject ********************************
@ Texture =Bind=> FrameBufferObject.
*/
void __GLEW_FBOBIND(GCUI32& FBO) {
	// opengl state Bind.FrameBufferObject
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
};
void __GLEW_FBOUNBIND() {
	// opengl state UnBind.FrameBufferObject
	// switch to default rendering texture.
	glBindFramebuffer(GL_FRAMEBUFFER, NULL);
	glBindFramebuffer(GL_FRAMEBUFFER, NULL);
};

void __GLEW_CLEARBUFFER() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

GCS_LoadFrameBuffer::GCS_LoadFrameBuffer() {
	// Create FrameBufferObject.
	glGenFramebuffers(1, &framebuffer_index);
}

void GCS_LoadFrameBuffer::bind_framebuffer(GCUI32 texture) {
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_index);
	// FrameBufferObject => Texture.
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, NULL);
	// 检查缓冲帧是否完整.
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		Log << Log.SET(CLOG_ERROR) << "Incomplete FrameBuffer!" << LogEnd;
}

GCUI32 GCS_LoadFrameBuffer::get_framebufferindex() {
	// opengl state UnBind.FrameBufferObject
	glBindFramebuffer(GL_FRAMEBUFFER, NULL);
	return framebuffer_index;
}

/*
@ ******************************** load information ********************************
@ opengl glew => gpu_system.
*/
#define INFO_INFOHEAD "[SystemInfo]: "

void GC_LoadInformation::_info_Command(GC_TEXT _command) {
	if (_command == "sysinfo GPU")     _info_GetGPUParameter();
	if (_command == "sysinfo Attribs") _info_GetAttribsMax();
	if (_command == "sysinfo Texture") _info_GetTextureMax();
	if (_command == "sysinfo MEMcpiy") _info_GetGPUMemory(false);
	if (_command == "sysinfo MEMsize") _info_GetGPUMemory(true);
}

void GC_LoadInformation::_info_GetAttribsMax() {
	GLint nrAttributes = NULL;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	_tempvalue = (GCSIZE)nrAttributes;

	if (_print_switch)
		Log << Log.SET(CLOG_INFO) << INFO_INFOHEAD << "AttribsMax[Location]: " << nrAttributes << " Entry" << LogEnd;
}

void GC_LoadInformation::_info_GetTextureMax() {
	GLint nrTexture = NULL;
	// GL_MAX_TEXTURE_UNITS.       - 基本纹理单元数量(可用于固定管线).
	// GL_MAX_TEXTURE_IMAGE_UNITS. - 可以用于Shader的纹理单元数量.
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &nrTexture);
	_tempvalue = (GCSIZE)nrTexture;

	if (_print_switch)
		Log << Log.SET(CLOG_INFO) << INFO_INFOHEAD << "TxetureMax[Units]: " << nrTexture << " Entry" << LogEnd;
}

void GC_LoadInformation::_info_GetGPUMemory(GCBIT _type) {
	GLint MemoryKb = NULL;
	if (!_type) glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &MemoryKb);   // MemCapacity.
	else        glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &MemoryKb); // MemSize.
	_tempvalue = (GCSIZE)MemoryKb;

	if (_print_switch) {
		if (!_type) Log << Log.SET(CLOG_PERF) << INFO_INFOHEAD << "MEM[Capacity]: " << MemoryKb / 1024 << " Mib" << LogEnd;
		else        Log << Log.SET(CLOG_PERF) << INFO_INFOHEAD << "MEM[Size]: " << MemoryKb / 1024 << " Mib" << LogEnd;
	}
}

void GC_LoadInformation::_info_GetGPUParameter() {
	if (_print_switch) {
		Log << Log.SET(CLOG_INFO) << INFO_INFOHEAD << "GPU Supplier:   " << glGetString(GL_VENDOR) << LogEnd;
		Log << Log.SET(CLOG_INFO) << INFO_INFOHEAD << "GPU Model:      " << glGetString(GL_RENDERER) << LogEnd;
		Log << Log.SET(CLOG_INFO) << INFO_INFOHEAD << "OpenGL Version: " << glGetString(GL_VERSION) << LogEnd;
		Log << Log.SET(CLOG_INFO) << INFO_INFOHEAD << "GLSL Version:   " << glGetString(GL_SHADING_LANGUAGE_VERSION) << LogEnd;
	}
}
// getnumber => outvalue.
GCSIZE GC_LoadInformation::get_sizevalue() { return _tempvalue; };

/*
@ ******************************** Delete ********************************
@ Global Object Class.
*/
#define INFO_DELETEHEAD "[Delete]: "

void GC_DeleteGLindex::Delete_ShaderProgram(GCUI32& program) {
	// opengl [GLEW_GET_FUN(__glewDeleteProgram)].
	Log << Log.SET(CLOG_INFO) << INFO_DELETEHEAD << "Delete ShaderProgram object. ID:"
		<< program << LogEnd;
	glDeleteProgram(program);
}

void GC_DeleteGLindex::Delete_ModelObject(GC_MODELINDEX& modelobject) {
	Log << Log.SET(CLOG_INFO) << INFO_DELETEHEAD << "Delete VAO,VBO object. ID:"
		<< modelobject.VAOindex << "," << modelobject.VBOindex << LogEnd;
	glDeleteVertexArrays(1, &modelobject.VAOindex);
	glDeleteBuffers(1, &modelobject.VBOindex);
}

void GC_DeleteGLindex::Delete_TextureObject(GC_TEXINDEX& texture) {
	// opengl [GLAPI void GLAPIENTRY].
	Log << Log.SET(CLOG_INFO) << INFO_DELETEHEAD << "Delete Texture object. ID:" 
		<< texture.Texture << LogEnd;
	glDeleteTextures(1, &texture.Texture);
}

void GC_DeleteGLindex::Delete_FrameBufferObject(GCUI32& fbo) {
	Log << Log.SET(CLOG_INFO) << INFO_DELETEHEAD << "Delete FBO. ID:"
		<< fbo << LogEnd;
	glDeleteFramebuffers(1, &fbo);
}
GC_DeleteGLindex GCSF_DeleteIndex;

//******************************** Perset Model ********************************
// TestCUBE shaderdata. [persets]
// vertex: pos, color, uv, normal. [*4]
GCFP32 ShaderTestCube[48] = {
	-10.0f, -10.0f,0.0f, 1.0f,0.0f,0.7f,1.0f, 0.0f,0.0f, 0.0f,0.0f,0.0f,
	 10.0f, -10.0f,0.0f, 1.0f,0.0f,0.7f,1.0f, 1.0,0.0f, 0.0f,0.0f,0.0f,
	 10.0f,  10.0f,0.0f, 1.0f,0.0f,0.7f,1.0f, 1.0f,1.0f, 0.0f,0.0f,0.0f,
	-10.0f,  10.0f,0.0f, 1.0f,0.0f,0.7f,1.0f, 0.0f,1.0f, 0.0f,0.0f,0.0f
};
GCSIZE ShaderTestCubeLen = 48;

GCFP32* FBO_RDwindowCube(GC_VEC2f WindowCoord) {
	static GCFP32 ShaderWindowRD[48] = {
	-WindowCoord.vector_x, -WindowCoord.vector_y,0.0f, 1.0f,0.0f,0.7f,1.0f, 0.0f,0.0f, 0.0f,0.0f,0.0f,
	 WindowCoord.vector_x, -WindowCoord.vector_y,0.0f, 1.0f,0.0f,0.7f,1.0f, 1.0,0.0f, 0.0f,0.0f,0.0f,
	 WindowCoord.vector_x,  WindowCoord.vector_y,0.0f, 1.0f,0.0f,0.7f,1.0f, 1.0f,1.0f, 0.0f,0.0f,0.0f,
	-WindowCoord.vector_x,  WindowCoord.vector_y,0.0f, 1.0f,0.0f,0.7f,1.0f, 0.0f,1.0f, 0.0f,0.0f,0.0f
	};
	return ShaderWindowRD;
}
GCSIZE FBO_RDwindowCubeLen = 48;