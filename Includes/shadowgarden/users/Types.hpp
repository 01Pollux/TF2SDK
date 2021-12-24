#pragma once

#include <string>

namespace ShadowGarden
{
	using WChar = wchar_t;
	using Char	= char;

#ifdef UNICODE
	using TChar = WChar;
#else
	using TChar = Char;
#endif;

	using WCString	= WChar*;
	using CString	= Char*;
	using TCString	= TChar*;

	using CWCString	= const WCString*;
	using CCString	= const CString*;
	using CTCString	= const TCString*;

	using String	= std::basic_string<Char>;
	using WString	= std::basic_string<WChar>;
	using TString	= std::basic_string<TChar>;
	
	using StringView	= std::basic_string_view<Char>;
	using WStringView	= std::basic_string_view<WChar>;
	using TStringView	= std::basic_string_view<TChar>;
	
	using Byte	= unsigned char;
	using Word	= unsigned short;
	using DWord	= unsigned long;
	using QWord = unsigned long long;
}