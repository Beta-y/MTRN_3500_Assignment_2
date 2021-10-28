/******************
EmbeddedFunctions.h
JAMES STEVENS 2020
******************/

/*
This class wraps the Galil commands for students to use. Whenever you send or receive from the board in your 
Galil class, use the Embeddedfunctions to do it. For more information on what each function does, see the 
Galil Library documentation.
*/

#pragma once

#include "gclib.h"
#include "gclibo.h"
#include "gclib_errors.h"
#include "gclib_record.h"

class EmbeddedFunctions {
	public:
		EmbeddedFunctions() {};
		~EmbeddedFunctions() {};

		virtual GReturn GOpen(GCStringIn address, GCon* g) {
			return ::GOpen(address, g);
		}
		virtual GReturn GClose(GCon g) {
			return ::GClose(g);
		}
		virtual GReturn GRead(GCon g, GBufOut buffer, GSize buffer_len, GSize* bytes_read) {
			return ::GRead(g, buffer, buffer_len, bytes_read);
		}
		virtual GReturn GWrite(GCon g, GBufIn buffer, GSize buffer_len) {
			return ::GWrite(g, buffer, buffer_len);
		}
		virtual GReturn GCommand(GCon g, GCStringIn command, GBufOut buffer, GSize buffer_len, GSize* bytes_returned) {
			return ::GCommand(g, command, buffer, buffer_len, bytes_returned);
		}
		virtual GReturn GVersion(GCStringOut ver, GSize ver_len) {
			return ::GVersion(ver, ver_len);
		}
		virtual GReturn GInfo(GCon g, GCStringOut info, GSize info_len) {
			return ::GInfo(g, info, info_len);
		}
};
