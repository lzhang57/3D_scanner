/*
 *-----------------------------------------------------------------------------
 *       Name				MVUtil.h
 *       Purpose			
 *       Development Kit	Microsoft Win32 SDK, Visual C++ 6.00 
 *
 *
 *       Description
 *
 *       
 *-----------------------------------------------------------------------------
 */

#if !defined(MV_MVUTIL)         //
#define MV_MVUTIL


/* By C++ language compiler */
#ifdef __cplusplus
extern "C" {
#endif

HVSTATUS __stdcall HVSaveJPEG(char *lpFileName, BYTE *pBuffer, int nWidth, int nHeight, int nBitCount, BOOL bVerFlip, int nQuality);
HVSTATUS __stdcall HVLoadJPEG(char *lpFileName, BYTE *pBuffer, int *pWidth, int *pHeight, int *pBitCount, BOOL bVerFlip);		

/* extren "C" { */
#ifdef __cplusplus
}
#endif

#endif