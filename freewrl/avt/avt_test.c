#include "avt.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static char *progname;

static void print_tree( int h, double z, void *opaque )
{
    fprintf( stdout, "Height=%d  z=%04.4f  opaque=%s\n", h, z,(char *)opaque );
}

main( int argc,  char **argv )
{
    int n, i;
    time_t seed;
    AVtree_t tree =(AVtree_t)NULL;
    AVcb_t cb = print_tree;
    
    progname = argv[0];
    if ( argc != 2 )
    {
        fprintf( stderr, "%s: usage: avt_test <n_point>\n", progname );
        exit( 1 );
    }
    n = atoi( argv[1] );
    /* Init seed: naive but enough for tests */
    (void)time( &seed );
    srand( (unsigned int)seed );
    for ( i = 0;i < n; i++ )
    {
        double z =(double)rand()/RAND_MAX;
        AVdat_t dat =(AVdat_t)malloc( sizeof( struct AVdat_st ) );
        if ( !dat )
        {
            fprintf( stderr, "Not enough memory !\n" );
            exit( 1 );
        }
        bzero( dat, sizeof( struct AVdat_st ) );
        dat->opaque =(void *)malloc( 20 * sizeof( char ) );
        sprintf( (char *)dat->opaque, "%04.4f", z );
        tree = AVt_insert( tree, z, dat );
        if ( i%100 ) fprintf( stdout, "." );
    }

    fprintf( stdout, "\n===> INSERTION terminated: Tree height = %d\n", tree->h );

    AVt_apply( tree, AVt_ASCENDING_WAY, cb );
}
