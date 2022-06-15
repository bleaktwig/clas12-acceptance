#include "../lib/bank_containers.h"

// TODO. This file could use a lot of improvement using interfaces and smart array handling.
// TODO. All strings here should be handled by `constants.h`.
// Write.
REC_Particle::REC_Particle() { // TODO. Can be made *partialy* generic.
    nrows = 0;
    bank_name = "REC::Particle";
    data = {
            {"pid",     {nullptr, {}}}, // particle id in LUND conventions.
            {"px",      {nullptr, {}}}, // x component of the momentum (GeV).
            {"py",      {nullptr, {}}}, // y component of the momentum (GeV).
            {"pz",      {nullptr, {}}}, // z component of the momentum (GeV).
            {"vx",      {nullptr, {}}}, // x component of the vertex (cm).
            {"vy",      {nullptr, {}}}, // y component of the vertex (cm).
            {"vz",      {nullptr, {}}}, // z component of the vertex (cm).
            {"vt",      {nullptr, {}}}, // RF and z corrected vertex time (ns).
            {"charge",  {nullptr, {}}}, // particle charge.
            {"beta",    {nullptr, {}}}, // particle beta measured by TOF.
            {"chi2pid", {nullptr, {}}}, // Chi2 of assigned PID.
            {"status",  {nullptr, {}}}, // Detector collection particle passed.
    };
}
int REC_Particle::set_nrows(int in_nrows) { // TODO. Can be made generic!
    nrows = in_nrows;
    for (it = data.begin(); it != data.end(); ++it) it->second.second->resize(nrows);
    return 0;
}
int REC_Particle::create_branches(TTree *t) { // TODO. Can be made generic!
    for (it = data.begin(); it != data.end(); ++it)
        t->Branch(Form("%s::%s", bank_name, it->first), &(it->second.second));
    return 0;
}
int REC_Particle::fill(hipo::bank b) { // TODO. Cannot be made generic :(.
    set_nrows(b.getRows());
    for (int row = 0; row < nrows; ++row) {
        data["pid"]    .second->at(row) = (Float_t) b.getInt  ("pid",     row);
        data["px"]     .second->at(row) = b.getFloat("px",      row);
        data["py"]     .second->at(row) = b.getFloat("py",      row);
        data["pz"]     .second->at(row) = b.getFloat("pz",      row);
        data["vx"]     .second->at(row) = b.getFloat("vx",      row);
        data["vy"]     .second->at(row) = b.getFloat("vy",      row);
        data["vz"]     .second->at(row) = b.getFloat("vz",      row);
        data["vt"]     .second->at(row) = b.getFloat("vt",      row);
        data["beta"]   .second->at(row) = b.getFloat("beta",    row);
        data["chi2pid"].second->at(row) = b.getFloat("chi2pid", row);
        data["charge"] .second->at(row) = (Float_t) b.getByte ("charge", row);
        data["status"] .second->at(row) = (Float_t) b.getShort("status", row);
    }
    return 0;
}

// Read.
REC_Particle::REC_Particle(TTree *t) { // TODO. Can be made *partially* generic!
    nrows = 0;
    bank_name = "REC::Particle";
    data = {
            {"pid",     {nullptr, nullptr}}, // particle id in LUND conventions.
            {"px",      {nullptr, nullptr}}, // x component of the momentum (GeV).
            {"py",      {nullptr, nullptr}}, // y component of the momentum (GeV).
            {"pz",      {nullptr, nullptr}}, // z component of the momentum (GeV).
            {"vx",      {nullptr, nullptr}}, // x component of the vertex (cm).
            {"vy",      {nullptr, nullptr}}, // y component of the vertex (cm).
            {"vz",      {nullptr, nullptr}}, // z component of the vertex (cm).
            {"vt",      {nullptr, nullptr}}, // RF and z corrected vertex time (ns).
            {"charge",  {nullptr, nullptr}}, // particle charge.
            {"beta",    {nullptr, nullptr}}, // particle beta measured by TOF.
            {"chi2pid", {nullptr, nullptr}}, // Chi2 of assigned PID.
            {"status",  {nullptr, nullptr}}, // Detector collection particle passed.
    };

    for (it = data.begin(); it != data.end(); ++it)
        t->SetBranchAddress(Form("%s::%s", bank_name, it->first),
                            &(it->second.second), &(it->second.first));
}
int REC_Particle::get_nrows() {return nrows;} // TODO. Can be made generic!
int REC_Particle::get_entries(TTree *t, int idx) { // TODO. Can be made generic!
    for (it = data.begin(); it != data.end(); ++it) it->second.first->GetEntry(t->LoadTree(idx));
    return 0;
}

