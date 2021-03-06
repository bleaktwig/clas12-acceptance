#ifndef ERR_HANDLER
#define ERR_HANDLER

#include <stdio.h>
#include <stdlib.h>

int make_ntuples_usage();
int make_ntuples_handle_args_err(int errcode, char **in_filename, int run_no);
int make_ntuples_err(int errcode, char **in_filename);
int extractsf_usage();
int extractsf_handle_args_err(int errcode, char **in_filename);
int extractsf_err(int errcode, char **in_filename);
int hipo2root_usage();
int hipo2root_handle_args_err(int errcode, char **in_filename);

#endif
