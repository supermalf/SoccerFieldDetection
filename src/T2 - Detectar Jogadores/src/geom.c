/* *************************************************************
*
* Class:  geom.c
* Created: 21/4/2006 - 10:35
* Author: malf
* $Id: $
*
* Description: 
* 
*
* $Log: $
*
* *************************************************************/

#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "geom.h"
#include "utils.h"

static float s_geom_dummy = (float)-1.0e30;

/************************************************************************
 *                                                                     
 ************************************************************************/
float GeomSetDummy(float geom_dummy)
{
 s_geom_dummy = geom_dummy;
 return s_geom_dummy;
}

/************************************************************************
 *                                                                     
 ************************************************************************/
float GeomGetDummy(void)
{
 return s_geom_dummy;
}

/************************************************************************
 *                                                                     
 ************************************************************************/
static GeomPoint2D _GeomJarvisNext (int npP, GeomPoint3D *P, GeomPoint2D p, GeomPoint2D v)
{
 GeomPoint2D p2d;
 double max=-99999, d1=99999, dist, ang;
 double ax, ay;
 int p_aux, i;
 for (i=0; i<npP; i++) {
  ax = P[i].x - p.x;
  ay = P[i].y - p.y;
  if (((dist = ax*ax + ay*ay) > GEOM_ABSTOL) && 
      ((ang = (ax*v.x + ay*v.y)/sqrt(dist))>max || (dist>d1 && fabs(ang-max)<GEOM_ABSTOL))) {
   max=ang;
   d1=dist;
   p_aux=i;
  }
 }
 p2d.x = P[p_aux].x;
 p2d.y = P[p_aux].y;
 return p2d;
}

/************************************************************************
 *                                                                     
 ************************************************************************/
static void _GeomJarvis(int npP, GeomPoint3D *P, GeomPol2D *pol)
{
 int i;
 GeomPoint2D v, v0, v1, vaux, dir;
 v0.x = P[0].x;
 v0.y = P[0].y;
 for (i=0; i<npP; i++)
  if (P[i].y < v0.y) {
   v0.x = P[i].x;
   v0.y = P[i].y;
  }
 pol->pol[0] = v0;
 pol->np = 1;
 v = v0;
 dir.x = 1; dir.y = 0;
 v1 = _GeomJarvisNext(npP, P, v, dir);
 pol->pol[1] = v1;
 pol->np = 2;
 dir.x = v1.x - v0.x;
 dir.y = v1.y - v0.y;
 vaux = _GeomJarvisNext(npP, P, v1, dir);
 while ((v.x-vaux.x)*(v.x-vaux.x) + (v.y-vaux.y)*(v.y-vaux.y) > GEOM_ABSTOL) {
  pol->pol[pol->np] = vaux;
  pol->np++;
  dir.x = vaux.x - v1.x;
  dir.y = vaux.y - v1.y;
  v1 = vaux;
  vaux = _GeomJarvisNext(npP, P, v1, dir);
 }
}

/************************************************************************
 *                                                                     
 ************************************************************************/
GeomPol2D* GeomGetOutline(int npP, GeomPoint3D *P)
{
 int i;
 GeomPol2D* outline = (GeomPol2D *)malloc(sizeof(GeomPol2D));
 outline->np = 0;
 outline->pol = (GeomPoint2D *)malloc(npP*sizeof(GeomPoint2D));
 _GeomJarvis(npP, P, outline);
 outline->pol[outline->np] = outline->pol[0];
 outline->np++;
 outline->min = outline->max = outline->pol[0];
 for (i=1; i<outline->np; i++) {
  if      (outline->pol[i].x < outline->min.x) outline->min.x = outline->pol[i].x;
  else if (outline->pol[i].x > outline->max.x) outline->max.x = outline->pol[i].x;
  if      (outline->pol[i].y < outline->min.y) outline->min.y = outline->pol[i].y;
  else if (outline->pol[i].y > outline->max.y) outline->max.y = outline->pol[i].y;
 }
 return outline;
}

/************************************************************************
 *                                                                     
 ************************************************************************/
