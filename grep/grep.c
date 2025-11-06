#include "grep.h"

#include <getopt.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  app_opts opts = {0};
  bool flag_f_err = false;

  parse_opts(argc, argv, &opts, &flag_f_err);
  if (flag_f_err == true) {
    return 0;
  }

  comp_reg_exs(&opts);

  int cur_file_i = optind + 1;
  int files_qty = argc - optind - 1;
  set_curfilei_and_fileqty(&opts, &cur_file_i, &files_qty);

  FILE *fp;
  for (int i = cur_file_i; i < argc; ++i) {
    if ((fp = fopen(argv[i], "r")) != NULL) {
      file_processing(&opts, (const char **)argv, i, fp, files_qty);
    } else
      flag_s_apply(&opts, argv[i]);
  }
  free_reg_exs();
  return 0;
}

void parse_opts(const int argc, char **argv, app_opts *opts, bool *flag_f_err) {
  const char options[] = "e:ivclnhsf:o";  // eivclnhsfo
  opts->no_flags = true;
  int getopt_cur_flag = 0;
  while ((getopt_cur_flag = getopt(argc, argv, options)) != -1 &&
         *flag_f_err == false) {
    opts->no_flags = false;
    switch (getopt_cur_flag) {
      case 'e':
        opts->flag_e = true;
        flag_e_apply();
        break;
      case 'i':
        opts->flag_i = true;
        break;
      case 'v':
        opts->flag_v = true;
        break;
      case 'c':
        opts->flag_c = true;
        break;
      case 'l':
        opts->flag_l = true;
        break;
      case 'n':
        opts->flag_n = true;
        break;
      case 'h':
        opts->flag_h = true;
        break;
      case 's':
        opts->flag_s = true;
        break;
      case 'f':
        opts->flag_f = true;
        flag_f_apply(flag_f_err);
        break;
      case 'o':
        opts->flag_o = true;
        break;
    }
  }

  no_flags_apply(opts, argv);
  reg_exs *reg_exs = get_reg_exs();
  reg_exs->tmpl_map_curi = 0;

  exceptions(opts);
}

// При устройстве таким образом, что весь функционал выделения и увеличения
// массива происходит неявное применение того или иного, поэтому лучше
// разделить
// их на две разные функции

#define REG_EXS_CAP 1

reg_exs *get_reg_exs() {
  static reg_exs re = {0};

  if (re.tmpl_map == NULL && re.map == NULL) {
    re.tmpl_map_cap = REG_EXS_CAP;
    re.cap = REG_EXS_CAP;
    // re.i = 0;
    re.tmpl_map = (char **)malloc(re.tmpl_map_cap * sizeof(char *));
    re.map = (reg_ex *)malloc(re.cap * sizeof(reg_ex));
  }

  return &re;
}

void update_reg_exs() {
  reg_exs *reg_exs = get_reg_exs();

  if (reg_exs->tmpl_map_curi == reg_exs->tmpl_map_sz) {
    ++(reg_exs->tmpl_map_sz);
  }

  if (reg_exs->curi == reg_exs->sz) {
    ++(reg_exs->sz);
  }

  if (reg_exs->tmpl_map_sz == reg_exs->tmpl_map_cap) {
    reg_exs->tmpl_map_cap *= 2;
    reg_exs->tmpl_map =
        realloc(reg_exs->tmpl_map, reg_exs->tmpl_map_cap * sizeof(char *));
  }

  if (reg_exs->sz == reg_exs->cap) {
    reg_exs->cap *= 2;
    reg_exs->map = realloc(reg_exs->map, reg_exs->cap * sizeof(reg_ex));
  }
}

#define FILES_CAP 2

void comp_reg_exs(const app_opts *opts) {
  reg_exs *re = get_reg_exs();
  int regcomp_mode = flag_i_apply(opts);
  static int j = 0;
  for (size_t i = re->tmpl_map_curi; i < re->tmpl_map_sz; ++i) {
    if (j++) {
      ++(re->curi);
    }
    update_reg_exs();
    (re->map[re->curi]).regcomp_status =
        regcomp(&((re->map[re->curi]).ex), re->tmpl_map[i], regcomp_mode);
    if ((re->map[re->curi]).regcomp_status) {  // > 0 – error code
      fprintf(stderr, "regex compilation error\n");
    }
  }
  re->curi = 0;
  re->tmpl_map_curi = 0;
}