REC_Track::REC_Track() {
    nrows  = 0;
    index  = {};
    pindex = {};
    sector = {};
    ndf    = {};
    chi2   = {};
}
REC_Track::REC_Track(TTree *t) {
    index  = nullptr; b_index  = nullptr;
    pindex = nullptr; b_pindex = nullptr;
    sector = nullptr; b_sector = nullptr;
    ndf    = nullptr; b_ndf    = nullptr;
    chi2   = nullptr; b_chi2   = nullptr;
    t->SetBranchAddress("REC::Track::index",  &index,  &b_index);
    t->SetBranchAddress("REC::Track::pindex", &pindex, &b_pindex);
    t->SetBranchAddress("REC::Track::sector", &sector, &b_sector);
    t->SetBranchAddress("REC::Track::ndf",    &ndf,    &b_ndf);
    t->SetBranchAddress("REC::Track::chi2",   &chi2,   &b_chi2);
}
int REC_Track::create_branches(TTree *t) {
    t->Branch("REC::Track::index",  &index);
    t->Branch("REC::Track::pindex", &pindex);
    t->Branch("REC::Track::sector", &sector);
    t->Branch("REC::Track::ndf",    &ndf);
    t->Branch("REC::Track::chi2",   &chi2);
    return 0;
}
int REC_Track::set_nrows(int in_nrows) {
    nrows = in_nrows;
    index ->resize(nrows);
    pindex->resize(nrows);
    sector->resize(nrows);
    ndf   ->resize(nrows);
    chi2  ->resize(nrows);
    return 0;
}
int REC_Track::get_nrows() {return nrows;}
int REC_Track::fill(hipo::bank b) {
    set_nrows(b.getRows());
    for (int row = 0; row < nrows; ++row) {
        index ->at(row) = (int16_t) b.getShort("index",  row);
        pindex->at(row) = (int16_t) b.getShort("pindex", row);
        sector->at(row) = (int8_t)  b.getByte ("sector", row);
        ndf   ->at(row) = (int16_t) b.getShort("NDF",    row);
        chi2  ->at(row) = b.getFloat("chi2", row);
    }
    return 0;
}
int REC_Track::get_entries(TTree *t, int idx) {
    b_index ->GetEntry(t->LoadTree(idx));
    b_pindex->GetEntry(t->LoadTree(idx));
    b_sector->GetEntry(t->LoadTree(idx));
    b_ndf   ->GetEntry(t->LoadTree(idx));
    b_chi2  ->GetEntry(t->LoadTree(idx));
    return 0;
}

REC_Calorimeter::REC_Calorimeter() {
    nrows = 0;
    pindex = {};
    layer  = {};
    sector = {};
    energy = {};
}
REC_Calorimeter::REC_Calorimeter(TTree *t) {
    pindex = nullptr; b_pindex = nullptr;
    layer  = nullptr; b_layer  = nullptr;
    sector = nullptr; b_sector = nullptr;
    energy = nullptr; b_energy = nullptr;
    t->SetBranchAddress("REC::Calorimeter::pindex", &pindex, &b_pindex);
    t->SetBranchAddress("REC::Calorimeter::layer",  &layer,  &b_layer);
    t->SetBranchAddress("REC::Calorimeter::sector", &sector, &b_sector);
    t->SetBranchAddress("REC::Calorimeter::energy", &energy, &b_energy);
}
int REC_Calorimeter::create_branches(TTree *t) {
    t->Branch("REC::Calorimeter::pindex", &pindex);
    t->Branch("REC::Calorimeter::layer",  &layer);
    t->Branch("REC::Calorimeter::sector", &sector);
    t->Branch("REC::Calorimeter::energy", &energy);
    return 0;
}
int REC_Calorimeter::set_nrows(int in_nrows) {
    nrows = in_nrows;
    pindex->resize(nrows);
    layer ->resize(nrows);
    sector->resize(nrows);
    energy->resize(nrows);
    return 0;
}
int REC_Calorimeter::get_nrows() {return nrows;}
int REC_Calorimeter::fill(hipo::bank b) {
    set_nrows(b.getRows());
    for (int row = 0; row < nrows; ++row) {
        pindex->at(row) = (int16_t) b.getShort("pindex", row);
        layer ->at(row) = (int8_t)  b.getByte ("layer",  row);
        sector->at(row) = (int8_t)  b.getByte ("sector", row);
        energy->at(row) = b.getFloat("energy", row);
    }
    return 0;
}
int REC_Calorimeter::get_entries(TTree *t, int idx) {
    b_pindex->GetEntry(t->LoadTree(idx));
    b_layer ->GetEntry(t->LoadTree(idx));
    b_sector->GetEntry(t->LoadTree(idx));
    b_energy->GetEntry(t->LoadTree(idx));
    return 0;
}

