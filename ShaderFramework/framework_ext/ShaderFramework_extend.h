// ShaderFramework_extend.
// 2023_5_11 version 1.2.0, framework extend.
// rcsz.

#ifndef _SHADERFRAMEWORK_EXTEND_H
#define _SHADERFRAMEWORK_EXTEND_H
#include "../ShaderFramework_define.hpp"

// ������ɫ���������õ��С.
void ShaderSetPointSize();

namespace ShaderExt {
	// _Ext [GC_DynamicModel] Points.
	struct GC_DynamicPoints {
		GCSIZE PointsDataLength;
		GCFP32* vertexData;
	};

	void __GLEW_SHADERDRAW_POINTS(GC_MODELINDEX& objectindex);
	void __GLEW_SHADERDRAW_DULINES(GC_MODELINDEX& objectindex);

#define GC_ShaderDrawPS __GLEW_SHADERDRAW_POINTS  // points.
#define GC_ShaderDrawLS __GLEW_SHADERDRAW_DULINES // lines. DOUBLE_POINTS

	// _Ext glew => loaddata -> gpu.
	// Load_PointsData => draw(Points, Lines).
	// 3*FP32 pos, 4*FP32 color.
	class Load_PointsData {
	public:
		// Create VA,VB object.
		// Location End = Begin + 2, [Begin, Begin + 2].
		void Load_DPointsData(GC_DynamicPoints input, GC_MODELINDEX& objectindex, GCUI32 begin);
	};

	// �ֶ����� VAO,VBO ��������.
	namespace ShaderData {
		// Create VAO,VBO. Bind VAO,VBO.
		void OpenGL_CreateBindObjectAB(GCUI32 & VAO, GCUI32 & VBO, GCUI32 data_length_byte, GCFP32 * vertex_data);
		// "glVertexAttribPointer", "glEnableVertexAttribArray".
		void OpenGL_VertexAttribPointer(GCUI32 location, GCUI32 vertex_size, GCUI32 vertex_group_size, GCUI32 data_bias);
		// UnBind VAO,VBO.
		void OpenGL_UnBindObjectAB();
	}

	// ���,��ȡ NVIDIA RTX ��չָ��.
	namespace ShaderRTX {
		GCBIT __GLEW_TESTNVIDIA_RTX();
		void __GLEW_GETRTXPOINTER();

#define GC_ShaderTestRTX   __GLEW_TESTNVIDIA_RTX // ����Ƿ�֧��RTX��չ.
#define GC_ShaderEnableRTX __GLEW_GETRTXPOINTER  // ����RTX��չ(��ȡ��չָ��).
	}
}

#endif