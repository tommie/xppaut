#include "torus.h"

#include "form_ode.h"
#include "ggets.h"
#include "load_eqn.h"
#include "ui-x11/torus-box.h"

static int commit_torus(void *cookie, const int *sel, int n) {
  for (int i = 0; i < n; i++)
    itor[i] = sel[i];

  TORUS = 0;
  for (int i = 0; i < NEQ; i++) {
    if (itor[i] == 1)
      TORUS = 1;
  }

  return 0;
}

void do_torus_com(int c) {
  if (c == 1) {
    /* None */
    TORUS = 0;
    for (int i = 0; i < MAXODE; i++)
      itor[i] = 0;

    return;
  }

  new_float("Period :", &TOR_PERIOD);
  if (TOR_PERIOD <= 0.0) {
    err_msg("Choose positive period");
    return;
  }

  if (c == 0) {
    /* All */
    TORUS = 1;
    for (int i = 0; i < MAXODE; i++)
      itor[i] = 1;

    return;
  }

  x11_tor_box_open("Fold which", (const char *)uvar_names, itor, NEQ,
                   commit_torus, NULL);
}
