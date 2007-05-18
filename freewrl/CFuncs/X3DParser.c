/*******************************************************************
 Copyright (C) 2006 John Stewart, CRC Canada
 DISTRIBUTED WITH NO WARRANTY, EXPRESS OR IMPLIED.
 See the GNU Library General Public License (file COPYING in the distribution)
 for conditions of use and redistribution.
*********************************************************************/

/*
 * $Id: X3DParser.c,v 1.19 2007/05/18 19:59:14 crc_canada Exp $
 *
 */


#include "jsapi.h"
#include "jsUtils.h"
#include "headers.h"


/*
#include <stdio.h>
#define TRUE 1==1
#define FALSE 1==0
#define MYBUFSIZ 100000
*/
#include "expat.h"

/* If XMLCALL isn't defined, use empty one */
#ifndef XMLCALL
 #define XMLCALL
#endif /* XMLCALL */

/* this ifdef sequence is kept around, for a possible Microsoft Vista port */
#ifdef XML_LARGE_SIZE
#if defined(XML_USE_MSC_EXTENSIONS) && _MSC_VER < 1400
#define XML_FMT_INT_MOD "I64"
#else
#define XML_FMT_INT_MOD "ll"
#endif
#else
#define XML_FMT_INT_MOD "l"
#endif

#define LINE XML_GetCurrentLineNumber(x3dparser)

static int X3DParser_initialized = FALSE;
static XML_Parser x3dparser;
static int parentIndex = 0;
#define PARENTSTACKSIZE 256
struct X3D_Node *parentStack[PARENTSTACKSIZE];

#define PARSING_NODES 1
#define PARSING_SCRIPT 2
static int parserMode = PARSING_NODES;


/* DEF/USE table  for X3D parser */
struct DEFnameStruct {
        struct X3D_Node *node;
        struct Uni_String *name;
};


struct DEFnameStruct *DEFnames = 0;
int DEFtableSize = -1;
int MAXDEFNames = 0;

/* Script table - script parameter names, values, etc. */
struct ScriptFieldStruct {
	int scriptNumber;
	struct Uni_String *fieldName;
	int type;
	int kind;
	int offs;
};

struct ScriptFieldStruct *ScriptFieldNames = 0;
int ScriptFieldTableSize = -1;
int MAXScriptFieldParams = 0;

/* record each field of each script - the type, kind, name, and associated script */
static void registerX3DScriptField(int myScriptNumber,int type,int kind, int myFieldOffs, char *name) {
	ScriptFieldTableSize ++;

	#ifdef X3DPARSERVERBOSE
	printf ("registering script field %s script %d index %d\n",name,myScriptNumber,ScriptFieldTableSize);
	printf ("	type %d kind %d fieldOffs %d\n",type,kind,myFieldOffs);
	#endif


	/* ok, we got a name and a type */
	if (ScriptFieldTableSize >= MAXScriptFieldParams) {
		/* oooh! not enough room at the table */
		MAXScriptFieldParams += 100; /* arbitrary number */
		ScriptFieldNames = (struct ScriptFieldStruct*)REALLOC (ScriptFieldNames, sizeof(*ScriptFieldNames) * MAXScriptFieldParams);
	}

	ScriptFieldNames[ScriptFieldTableSize].scriptNumber = myScriptNumber;
	ScriptFieldNames[ScriptFieldTableSize].fieldName = newASCIIString(name);
	ScriptFieldNames[ScriptFieldTableSize].type = type;
	ScriptFieldNames[ScriptFieldTableSize].kind = kind;
	ScriptFieldNames[ScriptFieldTableSize].offs = myFieldOffs;
}

/* "forget" the DEFs. Keep the table around, though, as the entries will simply be used again. */
void kill_X3DDefs(void) {
	DEFtableSize = -1;
}

/* return a node assoicated with this name. If the name exists, return the previous node. If not, return
the new node */
struct X3D_Node *DEFNameIndex (char *name, struct X3D_Node* node) {
	unsigned len;
	int ctr;
	struct Uni_String *tmp;

	
	/* printf ("start of DEFNameIndex, name %s, type %s\n",name,stringNodeType(node->_nodeType)); */


