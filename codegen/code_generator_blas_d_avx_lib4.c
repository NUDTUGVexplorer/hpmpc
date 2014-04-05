#include <stdio.h>

#include "problem_size.h"
#include "../include/block_size.h"

#define NZ NX+NU+1
#define PNZ D_MR*((NZ+D_MR-NU%D_MR+D_MR-1)/D_MR);



/* computes the lower triangular Cholesky factor of pC, */
/* and copies its transposed in pL                      */
void dpotrf_p_dcopy_p_t_code_generator(FILE *f, int n, int nna)
	{

	int i, j;
	
	const int bs = 4;
	
	const int sdc = PNZ;
	const int sdl = PNZ;
	
	j = 0;
	if(j<nna-3)
		{
	fprintf(f, "	kernel_dpotrf_dtrsv_4x4_sse_lib4(%d, %d, &pC[%d], %d);\n", n-j-4, (bs-(j+4)%bs)%bs, 0+j*bs+j*sdc, sdc);
	            	j += 4;     
	            	for(; j<nna-3; j+=4)
	            		{     
	            		i = j;     
	            		for(; i<n-4; i+=8)     
	            			{
	fprintf(f, "	kernel_dgemm_pp_nt_8x4_avx_lib4(%d, &pC[%d], &pC[%d], &pC[%d], &pC[%d], &pC[%d], %d, -1);\n", j, 0+i*sdc, 0+(i+4)*sdc, 0+j*sdc, 0+j*bs+i*sdc, 0+j*bs+(i+4)*sdc, bs);
	            			}     
	            		for(; i<n; i+=4)     
	            			{     
	fprintf(f, "	kernel_dgemm_pp_nt_4x4_avx_lib4(%d, &pC[%d], &pC[%d], &pC[%d], %d, -1);\n", j, 0+i*sdc, 0+j*sdc, 0+j*bs+i*sdc, bs);
	            			}     
	fprintf(f, "	kernel_dpotrf_dtrsv_4x4_sse_lib4(%d, %d, &pC[%d], %d);\n", n-j-4, (bs-(j+4)%bs)%bs, 0+j*bs+j*sdc, sdc);
	            		}     
		}
	int j0 = j;
	if(j==0) // assume that n>0
		{
	fprintf(f, "	kernel_dpotrf_dtrsv_dcopy_4x4_sse_lib4(%d, %d, &pC[%d], %d, %d, &pL[%d], %d);\n", n-j-4, (bs-(j+4)%bs)%bs, 0+j*bs+j*sdc, sdc, (bs-nna%bs)%bs, 0+(j-j0)*bs+((j-j0)/bs)*bs*sdc, sdl);
		
		j += 4;
		}
	for(; j<n; j+=4)
		{
		i = j;
		for(; i<n-4; i+=8)
			{
	fprintf(f, "	kernel_dgemm_pp_nt_8x4_avx_lib4(%d, &pC[%d], &pC[%d], &pC[%d], &pC[%d], &pC[%d], %d, -1);\n", j, 0+i*sdc, 0+(i+4)*sdc, 0+j*sdc, 0+j*bs+i*sdc, 0+j*bs+(i+4)*sdc, bs);
			
			}
		for(; i<n; i+=4)
			{
	fprintf(f, "	kernel_dgemm_pp_nt_4x4_avx_lib4(%d, &pC[%d], &pC[%d], &pC[%d], %d, -1);\n", j, 0+i*sdc, 0+j*sdc, 0+j*bs+i*sdc, bs);
			
			}
	fprintf(f, "	kernel_dpotrf_dtrsv_dcopy_4x4_sse_lib4(%d, %d, &pC[%d], %d, %d, &pL[%d], %d);\n", n-j-4, (bs-(j+4)%bs)%bs, 0+j*bs+j*sdc, sdc, (bs-nna%bs)%bs, 0+(j-j0)*bs+((j-j0)/bs)*bs*sdc, sdl);
		
		}

	}



