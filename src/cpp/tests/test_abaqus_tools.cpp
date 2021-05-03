/**
  * \file test_umat.cpp
  *
  * Tests for umat
  */


#define BOOST_TEST_MODULE test_umat
#include <boost/test/included/unit_test.hpp>

#include<vector_tools.h>

#include<abaqus_tools.h>

BOOST_AUTO_TEST_CASE( testColumnToRowMajor ){
    /*!
     * Test column to row major conversion function.
     * Uses c++ vectors and pointers to avoid adding fortran code to project.
     */

    //Fake a Fortran column major array in memory with a c++ row major vector
    std::vector< double > column_major = { 1, 4,
                                           2, 5,
                                           3, 6 };
    double *column_major_pointer = column_major.data();
    const int height = 2;
    const int width = 3;
    std::vector< std::vector< double > > row_major;
    row_major = abaqusTools::columnToRowMajor( column_major_pointer, height, width);
    std::vector< std::vector< double > > answer = { { 1, 2, 3 },
                                                    { 4, 5, 6 } };

    BOOST_CHECK( vectorTools::fuzzyEquals( row_major, answer ) );
}

BOOST_AUTO_TEST_CASE( testRowToColumnMajor ){
    /*!
     * Test row to column major conversion function.
     * Uses c++ vectors and pointers to avoid adding fortran code to project.
     */

    //Test the interface using a c++ vector of vectors
    //Fake a Fortran column major array in memory with a c++ row major vector
    std::vector< double > column_major(6);
    double *column_major_pointer = column_major.data( );
    std::vector< double > expected = { 1, 4,
                                       2, 5,
                                       3, 6 };
    const int height = 2;
    const int width = 3;
    std::vector< std::vector< double > > row_major = { { 1, 2, 3 },
                                                       { 4, 5, 6 } };
    abaqusTools::rowToColumnMajor( column_major_pointer, row_major, height, width );

    BOOST_CHECK( vectorTools::fuzzyEquals( column_major, expected ) );

    //Test the interface using a c++ vector saved in row major order
    //Reset fake Fortran column major array
    column_major = { 0, 0, 0, 0, 0, 0 };
    std::vector< double > row_major_vector = { 1, 2, 3,
                                               4, 5, 6 };
    abaqusTools::rowToColumnMajor( column_major_pointer, row_major_vector, height, width );

    BOOST_CHECK( vectorTools::fuzzyEquals( column_major, expected ) );

    //Test a single row to single column
    //Create a fake Fortran vector in memory with a c++ row major vector
    std::vector< double > fortran_vector(3);
    double *fortran_vector_pointer = fortran_vector.data( );
    std::vector< double > expected_vector = { 1, 2, 3 };
    std::vector< double > cpp_vector = { 1, 2, 3 };
    abaqusTools::rowToColumnMajor( fortran_vector_pointer, cpp_vector, 1, 3 );

    BOOST_CHECK( vectorTools::fuzzyEquals( fortran_vector, expected_vector ) );
}

BOOST_AUTO_TEST_CASE( testExpandAbaqusNTENSVector ){
    /*!
     * Test expansion of stress and strain type components to full Abaqus vectors
     */

     //Initialize common test variables
     int NDI;
     int NSHR;
     std::vector< double > vector_expansion(6, -666.);

     //Test full size vector
     std::vector< double > abaqus_full = { 11, 22, 33, 12, 13, 23 };
     std::vector< double > expected_full = { 11, 22, 33, 12, 13, 23 };
     NDI = 3;
     NSHR = 3;

     vector_expansion = abaqusTools::expandAbaqusNTENSVector( abaqus_full, NDI, NSHR );

     BOOST_CHECK( vectorTools::fuzzyEquals( vector_expansion, expected_full ) );

     //Test plane stress vector
     std::fill(vector_expansion.begin(), vector_expansion.end(), -666.);
     std::vector< double > abaqus_plane_stress = { 11, 22, 12 };
     std::vector< double > expected_plane_stress = { 11, 22, 0., 12, 0., 0. };
     NDI = 2;
     NSHR = 1;

     vector_expansion = abaqusTools::expandAbaqusNTENSVector( abaqus_plane_stress, NDI, NSHR );

     BOOST_CHECK( vectorTools::fuzzyEquals( vector_expansion, expected_plane_stress ) );
}