REC_Scintillator::REC_Scintillator() {
    nrows  = 0;
    pindex = {};
    time   = {};
}
REC_Scintillator::REC_Scintillator(TTree *t) {
    pindex = nullptr; b_pindex = nullptr;
    time   = nullptr; b_time   = nullptr;
    t->SetBranchAddress("REC::Scintillator::pindex", &pindex, &b_pindex);
    t->SetBranchAddress("REC::Scintillator::time",   &time,   &b_time);
}
int REC_Scintillator::create_branches(TTree *t) {
    t->Branch("REC::Scintillator::pindex", &pindex);
    t->Branch("REC::Scintillator::time",   &time);
    return 0;
}
int REC_Scintillator::set_nrows(int in_nrows) {
    nrows = in_nrows;
    pindex->resize(nrows);
    time  ->resize(nrows);
    return 0;
}
int REC_Scintillator::get_nrows() {return nrows;}
int REC_Scintillator::fill(hipo::bank b) {
    set_nrows(b.getRows());
    for (int row = 0; row < nrows; ++row) {
        pindex->at(row) = (int16_t) b.getShort("pindex", row);
        time  ->at(row) = b.getFloat("time", row);
    }
    return 0;
}
int REC_Scintillator::get_entries(TTree *t, int idx) {
    b_pindex->GetEntry(t->LoadTree(idx));
    b_time  ->GetEntry(t->LoadTree(idx));
    return 0;
}

// REC_Cherenkov::REC_Cherenkov() {
//     nrows  = 0;
//     pindex = {};
//     nphe   = {};
// }
// REC_Cherenkov::REC_Cherenkov(TTree *t) {
//     pindex   = nullptr; b_pindex   = nullptr;
//     detector = nullptr; b_detector = nullptr;
//     nphe     = nullptr; b_nphe     = nullptr;
//     t->SetBranchAddress("REC::Cherenkov::pindex",   &pindex,   &b_pindex);
//     t->SetBranchAddress("REC::Cherenkov::detector", &detector, &b_detector);
//     t->SetBranchAddress("REC::Cherenkov::nphe",     &nphe,     &b_nphe);
// }
// int REC_Cherenkov::create_branches(TTree *t) {
//     t->Branch("REC::Cherenkov::pindex",   &pindex);
//     t->Branch("REC::Cherenkov::detector", &detector);
//     t->Branch("REC::Cherenkov::nphe",     &nphe);
//     return 0;
// }
// int REC_Cherenkov::set_nrows(int in_nrows) {
//     nrows = in_nrows;
//     pindex  ->resize(nrows);
//     detector->resize(nrows);
//     nphe    ->resize(nrows);
//     return 0;
// }
// int REC_Cherenkov::get_nrows() {return nrows;}
// int REC_Cherenkov::fill(hipo::bank b) {
//     set_nrows(b.getRows());
//     for (int row = 0; row < nrows; ++row) {
//         pindex  ->at(row) = (int16_t) b.getShort("pindex", row);
//         detector->at(row) = (int8_t)  b.getByte("detector", row);
//         nphe    ->at(row) = b.getFloat("nphe", row);
//     }
//     return 0;
// }
// int REC_Cherenkov::get_entries(TTree *t, int idx) {
//     b_pindex  ->GetEntry(t->LoadTree(idx));
//     b_detector->GetEntry(t->LoadTree(idx));
//     b_nphe    ->GetEntry(t->LoadTree(idx));
//     return 0;
// }