	len = strlen(name) + 1; /* length includes null termination, in newASCIIString() */

	/* is this a duplicate name and type? types have to be same,
	   name lengths have to be the same, and the strings have to be the same.
	*/
	for (ctr=0; ctr<=DEFtableSize; ctr++) {
		tmp = DEFnames[ctr].name;
		if (strcmp(name,tmp->strptr)==0) {
			return DEFnames[ctr].node;
		}
	}

	/* nope, not duplicate */

	DEFtableSize ++;

	/* ok, we got a name and a type */
	if (DEFtableSize >= MAXDEFNames) {
		/* oooh! not enough room at the table */
		MAXDEFNames += 100; /* arbitrary number */
		DEFnames = (struct DEFnameStruct*)REALLOC (DEFnames, sizeof(*DEFnames) * MAXDEFNames);
	}

	DEFnames[DEFtableSize].name = newASCIIString(name);
	DEFnames[DEFtableSize].node = node;
	return node;
}

/* look through the script fields for this field, and return the values. */
static int getFieldFromScript (char *fieldName, int scriptno, int *offs, int *type) {
	int ctr;
	struct Uni_String *tmp;
	int len;

	#ifdef X3DPARSERVERBOSE
	printf ("getFieldFromScript, looking for %s\n",fieldName);
	#endif
	
	len = strlen(fieldName) +1; /* len in Uni_String has the '\0' on it */
	
        for (ctr=0; ctr<=ScriptFieldTableSize; ctr++) {
		if (scriptno == ScriptFieldNames[ctr].scriptNumber) {
                	tmp = ScriptFieldNames[ctr].fieldName;
                	if (strcmp(fieldName,tmp->strptr)==0) {
				*offs = ScriptFieldNames[ctr].offs;
				*type = ScriptFieldNames[ctr].type;
				#ifdef X3DPARSERVERBOSE
				printf ("getFieldFromScript - returning offset %d type %d (kind %d)\n",*offs,*type,
					ScriptFieldNames[ctr].kind);
				#endif
                	        return TRUE;
			}
                }
        }
        
	#ifdef X3DPARSERVERBOSE
	printf ("getFieldFromScript, did not find field %s in script %d\n",fieldName,scriptno);
	#endif
	
	/* did not find it */
	*offs = -1; *type = 0;
	return FALSE;
}

static int getRouteField (struct X3D_Node *node, int *offs, int* type, char *name, int dir) {
	int error = FALSE;
	int fieldInt;
	int ctmp;
	struct X3D_Script * sc;
	
 
	if (node->_nodeType == NODE_Script) {
		sc = (struct X3D_Script *) node;
		error = !(getFieldFromScript (name,sc->__scriptObj,offs,type));
	} else {

		/* lets see if this node has a routed field  fromTo  = 0 = from node, anything else = to node */
		fieldInt = findRoutedFieldInFIELDNAMES (node, name, dir);
		if (fieldInt >=0) findFieldInOFFSETS(NODE_OFFSETS[node->_nodeType], 
				fieldInt, offs, type, &ctmp);
	}
	if (*offs <0) {
		ConsoleMessage ("ROUTE: line %d Field %s not found in node type %s",LINE,
			name,stringNodeType(node->_nodeType));
		error = TRUE;
	}
	return error;
}

/* parse a ROUTE statement. Should be like:
	<ROUTE fromField="fraction_changed"  fromNode="TIME0" toField="set_fraction" toNode="COL_INTERP"/>
*/

