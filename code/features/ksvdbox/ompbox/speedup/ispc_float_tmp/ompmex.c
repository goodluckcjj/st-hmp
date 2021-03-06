/**************************************************************************
 *
 * File name: ompmex.c
 *
 * Ron Rubinstein
 * Computer Science Department
 * Technion, Haifa 32000 Israel
 * ronrubin@cs
 *
 * Last Updated: 18.8.2009
 *
 *************************************************************************/

#include "ompcore.h"
#include "omputils.h"
#include "mexutils.h"


/* Input Arguments */

#define IN_D          prhs[0]
#define IN_X          prhs[1]
#define IN_DtX        prhs[2]
#define IN_G          prhs[3]
#define IN_T          prhs[4]
#define IN_SPARSE_G   prhs[5]
#define IN_MSGDELTA   prhs[6]
#define IN_PROFILE    prhs[7]


/* Output Arguments */

#define	GAMMA_OUT     plhs[0]


/***************************************************************************************/


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[])

{
  double *D_d, *x_d, *DtX_d, *G_d;
  int i,size;
  float *D, *x, *DtX, *G, msgdelta;
  int gmode, profile, T;
  mwSize m, n, L;   /* D is n x m , X is n x L, DtX is m x L */
  
  
  /* check parameters */
  
  checkmatrix(IN_D, "OMP", "D");
  checkmatrix(IN_X, "OMP", "X");
  checkmatrix(IN_DtX, "OMP", "DtX");
  checkmatrix(IN_G, "OMP", "G");
  
  checkscalar(IN_T, "OMP", "T");
  checkscalar(IN_SPARSE_G, "OMP", "sparse_g");
  checkscalar(IN_MSGDELTA, "OMP", "msgdelta");
  checkscalar(IN_PROFILE, "OMP", "profile");

  
  /* get parameters */
  
  x = D = DtX = G = 0;
  
  if (!mxIsEmpty(IN_D)) {
    D_d = mxGetPr(IN_D);
    size=mxGetM(IN_D)*mxGetN(IN_D);
    D=(float*)mxMalloc(size*sizeof(float));
    for(i=0; i<size; i++)
      D[i]=(float)D_d[i];
  }
 
  if (!mxIsEmpty(IN_X)) {
    x_d = mxGetPr(IN_X);
    size=mxGetM(IN_X)*mxGetN(IN_X);
    x=(float*)mxMalloc(size*sizeof(float));
    for(i=0; i<size; i++)
      x[i]=(float)x_d[i];
  }
  
  if (!mxIsEmpty(IN_DtX)) {
    DtX_d = mxGetPr(IN_DtX);
    size=mxGetM(IN_DtX)*mxGetN(IN_DtX);
    DtX=(float*)mxMalloc(size*sizeof(float));
    for(i=0; i<size; i++)
      DtX[i]=(float)DtX_d[i];
  }
  
  if (!mxIsEmpty(IN_G)) {
    G_d = mxGetPr(IN_G);
    size=mxGetM(IN_G)*mxGetN(IN_G);
    G=(float*)mxMalloc(size*sizeof(float));
    for(i=0; i<size; i++)
      G[i]=(float)G_d[i];
  }
  
  T = (int)(mxGetScalar(IN_T)+1e-2);
  if ((int)(mxGetScalar(IN_SPARSE_G)+1e-2)) {
    gmode = SPARSE_GAMMA;
  }
  else {
    gmode = FULL_GAMMA;
  }
  msgdelta = mxGetScalar(IN_MSGDELTA);
  profile = (int)(mxGetScalar(IN_PROFILE)+1e-2);
  
  
  /* check sizes */
  
  if (D && x) {
    n = mxGetM(IN_D);
    m = mxGetN(IN_D);
    L = mxGetN(IN_X);
    
    if (mxGetM(IN_X) != n) {
      mexErrMsgTxt("D and X have incompatible sizes.");
    }
    
    if (G) {
      if (mxGetN(IN_G)!=mxGetM(IN_G)) {
        mexErrMsgTxt("G must be a square matrix.");
      }
      if (mxGetN(IN_G) != m) {
        mexErrMsgTxt("D and G have incompatible sizes.");
      }
    }
  }
  
  else if (DtX) {
    m = mxGetM(IN_DtX);
    L = mxGetN(IN_DtX);
    
    n = T;  /* arbitrary - it is enough to assume signal length is T */
    
    if (mxGetN(IN_G)!=mxGetM(IN_G)) {
      mexErrMsgTxt("G must be a square matrix.");
    }
    if (mxGetN(IN_G) != m) {
      mexErrMsgTxt("DtX and G have incompatible sizes.");
    }
  }
  
  else {
    mexErrMsgTxt("Either D and X, or DtX, must be specified.");
  }
  
  
  /* Do OMP! */

  GAMMA_OUT = ompcore(D, x, DtX, 0, G, n, m, L, T, 0, gmode, profile, msgdelta, 0);
  
  return;
}

