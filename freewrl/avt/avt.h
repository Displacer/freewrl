#ifndef _AVT_H__
#define _AVT_H__

struct AVdat_st {
    void *opaque;
    struct AVdat_st *next;
};

typedef struct AVdat_st *AVdat_t;

struct AVnode_st;

typedef struct AVnode_st *AVnode_t;
typedef struct AVnode_st *AVtree_t;

struct AVnode_st {
    double		z;
    struct AVnode_st   *l, *r;
    int			h;
    AVdat_t		dat;
};

typedef enum AVway_en {
    AVt_ASCENDING_WAY = -1, 
    AVt_DESCENDING_WAY = 1
} AVway_t;

typedef void(*AVcb_t)(int, double, void *);

extern AVtree_t AVt_reset ( AVtree_t tree );
extern AVtree_t AVt_insert( AVtree_t tree, double z, AVdat_t dat);
extern void	AVt_apply ( AVtree_t tree, AVway_t way, AVcb_t cbFunc );

#endif /* _AVT_H__ */