static void parseX3DRoutes (char **atts) {
	struct X3D_Node *fromNode = NULL;
	struct X3D_Node *toNode = NULL;	
	int fromOffset = -1;
	int toOffset = -1;
	int i;
	int error = FALSE;

	int fromType;
	int toType;

	int scriptDiri = 0;

	#ifdef X3DPARSERVERBOSE
	printf ("\nstart ofrouting\n");	
	#endif

	/* 2 passes - first, find the nodes */
	for (i = 0; atts[i]; i += 2) {
		#ifdef X3DPARSERVERBOSE
		printf("ROUTING pass 1 field:%s=%s\n", atts[i], atts[i + 1]);
		#endif

		if (strcmp("fromNode",atts[i]) == 0) {
			fromNode = DEFNameIndex (atts[i+1], NULL);
			if (fromNode == NULL) {
				ConsoleMessage ("ROUTE statement, line %d fromNode (%s) does not exist",LINE,atts[i+1]);
				error = TRUE;
			}
		} else if (strcmp("toNode",atts[i]) == 0) {
			toNode = DEFNameIndex (atts[i+1],NULL);
			if (toNode == NULL) {
				ConsoleMessage ("ROUTE statement, line %d toNode (%s) does not exist",LINE,atts[i+1]);
				error = TRUE;
			}
		} else if ((strcmp("fromField",atts[i])!=0) &&
				(strcmp("toField",atts[i]) !=0)) {
			ConsoleMessage ("Field in line %d ROUTE statement not understood: %s",LINE,atts[i]);
			error = TRUE;
		}
	}

	/* get out of here if an error is found */
	if (error) return;

	#ifdef X3DPARSERVERBOSE
	printf ("end of pass1, fromNode %d, toNode %d\n",fromNode,toNode);
	printf ("looking for a route from a %s to a %s\n",stringNodeType(fromNode->_nodeType),
			stringNodeType(toNode->_nodeType));
	#endif

	/* get the direction correct */
	if (fromNode->_nodeType == NODE_Script) scriptDiri += FROM_SCRIPT;
	if (toNode->_nodeType == NODE_Script) scriptDiri += TO_SCRIPT;
	
	/* second pass - get the fields of the nodes */
	for (i = 0; atts[i]; i += 2) {
		if (strcmp("fromField",atts[i])==0) {
			error = getRouteField(fromNode, &fromOffset, &fromType, atts[i+1],0);
		} else if (strcmp("toField",atts[i]) ==0) {
			error = getRouteField(toNode, &toOffset, &toType, atts[i+1],1);
		}
	}	

	/* get out of here if an error is found */
	if (error) return;


	/* is there a script here? if so, now change the script NODE pointer to a Script index */
	if (fromNode->_nodeType == NODE_Script) fromNode = ((struct X3D_Node*) ((struct X3D_Script*)fromNode)->__scriptObj);
	if (toNode->_nodeType == NODE_Script) toNode = ((struct X3D_Node*) ((struct X3D_Script*)toNode)->__scriptObj);

	#ifdef X3DPARSERVERBOSE
	printf ("now routing from a %s to a %s %d %d\n",FIELDTYPES[fromType], FIELDTYPES[toType],fromType,toType);
	printf ("	pointers %d %d to %d %d\n",fromNode, fromOffset, toNode, toOffset);
	#endif

	/* are the types the same? */
	if (fromType != toType) {
		ConsoleMessage ("Routing type mismatch line %d %s != %s",LINE,stringFieldtypeType(fromType), stringFieldtypeType(toType));
		error = TRUE;
	}

	/* get out of here if an error is found */
	if (error) return;


	/* can we register the route? */
	CRoutes_RegisterSimple(fromNode, fromOffset, toNode, toOffset, returnRoutingElementLength(fromType),scriptDiri);
}

static int canWeIgnoreThisNode(char *name) {

	if (strcmp ("Header",name) == 0) {return TRUE;}
	if (strcmp ("Metadata",name) == 0) {return TRUE;}
	if (strcmp ("Scene",name) == 0) {return TRUE;}
	if (strcmp ("meta",name) == 0) {return TRUE;}
	if (strcmp ("head",name) == 0) {return TRUE;}
	if (strcmp ("X3D",name) == 0) {return TRUE;}
return FALSE;
}

