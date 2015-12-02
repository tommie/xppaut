#include "menus.h"

char *ic_hint[]={
  "Integrate over a range of parameters, init data, etc",
  "Integrate over range of 2 parameters,init data, etc",
  "Pick up from last step of previous solution",
  "Use current initial data",
  "Use current initial data",
  "Specify initial data with mouse",
  "Pick up from last step and shift T to latest time",
  "Input new initial data",
  "Use the initial data from last shooting from fixed pt",
  "Read init data from file",
  "Type in function of 't' for t^th equation",
  "Repeated ICs using mouse ","Guess new values for DAE",
  "Integrate backwards"
};

char *sing_hint[]={
  "Find fixed points over range of parameter",
  " ",
  "Use mouse to guess fixed point",
  "Monte carlo search for fixed points"};

char *meth_hint[]={
  "Discrete time -- difference equations",
  "Euler method",
  "Heun method -- 2nd order Euler",
  "4th order Runge-Kutta",
  "4th order predictor-corrector",
  "Gear's method -- for stiff systems",
  "Integrator for Volterra equations",
  "Implicit Euler scheme",
  "4th order Runge-Kutta with adaptive steps <- RECOMMENDED",
  "Another stiff method if Gear fails",
  "Stiff - bad for discontinuous systems",
  "Dormand-Prince5",
  "Dormand-Prince8(3)",
  "Rosenbrock(2,3) - good with discontinuties",
  "Symplectic - x''=F(x)"};

char *edrh_hint[]={
  "Edit right-hand sides and auxiliaries",
  "Edit function definitions",
  "Save current file with new defs",
  "Load external C right-hand sides"
};

char *auto_hint[]={
  "Tell AUTO the parameters you may vary",
  "What will be plotted on the axes and what parameter(s)",
  "Tell AUTO range, direction, and tolerance",
  "Run the continuation",
  "Grab a point to continue from",
  "Tell AUTO to save at values of period or parameter",
  "Erase the screen",
  "Redraw the diagram",
  "Save and output options"};

char *no_hint[]={
  " "," "," "," "," "," "," "," "," "," ", " "," "," "," "};

char *aaxes_hint[]={
  "Plot maximum of variable vs parameter",
  "Plot norm of solution vs parameter",
  "Plot max/min of variable vs parameter",
  "Plot period of orbit vs parameter",
  "Set up two-parameter bifurcation",
  "Zoom in with mouse",
  "Zoom out with mouse",
  "Recall last 1 parameter plot",
  "Recall last 2 parameter plot",
  "Let XPP determine the bounds of the plot",
  "Plot frequency vs parameter",
  "Plot average of orbit vs parameter",
  "Return to default bounds of the plot"
};

char *afile_hint[]={
  "Load a computed orbit into XPP",
  "Write diagram info to file for reuse",
  "Load previously saved file for restart",
  "Create postscript file of picture",
  "Create SVG file of picture",
  "Delete all points of diagram and associated files",
  "Clear grab point to allow start from new point",
  "Write the x-y values of the current diagram to file",
  "Write all the info for the whole diagram!",
  "Save initial data for whole diagram",
  "Toggle automatic redraw",
  "Range over a marked branch",
  "Select a point in 2 parameter diagram",
  "Draw orbits of labeled points automatically",
  "Put all data from branch into browser",
};

char *aspecial_hint[]={
  "Bifurcation or branch point",
  "Endpoint of a branch",
  "Hopf bifurcation point",
  "Limit point or turning point of a branch",
  "Failure to converge",
  "Period doubling bifurcation",
  "Torus bifurcation from a periodic",
  "User defined function",
};

char *arun_hint[]={
  "Start at fixed point",
  "Start at periodic orbit",
  "Start at solution to boundary value problem",
  "Use integral phase condn for homoclinics"
};

char *browse_hint[]={
  "Find closest data point to given value",
  "Scroll up",
  "Scroll down",
  "Scroll up a page",
  "Scroll down a page",
  "Scroll left",
  "Scroll right",
  "First plotted point",
  "Last plotted point",
  "Mark first point for plotting",
  "Mark last point for plotting",
  "Redraw data",
  "Write data to ascii file",
  "Load first line of BROWSER to initial data",
  "Replace column by formula",
  "Unreplace last replacement",
  "Write a column of data in tabular format",
  "Load data from a file into BROWSER",
  " ",
  "Add a new column to BROWSER",
  "Delete a column from BROWSER"
};
