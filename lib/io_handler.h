#ifndef IO_HANDLER
#define IO_HANDLER

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "file_handler.h"

int make_ntuples_handle_args(int argc, char ** argv, bool * debug, int * nevents,
                             char ** input_file, int * run_no, double * beam_energy);
int extractsf_handle_args(int argc, char ** argv, bool * use_fmt, int * nevents,
                          char ** input_file, int * run_no);
int hipo2root_handle_args(int argc, char ** argv, char ** input_file, int * run_no);
int check_root_filename(char * input_file);
int handle_root_filename(char * input_file, int * run_no);
int handle_root_filename(char * input_file, int * run_no, double * beam_energy);
int check_hipo_filename(char * input_file);
int handle_hipo_filename(char * input_file, int * run_no);

#endif