/* parse normal X3D nodes/fields */
void parseNormalX3D(char *name, char** atts) {
	int i;

	struct X3D_Node *thisNode;
	struct X3D_Node *fromDEFtable;
	int coffset;
	int ctype;
	int ctmp;
	int foffset;

	int myNodeType;
	/* create this to be a new node */	
	myNodeType = findNodeInNODES(name);

	if (myNodeType != -1) {
		thisNode = createNewX3DNode(myNodeType);
		parentStack[parentIndex] = thisNode; 
		add_parent((void *)thisNode, parentStack[parentIndex-1]);


		if (thisNode->_nodeType == NODE_Script) {
			#ifdef X3DPARSERVERBOSE
			printf ("working through script parentIndex %d\n",parentIndex);
			#endif
			parserMode = PARSING_SCRIPT;

			((struct X3D_Script *)thisNode)->__scriptObj = nextScriptHandle();
			JSInit(((struct X3D_Script *)thisNode)->__scriptObj);
		}

		/* go through the fields, and link them in. SFNode and MFNodes will be handled 
		 differently - these are usually the result of a different level of parsing,
		 and the "containerField" value */
		for (i = 0; atts[i]; i += 2) {
			#ifdef X3DPARSERVERBOSE
			if (parserMode == PARSING_SCRIPT) {
				printf ("parsing script decl; have %s %s\n",atts[i], atts[i+1]);
			}
			#endif


			if (strcmp ("DEF",atts[i]) == 0) {
				#ifdef X3DPARSERVERBOSE
				printf ("this is a DEF, name %s\n",atts[i+1]);
				#endif

				fromDEFtable = DEFNameIndex (atts[i+1],thisNode);
				if (fromDEFtable != thisNode) {
					ConsoleMessage ("Warning - line %d duplicate DEF name: \'%s\'",LINE,atts[i+1]);
				}

			} else if (strcmp ("USE",atts[i]) == 0) {
				#ifdef X3DPARSERVERBOSE
				printf ("this is a USE, name %s\n",atts[i+1]);
				#endif

				fromDEFtable = DEFNameIndex (atts[i+1],thisNode);
				if (fromDEFtable == thisNode) {
					ConsoleMessage ("Warning - line %d DEF name: \'%s\' not found",LINE,atts[i+1]);
				} else {
					#ifdef X3DPARSERVERBOSE
					printf ("copying for field %s defName %s\n",atts[i], atts[i+1]);
					#endif

					if (fromDEFtable->_nodeType != fromDEFtable->_nodeType) {
						ConsoleMessage ("Warning, line %d DEF/USE mismatch, '%s', %s != %s", LINE,
							atts[i+1],stringNodeType(fromDEFtable->_nodeType), stringNodeType (thisNode->_nodeType));
					} else {
						thisNode = fromDEFtable;
						parentStack[parentIndex] = thisNode; 
						#ifdef X3DPARSERVERBOSE
						printf ("successful copying for field %s defName %s\n",atts[i], atts[i+1]);
						#endif

					}
				}
			} else {
				setField_fromJavascript (thisNode, atts[i],atts[i+1]);
			}
		}
	} else if (strcmp(name,"ROUTE") == 0) {
		parseX3DRoutes(atts);
	} else {
		ConsoleMessage ("X3D Parser, line %d node type %s not supported by FreeWRL",LINE,name);
		return;
	}


	if (parentIndex < (PARENTSTACKSIZE-2)) 
		parentIndex += 1;
	else ConsoleMessage ("X3DParser, line %d stack overflow",LINE);
	
}

