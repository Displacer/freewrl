/*
  $Id: RenderFuncs.c,v 1.166 2013/09/03 20:07:01 crc_canada Exp $

  FreeWRL support library.
  Scenegraph rendering.

*/

/****************************************************************************
    This file is part of the FreeWRL/FreeX3D Distribution.

    Copyright 2009 CRC Canada. (http://www.crc.gc.ca)

    FreeWRL/FreeX3D is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FreeWRL/FreeX3D is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FreeWRL/FreeX3D.  If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/



#include <config.h>
#include <system.h>
#include <display.h>
#include <internal.h>

#include <libFreeWRL.h>

#include "../vrml_parser/Structs.h"
#include "../main/headers.h"
#include "../opengl/Textures.h"
#include "../scenegraph/Component_ProgrammableShaders.h"

#include "Polyrep.h"
#include "Collision.h"
#include "../scenegraph/quaternion.h"
#include "Viewer.h"
#include "LinearAlgebra.h"
#include "../input/SensInterps.h"
#include "system_threads.h"
#include "threads.h"

#include "../opengl/OpenGL_Utils.h"
#include "../scenegraph/Component_Shape.h"
#include "RenderFuncs.h"

typedef float shaderVec4[4];


typedef struct pRenderFuncs{
	/* which arrays are enabled, and defaults for each array */
	int shaderNormalArray;// = TRUE;
	int shaderVertexArray;// = TRUE;
	int shaderColourArray;// = FALSE;
	int shaderTextureArray;// = FALSE;

	float light_linAtten[MAX_LIGHTS];
	float light_constAtten[MAX_LIGHTS];
	float light_quadAtten[MAX_LIGHTS];
	float light_spotCutoffAngle[MAX_LIGHTS];
	float light_spotBeamWidth[MAX_LIGHTS];
	shaderVec4 light_amb[MAX_LIGHTS];
	shaderVec4 light_dif[MAX_LIGHTS];
	shaderVec4 light_pos[MAX_LIGHTS];
	shaderVec4 light_spec[MAX_LIGHTS];
	shaderVec4 light_spotDir[MAX_LIGHTS];
    float light_radius[MAX_LIGHTS];
	GLint lightType[MAX_LIGHTS]; //0=point 1=spot 2=directional
	/* Rearrange to take advantage of headlight when off */
	int nextFreeLight;// = 0;

	/* lights status. Light HEADLIGHT_LIGHT is the headlight */
	GLint lightOnOff[MAX_LIGHTS];

	int cur_hits;//=0;
	void *empty_group;//=0;
	//struct point_XYZ ht1, ht2; not used
	struct point_XYZ hyper_r1,hyper_r2; /* Transformed ray for the hypersensitive node */
	struct currayhit rayph;
	struct X3D_Group *rootNode;//=NULL;	/* scene graph root node */
	struct X3D_Anchor *AnchorsAnchor;// = NULL;
	struct currayhit rayHit,rayHitHyper;
	struct trenderstate renderstate;
	int renderLevel;

	// which Shader is currently in use?
	GLint currentShader;

}* ppRenderFuncs;
void *RenderFuncs_constructor(){
	void *v = malloc(sizeof(struct pRenderFuncs));
	memset(v,0,sizeof(struct pRenderFuncs));
	return v;
}
void RenderFuncs_init(struct tRenderFuncs *t){
	//public

	t->BrowserAction = FALSE;
	//	t->hitPointDist; /* distance in ray: 0 = r1, 1 = r2, 2 = 2*r2-r1... */
	///* used to save rayhit and hyperhit for later use by C functions */
	//t->hyp_save_posn;
	//t->hyp_save_norm;t->ray_save_posn;
	t->hypersensitive = 0;
	t->hyperhit = 0;
	t->have_transparency=FALSE;/* did any Shape have transparent material? */
	/* material node usage depends on texture depth; if rgb (depth1) we blend color field
	   and diffusecolor with texture, else, we dont bother with material colors */
	t->last_texture_type = NOTEXTURE;

	//private
	t->prv = RenderFuncs_constructor();
	{
		ppRenderFuncs p = (ppRenderFuncs)t->prv;
		/* which arrays are enabled, and defaults for each array */
		p->shaderNormalArray = TRUE;
		p->shaderVertexArray = TRUE;
		p->shaderColourArray = FALSE;
		p->shaderTextureArray = FALSE;


		/* Rearrange to take advantage of headlight when off */
		p->nextFreeLight = 0;


		p->cur_hits=0;
		p->empty_group=0;
		p->rootNode=NULL;	/* scene graph root node */
		p->AnchorsAnchor = NULL;
		t->rayHit = (void *)&p->rayHit;
		t->rayHitHyper = (void *)&p->rayHitHyper;
		p->renderLevel = 0;
	}

	//setLightType(HEADLIGHT_LIGHT,2); // ensure that this is a DirectionalLight.
}
//	ppRenderFuncs p = (ppRenderFuncs)gglobal()->RenderFuncs.prv;




/* we assume max MAX_LIGHTS lights. The max light is the Headlight, so we go through 0-HEADLIGHT_LIGHT for Lights */
int nextlight() {
	ppRenderFuncs p = (ppRenderFuncs)gglobal()->RenderFuncs.prv;
	int rv = p->nextFreeLight;
	if(p->nextFreeLight == HEADLIGHT_LIGHT) { return -1; }
	p->nextFreeLight ++;
	return rv;
}

/* lightType 0=point 1=spot 2=directional */
void setLightType(GLint light, int type) {
	ppRenderFuncs p = (ppRenderFuncs)gglobal()->RenderFuncs.prv;
    p->lightType[light] = type;
}

/* keep track of lighting */
void setLightState(GLint light, int status) {
	ppRenderFuncs p = (ppRenderFuncs)gglobal()->RenderFuncs.prv;
    //ConsoleMessage ("start lightState, light %d, status %d\n",light,status);

    
    PRINT_GL_ERROR_IF_ANY("start lightState");

	if (light<0) return; /* nextlight will return -1 if too many lights */
	p->lightOnOff[light] = status;
    PRINT_GL_ERROR_IF_ANY("end lightState");
}

/* for local lights, we keep track of what is on and off */
void saveLightState(int *ls) {
	int i;
	ppRenderFuncs p = (ppRenderFuncs)gglobal()->RenderFuncs.prv;
	memcpy (ls,p->lightOnOff,sizeof(int)*HEADLIGHT_LIGHT);
} 

void restoreLightState(int *ls) {
	int i;
	ppRenderFuncs p = (ppRenderFuncs)gglobal()->RenderFuncs.prv;

	memcpy (p->lightOnOff,ls,sizeof(int)*HEADLIGHT_LIGHT);
}