FMT_Tracks::FMT_Tracks() {
    index = {};
    ndf   = {};
    vx    = {};
    vy    = {};
    vz    = {};
    px    = {};
    py    = {};
    pz    = {};
}
FMT_Tracks::FMT_Tracks(TTree *t) {
    index = nullptr; b_index = nullptr;
    ndf   = nullptr; b_ndf   = nullptr;
    vx    = nullptr; b_vx    = nullptr;
    vy    = nullptr; b_vy    = nullptr;
    vz    = nullptr; b_vz    = nullptr;
    px    = nullptr; b_px    = nullptr;
    py    = nullptr; b_py    = nullptr;
    pz    = nullptr; b_pz    = nullptr;
    t->SetBranchAddress("FMT::Tracks::index", &index, &b_index);
    t->SetBranchAddress("FMT::Tracks::ndf",   &ndf,   &b_ndf);
    t->SetBranchAddress("FMT::Tracks::vx",    &vx,    &b_vx);
    t->SetBranchAddress("FMT::Tracks::vy",    &vy,    &b_vy);
    t->SetBranchAddress("FMT::Tracks::vz",    &vz,    &b_vz);
    t->SetBranchAddress("FMT::Tracks::px",    &px,    &b_px);
    t->SetBranchAddress("FMT::Tracks::py",    &py,    &b_py);
    t->SetBranchAddress("FMT::Tracks::pz",    &pz,    &b_pz);
}
int FMT_Tracks::create_branches(TTree *t) {
    t->Branch("FMT::Tracks::index", &index);
    t->Branch("FMT::Tracks::ndf",   &ndf);
    t->Branch("FMT::Tracks::vx",    &vx);
    t->Branch("FMT::Tracks::vy",    &vy);
    t->Branch("FMT::Tracks::vz",    &vz);
    t->Branch("FMT::Tracks::px",    &px);
    t->Branch("FMT::Tracks::py",    &py);
    t->Branch("FMT::Tracks::pz",    &pz);
    return 0;
}
int FMT_Tracks::set_nrows(int in_nrows) {
    nrows = in_nrows;
    index->resize(nrows);
    ndf  ->resize(nrows);
    vx   ->resize(nrows);
    vy   ->resize(nrows);
    vz   ->resize(nrows);
    px   ->resize(nrows);
    py   ->resize(nrows);
    pz   ->resize(nrows);
    return 0;
}
int FMT_Tracks::get_nrows() {return nrows;}
int FMT_Tracks::fill(hipo::bank b) {
    set_nrows(b.getRows());
    for (int row = 0; row < nrows; ++row) {
        index->at(row) = (int16_t) b.getShort("index", row);
        ndf  ->at(row) = b.getInt("NDF", row);
        vx   ->at(row) = b.getFloat("Vtx0_x", row);
        vy   ->at(row) = b.getFloat("Vtx0_y", row);
        vz   ->at(row) = b.getFloat("Vtx0_z", row);
        px   ->at(row) = b.getFloat("p0_x",   row);
        py   ->at(row) = b.getFloat("p0_y",   row);
        pz   ->at(row) = b.getFloat("p0_z",   row);
    }
    return 0;
}
int FMT_Tracks::get_entries(TTree *t, int idx) {
    b_index->GetEntry(t->LoadTree(idx));
    b_ndf  ->GetEntry(t->LoadTree(idx));
    b_vx   ->GetEntry(t->LoadTree(idx));
    b_vy   ->GetEntry(t->LoadTree(idx));
    b_vz   ->GetEntry(t->LoadTree(idx));
    b_px   ->GetEntry(t->LoadTree(idx));
    b_py   ->GetEntry(t->LoadTree(idx));
    b_pz   ->GetEntry(t->LoadTree(idx));
    return 0;
}
