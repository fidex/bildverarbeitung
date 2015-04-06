//------------------------------------------------------------------------
// Programm "Entzerren"
//   Unterprogramm zum Einlesen der Kameraparameter
//
// B.Lang, HS Osnabrueck
//------------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <stdlib.h>
using namespace std;

void Kameraparameter_einlesen(
      const string& ParameterFile,  // parameter file name
      double intrinsic_d[3][3],     // intrinsics
      double distCoeffs[5],         // distortion
      double rotVect[3],            // rotation
      double transVect[3]           // translation
) {
  double fx,fy;
  double cx,cy;
  double k1,k2,k3;
  double p1,p2;
  double rx,ry,rz;
  double dummy;
  string separator, line;
  ifstream infile(ParameterFile.c_str());

  if(!infile.is_open()) {
	cerr << "Konnte Parameterdatei nicht oeffnen." << endl;
	exit(1);
  }

  getline(infile,line);
  infile >> fx    >> separator >> dummy >> separator >> cx;  getline(infile,line);
  infile >> dummy >> separator >> fy    >> separator >> cy;  getline(infile,line);
  getline(infile,line);
  getline(infile,line);
  infile >> k1 >> separator >> k2 >> separator >> k3;  getline(infile,line);
  getline(infile,line);
  infile >> p1 >> separator >> p2;  getline(infile,line);
  getline(infile,line);
  infile >> rx >> separator >> ry >> separator >> rz;  getline(infile,line);

  cout << "Eingelesene Kameraparameter: " << endl;
  cout << "fx,fy,cx,cy: " << fx << "," << fy << "," << cx << "," << cy << endl;
  cout << "k1,k2,k3:    " << k1 << "," << k2 << "," << k3 << endl;
  cout << "p1,p2:       " << p1 << "," << p2 << endl;
  cout << "rx,ry,rz:    " << rx << "," << ry << "," << rz << endl;
  cout << endl;

  intrinsic_d[0][0] = fx;
  intrinsic_d[0][1] = 0.0;
  intrinsic_d[0][2] = cx;
  intrinsic_d[1][0] = 0.0;
  intrinsic_d[1][1] = fy;
  intrinsic_d[1][2] = cy;
  intrinsic_d[2][0] = 0.0;
  intrinsic_d[2][1] = 0.0;
  intrinsic_d[2][2] = 1.0;
  distCoeffs[0] = k1;
  distCoeffs[1] = k2;
  distCoeffs[2] = p1;
  distCoeffs[3] = p2;
  distCoeffs[4] = k3;
  rotVect[0]    = rx;
  rotVect[1]    = ry;
  rotVect[2]    = rz;
}
