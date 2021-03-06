#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <TFile.h>
#include <TNtuple.h>
#include <TTree.h>

#include "../lib/bank_containers.h"
#include "../lib/constants.h"
#include "../lib/err_handler.h"
#include "../lib/file_handler.h"
#include "../lib/io_handler.h"
#include "../lib/particle.h"
#include "../lib/utilities.h"

// Find most precise TOF (Layers precision: FTOF1B, FTOF1A, FTOFB, PCAL, ECIN, ECOU).
double get_tof(REC_Scintillator rsci, REC_Calorimeter  rcal, int pindex) {
    int    most_precise_lyr = 0;
    double tof              = INFINITY;
    for (UInt_t i = 0; i < rsci.pindex->size(); ++i) {
        // Filter out incorrect pindex and hits not from FTOF.
        if (rsci.pindex->at(i) != pindex || rsci.detector->at(i) != FTOF_ID) continue;
        if (rsci.layer->at(i) == FTOF1B_LYR) {
            most_precise_lyr = FTOF1B_LYR;
            tof = rsci.time->at(i);
            break; // Things won't get better than this.
        }
        else if (rsci.layer->at(i) == FTOF1A_LYR) {
            if (most_precise_lyr == FTOF1A_LYR) continue;
            most_precise_lyr = FTOF1A_LYR;
            tof = rsci.time->at(i);
        }
        else if (rsci.layer->at(i) == FTOF2_LYR) {
            if (most_precise_lyr != 0) continue; // We already have a similar or better hit.
            most_precise_lyr = FTOF2_LYR;
            tof = rsci.time->at(i);
        }
    }
    if (most_precise_lyr == 0) { // No hits from FTOF, let's try ECAL.
        for (UInt_t i = 0; i < rcal.pindex->size(); ++i) {
            if (rcal.pindex->at(i) != pindex) continue; // Filter out incorrect pindex.
            if (rcal.layer->at(i) == PCAL_LYR) {
                most_precise_lyr = 10 + PCAL_LYR;
                tof = rcal.time->at(i);
                break; // Things won't get better than this.
            }
            else if (rcal.layer->at(i) == ECIN_LYR) {
                if (most_precise_lyr == 10 + ECIN_LYR) continue;
                most_precise_lyr = 10 + ECIN_LYR;
                tof = rcal.time->at(i);
            }
            else if (rcal.layer->at(i) == ECOU_LYR) {
                if (most_precise_lyr != 0) continue;
                most_precise_lyr = 10 + ECOU_LYR;
                tof = rcal.time->at(i);
            }
        }
    }

    return tof;
}

