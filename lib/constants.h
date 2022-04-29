#ifndef CONSTANTS
#define CONSTANTS

// Masses.
#define PRTMASS 0.938272 // Proton mass.
#define KMASS   0.493677 // Kaon mass.
#define PIMASS  0.139570 // Pion mass.
#define DMASS   1.875    // Deuterium mass.
#define NTRMASS 0.939565 // Neutron mass.
#define EMASS   0.000051 // Electron mass.

// Particle map keys.
#define PALL "All particles"
#define PPOS "Positive particles"
#define PNEG "Negative particles"
#define PNEU "Neutral particles"
#define PPIP "Pi+"
#define PPIM "Pi-"
#define PELC "e-"
#define PTRE "Trigger e-"

// Histogram map keys.
#define VZ       "Vz"
#define PHI      "#phi"
#define VZPHI    "Vz vs #phi"
#define THETA    "#theta"
#define VZTHETA  "Vz vs #theta"

#define VP       "Vp"
#define BETA     "#beta"
#define VPBETA   "Vp vs #beta"

#define DTOF     "#Delta TOF"
#define VPTOF    "Vp vs #Delta TOF"

#define E        "E"
#define EDIVP    "E/Vp"
#define PEDIVP   "Vp vs E/Vp"
#define EEDIVP   "E vs E/Vp"
#define PPCALE   "Vp vs E (PCAL)"
#define PECINE   "Vp vs E (ECIN)"
#define PECOUE   "Vp vs E (ECOU)"
#define ECALPCAL "E (ECAL) vs E (PCAL)"

// Sampling fraction.
extern const char *CALNAME[4]; // Calorimeters names.
extern const char *SFARR2D[4]; // Sampling Fraction (SF) 2D arr names.
extern const char *SFARR1D[4]; // SF 1D arr names.
extern const double PLIMITSARR[4][2]; // Momentum limits for 1D SF fits.
#define PCAL_IDX 0 // PCAL idx in Sampling fraction arrays.
#define ECIN_IDX 1 // ECIN idx in Sampling fraction arrays.
#define ECOU_IDX 2 // ECOU idx in Sampling fraction arrays.
#define CALS_IDX 3 // CALs idx in Sampling fraction arrays.
#define SF_PMIN  1.0 // GeV
#define SF_PMAX  9.0 // GeV
#define SF_PSTEP 0.4 // GeV
#define SF_CHI2CONFORMITY 2 // NOTE. This is a source of systematic error!

#define Q2_STR "Q^{2}"
#define NU_STR "#nu"
#define XB_STR "X_{bjorken}"
#define W2_STR  "W^{2}"
#define Q2NU_STR "Q^{2} vs #nu"

// RUN CONSTANTS (TODO. these should be in a map.)
#define BE11983 10.3894 //  50 nA.
#define BE12016 10.3894 // 250 nA.
#define BE12439  2.1864 //  15 nA.

// DETECTOR CONSTANTS
#define NSECTORS 6 // # of CLAS12 sectors.
#define PCAL_LYR 1 // PCAL's layer id.
#define ECIN_LYR 4 // EC inner layer id.
#define ECOU_LYR 7 // EC outer layer id.

#endif