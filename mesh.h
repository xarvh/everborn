
#include <GL/gl.h>


class meshC { public:
    meshC*	Next;

    char*	FullName;	// full name, but without radius and length
    char*	Name;		// name, stripped of everything, 0 for default part

    GLuint	GLList;		// playercolor faces go first

    float	Radius;		// physical object radius, regardless of mesh
    float	Length;		// physical object length, regardless of mesh

    //
    meshC(char* full_name);	// interpretes full_name
    ~meshC();
};




class meshFileC { public:
    meshFileC*	Next;		// required to reload all if the GL contex is lost

    char*	Fn;

    meshC*	MeshesHook;

    //
    meshFileC(char* fn);
    ~meshFileC();

    //
    int Load();
};



void meshReloadAll();




/*=============================================================================
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#include "main.h"


//void pr(char* a, char* b) { perror(b); }







/*=============================================================================
 * MESH
 */
meshC::meshC(char* full_name)
{
 // clean
 GLList = 0;
 Next = 0;

 // full name
 char* n = FullName = strdup(full_name);

 // skip left/right
 if(*n == 'r' || *n == 'l') n++;

 // skip location code
 while(*n >= 'A' && *n <= 'Z') n++;

 // name?
 Name = n;
 while(*n >= 'a' && *n <= 'z') n++;
 if(n == Name) Name = 0;	// no name specified, this is a default part

 // physical size
 if( 2 > sscanf(n, "%fx%f", &Radius, &Length))
    Radius = Length = 1.;

 // ensure decent values
 if(Radius > 10. || Radius < .1) Radius = 1.;
 if(Length > 10. || Length < .1) Length = 1.;

 // terminate name
 n = '\0';
}



meshC::~meshC()
{
 free(FullName);
 if(GLList) glDeleteLists(GLList, 1);
 if(Next) delete Next;
}





/*=============================================================================
 * MESH FILE
 */
static meshFileC* MeshFilesHook = 0;

meshFileC::meshFileC(char* fn)
{
 Fn = strdup(fn);
 MeshesHook = 0;

 Next = MeshFilesHook;
 MeshFilesHook = this;
}

meshFileC::~meshFileC()
{
 meshFileC** h;
 for(h = &MeshFilesHook; *h && *h != this; h = &(*h)->Next);
 if(*h) *h = Next;

 free(Fn);
 if(MeshesHook) delete MeshesHook;
}





/*=============================================================================
 *
 */
class mflFaceC;
class mflMaterialC { public:
    mflMaterialC*	Next;
    mflFaceC*		FacesHook;

    char*		Name;

    float		S[0];
    float		Diffuse[4];
    float		Ambient[4];
    float		Specular[4];
    float		Emission[4];
    float		Shininess[1];
    float		E[0];

    //
    mflMaterialC(char* name);
    ~mflMaterialC();
};



class mflVertexC { public:
    mflVertexC*		Next;
    float		V[3];

    bool		Used;

    //
    mflVertexC(mflVertexC**);
    ~mflVertexC();
};



class mflPointC { public:
    mflPointC*		Next;
    mflVertexC*		Vertex;
    mflVertexC*		Texture;
    mflVertexC*		Normal;

    //
    mflPointC(mflPointC**);
    ~mflPointC();
};

class mflFaceC { public:
    mflFaceC*		Next;
    mflPointC*		PointsHook;

    //
    mflFaceC(mflFaceC**);
    ~mflFaceC();
};





/*=============================================================================
 *
 */
static mflMaterialC* MatsHook = 0;
static mflVertexC* VertsHook = 0;
static mflVertexC* NormsHook = 0;
static mflVertexC* TextsHook = 0;





mflMaterialC::mflMaterialC(char* name)
{
 mflMaterialC** h = &MatsHook;
 while(*h) h = &(*h)->Next;
 *h = this;
 Next = 0;

 Name = strdup(name);
 FacesHook = 0;

 for(float* f = S; f < E; f++) *f = .0;
}

mflMaterialC::~mflMaterialC()
{
 float* f;
 pf("Material: %s\n", Name);
 f = Diffuse; pf("  diff: %f %f %f %f\n", f[0], f[1], f[2], f[3]);
 f = Ambient; pf("  ambi: %f %f %f %f\n", f[0], f[1], f[2], f[3]);
 f = Specular; pf("  spec: %f %f %f %f\n", f[0], f[1], f[2], f[3]);
 f = Emission; pf("  emi: %f %f %f %f\n", f[0], f[1], f[2], f[3]);


 free(Name);
 if(Next) delete Next;
 if(FacesHook) delete FacesHook;
}