void transformLightToEye(float *pos, float* dir)
{
	int i;
    GLDOUBLE modelMatrix[16], *b;
	float *a;
	shaderVec4 aux, auxt;
    FW_GL_GETDOUBLEV(GL_MODELVIEW_MATRIX, modelMatrix);

/*
ConsoleMessage ("nvm %4.2f %4.2f %4.2f %4.2f",modelMatrix[0],modelMatrix[1],modelMatrix[2],modelMatrix[3]);
ConsoleMessage ("nvm %4.2f %4.2f %4.2f %4.2f",modelMatrix[4],modelMatrix[5],modelMatrix[6],modelMatrix[7]);
ConsoleMessage ("nvm %4.2f %4.2f %4.2f %4.2f",modelMatrix[8],modelMatrix[9],modelMatrix[10],modelMatrix[11]);
ConsoleMessage ("nvm %4.2f %4.2f %4.2f %4.2f",modelMatrix[12],modelMatrix[13],modelMatrix[14],modelMatrix[15]);
*/

    /* pre-multiply the light position, as per the orange book, page 216,
     "OpenGL specifies that light positions are transformed by the modelview
     matrix when they are provided to OpenGL..." */
    /* DirectionalLight?  PointLight, SpotLight? */

	// assumes pos[3] = 0.0; only use first 3 of these numbers
	transformf(auxt,pos,modelMatrix);
	auxt[3] = 0.0;

/*
ConsoleMessage("LightToEye, after transformf, auxt %4.2f %4.2f %4.2f %4.2f, pos %4.2f %4.2f %4.2f %4.2f",
auxt[0],auxt[1],auxt[2],auxt[3],
pos[0],pos[1],pos[2],pos[3]);
*/

	for(i=0;i<4;i++){
		pos[i] = auxt[i];
	}
	b = modelMatrix;
	a = dir;
	aux[0] = (float) (b[0]*a[0] +b[4]*a[1] +b[8]*a[2] );
	aux[1] = (float) (b[1]*a[0] +b[5]*a[1] +b[9]*a[2] );
	aux[2] = (float) (b[2]*a[0] +b[6]*a[1] +b[10]*a[2]);
	for(i=0;i<3;i++)
		dir[i] = aux[i];

	// just initialize this to 0.0
	dir[3] = 0.0;

}

void fwglLightfv (int light, int pname, GLfloat *params) {
	ppRenderFuncs p = (ppRenderFuncs)gglobal()->RenderFuncs.prv;
	/*printf ("fwglLightfv light: %d ",light);
	switch (pname) {
		case GL_AMBIENT: printf ("GL_AMBIENT"); break;
		case GL_DIFFUSE: printf ("GL_DIFFUSE"); break;
		case GL_POSITION: printf ("GL_POSITION"); break;
		case GL_SPECULAR: printf ("GL_SPECULAR"); break;
		case GL_SPOT_DIRECTION: printf ("GL_SPOT_DIRECTION"); break;
        case GL_LIGHT_RADIUS: printf ("GL_LIGHT_RADIUS"); break;
	}
	printf (" %f %f %f %f\n",params[0], params[1],params[2],params[3]);
     */
    
	//printLTDebug(__FILE__,__LINE__);


	switch (pname) {
		case GL_AMBIENT:
			memcpy ((void *)p->light_amb[light],(void *)params,sizeof(shaderVec4));
			break;
		case GL_DIFFUSE:
			memcpy ((void *)p->light_dif[light],(void *)params,sizeof(shaderVec4));
			break;
		case GL_POSITION:
			memcpy ((void *)p->light_pos[light],(void *)params,sizeof(shaderVec4));
			//the following function call assumes spotdir has already been set - set it first from render_light

/*
ConsoleMessage("fwglLightfv - NOT transforming pos %3.2f %3.2f %3.2f %3.2f spd %3.2f %3.2f %3.2f %3.2f",
			p->light_pos[light][0],
			p->light_pos[light][1],
			p->light_pos[light][2],
			p->light_pos[light][3],
			p->light_spotDir[light][0],
			p->light_spotDir[light][1],
			p->light_spotDir[light][2],
			p->light_spotDir[light][3]);
*/
			if (light != HEADLIGHT_LIGHT)  transformLightToEye(p->light_pos[light], p->light_spotDir[light]);
			break;
		case GL_SPECULAR:
			memcpy ((void *)p->light_spec[light],(void *)params,sizeof(shaderVec4));
			break;
		case GL_SPOT_DIRECTION:
			memcpy ((void *)p->light_spotDir[light],(void *)params,sizeof(shaderVec4));
			break;
		default: {printf ("help, unknown fwgllightfv param %d\n",pname);}
	}
}

void fwglLightf (int light, int pname, GLfloat param) {
	ppRenderFuncs p = (ppRenderFuncs)gglobal()->RenderFuncs.prv;

#ifdef RENDERVERBOSE
	printf ("fwglLightf light: %d ",light);
	switch (pname) {
		case GL_CONSTANT_ATTENUATION: printf ("GL_CONSTANT_ATTENUATION"); break;
		case GL_LINEAR_ATTENUATION: printf ("GL_LINEAR_ATTENUATION"); break;
		case GL_QUADRATIC_ATTENUATION: printf ("GL_QUADRATIC_ATTENUATION"); break;
		case GL_SPOT_CUTOFF: printf ("GL_SPOT_CUTOFF"); break;
		case GL_SPOT_BEAMWIDTH: printf ("GL_SPOT_BEAMWIDTH"); break;
	}
	printf (" %f\n",param);
#endif
	
    
	switch (pname) {
		case GL_CONSTANT_ATTENUATION:
			p->light_constAtten[light] = param;
			break;
		case GL_LINEAR_ATTENUATION:
			p->light_linAtten[light] = param;
			break;
		case GL_QUADRATIC_ATTENUATION:
			p->light_quadAtten[light] = param;
			break;
		case GL_SPOT_CUTOFF:
			p->light_spotCutoffAngle[light] = param;
            //ConsoleMessage ("setting light_spotCutoffAngle for %d to %f\n",light,param);
			break;
		case GL_SPOT_BEAMWIDTH:
			p->light_spotBeamWidth[light] = param;
            //ConsoleMessage ("setting light_spotBeamWidth for %d to %f\n",light,param);

			break;
        case GL_LIGHT_RADIUS:
            p->light_radius[light] = param;
            break;

		default: {printf ("help, unknown fwgllightfv param %d\n",pname);}
	}
}


/* send light info into Shader. if OSX gets glGetUniformBlockIndex calls, we can do this with 1 call */
void sendLightInfo (s_shader_capabilities_t *me) {
	ppRenderFuncs p = (ppRenderFuncs)gglobal()->RenderFuncs.prv;
    int i;
    		

	// in case we are trying to render a node that has just been killed...
	if (me==NULL) return;

	PRINT_GL_ERROR_IF_ANY("BEGIN sendLightInfo");
	/* if one of these are equal to -1, we had an error in the shaders... */
	GLUNIFORM1IV(me->lightState,MAX_LIGHTS,p->lightOnOff);
	GLUNIFORM1IV(me->lightType,MAX_LIGHTS,p->lightType);
    GLUNIFORM1FV(me->lightRadius,MAX_LIGHTS,p->light_radius);
	PRINT_GL_ERROR_IF_ANY("MIDDLE1 sendLightInfo");
    
    // send in lighting info, but only for lights that are "on"
    for (i=0; i<MAX_LIGHTS; i++) {
        // this causes initial screen on Android to fail.
	//if (p->lightOnOff[i]) {
            GLUNIFORM1F (me->lightConstAtten[i], p->light_constAtten[i]);
            GLUNIFORM1F (me->lightLinAtten[i], p->light_linAtten[i]);
            GLUNIFORM1F(me->lightQuadAtten[i], p->light_quadAtten[i]);
            GLUNIFORM1F(me->lightSpotCutoffAngle[i], p->light_spotCutoffAngle[i]);
            GLUNIFORM1F(me->lightSpotBeamWidth[i], p->light_spotBeamWidth[i]);
            GLUNIFORM4FV(me->lightAmbient[i],1,p->light_amb[i]);
            GLUNIFORM4FV(me->lightDiffuse[i],1,p->light_dif[i]);
            GLUNIFORM4FV(me->lightPosition[i],1,p->light_pos[i]);
            GLUNIFORM4FV(me->lightSpotDir[i],1, p->light_spotDir[i]);
            GLUNIFORM4FV(me->lightSpecular[i],1,p->light_spec[i]);
        //}
    }
    PRINT_GL_ERROR_IF_ANY("END sendLightInfo");
}

