#ifndef _MXNET2API_H
#define _MXNET2API_H

#include "mxtypes.h"     // platform independent data types

#if defined _WIN32 || defined _WIN64
#include <windows.h>
    #define WINAPI __stdcall
#else
    #define WINAPI
#endif

namespace mxnet2 {

#ifdef __cplusplus
    extern "C" {
#endif


#if defined _WIN32 || defined _WIN64
        _mxINT16  WINAPI rInit_MatrixAPI(HINSTANCE hInst = NULL);
        WCHAR* WINAPI MxNet_SetIP(WCHAR*);
#else
        _mxINT16  WINAPI rInit_MatrixAPI();
        char* WINAPI MxNet_SetIP(char*);
#endif

        _mxINT16  WINAPI rRelease_MatrixAPI(void);
        _mxINT32  WINAPI rGetVersionAPI(void);
        _mxINT32  WINAPI rGetVersionDRV(void);
        _mxINT32  WINAPI rGetVersionDRV_USB(void);
        void      WINAPI rSetW95Access(_mxINT16);
        _mxINT16  WINAPI rGetPortAdr(_mxINT16);
        _mxINT16  WINAPI rDongle_WriteData(_mxINT32, _mxINT32*, _mxINT16, _mxINT16, _mxINT16);
        _mxINT16  WINAPI rDongle_WriteDataEx(_mxINT32, _mxINT32*, _mxINT16, _mxINT16, _mxINT16, _mxINT16);
        _mxINT16  WINAPI rDongle_ReadData(_mxINT32, _mxINT32*, _mxINT16, _mxINT16, _mxINT16);
        _mxINT16  WINAPI rDongle_ReadDataEx(_mxINT32, _mxINT32*, _mxINT16, _mxINT16, _mxINT16, _mxINT16);
        _mxINT32  WINAPI rDongle_ReadSerNr(_mxINT32, _mxINT16, _mxINT16);
        _mxINT16  WINAPI rDongle_Exit(void);
        _mxINT16  WINAPI rDongle_Find(void);
        _mxINT32  WINAPI rDongle_Version(_mxINT16, _mxINT16);
        _mxINT32  WINAPI rDongle_Model(_mxINT16, _mxINT16);
        _mxINT16  WINAPI rDongle_MemSize(_mxINT16, _mxINT16);
        _mxINT16  WINAPI rDongle_Count(_mxINT16);
        _mxINT16  WINAPI rPausePrinterActivity(void);
        _mxINT16  WINAPI rResumePrinterActivity(void);
        _mxINT16  WINAPI rSetConfig_MatrixNet(_mxINT16, char*);
        _mxINT16  WINAPI rLogIn_MatrixNet(_mxINT32, _mxINT16, _mxINT16);
        _mxINT16  WINAPI rLogOut_MatrixNet(_mxINT32, _mxINT16, _mxINT16);
        _mxINT32  WINAPI rGetConfig_MatrixNet(_mxINT16 Category);

        _mxINT16  WINAPI rDongle_WriteKey(_mxINT32 UserCode,
            _mxINT32* KeyData,
            _mxINT16 DongleNr,
            _mxINT16 PortNr);

        _mxINT16  WINAPI rDongle_GetKeyFlag(_mxINT32 UserCode, _mxINT16 DongleNr, _mxINT16 PortNr);

        _mxINT16  WINAPI rDongle_EncryptData(_mxINT32 UserCode, _mxINT32* DataBlock, _mxINT16 DongleNr, _mxINT16 PortNr);
        _mxINT16  WINAPI rDongle_DecryptData(_mxINT32 UserCode, _mxINT32* DataBlock, _mxINT16 DongleNr, _mxINT16 PortNr);

        _mxINT16  WINAPI rDongle_SetDriverFlag(_mxINT32 UserCode, _mxINT16 DriverFlag, _mxINT16 DongleNr, _mxINT16 PortNr);
        _mxINT16  WINAPI rDongle_GetDriverFlag(_mxINT32 UserCode, _mxINT16 DongleNr, _mxINT16 PortNr);

        _mxINT32 WINAPI MxNet_SetPort(_mxINT32);
        _mxINT32 WINAPI MxNet_SetTimeOut(_mxINT32);

        _mxINT32 WINAPI rDongle_GetDetectTime(_mxINT32 UserCode, _mxINT32* timeBuffer, _mxINT16 dNr, _mxINT16 pNr);
        _mxINT32 WINAPI rDongle_GetLoginCount(_mxINT32 UserCode, _mxINT16 appSlot, _mxINT16 dNr, _mxINT16 PortNr);
        _mxINT32 WINAPI rDongle_GetLoginCountEx(_mxINT32 UserCode, _mxINT16 appSlot, _mxINT16 dNr, _mxINT16 PortNr, char** ppszClientNames);

        void  WINAPI rReset_MatrixAPI(void);

#ifdef __cplusplus
    }
#endif

}

#endif  // _MXNET2API_H

