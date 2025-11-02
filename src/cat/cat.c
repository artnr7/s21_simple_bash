#include "cat.h"

#include <getopt.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  app_opt opts = {0};

  get_opt(argc, argv, &opts);

  // optind указывает на не опцию, следующий argv, но не флаг
  int all_strs_cnt = 1, occup_str_cnt = 1, empty_str_cnt = 0;
  int file_cnt = 0;
  FILE *fp;
  for (int i = optind; i < argc; i++) {
    if ((fp = fopen(argv[i], "r")) != NULL) {
      text_print(&fp, &opts, &all_strs_cnt, &occup_str_cnt, &empty_str_cnt,
                 &file_cnt);
      fclose(fp);
      ++file_cnt;
    } else
      fprintf(stderr, "cat: %s: No such file or directory\n", argv[i]);
  }
  return 0;
}

void text_print(FILE **fp, app_opt *opts, int *all_strs_cnt, int *occup_str_cnt,
                int *empty_str_cnt, int *file_cnt) {
  exceptions(opts);
  char *str_buf = NULL;
  size_t buf_size = 0;
  int len_str_buf = 0;
  bool file_cnt_check = false;
  if (*file_cnt) {
    file_cnt_check = true;
  }
  static char last_c = ' ';

  while ((len_str_buf = getline(&str_buf, &buf_size, *fp)) != -1) {
    if (flag_s_apply(opts, str_buf, empty_str_cnt, &file_cnt_check, last_c)) {
      continue;
    }
    flag_b_apply(opts, str_buf, occup_str_cnt, &file_cnt_check, last_c);
    flag_n_apply(opts, all_strs_cnt, &file_cnt_check, last_c);

    for (int i = 0; i < len_str_buf; i++) {
      last_c = str_buf[i];
      if (flag_t_apply(opts, last_c)) {
        continue;
      }
      flag_e_apply(opts, last_c);
      flag_v_apply(opts, &last_c);
    }
  }
  free(str_buf);
}

void exceptions(app_opt *opts) {
  if (opts->flag_b) {
    opts->flag_n = false;
  }
  //  предполагают -v
  if (opts->flag_e || opts->flag_t) {
    opts->flag_v = true;
  }
}

void flag_b_apply(app_opt *opts, char *str_buf, int *occup_str_cnt,
                  bool *file_cnt_check, char last_c) {
  if (opts->flag_b == false) {
    return;
  }

  if (is_str_occup(str_buf)) {
    if (*file_cnt_check) {
      if (last_c == '\n') {
        printf("%6d\t", (*occup_str_cnt)++);
      }
    } else {
      printf("%6d\t", (*occup_str_cnt)++);
    }
  }
  *file_cnt_check = false;
}

void flag_e_apply(app_opt *opts, char c) {
  if ((opts->flag_e || opts->flag_E) &&  // e
      c == '\n') {
    printf("$");
  }
}

void flag_n_apply(app_opt *opts, int *all_strs_cnt, bool *file_cnt_check,
                  char last_c) {
  if (opts->flag_n == false) {
    return;
  }

  if (*file_cnt_check) {
    if (last_c == '\n') {
      printf("%6d\t", (*all_strs_cnt)++);
    }
  } else {
    printf("%6d\t", (*all_strs_cnt)++);
  }
  *file_cnt_check = false;
}

bool flag_s_apply(app_opt *opts, char *str_buf, int *empty_str_cnt,
                  bool *file_cnt_check, char last_c) {
  if (opts->flag_s == false) {
    return false;
  }
  bool skip = false;
  if (*file_cnt_check && last_c != '\n' && is_str_buf_empty(str_buf)) {
    printf("\n");
  }

  if (is_str_buf_empty(str_buf)) {
    (*empty_str_cnt)++;
  } else {
    *empty_str_cnt = 0;
  }

  if (*empty_str_cnt > 1) {
    skip = true;
  }
  *file_cnt_check = false;
  return skip;
}

bool flag_t_apply(app_opt *opts, char c) {
  if (!(opts->flag_t || opts->flag_T)) {
    return false;
  }
  bool skip = false;
  if (c == '\t') {
    printf("^I");
    skip = true;
  }
  return skip;
}

void flag_v_apply(app_opt *opts, char *c) {
  if (opts->flag_v) {  // v
    if (*c < 32 && *c != '\n' && *c != '\t') {
      printf("^%c", *c + 64);
    } else if (*c == 127)
      printf("^?");
    else {
      printf("%c", *c);
    }
  } else {
    printf("%c", *c);
  }
}
bool is_str_buf_empty(char *str_buf) { return str_buf[0] == '\n'; }

bool is_str_occup(char *str_buf) { return !is_str_buf_empty(str_buf); }

void get_opt(int argc, char *argv[], app_opt *opts) {
  const char short_opts[9] = "benstvET";
  const struct option long_opts[] = {{"number-nonblank", 0, NULL, 'X'},
                                     {"number", 0, NULL, 'Y'},
                                     {"squeeze-blank", 0, NULL, 'Z'},
                                     {NULL, 0, NULL, 0}};
  int cur_flag_from_getopt = 0;

  while ((cur_flag_from_getopt =
              getopt_long(argc, argv, short_opts, long_opts, NULL)) != -1) {
    switch (cur_flag_from_getopt) {
      case 'b':
        opts->flag_b = true;
        break;
      case 'e':
        opts->flag_e = true;
        break;
      case 'n':
        opts->flag_n = true;
        break;
      case 's':
        opts->flag_s = true;
        break;
      case 't':
        opts->flag_t = true;
        break;
      case 'v':
        opts->flag_v = true;
        break;
      case 'E':
        opts->flag_E = true;
        break;
      case 'T':
        opts->flag_T = true;
        break;
      case 'X':
        opts->flag_b = true;
        break;
      case 'Y':
        opts->flag_n = true;
        break;
      case 'Z':
        opts->flag_s = true;
        break;
    }
  }
}