/* finished rendering thisshape. */
void finishedWithGlobalShader(void) {
    //printf ("finishedWithGlobalShader\n");
	ppRenderFuncs p = (ppRenderFuncs)gglobal()->RenderFuncs.prv;

    /* get rid of the shader */
    getAppearanceProperties()->currentShaderProperties = NULL;

	/* set array booleans back to defaults */
	p->shaderNormalArray = TRUE;
	p->shaderVertexArray = TRUE;
	p->shaderColourArray = FALSE;
	p->shaderTextureArray = FALSE;
}


/* should the system need to rebuild the OpenGL system (eg, Android, 
on restore of screen, iPhone?? Blackberry???) we ensure that the system
state is such that new information will get cached */

void resetGlobalShader() {
	ppRenderFuncs p = (ppRenderFuncs)gglobal()->RenderFuncs.prv;

	//ConsoleMessage ("resetGlobalShader called");

	/* no shader currently active */
	p->currentShader = 0;
	/* set array booleans back to defaults */
	p->shaderNormalArray = TRUE;
	p->shaderVertexArray = TRUE;
	p->shaderColourArray = FALSE;
	p->shaderTextureArray = FALSE;
}

void restoreGlobalShader(){
	ppRenderFuncs p = (ppRenderFuncs)gglobal()->RenderFuncs.prv;
    if (p->currentShader)
		USE_SHADER(p->currentShader);
}
/* choose and turn on a shader for this geometry */

void enableGlobalShader(s_shader_capabilities_t *myShader) {
	ppRenderFuncs p = (ppRenderFuncs)gglobal()->RenderFuncs.prv;

    //ConsoleMessage ("enableGlobalShader, have myShader %d",myShader->myShaderProgram);
    if (myShader == NULL) {
        finishedWithGlobalShader(); 
        return;
    };
    
    
    getAppearanceProperties()->currentShaderProperties = myShader;
    if (myShader->myShaderProgram != p->currentShader) {
		USE_SHADER(myShader->myShaderProgram);
		p->currentShader = myShader->myShaderProgram;
	}
}


/* send in vertices, normals, etc, etc... to either a shader or via older opengl methods */
void sendAttribToGPU(int myType, int dataSize, int dataType, int normalized, int stride, float *pointer, char *file, int line){

    s_shader_capabilities_t *me = getAppearanceProperties()->currentShaderProperties;

	// checking to see that we really have the data
	if (me==NULL) return;

#ifdef RENDERVERBOSE

ConsoleMessage ("sendAttribToGPU, getAppearanceProperties()->currentShaderProperties %p\n",getAppearanceProperties()->currentShaderProperties);
ConsoleMessage ("myType %d, dataSize %d, dataType %d, stride %d\n",myType,dataSize,dataType,stride);
	if (me != NULL) {
		switch (myType) {
			case FW_NORMAL_POINTER_TYPE:
				ConsoleMessage ("glVertexAttribPointer  Normals %d at %s:%d\n",me->Normals,file,line);
				break;
			case FW_VERTEX_POINTER_TYPE:
				ConsoleMessage ("glVertexAttribPointer  Vertexs %d at %s:%d\n",me->Vertices,file,line);
				break;
			case FW_COLOR_POINTER_TYPE:
				ConsoleMessage ("glVertexAttribPointer  Colours %d at %s:%d\n",me->Colours,file,line);
				break;
			case FW_TEXCOORD_POINTER_TYPE:
				ConsoleMessage ("glVertexAttribPointer  TexCoords %d at %s:%d\n",me->TexCoords,file,line);
				break;

			default : {ConsoleMessage ("sendAttribToGPU, unknown type in shader\n");}
		}
	}
#endif
#undef RENDERVERBOSE

	switch (myType) {
		case FW_NORMAL_POINTER_TYPE:
		if (me->Normals != -1) {
			glEnableVertexAttribArray(me->Normals);
			glVertexAttribPointer(me->Normals, 3, dataType, normalized, stride, pointer);
		}
			break;
		case FW_VERTEX_POINTER_TYPE:
		if (me->Vertices != -1) {
			glEnableVertexAttribArray(me->Vertices);
			glVertexAttribPointer(me->Vertices, dataSize, dataType, normalized, stride, pointer);
		}
			break;
		case FW_COLOR_POINTER_TYPE:
		if (me->Colours != -1) {
			glEnableVertexAttribArray(me->Colours);
			glVertexAttribPointer(me->Colours, dataSize, dataType, normalized, stride, pointer);
		}
			break;
		case FW_TEXCOORD_POINTER_TYPE:
		if (me->TexCoords != -1) {
			glEnableVertexAttribArray(me->TexCoords);
			glVertexAttribPointer(me->TexCoords, dataSize, dataType, normalized, stride, pointer);
		}
			break;

		default : {printf ("sendAttribToGPU, unknown type in shader\n");}
	}
}

void sendClientStateToGPU(int enable, int cap) {
	ppRenderFuncs p = (ppRenderFuncs)gglobal()->RenderFuncs.prv;
	if (getAppearanceProperties()->currentShaderProperties != NULL) {
		switch (cap) {
			case GL_NORMAL_ARRAY:
				p->shaderNormalArray = enable;
				break;
			case GL_VERTEX_ARRAY:
				p->shaderVertexArray = enable;
				break;
			case GL_COLOR_ARRAY:
				p->shaderColourArray = enable;
				break;
			case GL_TEXTURE_COORD_ARRAY:
				p->shaderTextureArray = enable;
				break;

			default : {printf ("sendClientStateToGPU, unknown type in shader\n");}
		}
#ifdef RENDERVERBOSE
printf ("sendClientStateToGPU: getAppearanceProperties()->currentShaderProperties %p \n",getAppearanceProperties()->currentShaderProperties);
if (p->shaderNormalArray) printf ("enabling Normal\n"); else printf ("disabling Normal\n");
if (p->shaderVertexArray) printf ("enabling Vertex\n"); else printf ("disabling Vertex\n");
if (p->shaderColourArray) printf ("enabling Colour\n"); else printf ("disabling Colour\n");
if (p->shaderTextureArray) printf ("enabling Texture\n"); else printf ("disabling Texture\n");
#endif

	}
}