int GeomIn(GeomPol2D *P, GeomPoint2D p)
{
 double vetx, vety;
 double ang1, ang2, angulo;
 GeomPoint2D *ppol;
 double px, py;
 int i;

 if (!P->fechado) 
   return 0;

 if ((p.x < P->min.x) || (p.y < P->min.y) || (p.x > P->max.x) || (p.y > P->max.y))
  return 0;

 for (ppol=P->pol, i=0; i<P->np; i++, ppol++)
 {
  if (ABS(ppol->x-p.x)<GEOM_ABSTOL && ABS(ppol->y-p.y)<GEOM_ABSTOL) 
    return 1;
 }

 ppol = P->pol;
 px = p.x; py = p.y;
 vetx = ppol->x - px;
 vety = ppol->y - py;
 ang2 = geomPAngle(vetx,vety);
 for (angulo=0, i=1, ppol++; i<P->np; i++, ppol++) {
  ang1 = ang2;
  vetx = ppol->x - px;
  vety = ppol->y - py;
  ang2 = geomPAngle(vetx,vety);
  geomAngle(angulo,ang1,ang2);
 }
 ang1 = ang2;
 vetx = P->pol[0].x - p.x;
 vety = P->pol[0].y - p.y;
 ang2 = geomPAngle(vetx,vety);
 geomAngle(angulo,ang1,ang2);

 return (ABS(angulo) > GEOM_ABSTOL);
}

/************************************************************************
 *                                                                     
 ************************************************************************/
int GeomInClosePol(GeomPol2D *P, GeomPoint2D p)
{
 double vetx, vety;
 double ang1, ang2, angulo;
 GeomPoint2D *ppol;
 double px, py;
 int i;

 if ((p.x < P->min.x) || (p.y < P->min.y) || (p.x > P->max.x) || (p.y > P->max.y))
  return 0;

 for (ppol=P->pol, i=0; i<P->np; i++, ppol++)
  if (ABS(ppol->x-p.x)<GEOM_ABSTOL && ABS(ppol->y-p.y)<GEOM_ABSTOL) return 1;

 ppol = P->pol;
 px = p.x; py = p.y;
 vetx = ppol->x - px;
 vety = ppol->y - py;
 ang2 = geomPAngle(vetx,vety);
 for (angulo=0, i=1, ppol++; i<P->np; i++, ppol++) {
  ang1 = ang2;
  vetx = ppol->x - px;
  vety = ppol->y - py;
  ang2 = geomPAngle(vetx,vety);
  geomAngle(angulo,ang1,ang2);
 }

 return (ABS(angulo) > GEOM_ABSTOL);
}

/************************************************************************
 *                                                                     
 ************************************************************************/
void GeomSimplifyPts(int npPIn, GeomPoint3D *PIn,
                     double ox, double oy, int nx, int ny, double cx, double cy,
                     int *npPOut, GeomPoint3D **POut)
{
 GeomPoint3D *PAux, *PAux2;
 int *npAux;
 int i, j, k, pos;
 double icx, icy;

 if (npPIn==0 || PIn==NULL) {
  *npPOut = 0;
  *POut = NULL;
  return;
 }

 PAux  = (GeomPoint3D *)malloc(nx*ny*sizeof(GeomPoint3D));
 npAux = (int *)malloc(nx*ny*sizeof(int));

 for (k=0; k<nx*ny; k++) {
  PAux[k].x = PAux[k].y = PAux[k].z = 0.0;
  npAux[k] = 0;
 }

 icx = 1.0/cx; icy = 1.0/cy;
 for (k=0; k<npPIn; k++) {
  i = (int)((PIn[k].x - ox)*icx + 0.5);
  if (i>=0 && i<nx) {
   j = (int)((PIn[k].y - oy)*icy + 0.5);
   if (j>=0 && j<ny) {
    pos = i*ny+j; 
    PAux[pos].x += PIn[k].x;
    PAux[pos].y += PIn[k].y;
    PAux[pos].z += PIn[k].z;
    npAux[pos]++;
   }
  }
 }

 *npPOut = 0;
 for (k=0; k<nx*ny; k++) {
  if (npAux[k]) {
   PAux[k].x = PAux[k].x/((double)npAux[k]);
   PAux[k].y = PAux[k].y/((double)npAux[k]);
   PAux[k].z = PAux[k].z/((float)npAux[k]);
   (*npPOut)++;
  }
 }

 PAux2 = (GeomPoint3D *)malloc((*npPOut)*sizeof(GeomPoint3D));
 for (i=0, k=0; k<nx*ny; k++)
  if (npAux[k]) {
   PAux2[i] = PAux[k];
   i++;
  }

 free(PAux);
 free(npAux);

 *POut = PAux2;
}

