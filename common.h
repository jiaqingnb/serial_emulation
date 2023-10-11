#ifndef COMMON_H
#define COMMON_H

typedef char              CHAR;
typedef signed char       INT8;
typedef unsigned char     UINT8;
typedef unsigned char     UCHAR;
typedef short int         INT16;
typedef unsigned short int UINT16;
typedef int               INT32;
typedef unsigned int      UINT32;
typedef float             FLOAT32;
typedef double            FLOAT64;

class Conver
{
public:
	static Conver& Getconver()
	{
		static Conver con;

		return con;
	}


	UINT16 ShortFromChar(const UINT8* pInput);

	UINT32 LongFromChar(const UINT8* pInput);

	void ShortToChar(UINT16 Input, UINT8* pOutput);

	void LongToChar(UINT32 Input, UINT8* pOutput);

	UINT16 ShortFromCharLE(const UINT8* pInput);

	UINT32 LongFromCharLE(const UINT8* pInput);

	void ShortToCharLE(UINT16 Input, UINT8* pOutput);

	void LongToCharLE(UINT32 Input, UINT8* pOutput);
private:
};


#endif // !1