/* preforms                                          */
/* C  = A * B'                                       */
/* where A, B and C are packed with block size 4,    */
/* and B is upper triangular                         */
void dtrmm_ppp_code_generator(FILE *f, int m, int n, int offset)
	{
	
	int i, j;
	
	const int bs = 4;
	
	const int sda = PNZ;
	const int sdb = PNZ;
	const int sdc = PNZ;

	if(offset%bs!=0)
	fprintf(f, "	pB = pB+%d;\n", bs*sdb+bs*bs);
	
	i = 0;
	for(; i<m-4; i+=8)
		{
		j = 0;
		for(; j<n-3; j+=4)
			{
fprintf(f, "	kernel_dgemm_pp_nt_8x4_avx_lib4(%d, &pA[%d], &pA[%d], &pB[%d], &pC[%d], &pC[%d], %d, 0);\n", n-j, j*bs+i*sda, j*bs+(i+4)*sda, j*bs+j*sdb, j*bs+i*sdc, j*bs+(i+4)*sdc, bs);
			}
		if(n-j==1)
			{
fprintf(f, "	corner_dtrmm_pp_nt_8x1_avx_lib4(&pA[%d], &pA[%d], &pB[%d], &pC[%d], &pC[%d], %d);\n", j*bs+i*sda, j*bs+(i+4)*sda, j*bs+j*sdb, j*bs+i*sdc, j*bs+(i+4)*sdc, bs);
			}
		else if(n-j==2)
			{
fprintf(f, "	corner_dtrmm_pp_nt_8x2_avx_lib4(&pA[%d], &pA[%d], &pB[%d], &pC[%d], &pC[%d], %d);\n", j*bs+i*sda, j*bs+(i+4)*sda, j*bs+j*sdb, j*bs+i*sdc, j*bs+(i+4)*sdc, bs);
			}
		else if(n-j==3)
			{
fprintf(f, "	corner_dtrmm_pp_nt_8x3_avx_lib4(&pA[%d], &pA[%d], &pB[%d], &pC[%d], &pC[%d], %d);\n", j*bs+i*sda, j*bs+(i+4)*sda, j*bs+j*sdb, j*bs+i*sdc, j*bs+(i+4)*sdc, bs);
			}
		}
	for(; i<m; i+=4)
		{
		j = 0;
		for(; j<n-3; j+=4)
			{
fprintf(f, "	kernel_dgemm_pp_nt_4x4_avx_lib4(%d, &pA[%d], &pB[%d], &pC[%d], %d, 0);\n", n-j, j*bs+i*sda, j*bs+j*sdb, j*bs+i*sdc, bs);
			}
		if(n-j==1)
			{
fprintf(f, "	corner_dtrmm_pp_nt_4x1_avx_lib4(&pA[%d], &pB[%d], &pC[%d], %d);\n", j*bs+i*sda, j*bs+j*sdb, j*bs+i*sdc, bs);
			}
		else if(n-j==2)
			{
fprintf(f, "	corner_dtrmm_pp_nt_4x2_avx_lib4(&pA[%d], &pB[%d], &pC[%d], %d);\n", j*bs+i*sda, j*bs+j*sdb, j*bs+i*sdc, bs);
			}
		else if(n-j==3)
			{
fprintf(f, "	corner_dtrmm_pp_nt_4x3_avx_lib4(&pA[%d], &pB[%d], &pC[%d], %d);\n", j*bs+i*sda, j*bs+j*sdb, j*bs+i*sdc, bs);
			}
		}

	// add to the last row
	for(j=0; j<n; j++)
		{
	fprintf(f, "	pC[%d] += pB[%d];\n", (m-1)%bs+j*bs+((m-1)/bs)*bs*sdc, j%bs+n*bs+(j/bs)*bs*sdb);
		
		}

/*	fprintf(f, "	\n");*/

	}