/* Gera um grid primário a partir da média ponderada pelo inverso da distância dos pontos próximos. */
/************************************************************************
 *                                                                     
 ************************************************************************/
void GeomCreateGrid1(int npP, GeomPoint3D *P,
                     double ox, double oy, int nx, int ny, double cx, double cy, float *zaux, int *naux)
{
 int i, j, k, index;
 GeomPoint2D g;
 float p;
 float *paux;
 double icx, icy;

 paux = (float *)malloc(nx*ny*sizeof(float));
 for (i=0; i<nx*ny; i++) {
  paux[i] = 0.0;
  naux[i] = 0;
 }

 icx = 1.0/cx; icy = 1.0/cy;
 for (k=0; k<npP; k++) {
  i = (int)((P[k].x - ox)*icx + 0.5);
  if (i>=0 && i<nx) {
   j = (int)((P[k].y - oy)*icy + 0.5);
   if (j>=0 && j<ny) {
    g.x = ox + i*cx;
    g.y = oy + j*cy;
    p = (float)(1.0/(1.0+geomDistance(g, P[k])));
    index = i*ny+j;
    if (!naux[index]) {
     zaux[index] = P[k].z*p;
     naux[index] = 1;
    } else zaux[index] += P[k].z*p;
    paux[index] += p;
   }
  }
 }

 for (k=0; k<nx*ny; k++)
  if (naux[k]) zaux[k] /= paux[k];
  else         zaux[k] = s_geom_dummy;

 free(paux);
}

/* Gera um grid primário a partir dos pontos mais próximos à cada nó do grid. */
/************************************************************************
 *                                                                     
 ************************************************************************/
void GeomCreateGrid2(int npP, GeomPoint3D *P,
                     double ox, double oy, int nx, int ny, double cx, double cy, float *zaux, int *naux)
{
 int i, j, k, index;
 GeomPoint2D g;
 double dist;
 double *daux;
 double icx, icy;

 daux = (double *)malloc(nx*ny*sizeof(double));
 for (i=0; i<nx*ny; i++) {
  daux[i] = DBL_MAX;
  naux[i] = 0;
 }

 icx = 1.0/cx; icy = 1.0/cy;
 for (k=0; k<npP; k++) {
  i = (int)((P[k].x - ox)*icx + 0.5);
  if (i>=0 && i<nx) {
   j = (int)((P[k].y - oy)*icy + 0.5);
   if (j>=0 && j<ny) {
    g.x = ox + i*cx;
    g.y = oy + j*cy;
    dist = geomDistance2(g, P[k]);
    index = i*ny+j;
    if (dist < daux[index]) {
     daux[index] = dist;
     zaux[index] = P[k].z;
     naux[index] = 1;
    }
   }
  }
 }

 free(daux);
}

/* Gera um grid primário a partir dos pontos mais próximos à cada nó do grid. */
/************************************************************************
 *                                                                     
 ************************************************************************/
void GeomCreateGrid2Near(int npP, GeomPoint3D *P,
                         double ox, double oy, int nx, int ny, double cx, double cy, float *zaux, int *naux,
                         double distprox)
{
 int i, j, k;
 GeomPoint2D g;
 double d, dmin;
 float *pz = zaux;
 int *pn = naux;
 double distprox2;

 distprox2 = distprox*distprox;
 for (i=0, g.x=ox; i<nx; i++, g.x+=cx)
  for (j=0, g.y=oy; j<ny; j++, g.y+=cy, pz++, pn++) {
   for (dmin = DBL_MAX, k=0; k<npP; k++)
    if ((d = geomDistance2(P[k],g)) < dmin) {
     dmin = d;
     *pz = P[k].z;
     if (dmin < GEOM_ABSTOL) break;
    }
   *pn = (dmin <= distprox2);
  }
}

/* Gera um grid primário a partir dos pontos mais próximos à cada nó do grid. */
/************************************************************************
 *                                                                     
 ************************************************************************/
