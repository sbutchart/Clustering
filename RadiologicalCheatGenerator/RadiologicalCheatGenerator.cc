#include "RadiologicalCheatGenerator.hh"


int RadiologicalCheatGenerator::Run() {
  
  for (fIter=0; fIter<fNEvent; ++fIter) {
    fParticles.clear();
    //enum PType{kAPA, kCPA, kNeut, kKryp, kPlon, kRdon};

    if      (fRequestedBackgroundType == kAr39 ) { GenerateAr39 (); }
    else if (fRequestedBackgroundType == kK40  ) { GenerateK40  (); }
    else if (fRequestedBackgroundType == kAr42 ) { GenerateAr42 (); }
    else if (fRequestedBackgroundType == kNi59 ) { GenerateNi59 (); }
    else if (fRequestedBackgroundType == kCo60 ) { GenerateCo60 (); }
    else if (fRequestedBackgroundType == kKr85 ) { GenerateKr85 (); }kKryp
    else if (fRequestedBackgroundType == kRn222) { GenerateRn222(); }kRdon
    else if (fRequestedBackgroundType == kTh232) { GenerateTh232(); }
    else if (fRequestedBackgroundType == kU238 ) { GenerateU238 (); }kNeut
    else {
      std::cout << "Incorrect background type!!" << std::endl;
      return 1;
    }

    fFile.open(fOutputFileName+Form("_%i",(int)fIter));
    fFile << Form("%i %i",(int)fIter, (int)fParticles.size()) << std::endl;
    for (auto const& it: fParticles)
      PrintParticle(it);
    fFile.close();

  }
  return 0;
}

void RadiologicalCheatGenerator::GenerateAr39() {

  double chan = rand.Uniform(1.);
  std::vector<Part> particle;
  for (auto const& it: fDecays[kAr39]) {
    if (chan<it.BranchingRatio) {
      Part p;
      p.pdg=11;
      p.M = 0.000510999;
      while(p.p<0) {
        p.E = it.Spectrum->GetRandom()*1.e-6 + p.M;
        p.p = p.E*p.E - p.M*p.M;
      }
      p.p = sqrt(p.p);
      FillParticlePositionInArgon(p);
      FillParticleIsotropicDirection(p);
      fParticles.push_back(p);
      break;
    }
  }

}

void RadiologicalCheatGenerator::GenerateK40() {
  double chan = rand.Uniform(1.);
  for (auto const& it: fDecays[kK40]) {
    if (chan<it.BranchingRatio) {
      Part p;
      if (it.Type == kBeta) {
        p.pdg=11;
        p.M = 0.000510999;
      } else {
        p.pdg=22;
        p.M = 0.;
      }
      while(p.p<0) {
        p.E = it.Spectrum->GetRandom()*1.e-6 + p.M;
        p.p = p.E*p.E - p.M*p.M;
      }
      p.p = sqrt(p.p);
      FillParticlePositionInArgon(p);
      FillParticleIsotropicDirection(p);
      fParticles.push_back(p);
      break;
    }
  }
}

void RadiologicalCheatGenerator::GenerateAr42() {

  double chan = rand.Uniform(1.);
  for (auto const& it: fDecays[kAr42]) {
    if (chan<it.BranchingRatio) {
      Part p;
      p.pdg=11;
      p.M = 0.000510999;
      while(p.p<0) {
        p.E = it.Spectrum->GetRandom()*1.e-6 + p.M;
        p.p = p.E*p.E - p.M*p.M;
      }
      p.p = sqrt(p.p);
      FillParticlePositionInArgon(p);
      FillParticleIsotropicDirection(p);
      fParticles.push_back(p);

      for (auto const& gamma: it.GammaEnergies) {
        Part g;
        g.pdg = 22;
        g.M = 0;
        g.E = gamma;
        g.p = gamma;
        g.x = p.x;
        g.y = p.y;
        g.z = p.z;
        FillParticleIsotropicDirection(g);  
        fParticles.push_back(g);
      }
      break;
    }
  }
}

void RadiologicalCheatGenerator::GenerateNi59() {
}

void RadiologicalCheatGenerator::GenerateCo60() {
  Part p;
  p.M = 0;
  p.pdg = 22;
  p.E = fDecays[kCo60].begin()->Spectrum->GetRandom()*1.e-6;
  p.p = p.E;
  FillParticlePositionOnAPA(p);
  FillParticleIsotropicDirection(p);
  fParticles.push_back(p);
  
}

void RadiologicalCheatGenerator::GenerateKr85() {
}

void RadiologicalCheatGenerator::GenerateRn222() {
  Part p;
  double t = 0.00548952;
  p.M = 3.727379;
  p.pdg = 1000020040;
  p.E = t + p.M;
  p.p = sqrt(p.E*p.E - p.M * p.M);
  FillParticlePositionOnPDS(p);
  FillParticleIsotropicDirection(p);
  fParticles.push_back(p);
}

void RadiologicalCheatGenerator::GenerateTh232() {
}

void RadiologicalCheatGenerator::GeneratePo210() {
}

void RadiologicalCheatGenerator::GenerateU238() {
}

void RadiologicalCheatGenerator::FillParticlePositionInArgon(Part& p) {
  p.x = -475. + 950.  * rand.Uniform();
  p.y = -750. + 1550. * rand.Uniform();
  p.z = -55.  + 1550. * rand.Uniform();
}

