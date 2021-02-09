/**
  ******************************************************************************
  * \file umat.h
  ******************************************************************************
  * The declarations and definitions required for an Abaqus UMAT c++ interface.
  ******************************************************************************
  */

#include<iostream>
#include<vector>
#include<string.h>
#include<stdio.h>

#include<vector_tools.h>

#include<cpp_stub.h>

extern "C" void UMAT( double *STRESS,       double *STATEV,       double *DDSDDE,       double &SSE,          double &SPD,
                      double &SCD,          double &RPL,          double *DDSDDT,       double *DRPLDE,       double &DRPLDT,
                      const double *STRAN,  const double *DSTRAN, const double *TIME,   const double &DTIME,  const double &TEMP,
                      const double &DTEMP,  const double *PREDEF, const double *DPRED,  const char *CMNAME,   const int &NDI,
                      const int &NSHR,      const int &NTENS,     const int &NSTATV,    const double *PROPS,  const int &NPROPS,
                      const double *COORDS, const double *DROT,   double &PNEWDT,       const double &CELENT, const double *DFGRD0,
                      const double *DFGRD1, const int &NOEL,      const int &NPT,       const int &LAYER,     const int &KSPT,
                      const int *JSTEP,     const int &KINC );

template< typename T >
std::vector< std::vector< double > > columnToRowMajor( T *myPointer, const int &width, const int &height );

template< typename T >
void rowToColumnMajor(  T *column_major, const std::vector< std::vector< T > > &row_major_array,
                       const int &width, const int &height);

char *FtoCString( int stringLength, const char* fString );
