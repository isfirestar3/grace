#include <Windows.h>
#include <string>

std::string EncodeUtf8fromWstring( std::wstring in )
{
	std::string s( in.length() * 3 + 1, ' ' );
	size_t len = ::WideCharToMultiByte( CP_UTF8, 0,
		in.c_str(), in.length(),
		&s[0], s.length(),
		NULL, NULL );
	s.resize( len );
	return s;
}

std::string EncodeUtf8fromString( std::string in )
{
	std::wstring wszStr;
	int nLength = MultiByteToWideChar( CP_ACP, 0, in.c_str(), -1, NULL, NULL );
	wszStr.resize( nLength );

	LPWSTR lpwszStr = new wchar_t[nLength];
	MultiByteToWideChar( CP_ACP, 0, in.c_str(), -1, lpwszStr, nLength );
	wszStr = lpwszStr;

	delete[] lpwszStr;

	return EncodeUtf8fromWstring( wszStr );
}

std::string WString2String( std::wstring in )
{
	int nLen = WideCharToMultiByte( CP_ACP, 0, in.c_str(), -1, NULL, 0, NULL, NULL );
	if ( nLen <= 0 ) return std::string( "" );
	char* pszDst = new char[nLen];
	if ( NULL == pszDst ) return std::string( "" );
	WideCharToMultiByte( CP_ACP, 0, in.c_str(), -1, pszDst, nLen, NULL, NULL );
	pszDst[nLen - 1] = 0;
	std::string strTemp( pszDst );
	delete[] pszDst;
	return strTemp;
}

std::string DecodeUtf8fromString( std::string in )
{
	int  len = 0;
	len = in.length();
	int  unicodeLen = ::MultiByteToWideChar( CP_UTF8, 0, in.c_str(), -1, NULL, 0 );
	wchar_t *  pUnicode;
	pUnicode = new  wchar_t[unicodeLen + 1];
	memset( pUnicode, 0, ( unicodeLen + 1 )*sizeof( wchar_t ) );
	::MultiByteToWideChar( CP_UTF8, 0, in.c_str(), -1, ( LPWSTR ) pUnicode, unicodeLen );
	std::wstring  rt;
	rt = ( wchar_t* ) pUnicode;
	delete  pUnicode;
	return  WString2String( rt );
}

VOID
PubCRTConvertUTF8ToUcstr(
_In_ PCSTR sourceString,
_Inout_ PWSTR destinationString,
_In_ int destinationSizeCch
)
{
	int minimumSizeCch;

	if ( !sourceString || !destinationString || destinationSizeCch <= 0 ) {
		return;
	}

	minimumSizeCch = MultiByteToWideChar(
		CP_UTF8,
		0,
		sourceString,
		-1,
		NULL,
		0
		);
	if ( destinationSizeCch >= minimumSizeCch ) {
		MultiByteToWideChar(
			CP_UTF8,
			0,
			sourceString,
			-1,
			destinationString,
			destinationSizeCch
			);
	}
}

VOID
PubCRTConvertUcstrToANSI(
_In_ PCWSTR sourceString,
_Inout_ PSTR destinationString,
_In_ int  destinationSizeCch
)
{
	int minimumSizeCch;

	if ( !sourceString || !destinationString || destinationSizeCch <= 0 ) {
		return;
	}

	minimumSizeCch = WideCharToMultiByte(
		CP_OEMCP,
		0,
		sourceString,
		-1,
		NULL,
		0,
		NULL,
		FALSE
		);
	if ( destinationSizeCch >= minimumSizeCch ) {
		WideCharToMultiByte(
			CP_OEMCP,
			0,
			sourceString,
			-1,
			destinationString,
			destinationSizeCch,
			NULL,
			FALSE
			);
	}
}

VOID
PubCRTConvertUTF8ToANSI(
_In_ PCSTR sourceString,
_Inout_ PSTR destinationString,
_In_ int destinationSizeCch
)
{
	PWSTR Intervening = NULL;

	if ( !sourceString || !destinationString || destinationSizeCch <= 0 ) {
		return;
	}

	Intervening = ( PWSTR ) malloc( destinationSizeCch * sizeof( WCHAR ) );
	if ( !Intervening ) return;

	PubCRTConvertUTF8ToUcstr( sourceString, Intervening, destinationSizeCch );

	PubCRTConvertUcstrToANSI( Intervening, destinationString, destinationSizeCch );

	free( Intervening );
}

int
verify_time_string( const std::basic_string<char> &timeroom)
{
	static const char *STD_TIME_ROOM = "2000-01-01 00:00:00";
	static const size_t STD_TIME_ROOM_CB = strlen( STD_TIME_ROOM );

	std::size_t timeroom_cb = timeroom.length();

	// 允许不填写
	if ( 0 == timeroom_cb ) return 0; 

	// 长度不符
	if ( timeroom_cb != STD_TIME_ROOM_CB ) return -1; 

	// 关键字节不符
	if ( timeroom[4] != '-' || timeroom[7] != '-' || timeroom[10] != ' ' || timeroom[13] != ':' || timeroom[16] != ':' ) return -1;

	return 0;
}