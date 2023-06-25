// Framework_ConsoleLog.
// crimson_logframe.[改] version 2.1.0
#ifndef __FRAMECONSOLELOG_H
#define __FRAMECONSOLELOG_H
#include <iostream>
#include <windows.h>
#include "../ShaderFramework_define.hpp"

#define LST (GCSIZE)  // => size_t [ull].
#define LCB (GC_TEXT) // => cchar.

#define CLOG_ERROR   0xF0A0 // Log_out <错误>
#define CLOG_WARRING 0xF0A1 // Log_out <警告>
#define CLOG_INFO    0xF0A2 // Log_out <信息>
#define CLOG_TRACE   0xF0A3 // Log_out <跟踪>
#define CLOG_PERF    0xF0A4 // Log_out <性能>

#define LogEnd "\n"

// crimson framelog console.
// version 2.2.0
namespace LOGCONS {
	class _crlog_coretool { // [windows].
	protected:
		HANDLE handle = nullptr;
		WORD colorold = NULL;
		GCBIT _SetColorFlag = false;
		GC_CHARPTR _printloglable = nullptr;

		void __logPrintColor(GC_TEXT _lable, WORD _color);
		void __logPrintEnd() const;
		GC_CHARPTR __logPrintSet(GCI32 MOD);
	public:
		_crlog_coretool();
		~_crlog_coretool() {};
	};

	class crlog_ostream :public _crlog_coretool {
	protected:
		static bool _print_state;
	public:
		template<typename _pLog>
		const crlog_ostream& operator<<(_pLog LogOutText) const {
			if (_print_state) std::cout << LogOutText;
			return *this;
		}
		const crlog_ostream& operator<<(GC_TEXT Cahrstr) const;

		// lable define "CLOG_"
		GC_CHARPTR SET(GCI32 Lable);
		void OUT_SEATE(bool print_switch);
	};
	extern crlog_ostream Log;
}

#endif