static void parseScriptField(char *name, char **atts) {
	int i;
	uintptr_t myScriptNumber;
	char myparams[3][255];
	int strl;
	int which;
	int myFieldNumber;
	
	/* check sanity for top of stack This should be a Script node */
	if (parentStack[parentIndex-1]->_nodeType != NODE_Script) {
		ConsoleMessage ("X3DParser, line %d, expected the parent to be a Script node",LINE);
		return;
	}
	
	myScriptNumber = ((struct X3D_Script *)parentStack[parentIndex-1])->__scriptObj;

	/* set up defaults for field parsing */
	for (i=0;i<3;i++) myparams[i][0] = '\0';
	

	/* copy the fields over */
	/* have a "key" "value" pairing here. */
	for (i = 0; atts[i]; i += 2) {
		/* skip any "appinfo" field here */
		if (strcmp(atts[i],"appinfo") != 0) {
			if (strcmp(atts[i],"name") == 0) { which = 0;	
			} else if (strcmp(atts[i],"accessType") == 0) { which = 1;
			} else if (strcmp(atts[i],"type") == 0) { which = 2;
			} else {
				ConsoleMessage ("X3D Script parsing line %d- unknown field type %s",LINE,atts[i]);
				return;
			}
			if (myparams[which][0] != '\0') {
				ConsoleMessage ("X3DScriptParsing line %d, field %s already has name - is %s",
					LINE,myparams[which],atts[i+1]);
			}

			/* have the key, copy the value over */
			strl = strlen(atts[i+1]); if (strl>250) strl=250;
			strncpy(myparams[which],atts[i+1],strl);
			myparams[which][strl]='\0';
		}
	}

	#ifdef X3DPARSERVERBOSE
	printf ("myparams:\n	%s\n	%s\n	%s\n",myparams[0],myparams[1],myparams[2]);
	#endif

	/* register this field with the Javascript Field Indexer */
	myFieldNumber = JSparamIndex(myparams[0],myparams[2]);

	registerX3DScriptField(myScriptNumber,
		findFieldInFIELDTYPES(myparams[2]),
		findFieldInX3DACCESSORS(myparams[1]),
		myFieldNumber,myparams[0]);

	/* and initialize it */
	InitScriptFieldC (myScriptNumber,
		findFieldInX3DACCESSORS(myparams[1]),
		findFieldInFIELDTYPES(myparams[2]),myparams[0],NULL);
}


static int inCDATA = FALSE;
static char *scriptText = NULL;
static int scriptTextMallocSize = 0;

/* we get script text from a number of sources; from the URL field, from CDATA, from a file
   pointed to by the URL field, etc... handle the data in one place */

void initScriptWithScript() {
	uintptr_t myScriptNumber;
	char *startingIndex;
	jsval rval;
	struct X3D_Script * me;
	char *myText = NULL;
	int i;
	struct Uni_String *myUni;
	char *mypath;
	char *thisurl;
	int count;
	char filename[1000];
	char firstBytes[4];
	int fromFile = FALSE;

	/* sanity checking... */
	me = (struct X3D_Script *)parentStack[parentIndex-1];

	if (me->_nodeType != NODE_Script) {
		ConsoleMessage ("initScriptWithScript - Expected to find a NODE_Script, got a %s\n",
		stringNodeType(me->_nodeType));
		return;
	}

	myScriptNumber = me->__scriptObj;

	/* did the script text come from a CDATA node?? */
	if (scriptText != NULL) if (scriptText[0] != '\0') myText = scriptText;

	/* do we still have nothing? Look in the url node for a file or a script. */
	if (myText == NULL) {
	        /* lets make up the path and save it, and make it the global path */
	        /* copy the parent path over */
	        mypath = strdup(me->__parenturl->strptr);
	        removeFilenameFromPath (mypath);

		/* try the first url, up to the last, until we find a valid one */
		count = 0;
		while (count < me->url.n) {
			thisurl = me->url.p[count]->strptr;

			/* leading whitespace removal */
			while ((*thisurl <= ' ') && (*thisurl != '\0')) thisurl++;

			/* is thisurl a vrml/ecma/javascript string?? */
			if ((strstr(thisurl,"ecmascript:")!= 0) ||
				(strstr(thisurl,"vrmlscript:")!=0) ||
				(strstr(thisurl,"javascript:")!=0)) {
				myText = thisurl;
				break;
			} else {
				/* check to make sure we don't overflow */
				if ((strlen(thisurl)+strlen(mypath)) > 900) return FALSE;

				/* we work in absolute filenames... */
				makeAbsoluteFileName(filename,mypath,thisurl,RUNNINGASPLUGIN || isMacPlugin);

				if (fileExists(filename,firstBytes,TRUE)) {
					myText = readInputString(filename,"");
					fromFile = TRUE;
					break;
				}
			}
			count ++;
		}
/* error condition, if count >= me->url.n */
	}

	/* still have a problem here? */
	if (myText == NULL) {
		ConsoleMessage ("could not find Script text in url or CDATA");
		return;
	}

	/* peel off the ecmascript etc unless it was read in from a file */
	if (!fromFile) {
		startingIndex = strstr(myText,"ecmascript:");
		if (startingIndex != NULL) { startingIndex += strlen ("ecmascript:");
		} else if (startingIndex == NULL) {
			startingIndex = strstr(myText,"vrmlscript:");
			if (startingIndex != NULL) startingIndex += strlen ("vrmlscript:");
		} else if (startingIndex == NULL) {
			startingIndex = strstr(myText,"javascript:");
			if (startingIndex != NULL) startingIndex += strlen ("javacript:");
		} else {
			/* text is from a file in the URL field */
			startingIndex = myText;
		}
	} else {
		startingIndex = myText; /* from a file, no ecmascript: required */
	}

	if (startingIndex == NULL) {
		ConsoleMessage ("X3DParser, line %d have Script node, but no valid script",LINE);
		return;
	}

	if (!ActualrunScript (myScriptNumber, startingIndex, &rval)) {
		ConsoleMessage ("X3DParser, script initialization error at line %d",LINE);
		return;
	}
}