int run(char * in_filename, bool debug, int nevn, int run_no, double beam_E) {
    // Extract sampling fraction parameters.
    double sf_params[NSECTORS][SF_NPARAMS][2];
    if (get_sf_params(Form("../data/sf_params_%06d.root", run_no), sf_params)) return 8;

    // Access input file. TODO. Make this input file*s*, as in multiple files.
    TFile *f_in  = TFile::Open(in_filename, "READ");
    TFile *f_out = TFile::Open("../root_io/ntuples.root", "RECREATE"); // NOTE. This path sucks.
    if (!f_in || f_in->IsZombie()) return 1;

    // Generate lists of variables.
    TString vars("");
    for (int vi = 0; vi < VAR_LIST_SIZE; ++vi) {
        vars.Append(Form("%s", S_VAR_LIST[vi]));
        if (vi != VAR_LIST_SIZE-1) vars.Append(":");
    }

    // Create TTree and TNTuples.
    TTree   * t_in  = f_in->Get<TTree>("Tree");
    TNtuple * t_out[2];
    t_out[0] = new TNtuple(S_DC,  S_DC,  vars);
    t_out[1] = new TNtuple(S_FMT, S_FMT, vars);

    // Associate banks to TTree.
    REC_Particle     rpart(t_in);
    REC_Track        rtrk (t_in);
    REC_Calorimeter  rcal (t_in);
    REC_Cherenkov    rche (t_in);
    REC_Scintillator rsci (t_in);
    FMT_Tracks       ftrk (t_in);

    // Counters for fancy progress bar.
    int divcntr     = 0;
    int evnsplitter = 0;

    // Iterate through input file. Each TTree entry is one event.
    printf("Reading %lld events from %s.\n", nevn == -1 ? t_in->GetEntries() : nevn, in_filename);

    for (int evn = 0; (evn < t_in->GetEntries()) && (nevn == -1 || evn < nevn); ++evn) {
        if (!debug && evn >= evnsplitter) {
            if (evn != 0) {
                printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
                printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
            }
            printf("[");
            for (int i = 0; i <= 50; ++i) {
                if (i <= divcntr/2) printf("=");
                else                printf(" ");
            }
            printf("] %2d%%", divcntr);
            fflush(stdout);
            divcntr++;
            evnsplitter = nevn == -1 ? (t_in->GetEntries() / 100) * divcntr : (nevn/100) * divcntr;
        }

        rpart.get_entries(t_in, evn);
        rtrk .get_entries(t_in, evn);
        rsci .get_entries(t_in, evn);
        rcal .get_entries(t_in, evn);
        rche .get_entries(t_in, evn);
        ftrk .get_entries(t_in, evn);

        // Filter events without the necessary banks.
        if (rpart.vz->size() == 0 || rtrk.pindex->size() == 0) continue;

        // Find trigger electron's TOF.
        float tre_tof = get_tof(rsci, rcal, rtrk.pindex->at(0));

        // Process DIS event.
        for (UInt_t pos = 0; pos < rtrk.index->size(); ++pos) {
            int pindex = rtrk.pindex->at(pos); // pindex is always equal to pos!

            // Get reconstructed particle from DC and from FMT.
            particle p[2];
            p[0] = particle_init(&rpart, &rtrk, pos);        // DC.
            p[1] = particle_init(&rpart, &rtrk, &ftrk, pos); // FMT.

            // Get deposited energy.
            float pcal_E = 0; // PCAL total deposited energy.
            float ecin_E = 0; // EC inner total deposited energy.
            float ecou_E = 0; // EC outer total deposited energy.
            for (UInt_t i = 0; i < rcal.pindex->size(); ++i) {
                if (rcal.pindex->at(i) != pindex) continue;
                int lyr = (int) rcal.layer->at(i);

                if      (lyr == PCAL_LYR) pcal_E += rcal.energy->at(i);
                else if (lyr == ECIN_LYR) ecin_E += rcal.energy->at(i);
                else if (lyr == ECOU_LYR) ecou_E += rcal.energy->at(i);
                else return 2;
            }
            float tot_E = pcal_E + ecin_E + ecou_E;

            // Get Cherenkov counters data.
            int htcc_nphe = 0; // Number of photoelectrons deposited in htcc.
            int ltcc_nphe = 0; // Number of photoelectrons deposited in ltcc.
            for (UInt_t i = 0; i < rche.pindex->size(); ++i) {
                if (rche.pindex->at(i) == pindex) {
                    int detector = rche.detector->at(i);
                    if      (detector == HTCC_ID) htcc_nphe += rche.nphe->at(i);
                    else if (detector == LTCC_ID) ltcc_nphe += rche.nphe->at(i);
                    else return 3;
                }
            }

            // Get TOF.
            float tof = get_tof(rsci, rcal, pindex);

            // Get miscellaneous data.
            int status = rpart.status->at(pindex);
            float chi2 = rtrk.chi2   ->at(pos);
            float ndf  = rtrk.ndf    ->at(pos);

            // Assign PID.
            for (int pi = 0; pi < 2; ++pi) {
                set_pid(&(p[pi]), rpart.pid->at(pindex), status, tot_E, pcal_E, htcc_nphe,
                        ltcc_nphe, sf_params[rtrk.sector->at(pos)]);
            }

            // Fill TNtuples. TODO. This probably should be implemented more elegantly.
            // NOTE. If adding new variables, check their order in S_VAR_LIST.
            for (int pi = 0; pi < 2; ++pi) {
                if (!p[pi].is_valid) continue;

                Float_t v[VAR_LIST_SIZE] = {
                        (Float_t) run_no, (Float_t) evn, (Float_t) beam_E,
                        (Float_t) p[pi].pid, (Float_t) status, (Float_t) p[pi].q, p[pi].mass,
                                p[pi].vx, p[pi].vy, p[pi].vz, p[pi].px, p[pi].py, p[pi].pz,
                                P(p[pi]), theta_lab(p[pi]), phi_lab(p[pi]), p[pi].beta,
                        chi2, ndf,
                        pcal_E, ecin_E, ecou_E, tot_E,
                        (tof - tre_tof),
                        Q2(p[pi], beam_E), nu(p[pi], beam_E),
                                Xb(p[pi], beam_E), W2(p[pi], beam_E)
                };
                t_out[pi]->Fill(v);
            }
        }
    }
    if (!debug) {
        printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
        printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
        printf("[==================================================] 100%% \n");
    }

    // Write to output file.
    f_out->Write();

    // Clean up after ourselves.
    f_in ->Close();
    f_out->Close();
    free(in_filename);

    return 0;
}

// Call program from terminal, C-style.
int main(int argc, char ** argv) {
    bool debug         = false;
    int nevn           = -1;
    int run_no         = -1;
    double beam_E      = -1;
    char * in_filename = NULL;

    if (make_ntuples_handle_args_err(make_ntuples_handle_args(argc, argv, &debug, &nevn,
            &in_filename, &run_no, &beam_E), &in_filename, run_no))
        return 1;
    return make_ntuples_err(run(in_filename, debug, nevn, run_no, beam_E), &in_filename);
}
