/* Zero a portion of the dataset. 

Takes: [j1=1 j2=1 ... f1=0 f2=0 ... n1=n1 n2=n2 ... max1= max2= ... min1= min2= ...]

jN defines the jump in N-th dimension
fN is the window start
nN is the window size
minN and maxN is the maximum and minimum in N-th dimension

Reverse of window. */
/*
  Copyright (C) 2004 University of Texas at Austin
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdio.h>
#include <string.h>

#include <rsf.h>

static void seektable (int dim, int *n, int *m, int *f, int *j, 
		       int n1, int n2, int n3, size_t *table);

int main (int argc, char *argv[])
{
    size_t *table, nleft, nbuf;
    int i, esize, dim, n1, n2, n3, m1, i2, i1, j1, jump;
    int n[SF_MAX_DIM], m[SF_MAX_DIM], j[SF_MAX_DIM], f[SF_MAX_DIM];
    float a, d[SF_MAX_DIM], o[SF_MAX_DIM];
    char key[7], *buf, buf0[BUFSIZ], *zero;
    bool verb;
    sf_file in, out;

    sf_init (argc,argv);
    in = sf_input ("in");
    out = sf_output ("out");

    if (!sf_histint (in,"esize",&esize) || esize <= 0)
	sf_error("Need esize > 0 in in");

    dim = sf_filedims(in,n);

    for (i=0; i < dim; i++) {
	/* get o's */
	snprintf(key,3,"o%d",i+1);
	if (!sf_histfloat(in,key,o+i)) o[i]=0.;
	
        /* get d's */
	snprintf(key,3,"d%d",i+1);
	if (!sf_histfloat(in,key,d+i)) d[i]=1.;

	/* get j's */
	snprintf(key,3,"j%d",i+1);
	if (!sf_getint(key,j+i)) {
	    snprintf(key,3,"d%d",i+1);
	    if (sf_getfloat(key,&a)) {
		j[i] = 0.5 + a/d[i];
	    } else {
		j[i] = 1;
	    }
	} 

	/* get f's */	
	snprintf(key,3,"f%d",i+1);
	if (!sf_getint(key,f+i)) {
	    snprintf(key,5,"min%d",i+1);
	    if (sf_getfloat(key,&a)) {
		f[i] = 0.5 + (a - o[i]) / d[i];
	    } else {
		f[i] = 0;
	    }
	}
	if (f[i] < 0) sf_error("Negative f%d=%d",i+1,f[i]);

	/* new values for o and d */
	o[i] += f[i]*d[i]; 	
	d[i] *= j[i];

	/* get n's */
	snprintf(key,3,"n%d",i+1);
	if (!sf_getint(key,m+i)) { 
	    snprintf(key,5,"max%d",i+1);
	    if (sf_getfloat(key,&a)) {
		m[i] = 1.5 + (a - o[i]) / d[i];
	    } else {
		m[i] = 1.5 + (n[i] - 1 - f[i]) / j[i];
	    }
	}
	while (1+(m[i]-1)*j[i] > n[i]) m[i]--;
    }

    if (!sf_getbool("verb",&verb)) verb=false;
    /* Verbosity flag */

    if (verb) {
	for (i=0; i < dim; i++) {
	    if (m[i] != n[i]) 
		sf_warning("Cutting f%d=%d j%d=%d n%d=%d min%d=%g max%d=%g",
			   i+1,f[i],i+1,j[i],i+1,m[i],
			   i+1,o[i],i+1,o[i]+(m[i]-1)*d[i]);
	}
    }

    sf_fileflush(out,in);
    sf_setform(in,SF_NATIVE);
    sf_setform(out,SF_NATIVE);
    
    /* Now do the actual work */
    n2 = n3 = 1;
    for (i=1; i < dim; i++) {
	n2 *= m[i];
	n3 *= n[i];
    }

    m1 = m[0]*esize;
    n1 = (1+(m[0]-1)*j[0])*esize;
    jump = (j[0]-1) * esize;
    n[0] *= esize;
    f[0] *= esize;

    buf = sf_charalloc (n1);
    zero = sf_charalloc (n1);
    table = (size_t*) sf_alloc (n2+1,sizeof(size_t));
    memset(zero,0,n1);

    seektable(dim,n,m,f,j,n1,n2,n3,table);

    for (i2=0; i2 <= n2; i2++) {
	for (nleft=table[i2]; nleft > 0; nleft -= nbuf) {
	    nbuf = (BUFSIZ < nleft)? BUFSIZ: nleft;
	    sf_charread  (buf0,nbuf,in);
	    sf_charwrite (buf0,nbuf,out);
	}

	if (i2==n2) break;

	sf_charread(buf,n1,in);
	if (jump) {
	    for (i1=j1=0; i1 < m1; j1 += jump) {
		memset(buf+j1,0,esize);
		j1 += esize;
		i1 += esize;
	    }
	    sf_charwrite(buf,n1,out);
	} else {
	    sf_charwrite(zero,n1,out);
	}
    }
    
    exit (0);
}

static void seektable(int dim, int *n, int *m, int *f, int *j, 
		      int n1, int n2, int n3, size_t *table)
{
    size_t t, t2;
    int i2, i, ii[SF_MAX_DIM];

    t2 = sf_cart2line (dim-1, n+1, f+1);
    table[0] = (size_t) t2*n[0] + f[0];

    for (i2=1; i2 < n2; i2++) {
	t = i2;
	for (i = 1; i < dim-1; i++) {
	    /* cartesian coordinates in window */
	    ii[i] = t%m[i];
	    t /= m[i];
	}
	t = f[dim-1] + t*j[dim-1];
	for (i = dim-2; i >= 1; i--) {
	    /* line coordinates in input */
	    t = t*n[i] + f[i] + ii[i]*j[i];
	}
	table[i2] = (t-t2)*n[0]-n1; 
	t2 = t;
    }
    
    table[n2] = (n3-t2)*n[0]-f[0]-n1;
}

/* 	$Id: cut.c 691 2004-07-04 19:28:08Z fomels $	 */