BOOST_AUTO_TEST_CASE( testContractAbaqusNTENSVector ){
    /*!
     * Test contraction of stress and strain type components to full Abaqus vectors
     */

     //Initialize common test variables
     int NDI;
     int NSHR;

     //Test full size vector
     std::vector< double > vector_contraction_full(6, -666.);
     std::vector< double > abaqus_full = { 11, 22, 33, 12, 13, 23 };
     std::vector< double > expanded_full = { 11, 22, 33, 12, 13, 23 };
     NDI = 3;
     NSHR = 3;

     vector_contraction_full = abaqusTools::contractAbaqusNTENSVector( expanded_full, NDI, NSHR );

     BOOST_CHECK( vectorTools::fuzzyEquals( vector_contraction_full, abaqus_full ) );

     //Test plane stress vector
     std::vector< double > vector_contraction_plane_stress(3, -666.);
     std::vector< double > abaqus_plane_stress = { 11, 22, 12 };
     std::vector< double > expanded_plane_stress = { 11, 22, 0., 12, 0., 0. };
     NDI = 2;
     NSHR = 1;

     vector_contraction_plane_stress = abaqusTools::contractAbaqusNTENSVector( expanded_plane_stress, NDI, NSHR );

     BOOST_CHECK( vectorTools::fuzzyEquals( vector_contraction_plane_stress, abaqus_plane_stress ) );

}

BOOST_AUTO_TEST_CASE( testContractAbaqusNTENSMatrix ){
    /*!
     * Test contraction of NTENS matrix components from full Abaqus matrices of size 6x6
     */

     //Initialize common test variables
     int NDI;
     int NSHR;

     //Test full size matrix
     std::vector< std::vector< double > > matrix_contraction_full( 6, std::vector< double >( 6, -666. ) );
     std::vector< std::vector< double > > abaqus_full   = { { 1111, 1122, 1133, 1112, 1113, 1123 },
                                                            { 1122, 2222, 2233, 2212, 2213, 2223 },
                                                            { 1133, 2233, 3333, 3312, 3313, 3323 },
                                                            { 1112, 2212, 3312, 1212, 1213, 1223 },
                                                            { 1113, 2213, 3313, 1213, 1313, 1323 },
                                                            { 1123, 2223, 3323, 1223, 1323, 2323 } };
     std::vector< std::vector< double > > expanded_full = { { 1111, 1122, 1133, 1112, 1113, 1123 },
                                                            { 1122, 2222, 2233, 2212, 2213, 2223 },
                                                            { 1133, 2233, 3333, 3312, 3313, 3323 },
                                                            { 1112, 2212, 3312, 1212, 1213, 1223 },
                                                            { 1113, 2213, 3313, 1213, 1313, 1323 },
                                                            { 1123, 2223, 3323, 1223, 1323, 2323 } };
     NDI = 3;
     NSHR = 3;

     matrix_contraction_full = abaqusTools::contractAbaqusNTENSMatrix( expanded_full, NDI, NSHR );

     BOOST_CHECK( vectorTools::fuzzyEquals( matrix_contraction_full, abaqus_full ) );

     //Test plane stress matrix
     std::vector< std::vector< double > > matrix_contraction_plane_stress( 3, std::vector< double >( 3, -666. ) );
     std::vector< std::vector< double > > abaqus_plane_stress   = { { 1111, 1122, 1112 },
                                                                    { 1122, 2222, 2212 },
                                                                    { 1112, 2212, 1212 } };
     std::vector< std::vector< double > > expanded_plane_stress = { { 1111, 1122, 0.00, 1112, 0.00, 0.00 },
                                                                    { 1122, 2222, 0.00, 2212, 0.00, 0.00 },
                                                                    { 0.00, 0.00, 0.00, 0.00, 0.00, 0.00 },
                                                                    { 1112, 2212, 0.00, 1212, 0.00, 0.00 },
                                                                    { 0.00, 0.00, 0.00, 0.00, 0.00, 0.00 },
                                                                    { 0.00, 0.00, 0.00, 0.00, 0.00, 0.00 } };
     NDI = 2;
     NSHR = 1;

     matrix_contraction_plane_stress = abaqusTools::contractAbaqusNTENSMatrix( expanded_plane_stress, NDI, NSHR );

     BOOST_CHECK( vectorTools::fuzzyEquals( matrix_contraction_plane_stress, abaqus_plane_stress ) );

}