void free_reg_exs() {
  reg_exs *re = get_reg_exs();
  for (size_t i = re->curi; i < re->sz; ++i) {
    if (re->map[i].regcomp_status == 0) {
      regfree(&(re->map[i].ex));
    }
  }
}

void set_curfilei_and_fileqty(const app_opts *opts, int *cur_file_i,
                              int *files_qty) {
  if (opts->flag_e || opts->flag_f) {
    *cur_file_i -= 1;
    *files_qty += 1;
  }
}

void file_processing(const app_opts *opts, const char **argv,
                     const int cur_file_i, FILE *fp, const int files_qty) {
  int file_str_i = 1;      // индекс строки в файле
  int match_strs_cnt = 0;  // количество совпадений
  bool repeat_filename = true;

  bool invert_mode_find_matches = flag_v_apply(opts);

  int str_len = 0;
  char *str = NULL;
  size_t buf_sz = 0;

  while ((str_len = getline(&str, &buf_sz, fp)) != -1) {
    bool match_found = false;

    match_reg_ex_and_str(str, invert_mode_find_matches, &match_found);
    increase_match_strs_cnt(match_found, &match_strs_cnt);

    print_filenames(opts, match_found, files_qty, argv, cur_file_i);

    flag_l_apply(opts, match_found, &repeat_filename, argv, cur_file_i);
    flag_n_apply(opts, match_found, file_str_i);
    flag_o_apply(opts, str, invert_mode_find_matches, argv, cur_file_i,
                 files_qty, file_str_i);

    print_str(opts, match_found, str, str_len);
    ++file_str_i;
  }
  flag_c_apply(opts, argv, files_qty, cur_file_i, match_strs_cnt);
  free(str);
}

void match_reg_ex_and_str(const char *str, const bool invert_mode_find_matches,
                          bool *match_found) {
  // если совпадение найдено (false) и режим НЕ инвертирован (false) →
  // match_found = true
  reg_exs *reg_exs = get_reg_exs();
  for (size_t i = reg_exs->curi; i < reg_exs->sz; ++i) {
    if (regexec(&(reg_exs->map[i].ex), str, 0, NULL, 0) ==
        invert_mode_find_matches) {
      *match_found = true;
    } else if (invert_mode_find_matches) {
      *match_found = false;
      break;
    }
  }
}

void no_flags_apply(const app_opts *opts, char **argv) {
  reg_exs *reg_exs = get_reg_exs();
  if (!opts->flag_e && !opts->flag_f) {
    update_reg_exs();
    reg_exs->tmpl_map[0] = argv[optind];
  }
}

void flag_e_apply() {
  reg_exs *re = get_reg_exs();
  if ((re->i)++) {
    ++(re->tmpl_map_curi);
  }
  update_reg_exs();
  re->tmpl_map[re->tmpl_map_curi] = optarg;
}

int flag_i_apply(const app_opts *opts) {
  int regcomp_mode = 0;
  if (opts->flag_i) {
    regcomp_mode = REG_ICASE;
  }
  return regcomp_mode;
}

bool flag_v_apply(const app_opts *opts) {
  bool invert_mode = false;
  if (opts->flag_v) {
    invert_mode = true;
  }
  return invert_mode;
}

void flag_c_apply(const app_opts *opts, const char **argv, const int files_qty,
                  const int cur_file_i, const int match_strs_cnt) {
  if (!opts->flag_c) {
    return;
  }
  if (!opts->flag_h) {
    print_if_many_files(files_qty, argv, cur_file_i);
  }
  printf("%d\n", match_strs_cnt);
}

void flag_l_apply(const app_opts *opts, const bool match_found,
                  bool *repeat_filename, const char **argv,
                  const int cur_file_i) {
  if (!opts->flag_l) {
    return;
  }
  if (match_found && *repeat_filename) {
    print_filename(argv, cur_file_i);
    printf("\n");
    *repeat_filename = false;
  }
}

void flag_n_apply(const app_opts *opts, const bool match_found,
                  const int file_str_i) {
  if (!opts->flag_n || opts->flag_o) {
    return;
  }
  if (match_found) {
    printf("%d:", file_str_i);
  }
}

void flag_s_apply(const app_opts *opts, const char *err_str) {
  if (opts->flag_s) {
    return;
  }

  fprintf(stderr, "grep: %s: No such file or directory\n", err_str);
}