/* preforms                                          */
/* C  = A * A'                                       */
/* where A, C are packed with block size 4           */
void dsyrk_ppp_code_generator(FILE *f, int n, int m)
	{
	
	int i, j;
	
	const int bs = 4;
	
	const int sda = PNZ;
/*	const int sdb = PNZ;*/
	const int sdc = PNZ;
	
	i = 0;
	for(; i<n-4; i+=8)
		{
		j = 0;
		for(; j<i+4; j+=4)
			{
	fprintf(f, "	kernel_dgemm_pp_nt_8x4_avx_lib4(%d, &pA[%d], &pA[%d], &pA[%d], &pC[%d], &pC[%d], %d, 1);\n", m, i*sda, (i+4)*sda, j*sda, (j+0)*bs+i*sdc, (j+0)*bs+(i+4)*sdc, bs);
			}
		if(j<i+8)
			{
	fprintf(f, "	kernel_dgemm_pp_nt_4x4_avx_lib4(%d, &pA[%d], &pA[%d], &pC[%d], %d, 1);\n", m, (i+4)*sda, j*sda, (j+0)*bs+(i+4)*sdc, bs);
			}
		}
	for(; i<n; i+=4)
		{
		j = 0;
		for(; j<i+4; j+=4)
			{
	fprintf(f, "	kernel_dgemm_pp_nt_4x4_avx_lib4(%d, &pA[%d], &pA[%d], &pC[%d], %d, 1);\n", m, i*sda, j*sda, (j+0)*bs+i*sdc, bs);
			}
		}

/*	fprintf(f, "	\n");*/

	}



void dgemv_p_t_code_generator(FILE *f, int n, int m, int offset, int alg)
	{
	
	const int bs = 4;
	
	const int sda = PNZ;

	int nna = (bs-offset%bs)%bs;
	
	int j;
	
	j=0;
	for(; j<m-7; j+=8)
		{
	fprintf(f, "	kernel_dgemv_t_8_avx_lib4(%d, %d, pA+%d, %d, x, y+%d, %d);\n", n, nna, j*bs, sda, j, alg);
		}
	for(; j<m-3; j+=4)
		{
	fprintf(f, "	kernel_dgemv_t_4_avx_lib4(%d, %d, pA+%d, %d, x, y+%d, %d);\n", n, nna, j*bs, sda, j, alg);
		}
	for(; j<m-1; j+=2)
		{
	fprintf(f, "	kernel_dgemv_t_2_avx_lib4(%d, %d, pA+%d, %d, x, y+%d, %d);\n", n, nna, j*bs, sda, j, alg);
		}
	for(; j<m; j++)
		{
	fprintf(f, "	kernel_dgemv_t_1_avx_lib4(%d, %d, pA+%d, %d, x, y+%d, %d);\n", n, nna, j*bs, sda, j, alg);
		}

/*	fprintf(f, "	\n");*/
	}



