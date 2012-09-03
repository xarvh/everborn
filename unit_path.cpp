/*
 UNIT_PATH.CPP		v0.03
 Pathfinder.

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
#include "unit.h"
#include "map.h"
#include "main.h"





/*=============================================================================
 * A* NODE
 */
nodeC::nodeC()
{
 Next = Parent = 0;
 Opened = Closed = false;
}



/*=============================================================================
 * A* GRID
 */
gridC::gridC(int w, int h)
{
 OpenedList = ClosedList = 0;

 W = w;
 H = h;
 MaxClosed = W+H;

 Grid = new nodeC[w*h];
 for(int y = 0; y < H; y++) for(int x = 0; x < W; x++)
 {
    Node(x,y)->X = x;
    Node(x,y)->Y = y;
 }

 Path = new char[w*h];
 PathLength = 0;
}

gridC::~gridC()
{
 delete[] Grid;
 delete[] Path;
}



nodeC* gridC::Node(signed x, signed y)
{
 while(x < 0) x += W;
 while(y < 0) y += H;
 return Grid + (x % W) + (y % H) * W;
}

nodeC* gridC::NodeSmac(nodeC* node, int direction)
{
 switch(direction) {
    case 0: return Node(node->X+0, node->Y+1);
    case 1: return Node(node->X+1, node->Y+1);
    case 2: return Node(node->X+1, node->Y+0);
    case 3: return Node(node->X+1, node->Y-1);
    case 4: return Node(node->X+0, node->Y-1);
    case 5: return Node(node->X-1, node->Y-1);
    case 6: return Node(node->X-1, node->Y+0);
    case 7: return Node(node->X-1, node->Y+1);
    default: return 0;
 }
}



/*=============================================================================
 */
void gridC::Close(nodeC* n)
{
 nodeC** h;

 // remove from OpenedList
 for(h = &OpenedList; *h != n; h = &(*h)->Next);
 *h = n->Next;
 n->Next = 0;

 // put in Closed List
 n->Next = ClosedList;
 ClosedList = n;

 n->Closed = true;
}





/*=============================================================================
 */
void gridC::Open(nodeC* n, nodeC* parent, int direction, nodeC* end)
{
 int incoming_direction = (direction+4) & 7;	// reverse direction!
 int cost = Cost(n->X, n->Y, incoming_direction);

 // staring cell MAY not be accessible
 if(parent && cost == 0) return; // cannot be accessed

 if(parent) cost += parent->Cost;

 if(n->Opened)
 {
    if(n->Cost <= cost) return;
 }
 else
 {
    n->Opened = true;
    n->Heu = Heu(n->X, n->Y, end->X, end->Y);

    // push into OpenedList
    n->Next = OpenedList;
    OpenedList = n;
 }

 n->Cost = cost;
 n->Parent = parent;
 n->ParentDirection = direction;
}


/*
static void cleanlist(nodeC** h)
{
 for(nodeC* n = *h; n; )
 {
    nodeC* o = n;
    n = n->Next;
    o->Opened = o->Closed = false;
    o->Next = 0;
 }

 *h = 0;
}
*/

/*=============================================================================
 * PATHFIND
 *
 * Builds the path.
 * Returns heuristic estimate of end (best) node.
 */
static nodeC* findBestNodeIn(nodeC* list)
{
 nodeC* best = 0;
 for(nodeC* n = list; n; n = n->Next)
    if(!best || n->Heu + n->Cost < best->Heu + best->Cost)
        best = n;
 return best;
}

static nodeC* findClosestNode(gridC& g)
{
 nodeC* best = 0;

 for(nodeC* n = g.ClosedList; n; n = n->Next)
    if(!best || n->Heu < best->Heu) best = n;
 for(nodeC* n = g.OpenedList; n; n = n->Next)
    if(!best || n->Heu < best->Heu) best = n;

 return best;
}

float gridC::Pathfind(
    int sx,
    int sy,
    int ex,
    int ey,
    float targ_heu,
    costFunctionT cf, heuFunctionT hf)
{
 nodeC* start = Node(sx, sy);
 nodeC* end = Node(ex, ey);
 Cost = cf;
 Heu = hf;

 int closedCnt = 0;

 // clean lists
/*
 cleanlist(&OpenedList);
 cleanlist(&ClosedList);
*/
/* for(nodeC* n = OpenedList; n; n = n->Next) n->Opened = n->Closed = false;
 for(nodeC* n = ClosedList; n; n = n->Next) n->Opened = n->Closed = false;
*/
 //@@ once pathfind is stable, clean this mess and maybe use cleanlist() above
 for(nodeC* n = Grid; n-Grid < W*H; n++)
 {
    n->Closed = n->Opened = false;
    n->Next = n->Parent = 0;
    n->DBBest = -1;
 }
 OpenedList = 0;
 ClosedList = 0;


 // open start node
 Open(start, 0, 0, end);

 nodeC* node = start;
 while(	(node = findBestNodeIn(OpenedList))
    &&	node->Heu > targ_heu
    &&	closedCnt < MaxClosed
    )
 {
    Close(node);    // close node
    closedCnt++;

    // open neighbouring nodes
    nodeC* n;
    for(int d = 0; n = NodeSmac(node, d); d++)
	if(!n->Closed) Open(n, node, d, end);
 }



 // OpenedList is empty ==> !node
// pf("start %d %d dest %d %d\n", start->X, start->Y, end->X, end->Y);
 if(node && node->Heu <= targ_heu) end = node;
 else
 {
    end = findClosestNode(*this);
//    pf("best %d %d (heu %d cost %d)\n", end->X, end->Y, end->Heu, end->Cost);

    // avoid useless movments
    // prevents units from bouncing back and forth when close to end
    if(start->Heu - end->Heu < .5*Heu(0, 1, 0, 0))
	return PathLength = 0;
 }





 // calculate path length
 PathLength = 0;
 nodeC* n;
 for(n = end; n && n != start; n = n->Parent) PathLength++;

 // write path
 node = end;
 for(int l = PathLength-1; l >= 0; l--)
 {
    Path[l] = node->ParentDirection;
    node->DBBest = Path[l];
    node = node->Parent;
 }

 return end->Heu / Heu(0, 1, 0, 0);
}





//EOF =========================================================================
