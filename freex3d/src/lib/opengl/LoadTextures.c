/*
=INSERT_TEMPLATE_HERE=

$Id: LoadTextures.c,v 1.1 2009/08/17 22:25:58 couannette Exp $

New implementation of the texture thread.
 - Setup renderer capabilities
 - Setup default texture & default shader
 - Routines to load textures from file/URL/inline.
 - ...

NOTE: a lot of work have to be done here ;*).
*/

#include <config.h>
#include <system.h>
#include <system_threads.h>
#include <display.h>
#include <internal.h>

#include "vrml_parser/Structs.h"
#include "main/ProdCon.h"

#include "OpenGL_Utils.h"
#include "LoadTextures.h"
#include "Textures.h"

#include <Imlib2.h>


/* Globals */

s_renderer_capabilities_t rdr_caps;

/* all functions here are named _mb_ and lower case */

/* utility routines */

static void rdr_caps_dump(s_renderer_capabilities_t *rc)
{
    if (!rc) {
	printf("renderer capabilities: NULL\n");
	return;
    } else {
	printf( "renderer capabilities: %p\n"
		" multi textures : %s\n"
		" texture unites : %u\n",
		rc,
		BOOL_STR(rc->av_multitexture),
		rc->texture_units );
    }
}

/* init before threading */

void texture_loader_initialize()
{
    /* init Imlib2, get load capabilities : not much to do ... */

    /* get renderer capabilities 
       interesting caps:
       - multi texture     : true/false
       - texture units     : integer
       - shader GLSL       : true/false
       - max texture size  : integer x integer
       - non PO2 texture   : true/false
    */
    memset(&rdr_caps, 0, sizeof(rdr_caps));

    if (GLEW_ARB_multitexture) {
	rdr_caps.av_multitexture = TRUE;
	glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &rdr_caps.texture_units);
	glPrintError("query texture units");
    }

    /* load default material */

    /* load default texture */

    /* load default shader */

    /* print some debug infos */
    rdr_caps_dump(&rdr_caps);
}

/* thread safe functions */

bool is_url(const char *url)
{
#define MAX_PROTOS 3
    static const char *protos[MAX_PROTOS] = { "ftp", "http", "https" };
#define MAX_URL_PROTOCOL_ID 4
    long l;
    unsigned i;
    char *pat;
    pat = strstr(url, "://");
    if (!pat) {
	return FALSE;
    }
    if ((long)(pat-url) > MAX_URL_PROTOCOL_ID) {
	return FALSE;
    }
    l = (long)(pat-url) - 3;
    for (i = 0; i < MAX_PROTOS ; i++) {
	if (strncasecmp(protos[i], url, l) == 0) {
	    return TRUE;
	}
    }
    return FALSE;
}

void findTextureFile_MB(int cwo)
{
    char *tmp;
    const char *filename;
    int i;

    struct X3D_ImageTexture *tex_node;
    struct Uni_String *tex_node_parent;
    struct Multi_String tex_node_url;

    tex_node = (struct X3D_ImageTexture *) loadThisTexture->scenegraphNode;

    switch (loadThisTexture->nodeType) {
    case NODE_ImageTexture:
	tex_node_parent = tex_node->__parenturl;
	tex_node_url = tex_node->url;
	break;
    default:
	WARN_MSG("findTextureFile_MB: node type = %d not implemented\n", loadThisTexture->nodeType);
	break;
    }

    filename = NULL;

    // Loop all strings in Multi_String */
    for (i = 0; i < tex_node_url.n ; i++) {

	tmp = (tex_node_url.p[i])->strptr;
	TRACE_MSG("findTextureFile_MB: processing url %d : %s\n", i, tmp);

	if (is_url(tmp)) {
	    filename = do_get_url(tmp);
	} else {
	    /* not an url: try a local filename */
	    if (do_file_exists(tmp)) {
		filename = strdup(tmp);
	    }
	}

	if (filename) {
	    if( load_texture_from_file(tex_node, tmp) ) {
		break;
	    }
	}

	/* no resource found, try next string */
    }
    WARN_MSG("findTextureFile_MB: no resource found for texture\n");
}

bool load_texture_from_file(struct X3D_ImageTexture *node, const char *filename)
{
    Imlib_Image image;

    image = imlib_load_image(filename);
    if (!image) {
	WARN_MSG("_mb_load_texture_from_file: failed to load image: %s\n", filename);
	return FALSE;
    }

    /* use this image */
    imlib_context_set_image(image);

    /* store actual filename, status, ... */
    loadThisTexture->filename = filename;
    loadThisTexture->status = TEX_NEEDSBINDING;
    loadThisTexture->hasAlpha = (imlib_image_has_alpha() == 1);
    loadThisTexture->imageType = 100; /* not -1, but not PNGTexture neither JPGTexture ... */
    /* query depth with imlib2 ??? */
    loadThisTexture->depth = (loadThisTexture->hasAlpha ? 4 : 3);
    loadThisTexture->frames = 0;
    loadThisTexture->x = imlib_image_get_width();
    loadThisTexture->y = imlib_image_get_height();
    /* this will trigger effective Imlib2 loading */
    loadThisTexture->texdata = (unsigned char *) imlib_image_get_data_for_reading_only();
    
    return TRUE;
}

/* called from the display thread */

bool bind_texture(struct X3D_ImageTexture *node)
{
    /* 
       input:
       node->__textureTableIndex : table entry [ texdata, pixelData ]

       output:
       node->__textureTableIndex : table entry [ OpenGLTexture ]
     */
    return FALSE;
}
