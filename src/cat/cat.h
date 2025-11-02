#ifndef CAT_H
#define CAT_H

#include <stdbool.h>
#include <stdio.h>

typedef struct {
  bool flag_b;
  bool flag_e;
  bool flag_n;
  bool flag_s;
  bool flag_t;
  bool flag_v;
  bool flag_E;
  bool flag_T;
} app_opt;

void get_opt(int argc, char *argv[], app_opt *opts);
void text_print(FILE **fp, app_opt *opts, int *all_strs_cnt, int *occup_str_cnt,
                int *empty_str_cnt, int *file_cnt);

// flags
void flag_b_apply(app_opt *opts, char *str_buf, int *occup_str_cnt,
                  bool *file_cnt_check, char last_c);
void flag_e_apply(app_opt *opts, char c);
void flag_n_apply(app_opt *opts, int *all_strs_cnt, bool *file_cnt_check,
                  char last_c);
bool flag_s_apply(app_opt *opts, char *str_buf, int *empty_str_cnt,
                  bool *file_cnt_check, char last_c);
bool flag_t_apply(app_opt *opts, char c);
void flag_v_apply(app_opt *opts, char *c);

// utils
void exceptions(app_opt *opts);
bool is_str_buf_empty(char *string_buffer);
bool is_str_occup(char *str_buf);

#endif