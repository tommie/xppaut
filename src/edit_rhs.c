#include "edit_rhs.h"

#include <stdlib.h>
#include <string.h>

#include "browse.h"
#include "extra.h"
#include "form_ode.h"
#include "ggets.h"
#include "load_eqn.h"
#include "main.h"
#include "menus.h"
#include "parserslow.h"
#include "pop_list.h"
#include "ui-x11/edit-box.h"
#include "ui-x11/file-selector.h"

/* --- Macros --- */
#define NEQMAXFOREDIT 20

void edit_menu(void) {
  Window temp = main_win;
  static char *n[] = {"RHS's", "Functions", "Save as", "Load DLL"};
  static char key[] = "rfsl";
  char ch;
  int edtype = 0, i;
  ch = (char)pop_up_list(&temp, "Edit Stuff", n, key, 4, 11, edtype, 10,
                         13 * DCURY + 8, edrh_hint, main_status_bar);
  edtype = -1;
  for (i = 0; i < 4; i++)
    if (ch == key[i])
      edtype = i;
  switch (edtype) {
  case 0:
    edit_rhs();
    break;
  case 1:
    edit_functions();
    break;
  case 2:
    save_as();
    break;
  case 3:
    load_new_dll();
    break;
  }
}

/**
 * Update ode_names with the given values.
 */
static int commit_rhs(void *cookie,
                      const char *values /*[MAX_N_EBOX][MAX_LEN_EBOX]*/,
                      int n) {
  for (int i = 0; i < n; i++, values += MAX_LEN_EBOX) {
    if (i >= NODE && i < NODE + NMarkov)
      continue;

    int command[200];
    int len;
    int err = parse_expr(values, command, &len);

    if (err == 1) {
      char name[MAX_LEN_EBOX];
      char msg[200];

      form_ode_format_lhs(name, MAX_LEN_EBOX, i);
      sprintf(msg, "Bad rhs:%s=%s", name, values);
      err_msg(msg);
    } else {
      ode_names[i] = realloc(ode_names[i], strlen(values) + 5);
      strcpy(ode_names[i], values);

      int i0 = i;
      if (i >= NODE)
        i0 = i0 + FIX_VAR - NMarkov;

      for (int j = 0; j < len; j++)
        my_ode[i0][j] = command[j];
    }
  }

  return 0;
}

void edit_rhs(void) {
  char **names, **values;
  int n = NEQ;

  if (NEQ > NEQMAXFOREDIT)
    return;
  names = (char **)malloc(n * sizeof(char *));
  values = (char **)malloc(n * sizeof(char *));
  for (int i = 0; i < n; i++) {
    values[i] = (char *)malloc(MAX_LEN_EBOX * sizeof(char));
    names[i] = (char *)malloc(MAX_LEN_EBOX * sizeof(char));
    form_ode_format_lhs(names[i], MAX_LEN_EBOX, i);
    strcpy(values[i], ode_names[i]);
  }

  x11_edit_box_open(n, "Right Hand Sides", names, values, commit_rhs, NULL);

  for (int i = 0; i < n; i++) {
    free(values[i]);
    free(names[i]);
  }
  free(values);
  free(names);
}

/**
 * Update ufun_rhs with the given values.
 */
static int commit_functions(void *cookie,
                            const char *values /*[MAX_N_EBOX][MAX_LEN_EBOX]*/,
                            int n) {
  for (int i = 0; i < n; i++, values += MAX_LEN_EBOX) {
    if (parser_set_ufun_rhs(i, values)) {
      char msg[200];

      sprintf(msg, "Bad func.:%s=%s", ufuns.elems[i].name, values);
      err_msg(msg);
    }
  }

  return 0;
}

void edit_functions(void) {
  char **names, **values;
  int n = ufuns.len;
  if (n == 0 || n > NEQMAXFOREDIT)
    return;
  names = (char **)malloc(n * sizeof(char *));
  values = (char **)malloc(n * sizeof(char *));
  for (int i = 0; i < n; i++) {
    values[i] = (char *)malloc(MAX_LEN_EBOX * sizeof(char));
    names[i] = (char *)malloc(MAX_LEN_EBOX * sizeof(char));
    strcpy(values[i], ufuns.elems[i].def);

    if (ufuns.elems[i].narg == 0) {
      sprintf(names[i], "%s()", ufuns.elems[i].name);
    }
    if (ufuns.elems[i].narg == 1) {
      sprintf(names[i], "%s(%s)", ufuns.elems[i].name, ufuns.elems[i].args[0]);
    }
    if (ufuns.elems[i].narg > 1)
      sprintf(names[i], "%s(%s,...,%s)", ufuns.elems[i].name,
              ufuns.elems[i].args[0],
              ufuns.elems[i].args[ufuns.elems[i].narg - 1]);
  }

  x11_edit_box_open(n, "Functions", names, values, commit_functions, NULL);

  for (int i = 0; i < n; i++) {
    free(values[i]);
    free(names[i]);
  }
  free(values);
  free(names);
}

int save_as(void) {
  int i, ok;
  FILE *fp;
  double z;
  char filename[256];
  sprintf(filename, "%s", this_file);
  ping();
  /* if(new_string("Filename: ",filename)==0)return; */
  if (!file_selector("Save As", filename, "*.ode"))
    return (-1);
  open_write_file(&fp, filename, &ok);
  if (!ok)
    return (-1);
  fp = fopen(filename, "w");
  if (fp == NULL)
    return (0);
  fprintf(fp, "%d", NEQ);
  for (i = 0; i < NODE; i++) {
    if (i % 5 == 0)
      fprintf(fp, "\nvariable ");
    fprintf(fp, " %s=%.16g ", uvar_names[i], last_ic[i]);
  }
  fprintf(fp, "\n");
  for (i = NODE; i < NEQ; i++) {
    if ((i - NODE) % 5 == 0)
      fprintf(fp, "\naux ");
    fprintf(fp, " %s ", uvar_names[i]);
  }
  fprintf(fp, "\n");
  for (i = 0; i < NUPAR; i++) {
    if (i % 5 == 0)
      fprintf(fp, "\nparam  ");
    get_val(upar_names[i], &z);
    fprintf(fp, " %s=%.16g   ", upar_names[i], z);
  }
  fprintf(fp, "\n");
  for (i = 0; i < ufuns.len; i++) {
    fprintf(fp, "user %s %d %s\n", ufuns.elems[i].name, ufuns.elems[i].narg,
            ufuns.elems[i].def);
  }
  for (i = 0; i < NODE; i++) {
    if (EqType[i] == 1)
      fprintf(fp, "i ");
    else
      fprintf(fp, "o ");
    fprintf(fp, "%s\n", ode_names[i]);
  }
  for (i = NODE; i < NEQ; i++)
    fprintf(fp, "o %s\n", ode_names[i]);
  for (i = 0; i < NODE; i++)
    fprintf(fp, "b %s \n", my_bc[i].string);
  fprintf(fp, "done\n");
  fclose(fp);

  return (1);
}
