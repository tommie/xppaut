#include "my_rhs.h"

#include <stdlib.h>

#include "dae_fun.h"
#include "extra.h"
#include "form_ode.h"
#include "main.h"
#include "parserslow.h"
#include "simplenet.h"

static void vec_rhs(double t, double *y, double *ydot, int neq);

int main(argc,argv)
     char **argv;
     int argc;
{
  do_main(argc,argv);

  exit(0);
}

void extra(y__y, t,nod,neq)
 double *y__y,t;
 int nod,neq;
 {
  int i;
  if(nod>=neq)return;
  SETVAR(0,t);
  for(i=0;i<nod;i++)
  SETVAR(i+1,y__y[i]);
  for(i=nod+FIX_VAR;i<nod+FIX_VAR+NMarkov;i++)SETVAR(i+1,y__y[i-FIX_VAR]);
  for(i=nod;i<nod+FIX_VAR;i++)
  SETVAR(i+1,evaluate(my_ode[i]));
  do_in_out();
  for(i=nod+NMarkov;i<neq;i++)
  y__y[i]=evaluate(my_ode[i+FIX_VAR-NMarkov]);
}

/* set_fix_rhs(t,y,neq)
     int neq;
     double t,*y;
{
  int i;
  SETVAR(0,t);
  for(i=0;i<neq;i++)
    SETVAR(i+1,y[i]);
  for(i=neq;i<neq+FIX_VAR;i++)
    SETVAR(i+1,evaluate(my_ode[i]));
  eval_all_nets();
  do_in_out();
  } */
void set_fix_rhs(t,y)
     double t,*y;
{
  int i;
  SETVAR(0,t);
  for(i=0;i<NODE;i++)
    SETVAR(i+1,y[i]);
  for(i=0;i<NMarkov;i++)
    SETVAR(i+1+NODE+FIX_VAR,y[i+NODE]);
  for(i=NODE;i<NODE+FIX_VAR;i++)
    SETVAR(i+1,evaluate(my_ode[i]));
  eval_all_nets();
  do_in_out();
}


int my_rhs( t,y,ydot,neq)
double t,*y,*ydot;
int neq;
{
  int i;
  SETVAR(0,t);
  for(i=0;i<NODE;i++)
  SETVAR(i+1,y[i]);

  for(i=NODE;i<NODE+FIX_VAR;i++){
  SETVAR(i+1,evaluate(my_ode[i]));
  /* plintf("%d %g \n",i+1,GETVAR(i+1)); */
  }
    /*printf("WTF %g\n",evaluate(my_ode[1]));
    */
    eval_all_nets();

    do_daes();
    do_in_out();
 for(i=0;i<NODE;i++)
  {
    ydot[i]=evaluate(my_ode[i]);
  }
 if(neq>NODE)vec_rhs(t,y,ydot,neq);

 return(1);
}

void update_based_on_current()
{
  int i;
   for(i=NODE;i<NODE+FIX_VAR;i++)
    SETVAR(i+1,evaluate(my_ode[i]));

  eval_all_nets();
  do_in_out();
}

void fix_only()
{
   int i;
  for(i=NODE;i<NODE+FIX_VAR;i++)
    SETVAR(i+1,evaluate(my_ode[i]));

}

void rhs_only(double *y,double *ydot)
{
  int i;
  for(i=0;i<NODE;i++){
    ydot[i]=evaluate(my_ode[i]);
  }
}

static void vec_rhs( t,y,ydot,neq)
double t,*y,*ydot;
int neq;
{


}
