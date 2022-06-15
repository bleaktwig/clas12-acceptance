#include <cstdlib>
#include <iostream>

#include "reader.h"
#include "utils.h"
#include "TFile.h"
#include "TTree.h"
#include "Compression.h"

#include "../lib/err_handler.h"
#include "../lib/io_handler.h"
#include "../lib/bank_containers.h"

int main(int argc, char **argv) {
    char *in_filename = NULL;
    int  run_no = -1;

    if (hipo2root_handle_args_err(hipo2root_handle_args(argc, argv, &in_filename, &run_no),
                                  &in_filename))
        return 1;

    char *out_filename = (char *) malloc(22 * sizeof(char));
    if      (run_no /     10 == 0) sprintf(out_filename, "../root_io/00000%d.root", run_no);
    else if (run_no /    100 == 0) sprintf(out_filename, "../root_io/0000%d.root", run_no);
    else if (run_no /   1000 == 0) sprintf(out_filename, "../root_io/000%d.root", run_no);
    else if (run_no /  10000 == 0) sprintf(out_filename, "../root_io/00%d.root", run_no);
    else if (run_no / 100000 == 0) sprintf(out_filename, "../root_io/0%d.root", run_no);
    else                           sprintf(out_filename, "../root_io/%d.root", run_no);

    TFile *f = TFile::Open(out_filename, "RECREATE");
    f->SetCompressionAlgorithm(ROOT::kLZ4);

    TTree *tree = new TTree("Tree", "Tree");
    REC_Particle rec_particle;         rec_particle.create_branches(tree);
    REC_Track rec_track;               rec_track.create_branches(tree);
    REC_Calorimeter rec_calorimeter;   rec_calorimeter.create_branches(tree);
    REC_Scintillator rec_scintillator; rec_scintillator.create_branches(tree);
    // REC_Cherenkov rec_cherenkov;       rec_cherenkov.create_branches(tree);
    FMT_Tracks fmt_tracks;             fmt_tracks.create_branches(tree);

    // Setup.
    hipo::reader reader;
    reader.open(in_filename);

    hipo::dictionary factory;
    reader.readDictionary(factory);

    hipo::bank rec_particle_b(factory.getSchema("REC::Particle"));
    hipo::bank rec_track_b(factory.getSchema("REC::Track"));
    hipo::bank rec_calorimeter_b(factory.getSchema("REC::Calorimeter"));
    hipo::bank rec_scintillator_b(factory.getSchema("REC::Scintillator"));
    // hipo::bank rec_cherenkov_b(factory.getSchema("REC::Cherenkov"));
    hipo::bank fmt_tracks_b(factory.getSchema("FMT::Tracks"));
    hipo::event event;

    int c = 0;
    while (reader.next()) {
        c++;
        if (c % 10000 == 0) {
            if (c != 10000) printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
            printf("Read %8d events...", c);
            fflush(stdout);
        }
        reader.read(event);

        event.getStructure(rec_particle_b);     rec_particle.fill(rec_particle_b);
        event.getStructure(rec_track_b);        rec_track.fill(rec_track_b);
        event.getStructure(rec_calorimeter_b);  rec_calorimeter.fill(rec_calorimeter_b);
        event.getStructure(rec_scintillator_b); rec_scintillator.fill(rec_scintillator_b);
        // event.getStructure(rec_cherenkov_b);    rec_cherenkov.fill(rec_cherenkov_b);
        event.getStructure(fmt_tracks_b);       fmt_tracks.fill(fmt_tracks_b);
        if (rec_particle.get_nrows()
                + rec_track.get_nrows()
                + rec_calorimeter.get_nrows()
                + rec_scintillator.get_nrows()
                // + rec_cherenkov.get_nrows()
                + fmt_tracks.get_nrows() > 0)
            tree->Fill();
    }
    printf("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
    printf("Read %8d events... Done!\n", c);

    // Clean up.
    f->Close();
    free(in_filename);
    free(out_filename);
    return 0;
}
