// ShaderFramework_define.
#ifndef _SHADERFRAMEWORK_DEFINE_HPP
#define _SHADERFRAMEWORK_DEFINE_HPP
#define TEXTSTR_END '\0'

#define GC_NTEQUAD_VERTEX 4
#define GC_VERTEX_COORD   3
#define GC_VERTEX_COLOR   4
#define GC_VERTEX_TEXUV   2

typedef bool          GCBIT;
typedef char          GCCHAR;
typedef unsigned char GCBYTE;

typedef __int32 GCI32;
typedef __int64 GCI64;

typedef float  GCFP32;
typedef double GCFP64;

typedef unsigned __int32 GCUI32;
typedef unsigned __int64 GCUI64;

typedef unsigned long long GCSIZE;

typedef char*       GC_CHARPTR;
typedef const char* GC_TEXT;

#define GC_SIZE_INT32 (GCSIZE)4 // sizeof GCI32.
#define GC_SIZE_INT64 (GCSIZE)8 // sizeof GCI64.
#define GC_SIZE_FP32  (GCSIZE)4 // sizeof GCFP32.
#define GC_SIZE_FP64  (GCSIZE)8 // sizeof GCFP64.

// Vertex Buffer Object, Vertex Array Object.
struct GC_MODELINDEX {
	GCUI32 VAOindex, VBOindex;
	constexpr GC_MODELINDEX() : VAOindex(0), VBOindex(0) { }
	constexpr GC_MODELINDEX(GCUI32 vao, GCUI32 vbo) :
		VAOindex(vao), VBOindex(vbo) { }
};

// Texture index[obj], Program index.
struct GC_TEXINDEX {
	GCUI32 Texture, Program;
	constexpr GC_TEXINDEX() : Texture(0), Program(0) { }
	constexpr GC_TEXINDEX(GCUI32 tex, GCUI32 pgm) :
		Texture(tex), Program(pgm) { }
};

//******************************** maths define ********************************

struct GC_Window {
	GCI32 window_width, window_height;
	GC_TEXT init_window_name;
	constexpr GC_Window() : window_width(0), window_height(0), init_window_name("null") { }
	constexpr GC_Window(GCI32 w, GCI32 h, GC_TEXT name) :
		window_width(w), window_height(h), init_window_name(name) { }
};

struct GC_VEC2f {
	GCFP32 vector_x, vector_y;
	constexpr GC_VEC2f() : vector_x(0.0f), vector_y(0.0f) { }
	constexpr GC_VEC2f(GCFP32 x, GCFP32 y) :
		vector_x(x), vector_y(y) { }
};

struct GC_VEC3f {
	GCFP32 vector_x, vector_y, vector_z;
	constexpr GC_VEC3f() : vector_x(0.0f), vector_y(0.0f), vector_z(0.0f) { }
	constexpr GC_VEC3f(GCFP32 x, GCFP32 y, GCFP32 z) :
		vector_x(x), vector_y(y), vector_z(z) { }
};

struct GC_VEC4f {
	GCFP32 vector_x, vector_y, vector_z, vector_w;
	constexpr GC_VEC4f() : vector_x(0.0f), vector_y(0.0f), vector_z(0.0f), vector_w(0.0f) { }
	constexpr GC_VEC4f(GCFP32 x, GCFP32 y, GCFP32 z, GCFP32 w) :
		vector_x(x), vector_y(y), vector_z(z), vector_w(w) { }
};

// Maths. [Matrix]
struct GC_Matrix3 { GCFP32 Matrix[3 * 3]; }; // Mat 3 * 3 FP32.
struct GC_Matrix4 { GCFP32 Matrix[4 * 4]; }; // Mat 4 * 4 FP32.

//******************************** model define ********************************

struct GC_VERTEX {
	GCFP32 verCoord[GC_VERTEX_COORD];   // Coordinate XYZ.
	GCFP32 verColor[GC_VERTEX_COLOR];   // Color RGBA.
	GCFP32 verTexture[GC_VERTEX_TEXUV]; // Texture UV.
	GCFP32 verNormal[GC_VERTEX_COORD];  // Normal XYZ.
};

// not Tessellation QUAD. ver_4
struct GC_NTEQUAD { GC_VERTEX Tvertex[GC_NTEQUAD_VERTEX]; };
struct GC_MODEL {
	GCSIZE ModelDataLength;
	GC_NTEQUAD* QuadsModel;
};

// ¶¯Ì¬Ä£ÐÍ. GC_DynamicModel => GPU.
struct GC_DynamicModel {
	GCSIZE ModelDataLength;
	GCFP32* vertexData;
};

#define GC_VERTEX_LENGTH   GC_VERTEX_COORD * 2 + GC_VERTEX_COLOR + GC_VERTEX_TEXUV
#define GC_VERTEX_BYTES    (GC_VERTEX_COORD * 2 + GC_VERTEX_COLOR + GC_VERTEX_TEXUV) * GC_SIZE_FP32

#define GC_SIZE_VERTEX   (GCSIZE)52  // sizeof GC_VERTEX.
#define GC_SIZE_NTEQUAD  (GCSIZE)156 // sizeof GC_TRIANGLE.
#define GC_SIZE_MODEL    (GCSIZE)16  // sizeof GC_MODEL.

#endif