static void XMLCALL startCDATA (void *userData) {
	#ifdef X3DPARSERVERBOSE
	printf ("start CDATA\n");
	#endif
	inCDATA = TRUE;
}

static void XMLCALL endCDATA (void *userData) {
	#ifdef X3DPARSERVERBOSE
	printf ("EndCData is %s\n",scriptText);
	#endif
	inCDATA = FALSE;

	/* check sanity for top of stack This should be a Script node */
	if (parentStack[parentIndex-1]->_nodeType != NODE_Script) {
		ConsoleMessage ("X3DParser, line %d, expected the parent to be a Script node",LINE);
		return;
	}
	
	#ifdef X3DPARSERVERBOSE
        printf ("returning from EndCData\n");
        #endif  

}

static void XMLCALL handleCDATA (void *userData, const char *string, int len) {
	char mydata[4096];
	char firstTime;
	if (inCDATA) {
		/* do we need to set this string larger? */
		if (len > scriptTextMallocSize-10) {
			firstTime = (scriptTextMallocSize == 0);
			scriptTextMallocSize +=4096;
			if (firstTime) {
				scriptText = MALLOC (scriptTextMallocSize);
				scriptText[0] = '\0';
			} else {
				scriptText = REALLOC (scriptText,scriptTextMallocSize);
			}
		}
		memcpy (mydata, string,len);
		mydata[len] = '\0';
		strcat (scriptText,mydata);
/*
		printf ("CDATA full text %s\n",scriptText);
*/
	
	}
}

static void XMLCALL startElement(void *unused, const char *name, const char **atts) {
	int i;

	/* is this a node that we can ignore? */
	if (canWeIgnoreThisNode(name)) return;

	#ifdef X3DPARSERVERBOSE
	for (i = 0; i < parentIndex; i++) putchar('\t');
	printf ("Node Name %s ",name);
	for (i = 0; atts[i]; i += 2) printf(" field:%s=%s", atts[i], atts[i + 1]);
	printf ("\n");
	#endif

	/* what is the mode? normal X3D? script parsing?? */
	if (parserMode == PARSING_SCRIPT) parseScriptField (name, atts);
	else parseNormalX3D(name, atts);
}
	
