#ifndef Kameraparameter_einlesen_H
#define Kameraparameter_einlesen_H
//------------------------------------------------------------------------
// Programm "Entzerren"
//   Prototyp zum Unterprogramm zum Einlesen der Kameraparameter
//
// B.Lang, HS Osnabrueck
//------------------------------------------------------------------------

void Kameraparameter_einlesen(
      const string& ParameterFile,  // parameter file name
      double intrinsic_d[3][3],     // intrinsics
      double distCoeffs[5],         // distortion
      double rotVect[3],            // rotation
      double transVect[3]           // translation
);

#endif
