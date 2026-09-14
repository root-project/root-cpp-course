#ifndef __MYVECTOR_H__
#define __MYVECTOR_H__

#include "Rtypes.h" // for the ClassDef macro

using fp_t = double;

class myVector {
public:
   myVector() {}
   myVector(fp_t x, fp_t y, fp_t z) : m_x(x), m_y(y), m_z(z), m_mag(0) {}
   fp_t X() { return m_x; }
   fp_t Y() { return m_y; }
   fp_t Z() { return m_z; }
   fp_t Mag() { return m_mag; }
   void SetX(fp_t v) { m_x = v; }
   void SetY(fp_t v) { m_y = v; }
   void SetZ(fp_t v) { m_z = v; }
   void SetMag(fp_t v) { m_mag = v; }
   void Print();

private:
   fp_t m_x = 0.;
   fp_t m_y = 0.;
   fp_t m_z = 0.;
   fp_t m_mag = 0.; // !

public:
   ClassDef(myVector, 3);
};

#endif