void GeomCreateGrid2Outline(int npP, GeomPoint3D *P, GeomPol2D* outline,
                            double ox, double oy, int nx, int ny, double cx, double cy, float *zaux, int *naux,
                            double distprox)
{
 int i, j, k;
 GeomPoint2D g;
 double d, dmin;
 float *pz = zaux;
 int *pn = naux;
 double distprox2;

 distprox2 = distprox*distprox;
 for (i=0, g.x=ox; i<nx; i++, g.x+=cx)
  for (j=0, g.y=oy; j<ny; j++, g.y+=cy, pz++, pn++)
   if (outline==NULL || GeomInClosePol(outline, g)) {
    for (dmin = DBL_MAX, k=0; k<npP; k++)
     if ((d = geomDistance2(P[k],g)) < dmin) {
      dmin = d;
      *pz = P[k].z;
      if (dmin < GEOM_ABSTOL) break;
     }
    *pn = (dmin <= distprox2);
   } else {
    *pn = 0;
    *pz = s_geom_dummy;
   }
}

/************************************************************************
 *                                                                     
 ************************************************************************/
static int _GeomLinesIntersect(double Ax, double Ay, double x1lo, double x1hi, double y1lo, double y1hi,
                              GeomPoint2D l1p1, GeomPoint2D l2p1, GeomPoint2D l2p2)
{
 double Bx, By, Cx, Cy;
 double d, e, f, t;

 Bx = l2p1.x - l2p2.x;
 if (Bx>0) {
  if(x1hi+GEOM_ABSTOL<l2p2.x || l2p1.x<x1lo-GEOM_ABSTOL) return 0;
 } else
  if(x1hi+GEOM_ABSTOL<l2p1.x || l2p2.x<x1lo-GEOM_ABSTOL) return 0;

 By = l2p1.y - l2p2.y;
 if(By>0) {
  if(y1hi+GEOM_ABSTOL<l2p2.y || l2p1.y<y1lo-GEOM_ABSTOL) return 0;
 } else
  if(y1hi+GEOM_ABSTOL<l2p1.y || l2p2.y<y1lo-GEOM_ABSTOL) return 0;

 f = Ay*Bx - Ax*By;
 if (ABS(f)<GEOM_ABSTOL) return 0;

 Cx = l1p1.x - l2p1.x;
 Cy = l1p1.y - l2p1.y;
 d = By*Cx - Bx*Cy;
 if (ABS(d) < GEOM_ABSTOL) return 0;
 t = d/f; /* l1p1 <-> l1p2 */
 if (t<GEOM_ABSTOL || t>1-GEOM_ABSTOL) return 0;

 e = Ax*Cy - Ay*Cx;
 if (ABS(e) < GEOM_ABSTOL) return 0;
 t = e/f; /* l2p1 <-> l2p2 */
 return !(t<0 || t>1);
}

/************************************************************************
 *                                                                     
 ************************************************************************/
static int _GeomLinesIntersectX(double Ax, double Ay, double x1lo, double x1hi, double y1lo, double y1hi,
                                GeomPoint2D l1p1, GeomPoint2D l2p1, GeomPoint2D l2p2)
{
 double By, Cx, Cy;
 double d, e, f, t;

 if(x1hi+GEOM_ABSTOL<l2p1.x || l2p2.x<x1lo-GEOM_ABSTOL) return 0;

 By = l2p1.y - l2p2.y;
 if(By>0) {
  if(y1hi+GEOM_ABSTOL<l2p2.y || l2p1.y<y1lo-GEOM_ABSTOL) return 0;
 } else
  if(y1hi+GEOM_ABSTOL<l2p1.y || l2p2.y<y1lo-GEOM_ABSTOL) return 0;

 f = -Ax*By;
 if (ABS(f)<GEOM_ABSTOL) return 0;

 Cx = l1p1.x - l2p1.x;
 Cy = l1p1.y - l2p1.y;
 d = By*Cx;
 if (ABS(d) < GEOM_ABSTOL) return 0;
 t = d/f; /* l1p1 <-> l1p2 */
 if (t<GEOM_ABSTOL || t>1-GEOM_ABSTOL) return 0;

 e = Ax*Cy - Ay*Cx;
 if (ABS(e) < GEOM_ABSTOL) return 0;
 t = e/f; /* l2p1 <-> l2p2 */
 return !(t<0 || t>1);
}