mflVertexC::mflVertexC(mflVertexC** h)
{
 while(*h) h = &(*h)->Next;
 *h = this;
 Next = 0;

 Used = false;
}

mflVertexC::~mflVertexC()
{
 if(!Used) pe("NOT USED!!!\n");

 if(Next) delete Next;
}





mflPointC::mflPointC(mflPointC** h)
{
 while(*h) h = &(*h)->Next;
 *h = this;
 Next = 0;

 Vertex = 0;
 Texture = 0;
 Normal = 0;
}

mflPointC::~mflPointC()
{
 if(Next) delete Next;
}





mflFaceC::mflFaceC(mflFaceC** h)
{
 while(*h) h = &(*h)->Next;
 *h = this;
 Next = 0;

 PointsHook = 0;
}

mflFaceC::~mflFaceC()
{
 if(Next) delete Next;
 if(PointsHook) delete PointsHook;
}





/*=============================================================================
 *
 */
static GLuint objToList()
{
 //@@ optimize/reduce


 // normalize normals!
 for(mflVertexC* n = NormsHook; n; n = n->Next)
 {
    double mod = 0;
    for(int i = 0; i < 3; i++) mod += n->V[i]*n->V[i];
    mod = sqrt(mod);
    for(int i = 0; i < 3; i++) n->V[i] /= mod;
 }


 // create list
 GLuint list = glGenLists(1);
 glNewList(list, GL_COMPILE);

 // fill list
 for(mflMaterialC* m = MatsHook; m; m = m->Next)
    if(m->FacesHook && m->FacesHook->PointsHook)
    {
	if(m != MatsHook)	// not "Player" material
	{
//	    __f("== material\n");
	    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m->Ambient);
	    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m->Diffuse);
	    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m->Specular);
	    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, m->Emission);
	    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, m->Shininess);
	}

	for(mflFaceC* f = m->FacesHook; f; f = f->Next) if(f->PointsHook)
	{
//	    __f("== face\n");

//	    glBegin(GL_POLYGON);
	    glBegin(GL_TRIANGLE_FAN);
		for(mflPointC* p = f->PointsHook; p; p = p->Next)
		{
		    glColor3f(1,1,1);
//		    __f("  - vertex %f %f %f\n", p->Vertex->V[0], p->Vertex->V[1], p->Vertex->V[2]);
		    if(p->Normal) glNormal3fv(p->Normal->V);
		    //if(p->Texture) glTextCoord2fv(p->Texture->V);
		    glVertex3fv(p->Vertex->V);
		}
	    glEnd();
	}
    }

 // close
 glEndList();
 return list;
}





/*=============================================================================
 *
 */
static mflVertexC* getV(mflVertexC* hook, int id)
{
 int q = id;//@@

 mflVertexC* v;
 for(v = hook; v && --id; v = v->Next);

 //@@
 if(v) v->Used = true;
 else pe("vertex %d not found\n", q);

 return v;
}

static char* skipToken(char* s)
{
 while(isalnum(*s)) s++;
 while(isblank(*s)) s++;
 return s;
}

static void sTerm(char* s)
{
 while(*s > ' ') s++;
 *s = '\0';
}

static char* isString(char* s, char* id)
{
 while(isblank(*s)) s++;
 while(*s == *id) s++, id++;	// compare
 if(*id) return 0;	// strings are different
 while(isblank(*s)) s++;
 return s;
}





void readFace(mflFaceC* f, char* s)
{
 while(*s != '\0')
 {
    int a;

    // skip spaces
    while(isblank(*s)) s++;

    // read vertex id
    a = strtol(s, &s, 10);
    if(!a) return;

    // check that vertex is valid
    mflVertexC* v = getV(VertsHook, a);
    if(!v) return;

    // valid vertex: allocate point and set vertex
    mflPointC* p = new mflPointC(&f->PointsHook);
    p->Vertex = v;

    // check '/'
    if(*s++ != '/') continue;

    // read texture id
    a = strtol(s, &s, 10);
    if(a) p->Texture = getV(TextsHook, a);

    // check '/'
    if(*s++ != '/') continue;

    // read normal id
    a = strtol(s, &s, 10);
    if(a) p->Normal = getV(NormsHook, a);
 }
}