void sendBindBufferToGPU (GLenum target, GLuint buffer, char *file, int line) {

	
/*
	if (target == GL_ARRAY_BUFFER_BINDING) printf ("glBindBuffer, GL_ARRAY_BUFFER_BINDING %d at %s:%d\n",buffer,file,line);
	else if (target == GL_ARRAY_BUFFER) printf ("glBindBuffer, GL_ARRAY_BUFFER %d at %s:%d\n",buffer,file,line);
	else if (target == GL_ELEMENT_ARRAY_BUFFER) printf ("glBindBuffer, GL_ELEMENT_ARRAY_BUFFER %d at %s:%d\n",buffer,file,line);
	else printf ("glBindBuffer, %d %d at %s:%d\n",target,buffer,file,line);
	
*/

	glBindBuffer(target,buffer);
}



static bool setupShader() {
    ppRenderFuncs p = (ppRenderFuncs)gglobal()->RenderFuncs.prv;
    s_shader_capabilities_t *mysp = getAppearanceProperties()->currentShaderProperties;

PRINT_GL_ERROR_IF_ANY("BEGIN setupShader");
	if (mysp == NULL) return FALSE;
        

		/* if we had a shader compile problem, do not draw */
		if (!(mysp->compiledOK)) {
#ifdef RENDERVERBOSE
			printf ("shader compile error\n");
#endif
			PRINT_GL_ERROR_IF_ANY("EXIT(false) setupShader");
			return false;
		}
        
#ifdef RENDERVERBOSE
        printf ("we have Normals %d Vertices %d Colours %d TexCoords %d \n",
                mysp->Normals,
                mysp->Vertices,
                mysp->Colours,
                mysp->TexCoords);
		if (p->shaderNormalArray) printf ("p->shaderNormalArray TRUE\n"); else printf ("p->shaderNormalArray FALSE\n");        
        if (p->shaderVertexArray) printf ("shaderVertexArray TRUE\n"); else printf ("shaderVertexArray FALSE\n");
        if (p->shaderColourArray) printf ("shaderColourArray TRUE\n"); else printf ("shaderColourArray FALSE\n");
		if (p->shaderTextureArray) printf ("shaderTextureArray TRUE\n"); else printf ("shaderTextureArray FALSE\n");               
#endif
        
        /* send along lighting, material, other visible properties */
        sendMaterialsToShader(mysp);
        sendMatriciesToShader(mysp);
        
		if (mysp->Normals != -1) {
			if (p->shaderNormalArray) glEnableVertexAttribArray(mysp->Normals);
			else glDisableVertexAttribArray(mysp->Normals);
		}
        
		if (mysp->Vertices != -1) {
			if (p->shaderVertexArray) glEnableVertexAttribArray(mysp->Vertices);
			else glDisableVertexAttribArray(mysp->Vertices);
		}
        
		if (mysp->Colours != -1) {
			if (p->shaderColourArray) glEnableVertexAttribArray(mysp->Colours);
			else glDisableVertexAttribArray(mysp->Colours);
		}
        
		if (mysp->TexCoords != -1) {
			if (p->shaderTextureArray) glEnableVertexAttribArray(mysp->TexCoords);
			else glDisableVertexAttribArray(mysp->TexCoords);
		}
        
	PRINT_GL_ERROR_IF_ANY("EXIT(true) setupShader");
    return true;
    
}


void sendArraysToGPU (int mode, int first, int count) {
#ifdef RENDERVERBOSE
	printf ("sendArraysToGPU start\n"); 
#endif
    

	// when glDrawArrays bombs it's usually some function left an array
	// enabled that's not supposed to be - try disabling something
//glDisableClientState(GL_VERTEX_ARRAY);
//glDisableClientState(GL_NORMAL_ARRAY);
//glDisableClientState(GL_INDEX_ARRAY);
//glDisableClientState(GL_COLOR_ARRAY);
//glDisableClientState(GL_SECONDARY_COLOR_ARRAY);
//glDisableClientState(GL_FOG_COORDINATE_ARRAY);
//glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//glDisableClientState(GL_EDGE_FLAG_ARRAY);

	if (setupShader())	
		glDrawArrays(mode,first,count);
    #ifdef RENDERVERBOSE
	printf ("sendArraysToGPU end\n"); 
    #endif
}



void sendElementsToGPU (int mode, int count, ushort *indices) {
    #ifdef RENDERVERBOSE
	printf ("sendElementsToGPU start\n"); 
    #endif
    
    if (setupShader())
        glDrawElements(mode,count,GL_UNSIGNED_SHORT,indices);

	#ifdef RENDERVERBOSE
	printf ("sendElementsToGPU finish\n"); 
	#endif
}


