/*
 * Median: find the median of an array (various implementations)
 *
 *    see also discussions in: 
 *    http://ndevilla.free.fr/median/median/index.html
 *
 *	20-jun-01	gcc3
 *       6-aug-2012     added median_torben
 *      15-jun-2013     added median_torben_info
 */

#include <stdinc.h>

// variables needed for the pmedian* functions that used a pointer array
static int nix=0;
static int *ix=NULL;

static int last_n = 0;
static real *last_x = NULL;

extern     void sortptr(real *x, int *ix, int n);

// sorted array versions 

real smedian(int n, real *x)
{
    if (n <= 0) error("median: n=%d",n);
    if (x == 0) error("median: x=NULL");

    if (n % 2)
        return x[(n-1)/2];
    else
        return  0.5*(x[n/2] + x[n/2-1]);
}

real smedian_q1(int n, real *x)
{
  int n1;
  if (n>4) {
    n1=(n+1)/4;
    return x[n1];
  } else
    error("smedian_q1: too few points");

}

real smedian_q3(int n, real *x)
{
  int n3;
  if (n>4) {
    n3=((n+1)*3)/4;
    return x[n3];
  } else
    error("smedian_q1: too few points");

}


// pointer array versions


real pmedian(int n, real *x)
{
    if (n <= 0) error("median: n=%d",n);
    if (x == 0) error("median: x=NULL");

    last_n = n;
    last_x = x;

    if (nix==0) {                       /* first time allocate */
        nix = n;
        ix = (int *) allocate(sizeof(int)*nix);
	dprintf(1,"MEDIAN alloc %d\n",nix);
    } else if (nix < n) {               /* re-allocation */
        nix = n;
        ix = (int *) reallocate(ix, sizeof(int)*nix);
	dprintf(1,"MEDIAN re-alloc %d\n",nix);
    }
    sortptr(x,ix,n);
    if (n % 2)
        return x[ix[(n-1)/2]];
    else
        return  0.5*(x[ix[n/2]] + x[ix[n/2-1]]);
}

/* these quartiles depend on a previous call to median, we
   are reusing the ix[] pointing array
   this code will thus not run in MP mode
*/

real pmedian_q1(int n, real *x)
{
  int n1;
  if (last_n==0) error("No previous median()");
  if (last_n != n || last_x != x) error("Bad previous median() call");
  if (n>4) {
    n1=(n+1)/4;
    return(x[ix[n1]]);
  } else
    error("median_q1: too few points");

}
real pmedian_q3(int n, real *x)
{
  int n3;
  if (last_n==0) error("No previous median()");
  if (last_n != n || last_x != x) error("Bad previous median() call");
  if (n>4) {
    n3=((n+1)*3)/4;
    return(x[ix[n3]]);
  } else
    error("median_q1: too few points");

}


// future ?
 
void init_median(int size)
{
    nix = size;
    ix = (int *) allocate(sizeof(real)*nix);
}

void finis_median(void)
{
    free(ix);
    ix = 0;
    nix = 0;
    last_n = 0;
    last_x = NULL;
}


/*
 *
 */


real median_torben_info(int n, real *x, real xmin, real xmax, real eps,  int *niter)
{
  real guess, max_lt_guess, min_gt_guess;
  int i, k=0, lt_count, gt_count, eq_count;
  int n2 = (n+1)/2;
  
  while(++k) {           /* iterate, and keep a convenience counter */
    guess = 0.5*(xmin+xmax);
    lt_count = gt_count = eq_count = 0;
    max_lt_guess = xmin;
    min_gt_guess = xmax;
    for (i=0; i<n; i++) {
      if (x[i] < guess) {
	lt_count++;
	if (x[i] > max_lt_guess) max_lt_guess = x[i];
      } else if (x[i] > guess) {
	gt_count++;
	if (x[i] < min_gt_guess) min_gt_guess = x[i];
      } else
	eq_count++;
    }
    if (lt_count <= n2 && gt_count <= n2)
      break;
    else if (lt_count > gt_count)
      xmax = max_lt_guess;
    else
      xmin = min_gt_guess;
    /* early exit if eps was specified */
    if (eps>0 && xmax-xmin<eps) {
      *niter = k;
      return 0.5*(xmax+xmin); /* good enough gamble */
    }
  }
  dprintf(1,"median_torben: iter k=%d n=%d\n",k,n);
  *niter = k;
  if (lt_count >= n2)
    return max_lt_guess;
  else if (lt_count + eq_count >= n2)
    return guess;
  else
    return min_gt_guess;
  /* will never get here */
  *niter = -1;
  return guess;  
}

real median_torben(int n, real *x, real xmin, real xmax)
{
  int niter;
  return median_torben_info(n,x,xmin,xmax,-1.0, &niter);
  
}

/*
 * wirth's algorithm
 * 
 * cf. NR's select() pp342.
 */

#define ELEM_SWAP(a,b) { register real t=(a);(a)=(b);(b)=t; }

real kth_smallest(int n, real  *x,  int k)
{
  register int i,j,l,m;
  register real xi;

  l=0 ; m=n-1 ;
  while (l<m) {
    xi=x[k] ;
    i=l ;
    j=m ;
    do {
      while (x[i]<xi) i++ ;
      while (xi<x[j]) j-- ;
      if (i<=j) {
	ELEM_SWAP(x[i],x[j]) ;
	i++ ; j-- ;
      }
    } while (i<=j) ;
    if (j<k) l=i ;
    if (k<i) m=j ;
  }
  return x[k] ;
}


real median_wirth(int n, real  *x) 
{
  if (n&1) 
    return kth_smallest(n, x,   n/2);      /* odd, picks the middle */
  else
    return kth_smallest(n, x,   n/2 - 1);  /* even, a slight cheat */
  /* NEVER REACHED */
  return x[0];
}
