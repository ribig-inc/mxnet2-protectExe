/***************************************************************************/
/*  MXTYPES.H   Definition of data types.                                  */
/*                                                                         */
/*  (C) TDi GmbH                                                           */
/*                                                                         */
/*  Include File for C/C++ (32-Bit and 64-Bit)                             */
/***************************************************************************/
#ifndef _MATRIXAPI_DATA_TYPES
#define _MATRIXAPI_DATA_TYPES


#ifdef __cplusplus
	
	#include <cstdint>

	using _mxINT32=std::int32_t;
	using _mxUINT32=std::uint32_t;
	using _mxINT16=std::int16_t;
	using _mxUINT16=std::uint16_t;

#else

	#include <limits.h>

	#if ULONG_MAX == 0xffffffff
	  #define  _mxINT32  long
	  #define  _mxUINT32 unsigned long
	  #define  _mxINT16  short
	  #define  _mxUINT16 unsigned short
	#elif UINT_MAX == 0xffffffff
	  #define  _mxINT32  int
	  #define  _mxUINT32 unsigned int
	  #define  _mxINT16  short
	  #define  _mxUINT16 unsigned short 
	#else
	  #error can not define Matrix-API data types
	#endif

#endif

#endif  // _MATRIXAPI_DATA_TYPES