static void XMLCALL endElement(void *unused, const char *name) {
	int i;

	int coffset;
	int ctype;
	int ctmp;
	uintptr_t *destnode;
	char *memptr;

	/* is this a node that we can ignore? */
	if (canWeIgnoreThisNode(name)) return;
	
	/* is this a Script? */
	if (strcmp (name,"field") == 0) {
		if (parserMode != PARSING_SCRIPT) {
			ConsoleMessage ("X3DParser: line %d Got a <field> but not parsing Scripts",LINE);
			printf ("Got a <fieldt> but not parsing Scripts\n");
		}
		return;
	}


	/* sanity check here */
	if (parserMode == PARSING_SCRIPT) {
		if (strcmp (name,"Script") != 0) 
		ConsoleMessage ("X3DParser line %d endElement, name %s, still PARSING_SCRIPTS",LINE,name);
	} 


	/* is this the end of a Script? */
	if (strcmp(name,"Script") == 0) {
		#ifdef X3DPARSERVERBOSE
		printf ("scriptText is %s\n",scriptText);
		#endif
		initScriptWithScript();

		FREE_IF_NZ(scriptText);
		scriptTextMallocSize = 0;
		#ifdef X3DPARSERVERBOSE
		printf ("got END of script - script should be registered\n");
		#endif

		parserMode = PARSING_NODES;
	}

	
	if (parentIndex > 1) parentIndex -= 1; else ConsoleMessage ("X3DParser, line %d stack underflow",LINE);


	#ifdef X3DPARSERVERBOSE
	for (i = 0; i < parentIndex; i++) putchar('\t');

	printf ("parentIndex %d, stack top %d, stack- %d\n",parentIndex,parentStack[parentIndex],parentStack[parentIndex-1]);
	printf ("ok, linking in %s to %s, field %s (%d)\n",
		stringNodeType(parentStack[parentIndex]->_nodeType),
		stringNodeType(parentStack[parentIndex-1]->_nodeType),
		stringFieldType(parentStack[parentIndex]->_defaultContainer),
		parentStack[parentIndex]->_defaultContainer);
	#endif


	/* Link it in; the parent containerField should exist, and should be an SF or MFNode  */
	findFieldInOFFSETS(NODE_OFFSETS[parentStack[parentIndex-1]->_nodeType], 
		parentStack[parentIndex]->_defaultContainer, &coffset, &ctype, &ctmp);

	if (coffset <= 0) {
		printf ("X3DParser - trouble finding field %s in node %s\n",
			stringFieldType(parentStack[parentIndex]->_defaultContainer),
			stringNodeType(parentStack[parentIndex-1]->_nodeType));
#ifdef xxx
		printf ("is this maybe a PROTO?? if so, it will be a Group node with __protoDef set to the pointer\n");
		if (node->_nodeType == NODE_Group) {
			group = (struct X3D_Group *)node;
			printf ("it IS a group...\n");
			if (group->__protoDef) {
				printf ("and, this is a PROTO...have to go through PROTO defs to get to it\n");
			}
		}
#endif
	}

	if ((ctype != FIELDTYPE_MFNode) && (ctype != FIELDTYPE_SFNode)) {
		ConsoleMessage ("X3DParser, line %d trouble linking to field %s, node type %s (this nodeType %s)", LINE,
			stringFieldType(parentStack[parentIndex]->_defaultContainer),
			stringNodeType(parentStack[parentIndex-1]->_nodeType),
			stringNodeType(parentStack[parentIndex]->_nodeType));
		return;
	}
	memptr = (char *)parentStack[parentIndex-1] + coffset;
	if (ctype == FIELDTYPE_SFNode) {
		/* copy over a single memory pointer */
		destnode = (uintptr_t *) memptr;
		*destnode = parentStack[parentIndex];
	} else {
		AddRemoveChildren (
			parentStack[parentIndex-1], /* parent */
			memptr,			/* where the children field is */
			&(parentStack[parentIndex]),	/* this child, 1 node */
                1, 1);

	}
}

int initializeX3DParser () {
	x3dparser = XML_ParserCreate(NULL);
	XML_SetElementHandler(x3dparser, startElement, endElement);
	XML_SetCdataSectionHandler (x3dparser, startCDATA, endCDATA);
	XML_SetDefaultHandler (x3dparser,handleCDATA);
	XML_SetUserData(x3dparser, &parentIndex);
}

void shutdownX3DParser () {
	if (X3DParser_initialized) {
		X3DParser_initialized = FALSE;
		XML_ParserFree(x3dparser);
	}
}

int X3DParse (struct X3D_Group* myParent, char *inputstring) {
	parentIndex = 0;
	parentStack[parentIndex] = myParent;
	parentIndex ++;
	
	if (XML_Parse(x3dparser, inputstring, strlen(inputstring), TRUE) == XML_STATUS_ERROR) {
		fprintf(stderr,
			"%s at line %" XML_FMT_INT_MOD "u\n",
			XML_ErrorString(XML_GetErrorCode(x3dparser)),
			XML_GetCurrentLineNumber(x3dparser));
		return FALSE;
	}
	if (parentIndex != 1) {
		ConsoleMessage ("X3DParser line %d  - stack issues.... parentIndex %d",LINE,parentIndex);
		return FALSE;
	}
	return TRUE;
}