BOOST_AUTO_TEST_CASE( testConstructFullNTENSTensor ){
    /*!
     * Test construction of full stress and strain type tensors from full Abaqus vectors
     */

     //Initialize common test variables
     std::vector< int > result( 9 );
     std::vector< int > abaqus_standard = { 11, 22, 33, 12, 13, 23 };
     std::vector< int > abaqus_explicit = { 11, 22, 33, 12, 23, 13 };
     std::vector< int > expected = { 11, 12, 13,
                                     12, 22, 23,
                                     13, 23, 33 };

     //Check full tensor construction for Abaqus/Standard
     result = abaqusTools::constructFullNTENSTensor( abaqus_standard, true );
     BOOST_CHECK( result == expected );

     //Check full tensor construction for Abaqus/Explicit
     result = abaqusTools::constructFullNTENSTensor( abaqus_explicit, false );
     BOOST_CHECK( result == expected );

     //Test overloaded variation with contracted vector input
     //Check full tensor construction for Abaqus/Standard
     result = abaqusTools::constructFullNTENSTensor( abaqus_standard, 3, 3, true );
     BOOST_CHECK( result == expected );

     //Check full tensor construction for Abaqus/Explicit
     result = abaqusTools::constructFullNTENSTensor( abaqus_explicit, 3, 3, false );
     BOOST_CHECK( result == expected );

}

BOOST_AUTO_TEST_CASE( testDestructFullNTENSTensor ){

     //Initialize common test variables
     std::vector< int > result( 6 );
     std::vector< int > full_tensor = { 11, 12, 13,
                                        12, 22, 23,
                                        13, 23, 33 };
     std::vector< int > abaqus_standard = { 11, 22, 33, 12, 13, 23 };
     std::vector< int > abaqus_explicit = { 11, 22, 33, 12, 23, 13 };

     //Check full tensor destruction for Abaqus/Standard
     result = abaqusTools::destructFullNTENSTensor( full_tensor, true );
     BOOST_CHECK( result == abaqus_standard );

     //Check full tensor destruction for Abaqus/Explicit
     result = abaqusTools::destructFullNTENSTensor( full_tensor, false );
     BOOST_CHECK( result == abaqus_explicit );

     //Test overloaded variation with contracted vector input
     //Check full tensor destruction for Abaqus/Standard
     result = abaqusTools::destructFullNTENSTensor( full_tensor, 3, 3, true );
     BOOST_CHECK( result == abaqus_standard );

     //Check full tensor destruction for Abaqus/Explicit
     result = abaqusTools::destructFullNTENSTensor( full_tensor, 3, 3, false );
     BOOST_CHECK( result == abaqus_explicit );

}

BOOST_AUTO_TEST_CASE( testDestructFullNTENSMatrix ){

     //Initialize common test variables
     std::vector< std::vector< int > > result( 6, std::vector< int >( 6 ) );
                                                         // 0     1     2     3     4     5     6     7     8
     std::vector< std::vector< int > > full_matrix = { { 1166, 1112, 1113, 0.00, 1122, 1123, 0.00, 0.00, 1133 },   // 0
                                                     { { 1112, 1212, 1213, 0.00, 2212, 1223, 0.00, 0.00, 3312 },   // 1
                                                     { { 1113, 1213, 1313, 0.00, 2213, 1323, 0.00, 0.00, 3313 },   // 2
                                                     { { 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00 },   // 3
                                                     { { 1122, 2212, 2213, 0.00, 2222, 2223, 0.00, 0.00, 2233 },   // 4
                                                     { { 1123, 1223, 1323, 0.00, 2223, 2323, 0.00, 0.00, 3323 },   // 5
                                                     { { 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00 },   // 6
                                                     { { 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00 },   // 7
                                                     { { 1133, 3312, 3313, 0.00, 2233, 3323, 0.00, 0.00, 3333 } }; // 8
                                                                        //  0     4     8     1     2     5
     std::vector< std::vector< int > > full_abaqus_standard_matrix = { { 1111, 1122, 1133, 1112, 1113, 1123 },   // 0
                                                                       { 1122, 2222, 2233, 2212, 2213, 2223 },   // 4
                                                                       { 1133, 2233, 3333, 3312, 3313, 3323 },   // 8
                                                                       { 1112, 2212, 3312, 1212, 1213, 1223 },   // 1
                                                                       { 1113, 2213, 3313, 1213, 1313, 1323 },   // 2
                                                                       { 1123, 2223, 3323, 1223, 1323, 2323 } }; // 5

     std::vector< std::vector< int > > full_abaqus_explicit_matrix = { { 1111, 1122, 1133, 1112, 1123, 1113 },
                                                                       { 1122, 2222, 2233, 2212, 2223, 2213 },
                                                                       { 1133, 2233, 3333, 3312, 3323, 3313 },
                                                                       { 1112, 2212, 3312, 1212, 1223, 1213 },
                                                                       { 1123, 2223, 3323, 1223, 1323, 1313 },
                                                                       { 1113, 2213, 3313, 1213, 1313, 2313 } };

     //Check full matrix destruction for Abaqus/Standard
     //Check full matrix destruction for Abaqus/Explicit

}
