#include "menus.h"

char *num_menu[]={
  "Total","Start time","tRansient",
  "Dt","Ncline ctrl","sIng pt ctrl","nOutput","Bounds","Method",
  "dElay","Color code","stocHast","Poincare map","rUelle plot",
  "looKup","bndVal","Averaging"};

char *fileon_menu[]={
  "Prt src","Write set","Read set",
  "Auto","Calculator","Edit","Save info",
  "Bell off","Help","Quit","Transpose","tIps","Get par set","cLone",".Xpprc","tUtorial"};

char *fileoff_menu[]={
  "Prt src","Write set","Read set",
  "Auto","Calculator","Edit","Save info",
  "Bell on","Help","Quit","Transpose","tIps","Get par set","cLone",".Xpprc","tUtorial"};


char *file_hint[]={
  "Display source and active comments",
  "Save information for restart",
  "Read information for restart",
  "Run AUTO, the bifurcation package",
  "A little calculator -- press ESC to exit",
  "Edit right-hand sides or functions or auxiliaries",
  "Save info about simulation in human readable format",
  "Turn bell on/off",
  "Browser help",
  "Duh!",
  "Transpose storage",
  "Turn off these silly tips",
  "Set predefined parameters",
  "Clone the ode file",
  "Edit your .xpprc preferences file",
  "Run a quick tutorial on XPPAUT"
};


char *num_hint[]={
  "Total time to integrate eqns",
  "Starting time -- T0",
  "Time to integrate before storing",
  "Time step to use",
  "Mesh for nullclines",
  "Numerical parameters for fixed points",
  "Number of steps per plotted point",
  "Maximum allowed size of any variable",
  "Integration method",
  "Maximum delay and delay related stuff",
  "Color trajectories according to velocity,etc",
  "Curve fitting, FFT, mean, variance, seed, etc",
  "Define Poincare map parameters",
  "Define shifted plots",
  "Modify lookup tables",
  "Numerical setup for boundary value solver",
  "Compute adjoint and averaged functions",
  "Return to main menu"
};

/* other hints  */

char *null_hint[]={
  "Compute new nullclines",
  "Redraw last nullclines",
  "Set automatic redraw -- X redraws when needed",
  "Only redraw when asked (Redraw)",
  "Freeze multiple nullclines",
  "Save nullcline values to a file"
};

char *null_freeze[]={
  "Freeze current clines",
  "Delete all frozen clines",
  "Range freeze a bunch of clines",
  "Animate nullclines"
};
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

char *wind_hint[]={
  "Manually choose 2D view",
  "Zoom into with mouse",
  "Zoom out with mouse",
  "Let XPP automatically choose window",
  "Reset to default view"
};

char *flow_hint[]={
  "Draw vector field for 2D section",
  "Draw regular series of trajectories",
  " ",
  "Color the PP on a grid",
  "Draw only directions"
};

char *phas_hint[]={
  "Each variable is on a circle",
  "No variable on circle",
  "Choose circle variables"
};

char *kin_hint[]={
  "Grab a screen shot",
  "Clear all screen shots",
  "Manually cycle thru screenshots",
  "Continuously cycle through screen shots",
  "Dump the screen shots to disk",
  "Make animated gif file from screenshots"
};

char *graf_hint[]={
  "Add another curve to the current plot",
  "Delete last added plot",
  "Remove all the added plots except the main one",
  "Edit parameters for a plot",
  "Create a postscript file of current plot",
  "Create a styleable svg file of current plot",
  "Options for permanently saving curve",
  "Axes label sizes and zero axes for postscript",
  "Export the numbers used in the graphs on the screen",
  "Change colormap"
};

char *cmap_hint[]={
  " blue-green-red",
  "red-...-violet-red",
  "black-red-yellow-white",
  "pale cyan->pale yellow",
  "blue-violet-red",
  "black-white"
};
char *frz_hint[]={
  "Permanently keep main curve -- even after reintegrating",
  "Delete specified frozen curve",
  "Edit specified frozen curve",
  "Remove all frozen curves",
  "Toggle key on/off",
  "Import bifurcation data",
  "Clear imported bifurcation curve",
  "Automatically freeze after each integration",
};

char *bvp_hint[]={
  "Solve BVP over range of parameters",
  "Don't show any but final step",
  "Show each step of iteration",
  "Solve BVP with periodic conditions",
  "Set up special homoclinic stuff"
};

char *adj_hint[]={
  "Compute a new adjoint function",
  "Compute averaging interaction function",
  "Load computed adjoint",
  "Load computed orbit",
  "Load computed interaction function",
  "Adjoint numerical parameters",
  "Range over stuff to computte many adjoints"
};


char *view_hint[]={
  "Two-dimensional view settings",
  "Three-dimensional view settings",
  "Plot array ",
  "Animation window"
};

char *half_hint[]={
  "Create new window",
  "Delete all but main window",
  "Delete last window",
  "Place current window at bottom",
  "Automatically redraw",
  "Redraw only when requested",
  "Plot all graphs simultaneously -- slows you down"};

char *text_hint[]={
  "Create text labels in different fonts ",
  "Add arrows to trajectories",
  "Create lines with arrowheads",
  "Add squares, circles, etc to plot",
  "Change properties, delete, or move existing add-on",
  "Delete all text, markers, arrows",
  "Create many markers based on browser data"
};

char *edit_hint[]={
  "Move the selected item",
  "Change properties of selected item",
  "Delete selected item"
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

char *color_hint[]={
  " ",
  "Color according to magnitude of derivative",
  "Color according to height of Z-axis"
};

char *tab_hint[]={"Edit the lookup tables","View a table in the data browser"};

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
