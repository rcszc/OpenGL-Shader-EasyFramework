// ShaderFramework_model.
// FILE: .OBJ [TYPE: RectangleModel].
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include "ShaderFramework_model.h"
#include "../framework_log/Framework_ConsoleLog.hpp"

using namespace std;
using namespace LOGCONS;
#define INFO_FILEOBJHEAD "[RectModel]: "

namespace RectModel {
	Model_LoadObj::Model_LoadObj(GC_TEXT File, GCI32 Limit) {
		string _GETSTRING = {}, _HEADLABLE = {}, _nameTemp = {};
		GCUI32 TrangCount = NULL;

		ifstream _load_file(File);
		if (_load_file) {
			Log << Log.SET(CLOG_INFO) << INFO_FILEOBJHEAD << "load.Obj File: " << File << LogEnd;

			while (getline(_load_file, _GETSTRING)) {
				istringstream DATA(_GETSTRING);
				DATA >> _HEADLABLE;

				if (_HEADLABLE == LF_FILEOBJ_VERTEX) { // vertex.
					DataGroup3F vertexdata = {};
					DATA >> vertexdata.DG3f_data[0] >> vertexdata.DG3f_data[1] >> vertexdata.DG3f_data[2];
					model_vertex.push_back(vertexdata);
				}

				if (_HEADLABLE == LF_FILEOBJ_NORMAL) { // normal.
					DataGroup3F normaldata = {};
					DATA >> normaldata.DG3f_data[0] >> normaldata.DG3f_data[1] >> normaldata.DG3f_data[2];
					model_normal.push_back(normaldata);
				}

				if (_HEADLABLE == LF_FILEOBJ_TEXTURE) { // uv coord.
					DataGroup2F uvdata = {};
					DATA >> uvdata.DG2f_data[0] >> uvdata.DG2f_data[1];
					model_uvcoord.push_back(uvdata);
				}

				// load FACE index. (x/x/x, x//x, x)
				if (_HEADLABLE == LF_FILEOBJ_FACEINDEX) { // face index.
					struct DataGroup3x4_FaceIndex facindex;
					string StrTmp[4] = {};
					DATA >> StrTmp[0] >> StrTmp[1] >> StrTmp[2] >> StrTmp[3];

					if (StrTmp[3] == "") { TrangCount++; }

					GCUI32 _outnum[LF_FILEOBJ_FACEVER][3] = {};
					for (GCSIZE j = NULL; j < LF_FILEOBJ_FACEVER; j++) {

						GCCHAR _inchar[LF_FILEOBJ_FACEVERBYTESMAX] = {}, _outchar[LF_FILEOBJ_FACEVER][LF_FILEOBJ_FACEVERBYTESMAX] = {};
						GCSIZE _count[2] = {};
						strcpy_s(_inchar, StrTmp[j].c_str()); // string->char*

						for (GCSIZE i = NULL; i < LF_FILEOBJ_FACEVERBYTESMAX; i++) { // È¥µô'//'·ÖÀë xxx//xxx x/x/x.
							if (_inchar[i] != '/') {
								_outchar[_count[0]][_count[1]] = _inchar[i];
								_count[1]++;
							}
							else {
								_count[0]++;
								_count[1] = NULL;
							}
						}
						for (GCSIZE i = NULL; i < 3; i++)
							_outnum[j][i] = atoi(_outchar[i]) - 1; // index 0.
					}

					for (GCSIZE i = 0; i < 3; i++) {
						facindex.DG3i_dataA[i] = _outnum[0][i];
						facindex.DG3i_dataB[i] = _outnum[1][i];
						facindex.DG3i_dataC[i] = _outnum[2][i];
						facindex.DG3i_dataD[i] = _outnum[3][i];
					}
					model_faceindex.push_back(facindex);
				}
			}

			Log << Log.SET(CLOG_INFO) << INFO_FILEOBJHEAD << "ObjModel vertex: " << model_vertex.size() << LogEnd;
			Log << Log.SET(CLOG_INFO) << INFO_FILEOBJHEAD << "ObjModel normal: " << model_normal.size() << LogEnd;
			Log << Log.SET(CLOG_INFO) << INFO_FILEOBJHEAD << "ObjModel uv: " << model_uvcoord.size() << LogEnd;
			Log << Log.SET(CLOG_INFO) << INFO_FILEOBJHEAD << "ObjModel face: " << model_faceindex.size() << LogEnd;

			if (TrangCount != NULL) {
				FileState = -2;
				Log << Log.SET(CLOG_WARRING) << INFO_FILEOBJHEAD << "ObjModel face_Triangles: " << TrangCount << LogEnd;
			}
		}
		else {
			FileState = -1;
			Log << Log.SET(CLOG_ERROR) << INFO_FILEOBJHEAD << "load.Obj Failed: " << File << LogEnd;
		}
	}

	void Model_LoadObj::_model_face_encoding() {

	}
}