/************************************************************************
 *                                                                     
 ************************************************************************/
static int _GeomLinesIntersectY(double Ax, double Ay, double x1lo, double x1hi, double y1lo, double y1hi,
                                GeomPoint2D l1p1, GeomPoint2D l2p1, GeomPoint2D l2p2)
{
 double Bx, Cx, Cy;
 double d, e, f, t;

 Bx = l2p1.x - l2p2.x;
 if (Bx>0) {
  if(x1hi+GEOM_ABSTOL<l2p2.x || l2p1.x<x1lo-GEOM_ABSTOL) return 0;
 } else
  if(x1hi+GEOM_ABSTOL<l2p1.x || l2p2.x<x1lo-GEOM_ABSTOL) return 0;

 if(y1hi+GEOM_ABSTOL<l2p1.y || l2p2.y<y1lo-GEOM_ABSTOL) return 0;

 f = Ay*Bx;
 if (ABS(f)<GEOM_ABSTOL) return 0;

 Cx = l1p1.x - l2p1.x;
 Cy = l1p1.y - l2p1.y;
 d = -Bx*Cy;
 if (ABS(d) < GEOM_ABSTOL) return 0;
 t = d/f; /* l1p1 <-> l1p2 */
 if (t<GEOM_ABSTOL || t>1-GEOM_ABSTOL) return 0;

 e = Ax*Cy - Ay*Cx;
 if (ABS(e) < GEOM_ABSTOL) return 0;
 t = e/f; /* l2p1 <-> l2p2 */
 return !(t<0 || t>1);
}

#define area(a,b,c) ((b.x-a.x)*(c.y-a.y)-(b.y-a.y)*(c.x-a.x))

/************************************************************************
 *                                                                     
 ************************************************************************/
int GeomCross(GeomPoint2D x0, GeomPoint3D X, int npol, GeomPol2D *P)
{
 double xlo, xhi;
 double ylo, yhi;
 double Ax, Ay;
 GeomPoint2D p1, p2, p3, p4;
 int i, j;
 GeomPol2D *pP;
 GeomPoint2D *pp;

 Ax = X.x - x0.x;
 if (Ax<0) {xlo=X.x; xhi=x0.x;}
 else      {xhi=X.x; xlo=x0.x;}
 Ay = X.y - x0.y;
 if (Ay<0) {ylo=X.y; yhi=x0.y;}
 else      {yhi=X.y; ylo=x0.y;}

 for (pP=P, i=0; i<npol; pP++, i++) {
  p1.x = pP->min.x; p1.y = pP->min.y;
  p2.x = p1.x;      p2.y = pP->max.y;
  p3.x = pP->max.x; p3.y = p2.y;
  p4.x = p3.x;      p4.y = p1.y;
  if (xhi<p1.x || xlo>p3.x || yhi<p1.y || ylo>p3.y)
   continue;
  if ((X.x>=p1.x && X.x<=p3.x && X.y>=p1.y && X.y<=p3.y) ||
      _GeomLinesIntersectX(Ax, Ay, xlo, xhi, ylo, yhi, x0, p1, p2) || _GeomLinesIntersectY(Ax, Ay, xlo, xhi, ylo, yhi, x0, p2, p3) || 
      _GeomLinesIntersectX(Ax, Ay, xlo, xhi, ylo, yhi, x0, p3, p4) || _GeomLinesIntersectY(Ax, Ay, xlo, xhi, ylo, yhi, x0, p4, p1))
   for (pp=pP->pol, j=1; j<pP->np; j++, pp++)
    if (!pP->fechado || (pP->fechado && area((*pp),  (*(pp+1)), x0) > 0))
     if (_GeomLinesIntersect(Ax, Ay, xlo, xhi, ylo, yhi, x0, *pp,  *(pp+1)))
      return 1;
 }
 return 0;
}

/************************************************************************
 *                                                                     
 ************************************************************************/
double GeomAreaPol(GeomPol2D pol)
{
 int i, j;
 double area = 0;
 int np = pol.np;
 GeomPoint2D *p = pol.pol;
 for (i=0;i<np;i++) {
  j = (i + 1) % np;
  area += p[i].x * p[j].y;
  area -= p[i].y * p[j].x;
 }
 area *= 0.5;
 return ABS(area);
}
