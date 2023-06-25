// ShaderFramework_model.
#ifndef _SHADERFRAMEWORK_MODEL_H
#define _SHADERFRAMEWORK_MODEL_H
#include <vector>
#include "../ShaderFramework_define.hpp"

// Rectangle OBJ Model. FaceVertex = 4.
namespace RectModel {

#define LF_FILEOBJ_VERTEX    "v"  // vertex coord.
#define LF_FILEOBJ_NORMAL    "vn" // normal coord.
#define LF_FILEOBJ_TEXTURE   "vt" // UV coord.
#define LF_FILEOBJ_FACEINDEX "f"  // face index.[RECT4]

#define LF_FILEOBJ_FACEVER         (size_t)4
#define LF_FILEOBJ_FACEVERBYTESMAX (size_t)32

	struct DataGroup2F { GCFP32 DG2f_data[2]; };
	struct DataGroup3F { GCFP32 DG3f_data[3]; };

	struct DataGroup3x4_FaceIndex {
		GCUI32 DG3i_dataA[3]; // RectangleMesh pointA (0,0).
		GCUI32 DG3i_dataB[3]; // RectangleMesh pointB (1,0).
		GCUI32 DG3i_dataC[3]; // RectangleMesh pointC (1,1).
		GCUI32 DG3i_dataD[3]; // RectangleMesh pointD (0,1).
	};

	class Model_LoadObj {
	protected:
		std::vector<DataGroup3F> model_vertex;
		std::vector<DataGroup3F> model_normal;
		std::vector<DataGroup2F> model_uvcoord;
		std::vector<DataGroup3x4_FaceIndex> model_faceindex;

		void _model_face_encoding();
	public:
		Model_LoadObj(GC_TEXT File, GCI32 Limit);
		~Model_LoadObj() {};

		GCUI32 FileState = NULL;
	};
}

#endif