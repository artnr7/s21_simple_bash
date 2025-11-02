#ifndef GREP_H
#define GREP_H

#include <regex.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct {
  bool flag_e;
  bool flag_i;
  bool flag_v;
  bool flag_c;
  bool flag_l;
  bool flag_n;
  bool flag_h;
  bool flag_s;
  bool flag_f;
  bool flag_o;
  bool no_flags;
} app_opts;

typedef struct {
  regex_t ex;
  int regcomp_status;
} reg_ex;

typedef struct {
  char **tmpl_map;
  size_t tmpl_map_cap;
  size_t tmpl_map_sz;
  size_t tmpl_map_curi;
  int i;
  reg_ex *map;
  size_t cap;
  size_t sz;
  size_t curi;
} reg_exs;

void set_curfilei_and_fileqty(const app_opts *opts, int *cur_file_i,
                              int *files_qty);
void file_processing(const app_opts *opts, const char **argv,
                     const int cur_file_i, FILE *fp, const int files_qty);

void match_reg_ex_and_str(const char *str, const bool invert_mode_find_matches,
                          bool *match_found);

void increase_match_strs_cnt(const bool match_found, int *match_strs_cnt);

// Parsers
void parse_opts(const int argc, char **argv, app_opts *opts, bool *flag_f_err);
void parse_reg_exs();
void comp_reg_exs(const app_opts *opts);
void free_reg_exs();

// Getter reg_exs
reg_exs *get_reg_exs();
void update_reg_exs();
void free_reg_exs();

// Flags
void no_flags_apply(const app_opts *opts, char **argv);
void flag_e_apply();
int flag_i_apply(const app_opts *opts);
bool flag_v_apply(const app_opts *opts);
void flag_c_apply(const app_opts *opts, const char **argv, const int files_qty,
                  const int cur_file_i, const int match_strs_cnt);

void flag_l_apply(const app_opts *opts, const bool match_found,
                  bool *repeat_filename, const char **argv,
                  const int cur_file_i);

void flag_n_apply(const app_opts *opts, const bool match_found,
                  const int file_str_i);

void flag_s_apply(const app_opts *opts, const char *err_str);

void flag_f_apply(bool *flag_f_err);

void flag_o_apply(const app_opts *opts, const char *str,
                  const bool invert_mode_find_matches, const char **argv,
                  const int cur_file_i, const int files_qty,
                  const int file_str_i);

int flag_o_util(const char *str, const char *s, regmatch_t *match,
                const bool invert_mode_find_matches);

// Print
void print_filenames(const app_opts *opts, const bool match_found,
                     const int files_qty, const char **argv,
                     const int cur_file_i);
void print_if_many_files(const int files_qty, const char **argv,
                         const int cur_file_i);
void print_filename(const char **argv, const int cur_file_i);
void print_str(const app_opts *opts, const bool match_found, const char *str,
               const int str_len);
void print_arrs(int argc, char **argv);

void exceptions(app_opts *opts);
#endif