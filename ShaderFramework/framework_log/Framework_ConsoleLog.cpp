// Framework_ConsoleLog.
#include <iostream>
#include <windows.h>
#include <string>
#include "Framework_ConsoleLog.hpp"

using namespace std;

// system color define.
#define WINSYS_LOGCOLOR_RED    0x04
#define WINSYS_LOGCOLOR_ORANGE 0x06
#define WINSYS_LOGCOLOR_GREY   0x08
#define WINSYS_LOGCOLOR_LBLUE  0x0B
#define WINSYS_LOGCOLOR_BLUE   0x09
#define WINSYS_LOGCOLOR_PURPLE 0x05

// value_fill_zero.
GC_CHARPTR __NumberFillzero(GC_TEXT intime, GCI32 numberbits) {
	static GC_CHARPTR _returnstr = {};
	string strvalue = intime;
	string retstr = {};

	for (GCSIZE i = 0; i < LST(numberbits) - strvalue.size(); i++)
		retstr += '0';
	retstr += strvalue;

	_returnstr = new GCCHAR[retstr.size() + 1];
	strcpy_s(_returnstr, retstr.size() + 1, retstr.c_str());
	return _returnstr;
}

// Time [xxxx.xx.xx.xx:xx:xx:xx ms].
GC_CHARPTR __GetCurrenttime() {
	SYSTEMTIME _time = {};
	GetLocalTime(&_time); // GetSystemTime ms.
	GCCHAR tmpbuff[16] = {};

	sprintf_s(tmpbuff, "%d", _time.wYear);   string year = tmpbuff;
	sprintf_s(tmpbuff, "%d", _time.wMonth);  string month = __NumberFillzero(tmpbuff, 2);
	sprintf_s(tmpbuff, "%d", _time.wDay);    string day = __NumberFillzero(tmpbuff, 2);
	sprintf_s(tmpbuff, "%d", _time.wHour);   string hour = __NumberFillzero(tmpbuff, 2);
	sprintf_s(tmpbuff, "%d", _time.wMinute); string minute = __NumberFillzero(tmpbuff, 2);
	sprintf_s(tmpbuff, "%d", _time.wSecond); string second = __NumberFillzero(tmpbuff, 2);
	sprintf_s(tmpbuff, "%d", _time.wMilliseconds); string millisecond = __NumberFillzero(tmpbuff, 3);

	string current_time = "null";
	current_time = "[" + year + "." + month + "." + day + " " + hour + ":" + minute + ":" + second + " " + millisecond + " ms]";
	GC_CHARPTR rettimelable = new GCCHAR[LST(current_time.size() + 1)];
	strcpy_s(rettimelable, LST(current_time.size() + 1), current_time.c_str());
	return rettimelable;
}

void __freechar(GC_CHARPTR _CharMem, GCBIT _Flag) {
	delete[] _CharMem;
	_CharMem = nullptr;
	_Flag = FALSE;
}

namespace LOGCONS {
	crlog_ostream Log;
	// str.Highlight + lable
	void _crlog_coretool::__logPrintColor(GC_TEXT _lable, WORD _color) {
		string retprinttext = __GetCurrenttime();
		CONSOLE_SCREEN_BUFFER_INFO csbi = {};

		handle = ::GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleScreenBufferInfo(handle, &csbi);
		colorold = csbi.wAttributes;
		SetConsoleTextAttribute(handle, _color);

		retprinttext += ":";
		retprinttext += _lable;
		_printloglable = new GCCHAR[retprinttext.size() + 1];
		strcpy_s(_printloglable, retprinttext.size() + 1, retprinttext.c_str());
	}

	// console log.
	GC_CHARPTR _crlog_coretool::__logPrintSet(GCI32 MOD) {
		switch (MOD) {
		case(CLOG_ERROR):   { __logPrintColor("[ERROR] ", WINSYS_LOGCOLOR_RED); break; }
		case(CLOG_WARRING): { __logPrintColor("[WARRING] ", WINSYS_LOGCOLOR_ORANGE); break; }
		case(CLOG_INFO):    { __logPrintColor("[INFO] ", WINSYS_LOGCOLOR_GREY); break; }
		case(CLOG_TRACE):   { __logPrintColor("[TRACE] ", WINSYS_LOGCOLOR_LBLUE); break; }
		case(CLOG_PERF):    { __logPrintColor("[PERF] ", WINSYS_LOGCOLOR_PURPLE); break; }
		}
		return _printloglable;
	}

	void _crlog_coretool::__logPrintEnd() const {
		// SET => End.set_oldcolor.
		if (_SetColorFlag)
			__freechar(_printloglable, &_SetColorFlag);
		SetConsoleTextAttribute(handle, colorold);
	}

	_crlog_coretool::_crlog_coretool() {
		// off buffering. [speed]
		ios::sync_with_stdio(NULL);
	}

	bool crlog_ostream::_print_state = true;
	const crlog_ostream& crlog_ostream::operator<<(GC_TEXT Charstr) const {
		// Terminator, free lable.
		if ((Charstr == LogEnd)) __logPrintEnd();
		if (_print_state) cout << Charstr;
		return *this;
	}

	GC_CHARPTR crlog_ostream::SET(GCI32 Lable) {
		_SetColorFlag = true;
		return __logPrintSet(Lable);
		//else return (GC_CHARPTR)'\0';
	}
	void crlog_ostream::OUT_SEATE(bool print_switch) {
		// set print state.
		_print_state = print_switch;
	}
}