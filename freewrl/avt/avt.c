
#include <stdlib.h>
#include "avt.h"

/**
 *
 * Private internal functions for average load tree (aka balanced tree)
 *
 **/

#define AVt_MAX(a, b)	((a)>(b)?(a):(b))

    /*
     * AVt_height
     * Returns the max height of the tree
     */
	static int
AVt_height( AVnode_t n )
{
    return ((AVnode_t)NULL == n ? -1 : n->h);
}

    /*
     * AVt_leftRot
     * Realize a single rotation of node around the left
     *
     * pre-condition: n1->l exists
     */
	static AVnode_t
AVt_leftRot( AVnode_t n1 )
{
    AVnode_t n0;

    if ( (AVnode_t)NULL == n1 || (AVnode_t)NULL == n1->l ) return n1;

    n0 = n1->l;
    n1->l = n0->r;
    n0->r = n1;

    n1->h = AVt_MAX( AVt_height( n1->l ), AVt_height( n1->r ) ) +1;
    n0->h = AVt_MAX( AVt_height( n0->l ), n1->h ) +1;

    return n0;
}

    /*
     * AVt_rightRot
     * Realize a single rotation of node around the right
     *
     * pre-condition: n0->r exists
     */
	static AVnode_t
AVt_rightRot( AVnode_t n0 )
{
    AVnode_t n1;

    if ( (AVnode_t)NULL == n0 || (AVnode_t)NULL == n0->r ) return n1;

    n1 = n0->r;
    n0->r = n1->l;
    n1->l = n0;

    n0->h = AVt_MAX( AVt_height( n0->l ), AVt_height( n0->r ) ) +1;
    n1->h = AVt_MAX( AVt_height( n1->r ), n0->h ) +1;

    return n1;
}

    /*
     * AVt_leftRightDblRot
     * Left-Right double rotation
     * pre-condition: <n> has a left & <n> left has a right
     */
	static AVnode_t
AVt_leftRightDblRot( AVnode_t n )
{
    if ( (AVnode_t)NULL == n || (AVnode_t)NULL == n->l || (AVnode_t)NULL == n->l->r ) return n;
    n->l = AVt_rightRot( n->l );
    return AVt_leftRot( n );
}

    /*
     * AVt_rightLeftDblRot
     * Right-Left double rotation
     * pre-condition: <n> has a right & <n> right has a left
     */
	static AVnode_t
AVt_rightLeftDblRot( AVnode_t n )
{
    if ( (AVnode_t)NULL == n || (AVnode_t)NULL == n->r || (AVnode_t)NULL == n->r->l ) return n;
    n->r = AVt_leftRot( n->r );
    return AVt_rightRot( n );
}

/**
 *
 * Public interface implementation of average load tree (aka balanced tree)
 *
 **/

    /*
     * AVt_reset
     * ---------
     * Reset the AVtree
     *
     * params:	tree - Tree to reset
     * return:  the new tree root (empty)
     */
	AVtree_t
AVt_reset ( AVtree_t tree )
{
    if ( (AVtree_t)NULL != tree )
    {
        AVdat_t cur;

        AVt_reset( tree->l );
        AVt_reset( tree->r );
        cur = tree->dat;
        while ( cur )
        {
            AVdat_t tmp = cur;
            cur = cur->next;
            bzero( tmp, sizeof( struct AVdat_st ) );
            free( (void *)tmp );
        }
        bzero( tree, sizeof( struct AVnode_st ) );
        free( (void *)tree );
        tree =(AVtree_t)NULL;
    }
    return tree;
}

    /*
     * AVt_insert
     * ----------
     * Insert an element in the tree and ensure balancing
     *
     * params:  tree   - Tree in which inserting
     *          z      - The z value of the inserted element
     *          opaque - An opaque for user data (passed to callbacks)
     * return:  The new root of the balanced tree
     */
	AVtree_t
AVt_insert( AVtree_t tree, double z, AVdat_t dat )
{
    if ( (AVnode_t)NULL == tree )
    {
        /* Create new tree */
        tree =(AVtree_t)malloc( sizeof( struct AVnode_st ) );
        if ( (AVnode_t)NULL == tree )
          return( tree );
        else
        {
            bzero( tree, sizeof( struct AVnode_st ) );
            tree->z = z;
            tree->dat = dat;
        }
    }
    else if ( z < tree->z )
    {
        tree->l = AVt_insert( tree->l, z, dat );
        if ( AVt_height( tree->l ) - AVt_height( tree->r ) == 2 )
        {
            if ( z < tree->l->z )
              tree = AVt_leftRot( tree );
            else
              tree = AVt_leftRightDblRot( tree );
        }
    }
    else if ( z > tree->z )
    {
        tree->r = AVt_insert( tree->r, z, dat );
        if ( AVt_height( tree->r ) -AVt_height( tree->l ) == 2 )
        {
            if ( z > tree->r->z )
              tree = AVt_rightRot( tree );
            else
              tree = AVt_rightLeftDblRot( tree );
        }
    }
    else if ( z == tree->z )
    {
        AVdat_t cur = tree->dat;
        if ( (AVdat_t)NULL != cur )
        {
            while ( cur->next ) cur = cur->next;
            cur->next = dat;
        }
        else
          tree->dat = dat;
    }
    tree->h = AVt_MAX( AVt_height( tree->l ), AVt_height( tree->r ) ) +1;
    return tree;
}

    /*
     * AVt_apply
     * ---------
     * Apply a callback function to each node. Nodes are followed in
     * either ascending or descending order (as specified in 'way').
     * The callback 'cbFunc' is passed the opaque of the node.
     *   void (*cbFunc)( int height, double z, void *opaque );
     */
	void
AVt_apply ( AVtree_t tree, AVway_t way, AVcb_t cbFunc )
{
    if ( (AVtree_t)NULL != tree )
    {
        AVdat_t cur = tree->dat;

        AVt_apply( way == AVt_ASCENDING_WAY ? tree->l : tree->r, way, cbFunc );
        while ( cur )
        {
            (*cbFunc)( tree->h, tree->z, cur->opaque );
            cur = cur->next;
        }
        AVt_apply( way == AVt_ASCENDING_WAY ? tree->r : tree->l, way, cbFunc );
    }
}



