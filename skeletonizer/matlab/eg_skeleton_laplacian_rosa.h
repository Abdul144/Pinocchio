/*
 * MATLAB Compiler: 4.17 (R2012a)
 * Date: Fri Sep 21 15:49:46 2012
 * Arguments: "-B" "macro_default" "-l" "eg_skeleton_laplacian_rosa.m" 
 */

#ifndef __eg_skeleton_laplacian_rosa_h
#define __eg_skeleton_laplacian_rosa_h 1

#if defined(__cplusplus) && !defined(mclmcrrt_h) && defined(__linux__)
#  pragma implementation "mclmcrrt.h"
#endif
#include "mclmcrrt.h"
#ifdef __cplusplus
extern "C" {
#endif

#if defined(__SUNPRO_CC)
/* Solaris shared libraries use __global, rather than mapfiles
 * to define the API exported from a shared library. __global is
 * only necessary when building the library -- files including
 * this header file to use the library do not need the __global
 * declaration; hence the EXPORTING_<library> logic.
 */

#ifdef EXPORTING_eg_skeleton_laplacian_rosa
#define PUBLIC_eg_skeleton_laplacian_rosa_C_API __global
#else
#define PUBLIC_eg_skeleton_laplacian_rosa_C_API /* No import statement needed. */
#endif

#define LIB_eg_skeleton_laplacian_rosa_C_API PUBLIC_eg_skeleton_laplacian_rosa_C_API

#elif defined(_HPUX_SOURCE)

#ifdef EXPORTING_eg_skeleton_laplacian_rosa
#define PUBLIC_eg_skeleton_laplacian_rosa_C_API __declspec(dllexport)
#else
#define PUBLIC_eg_skeleton_laplacian_rosa_C_API __declspec(dllimport)
#endif

#define LIB_eg_skeleton_laplacian_rosa_C_API PUBLIC_eg_skeleton_laplacian_rosa_C_API


#else

#define LIB_eg_skeleton_laplacian_rosa_C_API

#endif

/* This symbol is defined in shared libraries. Define it here
 * (to nothing) in case this isn't a shared library. 
 */
#ifndef LIB_eg_skeleton_laplacian_rosa_C_API 
#define LIB_eg_skeleton_laplacian_rosa_C_API /* No special import/export declaration */
#endif

extern LIB_eg_skeleton_laplacian_rosa_C_API 
bool MW_CALL_CONV eg_skeleton_laplacian_rosaInitializeWithHandlers(
       mclOutputHandlerFcn error_handler, 
       mclOutputHandlerFcn print_handler);

extern LIB_eg_skeleton_laplacian_rosa_C_API 
bool MW_CALL_CONV eg_skeleton_laplacian_rosaInitialize(void);

extern LIB_eg_skeleton_laplacian_rosa_C_API 
void MW_CALL_CONV eg_skeleton_laplacian_rosaTerminate(void);



extern LIB_eg_skeleton_laplacian_rosa_C_API 
void MW_CALL_CONV eg_skeleton_laplacian_rosaPrintStackTrace(void);

extern LIB_eg_skeleton_laplacian_rosa_C_API 
bool MW_CALL_CONV mlxEg_skeleton_laplacian_rosa(int nlhs, mxArray *plhs[], int nrhs, 
                                                mxArray *prhs[]);



extern LIB_eg_skeleton_laplacian_rosa_C_API bool MW_CALL_CONV mlfEg_skeleton_laplacian_rosa();

#ifdef __cplusplus
}
#endif
#endif
