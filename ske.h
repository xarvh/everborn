/*
 SKE.H			v0.00
 Skeletal animations.

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
#ifndef H_FILE_SKE
#define H_FILE_SKE
//=============================================================================

#define TORSO_PITCH	0
#define TORSO_ROLL	1

#define L_SHX		2
#define L_SHZ		3
#define L_ELB		4
#define L_HAND		5

#define R_SHX		6
#define R_SHZ		7
#define R_ELB		8
#define R_HAND		9

#define L_HIP		10
#define L_KNEE		11
#define L_FOOT		12

#define R_HIP		13
#define R_KNEE		14
#define R_FOOT		15
#define SKE_TOTAL	16

#define JOINTS_CNT	SKE_TOTAL	//@@duplicate!!!
#define PARMS_CNT	6





typedef struct skeParmS {
    char	*name;
    double	min, max;

    char	*def;
} skeParmT;

typedef struct skeValueS {
    unsigned	parm_id;
    double	constant;
} skeValueT;

typedef struct skeFrameS {
    struct skeFrameS	*next;
    double		duration;
    skeValueT		joint[JOINTS_CNT];
} skeFrameT;

class animationC {
    private:
	animationC*	Next;
	char*		Name;
	unsigned	FramesCnt;
	skeFrameT*	FirstFrame;

	void		Hang();

    public:
	animationC(char* name, char* text);
	static animationC* Get(char*);

	double		Status(double* parms, double time, int jID);
};




#endif
//EOF =========================================================================