void dtrsv_p_t_code_generator(FILE *f, int n)
	{
	
	const int bs = 4;
	
	const int sda = PNZ;

	int i, j;
	
	int rn = n%bs;
	int qn = n/bs;
	int ri, qi;
	
/*	fprintf(f, "	double *ptrA, *ptrx;\n");*/
	
	// clean up stuff at the end
	j = 0;
	fprintf(f, "	ptrA = pA + %d;\n", qn*bs*(sda+bs));
	fprintf(f, "	ptrx = x + %d;\n", qn*bs);

	for(; j<rn%2; j++)
		{
		i = rn-1-j;
	fprintf(f, "	kernel_dgemv_t_1_avx_lib4(%d, %d, &ptrA[%d], %d, &ptrx[%d], &ptrx[%d], -1);\n", j, j, i+1+bs*(i+0), sda, i+1, i);
	fprintf(f, "	ptrx[%d] = (ptrx[%d]) / ptrA[%d];\n", i+0, i+0, i+0+bs*(i+0));
		}
	for(; j<rn; j+=2)
		{
		i = rn-2-j;
	fprintf(f, "	kernel_dgemv_t_2_avx_lib4(%d, %d, &ptrA[%d], %d, &ptrx[%d], &ptrx[%d], -1);\n", j, j, i+2+bs*(i+0), sda, i+2, i);
	fprintf(f, "	ptrx[%d] = (ptrx[%d]) / ptrA[%d];\n", i+1, i+1, (i+1)+bs*(i+1));
	fprintf(f, "	ptrx[%d] = (ptrx[%d] - ptrA[%d]*ptrx[%d]) / ptrA[%d];\n", i+0, i+0, (i+1)+bs*(i+0), i+1, (i+0)+bs*(i+0));
		}

	// blocks of 8
	j = 0;
	for(; j<qn-1; j+=2)
		{
		
		// all 4 rows
	fprintf(f, "	ptrA = pA + %d;\n", (qn-j-2)*bs*(sda+bs));
	fprintf(f, "	ptrx = x  + %d;\n", (qn-j-2)*bs);
		

		// correct
	fprintf(f, "	kernel_dgemv_t_8_avx_lib4(%d, 0, ptrA+%d, sda, ptrx+8, ptrx, -1);\n", rn+j*bs, 2*bs*sda);
		

		// last 4 rows
	fprintf(f, "	ptrA = pA + %d;\n", (qn-j-1)*bs*(sda+bs));
	fprintf(f, "	ptrx = x  + %d;\n", (qn-j-1)*bs);

		// solve
	fprintf(f, "	ptrx[3] = (ptrx[3]) / ptrA[%d];\n", 3+bs*3);
	fprintf(f, "	ptrx[2] = (ptrx[2] - ptrA[%d]*ptrx[3]) / ptrA[%d];\n", 3+bs*2, 2+bs*2);
	fprintf(f, "	ptrx[1] = (ptrx[1] - ptrA[%d]*ptrx[3] - ptrA[%d]*ptrx[2]) / ptrA[%d];\n", 3+bs*1, 2+bs*1, 1+bs*1);
	fprintf(f, "	ptrx[0] = (ptrx[0] - ptrA[%d]*ptrx[3] - ptrA[%d]*ptrx[2] - ptrA[%d]*ptrx[1]) / ptrA[%d];\n", 3+bs*0, 2+bs*0, 1+bs*0, 0+bs*0);

		// first 4 rows
	fprintf(f, "	ptrA = pA + %d;\n", (qn-j-2)*bs*(sda+bs));
	fprintf(f, "	ptrx = x  + %d;\n", (qn-j-2)*bs);

		// correct
	fprintf(f, "	kernel_dgemv_t_4_avx_lib4(4, 0, ptrA+%d, sda, ptrx+4, ptrx, -1);\n", bs*sda);

		// solve
	fprintf(f, "	ptrx[3] = (ptrx[3]) / ptrA[%d];\n", 3+bs*3);
	fprintf(f, "	ptrx[2] = (ptrx[2] - ptrA[%d]*ptrx[3]) / ptrA[%d];\n", 3+bs*2, 2+bs*2);
	fprintf(f, "	ptrx[1] = (ptrx[1] - ptrA[%d]*ptrx[3] - ptrA[%d]*ptrx[2]) / ptrA[%d];\n", 3+bs*1, 2+bs*1, 1+bs*1);
	fprintf(f, "	ptrx[0] = (ptrx[0] - ptrA[%d]*ptrx[3] - ptrA[%d]*ptrx[2] - ptrA[%d]*ptrx[1]) / ptrA[%d];\n", 3+bs*0, 2+bs*0, 1+bs*0, 0+bs*0);

		}
	
	// blocks of 4
	for(; j<qn; j++)
		{
		
		// first 4 rows
	fprintf(f, "	ptrA = pA + %d;\n", (qn-j-1)*bs*(sda+bs));
	fprintf(f, "	ptrx = x  + %d;\n", (qn-j-1)*bs);
		
	fprintf(f, "	kernel_dgemv_t_4_avx_lib4(%d, 0, ptrA+%d, sda, ptrx+4, ptrx, -1);\n", rn+j*bs, bs*sda);
	fprintf(f, "	ptrx[3] = (ptrx[3]) / ptrA[%d];\n", 3+bs*3);
	fprintf(f, "	ptrx[2] = (ptrx[2] - ptrA[%d]*ptrx[3]) / ptrA[%d];\n", 3+bs*2, 2+bs*2);
	fprintf(f, "	ptrx[1] = (ptrx[1] - ptrA[%d]*ptrx[3] - ptrA[%d]*ptrx[2]) / ptrA[%d];\n", 3+bs*1, 2+bs*1, 1+bs*1);
	fprintf(f, "	ptrx[0] = (ptrx[0] - ptrA[%d]*ptrx[3] - ptrA[%d]*ptrx[2] - ptrA[%d]*ptrx[1]) / ptrA[%d];\n", 3+bs*0, 2+bs*0, 1+bs*0, 0+bs*0);

		}

/*	fprintf(f, "	\n");*/

	}