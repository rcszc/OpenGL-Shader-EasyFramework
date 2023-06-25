// ShaderFramework_extend.
#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include "ShaderFramework_extend.h"
#include "../ShaderFramework.h"

using namespace LOGCONS;

void ShaderSetPointSize() { glEnable(GL_PROGRAM_POINT_SIZE); };
/*
@ ******************************** load shaderdata ********************************
*/
namespace ShaderExt {
#define INFO_SDATAHEAD_ext "[ShaderData](Ext): "

	void __GLEW_SHADERDRAW_POINTS(GC_MODELINDEX& objectindex) {
		glBindVertexArray(objectindex.VAOindex);
		glDrawArrays(GL_POINTS, NULL, 1);
	};

	void __GLEW_SHADERDRAW_DULINES(GC_MODELINDEX& objectindex) {
		glBindVertexArray(objectindex.VAOindex);
		glDrawArrays(GL_LINES, NULL, 2);
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
	}

	// vertex data => opengl. VAO VBO.
	inline void __GLEW_DataLoad(GCUI32& VAO, GCUI32& VBO, GCUI32 data_length_byte, GCFP32* vertex_data, GCUI32 lbegin) {
		// Create VAO, VBO.
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);             // bind vertex.array
		glBindBuffer(GL_ARRAY_BUFFER, VBO); // bind vertex.buffer

		glBufferData(GL_ARRAY_BUFFER, data_length_byte, vertex_data, GL_STATIC_DRAW);
		// "Location" 0~1.
		// data struct: { pos, color, textureUV, normal }.
		__GLEW_DataIndex(GC_VERTEX_BYTES, lbegin);

		glBindVertexArray(NULL); // unbind vertex array.
		glBindBuffer(GL_ARRAY_BUFFER, NULL);

		Log << Log.SET(CLOG_INFO) << INFO_SDATAHEAD_ext << "TransmitData VertexGroup: " << data_length_byte << " bytes" << LogEnd;
	}

	void Load_PointsData::Load_DPointsData(GC_DynamicPoints input, GC_MODELINDEX& objectindex, GCUI32 begin) {
		// Load_ModelData => __GLEW_DataLoad -> __GLEW_DataIndex.
		__GLEW_DataLoad(objectindex.VAOindex, objectindex.VBOindex,
			(GCUI32)input.PointsDataLength * GC_SIZE_FP32, input.vertexData, begin);
	}

	namespace ShaderData {
		void OpenGL_CreateBindObjectAB(GCUI32& VAO, GCUI32& VBO, GCUI32 data_length_byte, GCFP32* vertex_data) {
			glGenVertexArrays(1, &VAO);
			glGenBuffers(1, &VBO);

			glBindVertexArray(VAO);             // bind vertex.array
			glBindBuffer(GL_ARRAY_BUFFER, VBO); // bind vertex.buffer

			glBufferData(GL_ARRAY_BUFFER, data_length_byte, vertex_data, GL_STATIC_DRAW);
			Log << Log.SET(CLOG_INFO) << INFO_SDATAHEAD_ext << "TransmitData VertexGroup: " << data_length_byte << " bytes" << LogEnd;
		}

		void OpenGL_UnBindObjectAB() {
			glBindVertexArray(NULL);
			glBindBuffer(GL_ARRAY_BUFFER, NULL);
		}

		void OpenGL_VertexAttribPointer(GCUI32 location, GCUI32 vertex_size, GCUI32 vertex_group_size, GCUI32 data_bias) {
			glVertexAttribPointer(location, vertex_size, GL_FLOAT, GL_FALSE, vertex_group_size * GC_SIZE_FP32,
				(void*)(data_bias * GC_SIZE_FP32)); // bias_pointer.
			glEnableVertexAttribArray(location);
		}
	}

	namespace ShaderRTX {
#define INFO_TESTHEAD_ext "[NVIDIA_RTX]: "

		// detect RTX extensions.
		GCBIT __GLEW_TESTNVIDIA_RTX() {
			GCBIT returnTestRTX = true;
			if (!glewIsExtensionSupported("GL_NV_ray_tracing")) {
				returnTestRTX = false;
				Log << Log.SET(CLOG_WARRING) << INFO_TESTHEAD_ext << "NvExt: !'GL_NV_ray_tracing'" << LogEnd;
			}
			if (!glewIsExtensionSupported("GL_NV_shader_buffer_load")) {
				returnTestRTX = false;
				Log << Log.SET(CLOG_WARRING) << INFO_TESTHEAD_ext << "NvExt: !'GL_NV_shader_buffer_load'" << LogEnd;
			}

			if (returnTestRTX)
				Log << Log.SET(CLOG_INFO) << INFO_TESTHEAD_ext << "NvExt: Supports RTX extension" << LogEnd;
			return returnTestRTX;
		}

		// get RTX extension pointer.
		void __GLEW_GETRTXPOINTER() {
			// "wglGetProcAddress" Windows.
			PFNGLGETTEXTUREHANDLENVPROC glGetTextureHandleNV =
				(PFNGLGETTEXTUREHANDLENVPROC)wglGetProcAddress("glGetTextureHandleNV");
			PFNGLMAKEIMAGEHANDLENONRESIDENTNVPROC glMakeImageHandleNonResidentNV =
				(PFNGLMAKEIMAGEHANDLENONRESIDENTNVPROC)wglGetProcAddress("glMakeImageHandleNonResidentNV");
		}
	}
}