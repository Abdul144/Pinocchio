/*
 * MATLAB Compiler: 4.17 (R2012a)
 * Date: Fri Sep 21 15:49:46 2012
 * Arguments: "-B" "macro_default" "-l" "eg_skeleton_laplacian_rosa.m" 
 */

#include <stdio.h>
#define EXPORTING_eg_skeleton_laplacian_rosa 1
#include "eg_skeleton_laplacian_rosa.h"

static HMCRINSTANCE _mcr_inst = NULL;


#if defined( _MSC_VER) || defined(__BORLANDC__) || defined(__WATCOMC__) || defined(__LCC__)
#ifdef __LCC__
#undef EXTERN_C
#endif
#include <windows.h>

static char path_to_dll[_MAX_PATH];

BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, void *pv)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        if (GetModuleFileName(hInstance, path_to_dll, _MAX_PATH) == 0)
            return FALSE;
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
    }
    return TRUE;
}
#endif
#ifdef __cplusplus
extern "C" {
#endif

static int mclDefaultPrintHandler(const char *s)
{
  return mclWrite(1 /* stdout */, s, sizeof(char)*strlen(s));
}

#ifdef __cplusplus
} /* End extern "C" block */
#endif

#ifdef __cplusplus
extern "C" {
#endif

static int mclDefaultErrorHandler(const char *s)
{
  int written = 0;
  size_t len = 0;
  len = strlen(s);
  written = mclWrite(2 /* stderr */, s, sizeof(char)*len);
  if (len > 0 && s[ len-1 ] != '\n')
    written += mclWrite(2 /* stderr */, "\n", sizeof(char));
  return written;
}

#ifdef __cplusplus
} /* End extern "C" block */
#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_eg_skeleton_laplacian_rosa_C_API
#define LIB_eg_skeleton_laplacian_rosa_C_API /* No special import/export declaration */
#endif

LIB_eg_skeleton_laplacian_rosa_C_API 
bool MW_CALL_CONV eg_skeleton_laplacian_rosaInitializeWithHandlers(
    mclOutputHandlerFcn error_handler,
    mclOutputHandlerFcn print_handler)
{
    int bResult = 0;
  if (_mcr_inst != NULL)
    return true;
  if (!mclmcrInitialize())
    return false;
  if (!GetModuleFileName(GetModuleHandle("eg_skeleton_laplacian_rosa"), path_to_dll, _MAX_PATH))
    return false;
    {
        mclCtfStream ctfStream = 
            mclGetEmbeddedCtfStream(path_to_dll);
        if (ctfStream) {
            bResult = mclInitializeComponentInstanceEmbedded(   &_mcr_inst,
                                                                error_handler, 
                                                                print_handler,
                                                                ctfStream);
            mclDestroyStream(ctfStream);
        } else {
            bResult = 0;
        }
    }  
    if (!bResult)
    return false;
  return true;
}

LIB_eg_skeleton_laplacian_rosa_C_API 
bool MW_CALL_CONV eg_skeleton_laplacian_rosaInitialize(void)
{
  return eg_skeleton_laplacian_rosaInitializeWithHandlers(mclDefaultErrorHandler, 
                                                          mclDefaultPrintHandler);
}

LIB_eg_skeleton_laplacian_rosa_C_API 
void MW_CALL_CONV eg_skeleton_laplacian_rosaTerminate(void)
{
  if (_mcr_inst != NULL)
    mclTerminateInstance(&_mcr_inst);
}

LIB_eg_skeleton_laplacian_rosa_C_API 
void MW_CALL_CONV eg_skeleton_laplacian_rosaPrintStackTrace(void) 
{
  char** stackTrace;
  int stackDepth = mclGetStackTrace(&stackTrace);
  int i;
  for(i=0; i<stackDepth; i++)
  {
    mclWrite(2 /* stderr */, stackTrace[i], sizeof(char)*strlen(stackTrace[i]));
    mclWrite(2 /* stderr */, "\n", sizeof(char)*strlen("\n"));
  }
  mclFreeStackTrace(&stackTrace, stackDepth);
}


LIB_eg_skeleton_laplacian_rosa_C_API 
bool MW_CALL_CONV mlxEg_skeleton_laplacian_rosa(int nlhs, mxArray *plhs[], int nrhs, 
                                                mxArray *prhs[])
{
  return mclFeval(_mcr_inst, "eg_skeleton_laplacian_rosa", nlhs, plhs, nrhs, prhs);
}

LIB_eg_skeleton_laplacian_rosa_C_API 
bool MW_CALL_CONV mlfEg_skeleton_laplacian_rosa()
{
  return mclMlfFeval(_mcr_inst, "eg_skeleton_laplacian_rosa", 0, 0, 0);
}

