/*
=INSERT_TEMPLATE_HERE=

$Id: ColladaParser.c,v 1.1 2009/12/21 23:23:25 crc_canada Exp $

???

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
#include "../vrml_parser/CParseGeneral.h"
#include "../scenegraph/Vector.h"
#include "../vrml_parser/CFieldDecls.h"
#include "../world_script/CScripts.h"
#include "../world_script/JScript.h"
#include "../world_script/fieldSet.h"
#include "../vrml_parser/CParseParser.h"
#include "../vrml_parser/CParseLexer.h"
#include "../vrml_parser/CProto.h"
#include "../vrml_parser/CParse.h"
#include "../input/EAIHeaders.h"        /* resolving implicit declarations */
#include "../input/EAIHelpers.h"        /* resolving implicit declarations */


#include "ColladaParser.h"

#if HAVE_EXPAT_H
# include <expat.h>
#endif

#define PROTOINSTANCE_MAX_LEVELS 10
static XML_Parser colladaParser[PROTOINSTANCE_MAX_LEVELS];
static XML_Parser currentColladaParser = NULL;
static int parentIndex = 0;
static int ColladaParserRecurseLevel = 0;
static int inCDATA = FALSE;
struct X3D_Node *colladaParentStack[PARENTSTACKSIZE];
static int indentLevel = 0;


static void XMLCALL startCDATA (void *userData) {
        if (CDATA_Text_curlen != 0) {
/*
                ConsoleMessage ("X3DParser - hmmm, expected CDATA_Text_curlen to be 0, is not");
                printf ("CDATA_TEXT_CURLEN is %d\n",CDATA_Text_curlen);
printf ("CADAT_Text:%s:\n",CDATA_Text);
*/
                CDATA_Text_curlen = 0;
        }

        #ifdef COLLADAPARSERVERBOSE
        printf ("startCDATA -parentIndex %d parserMode %s\n",parentIndex,parserModeStrings[getParserMode()]);
        #endif
        inCDATA = TRUE;
}

static void XMLCALL endCDATA (void *userData) {
        #ifdef COLLADAPARSERVERBOSE
        printf ("endCDATA, cur index %d\n",CDATA_Text_curlen);
        printf ("endCDATA -parentIndex %d parserMode %s\n",parentIndex,parserModeStrings[getParserMode()]);
        #endif
        inCDATA = FALSE;

        dumpCDATAtoProtoBody (CDATA_Text);

        #ifdef COLLADAPARSERVERBOSE
        printf ("returning from EndCData\n");
        #endif


}

static void XMLCALL handleCDATA (void *userData, const char *string, int len) {
/*
        printf ("handleCDATA...(%d)...",len);
if (inCDATA) printf ("inCDATA..."); else printf ("not inCDATA...");
printf ("\n");
*/
}

static void XMLCALL startElement(void *unused, const char *name, const char **atts) {
	int myNodeIndex;

#ifdef COLLADAVERBOSE
{int i,j; for (j=0; j< indentLevel; j++) printf ("  ");
	printf ("startElement: %s : level %d\n",name,indentLevel);
       for (i = 0; atts[i]; i += 2) {
		for (j=0; j< indentLevel; j++) printf ("  ");
                printf("    field:%s=%s\n", atts[i], atts[i + 1]);
	}
}
#endif

	indentLevel++;
}

static void XMLCALL endElement(void *unused, const char *name) {
	indentLevel--;

#ifdef COLLADAVERBOSE
{int i; for (i=0; i< indentLevel; i++) printf ("  ");
	printf ("endElement: %s : level %d\n",name,indentLevel);
}
#endif

}


static XML_Parser initializeColladaParser () {
	ColladaParserRecurseLevel++;

	if (ColladaParserRecurseLevel >= PROTOINSTANCE_MAX_LEVELS) {
		ConsoleMessage ("XML_PARSER init: XML file PROTO nested too deep\n");
		ColladaParserRecurseLevel--;
	} else {
		colladaParser[ColladaParserRecurseLevel] = XML_ParserCreate(NULL);
		XML_SetElementHandler(colladaParser[ColladaParserRecurseLevel], startElement, endElement);
		XML_SetCdataSectionHandler (colladaParser[ColladaParserRecurseLevel], startCDATA, endCDATA);
		XML_SetDefaultHandler (colladaParser[ColladaParserRecurseLevel],handleCDATA);
		XML_SetUserData(colladaParser[ColladaParserRecurseLevel], &parentIndex);
	}
	/* printf ("initializeColladaParser, level %d, parser %u\n",colladaParser[ColladaParserRecurseLevel]); */

	return colladaParser[ColladaParserRecurseLevel];
}

static void shutdownColladaParser () {
	/* printf ("shutdownColladaParser, recurseLevel %d\n",ColladaParserRecurseLevel); */
	XML_ParserFree(colladaParser[ColladaParserRecurseLevel]);
	ColladaParserRecurseLevel--;
	
	/* lets free up memory here for possible PROTO variables */
	if (ColladaParserRecurseLevel == INT_ID_UNDEFINED) {
		/* if we are at the bottom of the parser call nesting, lets reset parentIndex */
		parentIndex = 0;
		freeProtoMemory ();
	}

	if (ColladaParserRecurseLevel < INT_ID_UNDEFINED) {
		ConsoleMessage ("XML_PARSER close underflow");
		ColladaParserRecurseLevel = INT_ID_UNDEFINED;
	}

	/* CDATA text space, free it up */
        FREE_IF_NZ(CDATA_Text);
#ifdef OLDCODE

        CDATA_TextMallocSize = 0; 
#endif
	if (ColladaParserRecurseLevel > INT_ID_UNDEFINED)
		currentColladaParser = colladaParser[ColladaParserRecurseLevel];

	/* printf ("shutdownColladaParser, current ColladaParser %u\n",currentColladaParser); */
}

int ColladaParse (struct X3D_Group* myParent, const char *inputstring) {
	currentColladaParser = initializeColladaParser();

	/* printf ("X3DParse, current ColladaParser is %u\n",currentColladaParser); */


	INCREMENT_PARENTINDEX
	colladaParentStack[parentIndex] = X3D_NODE(myParent);

	if (XML_Parse(currentColladaParser, inputstring, strlen(inputstring), TRUE) == XML_STATUS_ERROR) {
		fprintf(stderr,
			"%s at line %" XML_FMT_INT_MOD "u\n",
			XML_ErrorString(XML_GetErrorCode(currentColladaParser)),
			XML_GetCurrentLineNumber(currentColladaParser));
		shutdownColladaParser();
		return FALSE;
	}
	shutdownColladaParser();
	return TRUE;
}
