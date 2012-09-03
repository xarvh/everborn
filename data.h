/*
 DATA.H			v0.25
 GL Textures loadup/managing. For now.

 Copyright (c) 2004 Francesco Orsenigo <francesco.orsenigo@gmail.com>

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; see the file COPYING.  If not, write to
 the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 Boston, MA 02111-1307, USA.
*/
#ifndef H_FILE_DATA
#define H_FILE_DATA
//=============================================================================

class dataC {
    void	Init(char*, double, double, dataC**);

  public:
    dataC*	Next;
    char	Fn[64];

    int		(*Proc)(void* bitmap, void* proc_v);
    void*	ProcV;
    unsigned	Id;

    double	QuadX;	// left texture coordinate (used to cut a pixel away)
    double	QuadY;	// up texture coordinate (used to cut a pixel away)

    double	Cols;	// subimages
    double	Rows;


    //
    ~dataC();
    dataC();				// constructors for session data,
    dataC(double cols, double rows);	//  unloaded when session is closed

    dataC(char* fn);				// constructor for base data,
    dataC(char* fn, double cols, double rows);	//  always required

    // load and unload
    int  Load();
    void Unload();

    // gl calls
    void BindTexture();
    void Select(int n, double w, double h);
    void Rect(int n, double w, double h);
    void Quad(int n);

//	in THIS case inheritance may actually be useful...
//	(is "virtual" really required?)
//    virtual Load
//    virtual Destroy
//    	operator() provides the actual resource (different return type)
};





/*=============================================================================
 * BIND
 *
 * Bind and enable texture, and reset glMatrixMode(GL_TEXTURE) transformations
 * No other direct call to gBindTexture() should be allowed in the program.
 */
void dataBindTexture(int glText);
void dataBindTexture(int glText, double tX, double tY, double sW, double sH);



/*=============================================================================
 * SUPPORT
 */
char* dataFindPath(char *);
void dataUnload();		// destroys only sesson data
int  dataLoadMissing();
int  dataReload();



#endif
//EOF =========================================================================