void initializeLightTables() {
	int i;
        float pos[] = { 0.0f, 0.0f, 1.0f, 0.0f };
        float dif[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        float shin[] = { 0.0f, 0.0f, 0.0f, 1.0f }; /* light defaults - headlight is here, too */
        float As[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        float zeroes[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	ppRenderFuncs p = (ppRenderFuncs)gglobal()->RenderFuncs.prv;

      PRINT_GL_ERROR_IF_ANY("start of initializeightTables");

	for(i=0; i<MAX_LIGHTS; i++) {
                p->lightOnOff[i] = TRUE;
                setLightState(i,FALSE);
            
		FW_GL_LIGHTFV(i, GL_SPOT_DIRECTION, pos);
       		FW_GL_LIGHTFV(i, GL_POSITION, pos);
       		FW_GL_LIGHTFV(i, GL_AMBIENT, As);
       		FW_GL_LIGHTFV(i, GL_DIFFUSE, dif);
       		FW_GL_LIGHTFV(i, GL_SPECULAR, shin);
         	FW_GL_LIGHTF(i, GL_CONSTANT_ATTENUATION,1.0f);
       		FW_GL_LIGHTF(i, GL_LINEAR_ATTENUATION,0.0f);
       		FW_GL_LIGHTF(i, GL_QUADRATIC_ATTENUATION,0.0f);
       		FW_GL_LIGHTF(i, GL_SPOT_CUTOFF,0.0f);
       		FW_GL_LIGHTF(i, GL_SPOT_BEAMWIDTH,0.0f);
           	FW_GL_LIGHTF(i, GL_LIGHT_RADIUS, 100000.0); /* just make it large for now*/ 
            
            	PRINT_GL_ERROR_IF_ANY("initizlizeLight2.10");
        }
        setLightState(HEADLIGHT_LIGHT, TRUE);

#ifdef OLDCODE
OLDCODE	#ifndef GL_ES_VERSION_2_0
OLDCODE        FW_GL_LIGHTMODELI(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
OLDCODE        FW_GL_LIGHTMODELI(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
OLDCODE        FW_GL_LIGHTMODELI(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_FALSE);
OLDCODE        FW_GL_LIGHTMODELI(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
OLDCODE        FW_GL_LIGHTMODELFV(GL_LIGHT_MODEL_AMBIENT,As);
OLDCODE	#else
OLDCODE	//printf ("skipping light setups\n");
OLDCODE	#endif
#endif //OLDCODE

    LIGHTING_INITIALIZE
	

    PRINT_GL_ERROR_IF_ANY("end initializeLightTables");
}


ttrenderstate renderstate()
{
	ppRenderFuncs p = (ppRenderFuncs)gglobal()->RenderFuncs.prv;
	return &p->renderstate;
}


//true statics:
GLint viewport[4] = {-1,-1,2,2};  //doesn't change, used in glu unprojects
/* These two points define a ray in window coordinates */
struct point_XYZ r1 = {0,0,-1},r2 = {0,0,0},r3 = {0,1,0};


struct X3D_Anchor *AnchorsAnchor()
{
	ppRenderFuncs p = (ppRenderFuncs)gglobal()->RenderFuncs.prv;
	return p->AnchorsAnchor;
}
void setAnchorsAnchor(struct X3D_Anchor* anchor)
{
	ppRenderFuncs p = (ppRenderFuncs)gglobal()->RenderFuncs.prv;
	p->AnchorsAnchor = anchor;
}


//static struct currayhit rayph;
//struct currayhit rayHit,rayHitHyper;
/* used to test new hits */



//struct X3D_Group *_rootNode=NULL;	/* scene graph root node */
struct X3D_Group *rootNode()
{
	// ConsoleMessage ("rootNode called");
	ppRenderFuncs p = (ppRenderFuncs)gglobal()->RenderFuncs.prv;	
	if (p==NULL) {
		ConsoleMessage ("rootNode, p null");
		return NULL;
	}
	return p->rootNode;
}
void setRootNode(struct X3D_Group *rn)
{
	ppRenderFuncs p = (ppRenderFuncs)gglobal()->RenderFuncs.prv;
	p->rootNode = rn;
}
//void *empty_group=0;

/*******************************************************************************/

/* Sub, rather than big macro... */
void rayhit(float rat, float cx,float cy,float cz, float nx,float ny,float nz,
	    float tx,float ty, char *descr)  {
	GLDOUBLE modelMatrix[16];
	GLDOUBLE projMatrix[16];
	ppRenderFuncs p;
	ttglobal tg = gglobal();
	p = (ppRenderFuncs)tg->RenderFuncs.prv;

	/* Real rat-testing */
#ifdef RENDERVERBOSE
	//printf("RAY HIT %s! %f (%f %f %f) (%f %f %f)\n\tR: (%f %f %f) (%f %f %f)\n",
	//       descr, rat,cx,cy,cz,nx,ny,nz,
	//       t_r1.x, t_r1.y, t_r1.z,
	//       t_r2.x, t_r2.y, t_r2.z
	//	);
#endif

	if(rat<0 || (rat>tg->RenderFuncs.hitPointDist && tg->RenderFuncs.hitPointDist >= 0)) {
		return;
	}
	FW_GL_GETDOUBLEV(GL_MODELVIEW_MATRIX, modelMatrix);
	FW_GL_GETDOUBLEV(GL_PROJECTION_MATRIX, projMatrix);
	FW_GLU_PROJECT(cx,cy,cz, modelMatrix, projMatrix, viewport, &tg->RenderFuncs.hp.x, &tg->RenderFuncs.hp.y, &tg->RenderFuncs.hp.z);
	tg->RenderFuncs.hitPointDist = rat;
	p->rayHit=p->rayph;
	p->rayHitHyper=p->rayph;
#ifdef RENDERVERBOSE 
//	printf ("Rayhit, hp.x y z: - %f %f %f rat %f hitPointDist %f\n",hp.x,hp.y,hp.z, rat, tg->RenderFuncs.hitPointDist);
#endif
}


/* Call this when modelview and projection modified */
void upd_ray() {
	struct point_XYZ t_r1,t_r2,t_r3;
	GLDOUBLE modelMatrix[16];
	GLDOUBLE projMatrix[16];
	ttglobal tg = gglobal();
	FW_GL_GETDOUBLEV(GL_MODELVIEW_MATRIX, modelMatrix);
	FW_GL_GETDOUBLEV(GL_PROJECTION_MATRIX, projMatrix);
/*

{int i; printf ("\n"); 
printf ("upd_ray, pm %p\n",projMatrix);
for (i=0; i<16; i++) printf ("%4.3lf ",modelMatrix[i]); printf ("\n"); 
for (i=0; i<16; i++) printf ("%4.3lf ",projMatrix[i]); printf ("\n"); 
} 
*/


	FW_GLU_UNPROJECT(r1.x,r1.y,r1.z,modelMatrix,projMatrix,viewport,
		     &t_r1.x,&t_r1.y,&t_r1.z);
	FW_GLU_UNPROJECT(r2.x,r2.y,r2.z,modelMatrix,projMatrix,viewport,
		     &t_r2.x,&t_r2.y,&t_r2.z);
	FW_GLU_UNPROJECT(r3.x,r3.y,r3.z,modelMatrix,projMatrix,viewport,
		     &t_r3.x,&t_r3.y,&t_r3.z);

	VECCOPY(tg->RenderFuncs.t_r1,t_r1);
	VECCOPY(tg->RenderFuncs.t_r2,t_r2);
	VECCOPY(tg->RenderFuncs.t_r3,t_r3);

/*
	printf("Upd_ray: (%f %f %f)->(%f %f %f) == (%f %f %f)->(%f %f %f)\n",
	r1.x,r1.y,r1.z,r2.x,r2.y,r2.z,
	t_r1.x,t_r1.y,t_r1.z,t_r2.x,t_r2.y,t_r2.z);
*/

}


/* if a node changes, void the display lists */
/* Courtesy of Jochen Hoenicke */

void update_node(struct X3D_Node *node) {
	int i;
	
#ifdef VERBOSE
	printf ("update_node for %d %s nparents %d renderflags %x\n",node, stringNodeType(node->_nodeType),node->_nparents, node->_renderFlags); 
	if (node->_nparents == 0) {
		if (node == rootNode) printf ("...no parents, this IS the rootNode\n"); 
		else printf ("...no parents, this IS NOT the rootNode\n");
	}



	for (i = 0; i < node->_nparents; i++) {
		struct X3D_Node *n = X3D_NODE(node->_parents[i]);
		if( n != 0 ) {
			printf ("	parent %u is %s\n",n,stringNodeType(n->_nodeType));
		} else {
			printf ("	parent %d is NULL\n",i);
		}
	}
#endif

	node->_change ++;

	/* parentVector here yet?? */
	if (node->_parentVector == NULL) {
		return;
	}

	for (i = 0; i < vectorSize(node->_parentVector); i++) {
		struct X3D_Node *n = vector_get(struct X3D_Node *, node->_parentVector,i);
		if(n == node) {
			fprintf(stderr, "Error: self-referential node structure! (node:'%s')\n", stringNodeType(node->_nodeType));
			vector_set(struct X3D_Node*, node->_parentVector, i,NULL);
		} else if( n != 0 ) {
			update_node(n);
		}
	}
}

/*********************************************************************
 *********************************************************************
 *
 * render_node : call the correct virtual functions to render the node
 * depending on what we are doing right now.
 */

//#ifdef RENDERVERBOSE
//static int renderLevel = 0;
//#endif

#define PRINT_NODE(_node, _v)  do {					\
		if (gglobal()->internalc.global_print_opengl_errors && (gglobal()->display._global_gl_err != GL_NO_ERROR)) { \
			printf("Render_node_v %p (%s) PREP: %p REND: %p CH: %p FIN: %p RAY: %p HYP: %p\n",_v, \
			       stringNodeType(_node->_nodeType),	\
			       _v->prep,				\
			       _v->rend,				\
			       _v->children,			\
			       _v->fin,				\
			       _v->rendray,			\
			       gglobal()->RenderFuncs.hypersensitive);			\
			printf("Render_state geom %d light %d sens %d\n", \
			       renderstate()->render_geom,				\
			       renderstate()->render_light,				\
			       renderstate()->render_sensitive);			\
			printf("pchange %d pichange %d \n", _node->_change, _node->_ichange); \
		}							\
	} while (0)

//static int renderLevel = 0;
//#define RENDERVERBOSE
void render_node(struct X3D_Node *node) {
	struct X3D_Virt *virt;

	int srg = 0;
	int sch = 0;
	struct currayhit srh;
	ppRenderFuncs p;
	ttglobal tg = gglobal();
	p = (ppRenderFuncs)tg->RenderFuncs.prv;

	X3D_NODE_CHECK(node);

#ifdef RENDERVERBOSE
	p->renderLevel ++;
#endif

	if(!node) {
#ifdef RENDERVERBOSE
		DEBUG_RENDER("%d no node, quick return\n", renderLevel);
		p->renderLevel--;
#endif
		return;
	}

	virt = virtTable[node->_nodeType];

#ifdef RENDERVERBOSE 
	//printf("%d =========================================NODE RENDERED===================================================\n",renderLevel);
	{
		int i;
		for(i=0;i<p->renderLevel;i++) printf(" ");
	}
	printf("%d node %u (%s) , v %u renderFlags %x ",p->renderLevel, node,stringNodeType(node->_nodeType),virt,node->_renderFlags);

	if ((node->_renderFlags & VF_Viewpoint) == VF_Viewpoint) printf (" VF_Viewpoint");
	if ((node->_renderFlags & VF_Geom )== VF_Geom) printf (" VF_Geom");
	if ((node->_renderFlags & VF_localLight )== VF_localLight) printf (" VF_localLight");
	if ((node->_renderFlags & VF_Sensitive) == VF_Sensitive) printf (" VF_Sensitive");
	if ((node->_renderFlags & VF_Blend) == VF_Blend) printf (" VF_Blend");
	if ((node->_renderFlags & VF_Proximity) == VF_Proximity) printf (" VF_Proximity");
	if ((node->_renderFlags & VF_Collision) == VF_Collision) printf (" VF_Collision");
	if ((node->_renderFlags & VF_globalLight) == VF_globalLight) printf (" VF_globalLight");
	if ((node->_renderFlags & VF_hasVisibleChildren) == VF_hasVisibleChildren) printf (" VF_hasVisibleChildren");
	if ((node->_renderFlags & VF_shouldSortChildren) == VF_shouldSortChildren) printf (" VF_shouldSortChildren");
	if ((node->_renderFlags & VF_Other) == VF_Other) printf (" VF_Other");
	/*
	if ((node->_renderFlags & VF_inPickableGroup == VF_inPickableGroup) printf (" VF_inPickableGroup");
	if ((node->_renderFlags & VF_PickingSensor == VF_PickingSensor) printf (" VF_PickingSensor");
	*/
	printf ("\n");

	//printf("PREP: %d REND: %d CH: %d FIN: %d RAY: %d HYP: %d\n",virt->prep, virt->rend, virt->children, virt->fin,
	//       virt->rendray, hypersensitive);
	//printf("%d state: vp %d geom %d light %d sens %d blend %d prox %d col %d ", renderLevel, 
 //        	render_vp,render_geom,render_light,render_sensitive,render_blend,render_proximity,render_collision); 
	//printf("change %d ichange %d \n",node->_change, node->_ichange);
#endif

        /* if we are doing Viewpoints, and we don't have a Viewpoint, don't bother doing anything here */ 
        if (renderstate()->render_vp == VF_Viewpoint) { 
                if ((node->_renderFlags & VF_Viewpoint) != VF_Viewpoint) { 
#ifdef RENDERVERBOSE
                        printf ("doing Viewpoint, but this  node is not for us - just returning\n"); 
			p->renderLevel--;
#endif
                        return; 
                } 
        }

	/* are we working through global PointLights, DirectionalLights or SpotLights, but none exist from here on down? */
        if (renderstate()->render_light == VF_globalLight) { 
                if ((node->_renderFlags & VF_globalLight) != VF_globalLight) { 
#ifdef RENDERVERBOSE
                        printf ("doing globalLight, but this  node is not for us - just returning\n"); 
			p->renderLevel--;
#endif
                        return; 
                }
        }

	if(virt->prep) {
		DEBUG_RENDER("rs 2\n");
		virt->prep(node);
		if(renderstate()->render_sensitive && !tg->RenderFuncs.hypersensitive) {
			upd_ray();
		}
		PRINT_GL_ERROR_IF_ANY("prep"); PRINT_NODE(node,virt);
	}
	if(renderstate()->render_proximity && virt->proximity) {
		DEBUG_RENDER("rs 2a\n");
		virt->proximity(node);
		PRINT_GL_ERROR_IF_ANY("render_proximity"); PRINT_NODE(node,virt);
	}
	
	if(renderstate()->render_collision && virt->collision) {
		DEBUG_RENDER("rs 2b\n");
		virt->collision(node);
		PRINT_GL_ERROR_IF_ANY("render_collision"); PRINT_NODE(node,virt);
	}

	if(renderstate()->render_geom && !renderstate()->render_sensitive && virt->rend) {
		DEBUG_RENDER("rs 3\n");
		PRINT_GL_ERROR_IF_ANY("BEFORE render_geom"); PRINT_NODE(node,virt);
		virt->rend(node);
		PRINT_GL_ERROR_IF_ANY("render_geom"); PRINT_NODE(node,virt);
	}

	if(renderstate()->render_other && virt->other )
	{
#if DJTRACK_PICKSENSORS
		DEBUG_RENDER("rs 4a\n");
		virt->other(node); //other() is responsible for push_renderingState(VF_inPickableGroup);
		PRINT_GL_ERROR_IF_ANY("render_other"); PRINT_NODE(node,virt);
#endif
	} //other

	if(renderstate()->render_sensitive && (node->_renderFlags & VF_Sensitive)) {
		DEBUG_RENDER("rs 5\n");
		srg = renderstate()->render_geom;
		renderstate()->render_geom = 1;
		DEBUG_RENDER("CH1 %d: %d\n",node, p->cur_hits, node->_hit);
		sch = p->cur_hits;
		p->cur_hits = 0;
		/* HP */
		srh = p->rayph;
		p->rayph.hitNode = node;
		FW_GL_GETDOUBLEV(GL_MODELVIEW_MATRIX, p->rayph.modelMatrix);
		FW_GL_GETDOUBLEV(GL_PROJECTION_MATRIX, p->rayph.projMatrix);
		PRINT_GL_ERROR_IF_ANY("render_sensitive"); PRINT_NODE(node,virt);
	}

	if(renderstate()->render_geom && renderstate()->render_sensitive && !tg->RenderFuncs.hypersensitive && virt->rendray) {
		DEBUG_RENDER("rs 6\n");
		virt->rendray(node);
		PRINT_GL_ERROR_IF_ANY("rs 6"); PRINT_NODE(node,virt);
	}

    if((renderstate()->render_sensitive) && (tg->RenderFuncs.hypersensitive == node)) {
		DEBUG_RENDER("rs 7\n");
		p->hyper_r1 = tg->RenderFuncs.t_r1;
		p->hyper_r2 = tg->RenderFuncs.t_r2;
		tg->RenderFuncs.hyperhit = 1;
    }

	/* start recursive section */
    if(virt->children) { 
		DEBUG_RENDER("rs 8 - has valid child node pointer\n");
		virt->children(node);
		PRINT_GL_ERROR_IF_ANY("children"); PRINT_NODE(node,virt);
    }
	/* end recursive section */

	if(renderstate()->render_other && virt->other)
	{
#if DJTRACK_PICKSENSORS
		//pop_renderingState(VF_inPickableGroup);
#endif
	}

	if(renderstate()->render_sensitive && (node->_renderFlags & VF_Sensitive)) {
		DEBUG_RENDER("rs 9\n");
		renderstate()->render_geom = srg;
		p->cur_hits = sch;
		DEBUG_RENDER("CH3: %d %d\n",p->cur_hits, node->_hit);
		/* HP */
		p->rayph = srh;
	}

	if(virt->fin) {
		DEBUG_RENDER("rs A\n");
		virt->fin(node);
		if(renderstate()->render_sensitive && virt == &virt_Transform) {
			upd_ray();
		}
		PRINT_GL_ERROR_IF_ANY("fin"); PRINT_NODE(node,virt);
	}

#ifdef RENDERVERBOSE 
	{
		int i;
		for(i=0;i<p->renderLevel;i++)printf(" ");
	}
	printf("%d (end render_node)\n",p->renderLevel);
	p->renderLevel--;
#endif
}
//#undef RENDERVERBOSE

/*
 * The following code handles keeping track of the parents of a given
 * node. This enables us to traverse the scene on C level for optimizations.
 *
 * We use this array code because VRML nodes usually don't have
 * hundreds of children and don't usually shuffle them too much.
 */
//dug9 dec 13 >>
struct X3D_Node* getTypeNode(struct X3D_Node *node);

void add_parent(struct X3D_Node *node, struct X3D_Node *parent, char *file, int line) {
	struct X3D_Node* itype;
	if(!node) return;
	//if(node->_nodeType == NODE_PlaneSensor)
	//	printf("hi from add_parent, have a Planesensor");
#ifdef CHILDVERBOSE
	printf ("add_parent; adding node %u ,to parent %u at %s:%d\n",node,  parent,file,line);
	printf ("add_parent; adding node %x ,to parent %x (hex) at %s:%d\n",node,  parent,file,line);
	printf ("add_parent; adding node %p ,to parent %p (ptr) at %s:%d\n",node,  parent,file,line);


	printf ("add_parent; adding node %u (%s) to parent %u (%s) at %s:%d\n",node, stringNodeType(node->_nodeType), 
		parent, stringNodeType(parent->_nodeType),file,line);
#endif

	parent->_renderFlags = parent->_renderFlags | node->_renderFlags;

	/* add it to the parents list */
	vector_pushBack (struct X3D_Node*,node->_parentVector, parent);
	/* tie in sensitive nodes */
	itype = getTypeNode(node);
	if(itype)
		setSensitive (parent, itype );
}

void remove_parent(struct X3D_Node *child, struct X3D_Node *parent) {
	int i;
	int pi;

	if(!child) return;
	if(!parent) return;
	
#ifdef CHILDVERBOSE
	printf ("remove_parent, parent %u (%s) , child %u (%s)\n",parent, stringNodeType(parent->_nodeType),
		child, stringNodeType(child->_nodeType));
#endif

		pi = -1;
		for (i=0; i<vectorSize(child->_parentVector); i++) {
			struct X3D_Node *n = vector_get(struct X3D_Node *, child->_parentVector,i);
			if (n==parent) pi = i;
		}

		if (pi >=0) {
			struct X3D_Node *n = vector_get(struct X3D_Node *, child->_parentVector,vectorSize(child->_parentVector)-1);

			/* get the last entry, and overwrite the entry found */
			vector_set(struct X3D_Node*, child->_parentVector, pi,n);

			/* take that last entry off the vector */
			vector_popBack(struct X3D_Node*, child->_parentVector);
	}
}

void
render_hier(struct X3D_Group *g, int rwhat) {
	/// not needed now - see below struct point_XYZ upvec = {0,1,0};
	/// not needed now - see below GLDOUBLE modelMatrix[16];

#ifdef render_pre_profile
	/*  profile */
	double xx,yy,zz,aa,bb,cc,dd,ee,ff;
	struct timeval mytime;
	struct timezone tz; /* unused see man gettimeofday */
#endif
	ppRenderFuncs p;
	ttglobal tg = gglobal();
	ttrenderstate rs;
	p = (ppRenderFuncs)tg->RenderFuncs.prv;
	rs = renderstate();

	rs->render_vp = rwhat & VF_Viewpoint;
	rs->render_geom =  rwhat & VF_Geom;
	rs->render_light = rwhat & VF_globalLight;
	rs->render_sensitive = rwhat & VF_Sensitive;
	rs->render_blend = rwhat & VF_Blend;
	rs->render_proximity = rwhat & VF_Proximity;
	rs->render_collision = rwhat & VF_Collision;
	rs->render_other = rwhat & VF_Other;
#ifdef DJTRACK_PICKSENSORS
	rs->render_picksensors = rwhat & VF_PickingSensor;
	rs->render_pickables = rwhat & VF_inPickableGroup;
#endif
	p->nextFreeLight = 0;
	tg->RenderFuncs.hitPointDist = -1;


#ifdef render_pre_profile
	if (rs->render_geom) {
		gettimeofday (&mytime,&tz);
		aa = (double)mytime.tv_sec+(double)mytime.tv_usec/1000000.0;
	}
#endif
#ifdef RENDERVERBOSE
	 printf ("render_hier vp %d geom %d light %d sens %d blend %d prox %d col %d\n",
	   rs->render_vp,rs->render_geom,rs->render_light,rs->render_sensitive,rs->render_blend,rs->render_proximity,rs->render_collision);  
#endif

	if (!g) {
		/* we have no geometry yet, sleep for a tiny bit */
		usleep(1000);
		return;
	}

#ifdef RENDERVERBOSE
	printf("Render_hier node=%d what=%d\n", g, rwhat);
#endif

#ifdef render_pre_profile
	if (rs->render_geom) {
		gettimeofday (&mytime,&tz);
		bb = (double)mytime.tv_sec+(double)mytime.tv_usec/1000000.0;
	}
#endif

	if (rs->render_sensitive) {
		upd_ray();
	}

#ifdef render_pre_profile
	if (rs->render_geom) {
		gettimeofday (&mytime,&tz);
		cc = (double)mytime.tv_sec+(double)mytime.tv_usec/1000000.0;
	}
#endif

	render_node(X3D_NODE(g));

#ifdef render_pre_profile
	if (rs->render_geom) {
		gettimeofday (&mytime,&tz);
		dd = (double)mytime.tv_sec+(double)mytime.tv_usec/1000000.0;
		printf ("render_geom status %f ray %f geom %f\n",bb-aa, cc-bb, dd-cc);
	}
#endif
}


/******************************************************************************
 *
 * shape compiler "thread"
 *
 ******************************************************************************/

void compileNode (void (*nodefn)(void *, void *, void *, void *, void *), void *node, void *Icoord, void *Icolor, void *Inormal, void *ItexCoord) {
	void *coord; void *color; void *normal; void *texCoord;

	/* are any of these SFNodes PROTOS? If so, get the underlying real node, as PROTOS are handled like Groups. */
	POSSIBLE_PROTO_EXPANSION(void *, Icoord,coord)
		POSSIBLE_PROTO_EXPANSION(void *, Icolor,color)
		POSSIBLE_PROTO_EXPANSION(void *, Inormal,normal)
		POSSIBLE_PROTO_EXPANSION(void *, ItexCoord,texCoord)

	nodefn(node, coord, color, normal, texCoord);
}


/* for CRoutes, we need to have a function pointer to an interpolator to run, if we
   route TO an interpolator */
void *returnInterpolatorPointer (const char *x) {
	if (strcmp("OrientationInterpolator",x)==0) { return (void *)do_Oint4;
	} else if (strcmp("CoordinateInterpolator2D",x)==0) { return (void *)do_OintCoord2D;
	} else if (strcmp("PositionInterpolator2D",x)==0) { return (void *)do_OintPos2D;
	} else if (strcmp("ScalarInterpolator",x)==0) { return (void *)do_OintScalar;
	} else if (strcmp("ColorInterpolator",x)==0) { return (void *)do_ColorInterpolator;
	} else if (strcmp("PositionInterpolator",x)==0) { return (void *)do_PositionInterpolator;
	} else if (strcmp("CoordinateInterpolator",x)==0) { return (void *)do_OintCoord;
	} else if (strcmp("NormalInterpolator",x)==0) { return (void *)do_OintNormal;
	} else if (strcmp("GeoPositionInterpolator",x)==0) { return (void *)do_GeoPositionInterpolator;
	} else if (strcmp("BooleanFilter",x)==0) { return (void *)do_BooleanFilter;
	} else if (strcmp("BooleanSequencer",x)==0) { return (void *)do_BooleanSequencer;
	} else if (strcmp("BooleanToggle",x)==0) { return (void *)do_BooleanToggle;
	} else if (strcmp("BooleanTrigger",x)==0) { return (void *)do_BooleanTrigger;
	} else if (strcmp("IntegerTrigger",x)==0) { return (void *)do_IntegerTrigger;
	} else if (strcmp("IntegerSequencer",x)==0) { return (void *)do_IntegerSequencer;
	} else if (strcmp("TimeTrigger",x)==0) { return (void *)do_TimeTrigger;
	
	} else {
		return 0;
	}
}




void checkParentLink (struct X3D_Node *node,struct X3D_Node *parent) {
        int n;

	int *offsetptr;
	char *memptr;
	struct Multi_Node *mfn;
	uintptr_t *voidptr;

        if (node == NULL) return;

	/* printf ("checkParentLink for node %u parent %u type %s\n",node,parent,stringNodeType(node->_nodeType)); */
 
        if (parent != NULL) ADD_PARENT(node, parent);

	if ((node->_nodeType<0) || (node->_nodeType>NODES_COUNT)) {
		ConsoleMessage ("checkParentLink - %d not a valid nodeType",node->_nodeType);
		return;
	}

	/* find all the fields of this node */
	offsetptr = (int *) NODE_OFFSETS[node->_nodeType];

	/* FIELDNAMES_bboxCenter, offsetof (struct X3D_Group, bboxCenter),  FIELDTYPE_SFVec3f, KW_field, */
	while (*offsetptr >= 0) {

		/* 
		   printf ("	field %s",FIELDNAMES[offsetptr[0]]); 
		   printf ("	offset %d",offsetptr[1]);
		   printf ("	type %s",FIELDTYPES[offsetptr[2]]);
		   printf ("	kind %s\n",KEYWORDS[offsetptr[3]]);
		*/

		/* worry about SFNodes and MFNodes */
		if ((offsetptr[2] == FIELDTYPE_SFNode) || (offsetptr[2] == FIELDTYPE_MFNode)) {
			if ((offsetptr[3] == KW_initializeOnly) || (offsetptr[3] == KW_inputOutput)) {

				/* create a pointer to the actual field */
				memptr = (char *) node;
				memptr += offsetptr[1];

				if (offsetptr[2] == FIELDTYPE_SFNode) {
					/* get the field as a POINTER VALUE, not just a pointer... */
					voidptr = (uintptr_t *) memptr;
					voidptr = (uintptr_t *) *voidptr;

					/* is there a node here? */
					if (voidptr != NULL) {
						checkParentLink(X3D_NODE(voidptr),node);
					}
				} else {
					mfn = (struct Multi_Node*) memptr;
					/* printf ("MFNode has %d children\n",mfn->n); */
					for (n=0; n<mfn->n; n++) {
				                checkParentLink(mfn->p[n],node);
					}
				}
			}

		}
		offsetptr+=5;
	}
}

#define X3D_COORD(node) ((struct X3D_Coordinate*)node)
#define X3D_GEOCOORD(node) ((struct X3D_GeoCoordinate*)node)

/* get a coordinate array - (SFVec3f) from either a NODE_Coordinate or NODE_GeoCoordinate */
struct Multi_Vec3f *getCoordinate (struct X3D_Node *innode, char *str) {
	struct X3D_Coordinate * xc;
	struct X3D_GeoCoordinate *gxc;
	struct X3D_Node *node;

	POSSIBLE_PROTO_EXPANSION (struct X3D_Node *, innode,node)

		xc = X3D_COORD(node);
	/* printf ("getCoordinate, have a %s\n",stringNodeType(xc->_nodeType)); */

	if (xc->_nodeType == NODE_Coordinate) {
		return &(xc->point);
	} else if (xc->_nodeType == NODE_GeoCoordinate) {
		COMPILE_IF_REQUIRED_RETURN_NULL_ON_ERROR;
		gxc = X3D_GEOCOORD(node);
		return &(gxc->__movedCoords);
	} else {
		ConsoleMessage ("%s - coord expected but got %s\n", stringNodeType(xc->_nodeType));
	}
	return NULL;
}



/*
void printLTDebug(char * fn, int ln)
{
 ppRenderFuncs p = (ppRenderFuncs)gglobal()->RenderFuncs.prv;
ConsoleMessage ("headlight pos %f %f %f %f at %s:%d",
p->light_pos[HEADLIGHT_LIGHT][0],p->light_pos[HEADLIGHT_LIGHT][1],
                p->light_pos[HEADLIGHT_LIGHT][2],p->light_pos[HEADLIGHT_LIGHT][3],fn,ln);
}
*/