void flag_f_apply(bool *flag_f_err) {
  reg_exs *re = get_reg_exs();
  FILE *f;
  if ((f = fopen(optarg, "r")) != NULL) {
    char *str = NULL;
    int str_len = 0;
    size_t buf_sz = 0;

    while ((str_len = getline(&str, &buf_sz, f)) != -1) {
      if ((re->i)++) {
        ++(re->tmpl_map_curi);
      }
      update_reg_exs();

      re->tmpl_map[re->tmpl_map_curi] =
          (char *)malloc((strlen(str) + 1) * sizeof(char));

      strcpy(re->tmpl_map[re->tmpl_map_curi], str);

      if (re->tmpl_map[re->tmpl_map_curi]
                      [strlen(re->tmpl_map[re->tmpl_map_curi]) - 1] == '\n') {
        re->tmpl_map[re->tmpl_map_curi]
                    [strlen(re->tmpl_map[re->tmpl_map_curi]) - 1] = '\0';
      }
    }
    free(str);
  } else {
    fprintf(stderr, "grep: %s: No such file or directory\n", optarg);
    *flag_f_err = true;
  }
}

void flag_o_apply(const app_opts *opts, const char *str,
                  const bool invert_mode_find_matches, const char **argv,
                  const int cur_file_i, const int files_qty,
                  const int file_str_i) {
  if (!opts->flag_o || invert_mode_find_matches == true) {
    return;
  }
  reg_exs *reg_exs = get_reg_exs();
  regmatch_t match;

  char *s = (char *)str;
  int earliest_regi = -1;

  while ((earliest_regi =
              flag_o_util(str, s, &match, invert_mode_find_matches)) != -1) {
    if (regexec(&(reg_exs->map[earliest_regi].ex), s, 1, &match, 0) !=
        invert_mode_find_matches) {
      continue;
    }

    int len = match.rm_eo - match.rm_so;
    if (!opts->flag_h) {
      print_if_many_files(files_qty, argv, cur_file_i);
    }
    if (opts->flag_n) {
      printf("%d:", file_str_i);
    }
    printf("%.*s\n", len, s + match.rm_so);
    s += match.rm_eo;
  }
}

int flag_o_util(const char *str, const char *s, regmatch_t *match,
                const bool invert_mode_find_matches) {
  reg_exs *reg_exs = get_reg_exs();

  int earliest_match = strlen(str);
  int earliest_regi = -1;

  for (size_t i = reg_exs->curi; i < reg_exs->sz; ++i) {
    if (regexec(&(reg_exs->map[i].ex), s, 1, match, 0) ==
        invert_mode_find_matches) {
      if (match->rm_so < earliest_match) {
        earliest_match = match->rm_so;
        earliest_regi = i;
      }
    }
  }
  return earliest_regi;
}
void increase_match_strs_cnt(const bool match_found, int *match_strs_cnt) {
  if (match_found) {
    ++(*match_strs_cnt);
  }
}

void print_filenames(const app_opts *opts, const bool match_found,
                     const int files_qty, const char **argv,
                     const int cur_file_i) {
  if (opts->flag_c || opts->flag_h || opts->flag_o || opts->flag_l) {
    return;
  }
  if (match_found && (opts->flag_e || opts->flag_i || opts->flag_v ||
                      opts->flag_n || opts->flag_s || opts->flag_f)) {
    print_if_many_files(files_qty, argv, cur_file_i);
  }
}

void print_if_many_files(const int files_qty, const char **argv,
                         const int cur_file_i) {
  if (files_qty >= 2) {
    print_filename(argv, cur_file_i);
    printf(":");
  }
}

void print_filename(const char **argv, const int cur_file_i) {
  printf("%s", argv[cur_file_i]);
}

void print_str(const app_opts *opts, const bool match_found, const char *str,
               const int str_len) {
  // Возврат из функции когда в условии флага есть пометка "Выводит ТОЛЬКО"
  if (opts->flag_o || opts->flag_c || opts->flag_l) {
    return;
  }
  if (match_found &&
      (opts->flag_e || opts->flag_i || opts->flag_n || opts->flag_h ||
       opts->flag_s || opts->flag_f || opts->flag_v)) {
    printf("%s", str);
    if (str[str_len - 1] != '\n') printf("\n");
  }
}

void exceptions(app_opts *opts) {
  // Если какой-то функционал флага можно выключить при совместном
  // использовании
  // нескольких опций, то они прописаны здесь
  // Иначе, при пересечении функционала основных функций флагов, то они
  // прописаны точечно внутри них
  if (opts->flag_l) {
    opts->flag_c = false;
    opts->flag_n = false;
    opts->flag_o = false;
  }
  if (opts->flag_c) {
    opts->flag_n = false;
    opts->flag_o = false;
  }
}