void RadiologicalCheatGenerator::FillParticlePositionOnPDS(Part& p) {
  p.x =  0.477 + 1. * rand.Uniform();
  p.y = -600.  + 1200. * rand.Uniform();
  p.z = 1395 * rand.Uniform(); 
}

void RadiologicalCheatGenerator::FillParticlePositionOnAPA(Part& p) {
  p.x = -0.5 + 1. * rand.Uniform();
  p.y = -600. + 1200. * rand.Uniform();
  p.z = 1395. * rand.Uniform();
}

void RadiologicalCheatGenerator::FillParticlePositionOnCPA(Part& p) {
 p.x = 349.5 + 1. * rand.Uniform();
 p.y = -600. + 1200. * rand.Uniform();
 p.z = 1395. * rand.Uniform();
}

void RadiologicalCheatGenerator::FillParticleIsotropicDirection(Part& p) {
  double x, y, z;
  rand.Sphere(x,y,z,1);
  p.px = p.p*x;
  p.py = p.p*y;
  p.pz = p.p*z;
}

RadiologicalCheatGenerator::RadiologicalCheatGenerator():
  fNEvent(0),
  fOutputFileName("") {
  fConvertor["Ar39"   ] = kAr39    ;
  fConvertor["K40"    ] = kK40     ;
  fConvertor["Ar42"   ] = kAr42    ;
  // fConvertor["Neutron"] = kNeutron ;
  // fConvertor["CPA"    ] = kCPA     ;
  // fConvertor["Kr85"   ] = kKr85    ;
  fConvertor["Rn222"  ] = kRn222   ;
  fConvertor["Co60"   ] = kCo60   ;
  // fConvertor["Po210"  ] = kPo210   ;
  gRandom = &rand;
  Decay d;
  // ARGON 39
  d.Spectrum = GetSpectrum("Argon_39.root")[0];
  fDecays[kAr39].push_back(d);

  // ARGON 42
  d = Decay();
  d.BranchingRatio = 0.819;
  d.Spectrum = GetSpectrum("Argon_42_1.root")[0];
  fDecays[kAr42].push_back(d);

  d = Decay();
  d.BranchingRatio = 0.9954;
  d.Spectrum = GetSpectrum("Argon_42_2.root")[0];
  d.GammaEnergies.push_back(0.0015246);
  fDecays[kAr42].push_back(d);

  d = Decay();
  d.BranchingRatio = 0.9988;
  d.Spectrum = GetSpectrum("Argon_42_3.root")[0];
  d.GammaEnergies.push_back(0.0003126);
  d.GammaEnergies.push_back(0.0015246);
  fDecays[kAr42].push_back(d);

  d = Decay();
  d.BranchingRatio = 0.99916;
  d.Spectrum = GetSpectrum("Argon_42_4.root")[0];
  d.GammaEnergies.push_back(0.0008997);
  d.GammaEnergies.push_back(0.0015246);
  fDecays[kAr42].push_back(d);

  d = Decay();
  d.BranchingRatio = 0.9993;
  d.Spectrum = GetSpectrum("Argon_42_4.root")[0];
  d.GammaEnergies.push_back(0.0024243);
  fDecays[kAr42].push_back(d);

  d = Decay();
  d.BranchingRatio = 0.999335;
  d.Spectrum = GetSpectrum("Argon_42_5.root")[0];
  d.GammaEnergies.push_back(0.000692);
  d.GammaEnergies.push_back(0.001228);
  fDecays[kAr42].push_back(d);

  d = Decay();
  d.BranchingRatio = 0.99955;
  d.Spectrum = GetSpectrum("Argon_42_5.root")[0];
  d.GammaEnergies.push_back(0.0010212);
  d.GammaEnergies.push_back(0.0024243);
  fDecays[kAr42].push_back(d);

  d = Decay();
  d.Spectrum = GetSpectrum("Argon_42_5.root")[0];
  d.GammaEnergies.push_back(0.0019208);
  d.GammaEnergies.push_back(0.0015246);
  fDecays[kAr42].push_back(d);

  // COBALT 60 on APA
  d = Decay();
  d.Spectrum = GetSpectrum("Cobalt_60.root")[0];
  d.Type = kGamma;
  fDecays[kCo60].push_back(d);
  
  // POTASSIUM 40 on CPA
  d = Decay();
  std::vector<TH1D*> spectra = GetSpectrum("Potassium_40.root");
  d.Spectrum = spectra[0];
  d.BranchingRatio = spectra[0]->Integral() / (spectra[0]->Integral() +
                                               spectra[1]->Integral());
  d.Type = kBeta;
  fDecays[kK40].push_back(d);

  d = Decay();
  d.Type = kGamma;
  d.Spectrum = spectra[1];

  fDecays[kK40].push_back(d);
  
  // RADON 222 ON PDS
  d.BranchingRatio = 1;
  d.Type = kAlpha;
  d.Spectrum = NULL;
  fDecays[kRn222].push_back(d);
 
};


void RadiologicalCheatGenerator::PrintParticle(const Part& p) {
  fFile << Form("1 %i 0 0 0 0 %f %f %f %f %f %f %f %f 0.", p.pdg, p.px, p.py, p.pz, p.E, p.M, p.x, p.y, p.z) << std::endl;

}