static void closeMesh(char* name, meshC** h)
{
 // create mesh if list is valid
 if(name)
    if(GLuint list = objToList())
    {
	meshC* m = new meshC(name);
	m->Next = *h;
        *h = m;

	m->GLList = list;
    }


 // clear
 for(mflMaterialC* m = MatsHook; m; m = m->Next)
 {
    delete m->FacesHook;
    m->FacesHook = 0;
 }
}





static void loadMaterialLibrary(char* fn)
{
 pf("opening [%s]\n", fn);
 FILE* f = fopen(fn, "rt");
 if(!f)
 {
    pr(fn, "fopen");
    return;
 }


 mflMaterialC* mt = 0;

 pf("--loading %s\n", fn);

 char bf[2048];
 while(fgets(bf, sizeof(bf), f))
    switch(bf[0]) {
	case 'n': if(char* n = isString(bf, "newmtl")) {
	    sTerm(n);
	    if(!strcasecmp(n, "player")) mt = 0;
	    else mt = new mflMaterialC(n);
	} break;


	case 'K': {
	    if(!mt) continue;	// no material selected

	    float* t = 0;
	    switch(bf[1]) {
		case 'd': t = mt->Diffuse; break;
		case 'a': t = mt->Ambient; break;
		case 's': t = mt->Specular; break;
		case 'e': t = mt->Emission; break;
		default: break;
	     }

	    if(!t) continue; // invalid line

	    int n = sscanf(bf+3, "%f %f %f %f", t, t+1, t+2, t+3);
	    pf("scan %s: %d (%f %f %f %f)\n", mt->Name, n, t[0], t[1], t[2], t[3]);
	    for(int i = n; i < 4; i++) t[i] = 1.;
	} break;


	case 'N': if(bf[1] == 's') {
	    if(!mt) continue;
	    sscanf(bf+3, "%f", mt->Shininess);
	} break;

	default: continue;
    }

 fclose(f);
}





int meshFileC::Load()
{
 // destroy old meshes
 if(MeshesHook)
 {
    delete MeshesHook;
    MeshesHook = 0;
 }


 // open file
 FILE* f = fopen(Fn, "rt");
 if(!f)
 {
    pr(Fn, "fopen");
    return -1;
 }


 // create player material, no need to inizialize anything
 mflMaterialC* currentMaterial = new mflMaterialC("");


 // empty mesh name
 char* meshName = 0;

 // read the file, line by line
 char bf[2048];
 while(fgets(bf, sizeof(bf), f))
    switch(bf[0]) {
	case 'v': {
	    // vertex, texture or normal?
	    mflVertexC** hook;
	    if(isblank(bf[1])) hook = &VertsHook;
	    else if(bf[1] == 't') hook = &TextsHook;
	    else if(bf[1] == 'n') hook = &NormsHook;

	    // read, success does not matter
	    mflVertexC* v = new mflVertexC(hook);
	    sscanf(bf+2, "%f %f %f", v->V, v->V+1, v->V+2);
	} break;


	case 'o': if(char* n = skipToken(bf)) {
	    // close previous mesh
	    closeMesh(meshName, &MeshesHook);
	    if(meshName) free(meshName);

	    // init new
	    meshName = strdup(n);
	} break;


	case 'f': if(char* n = skipToken(bf)) {
	    mflFaceC* f = new mflFaceC(&currentMaterial->FacesHook);
	    readFace(f, n);
	} break;


	case 'm': if(char* n = isString(bf, "mtllib")) {
	    sTerm(n);
	    loadMaterialLibrary(n);
	} break;

	case 'u': if(char* n = isString(bf, "usemtl")) {
	    currentMaterial = MatsHook; // if material is not found, use player color

	    sTerm(n);
	    for(mflMaterialC* m = MatsHook->Next; m; m = m->Next)
		if(!strcmp(m->Name, n))
		    currentMaterial = m;
	    pf("selected: [%s]\n", currentMaterial->Name);
	} break;

	default: break;
    }


 // close last mesh
 closeMesh(meshName, &MeshesHook);


 // close file
 fclose(f);


 // clean
 if(MatsHook) delete MatsHook;		MatsHook = 0;
 if(VertsHook) delete VertsHook;	VertsHook = 0;
 if(NormsHook) delete NormsHook;	NormsHook = 0;

 // done
 return 0;
}





//EOF =========================================================================
