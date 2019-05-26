template < int N >
using ap_int = int;

void md(double force_x[256], double force_y[256], double force_z[256], double position_x[256], double position_y[256], double position_z[256], ap_int<32> nl[4096]) {

  double delx = 0.0;
  double dely = 0.0;
  double delz = 0.0;
  double r2inv = 0.0;
  double r6inv = 0.0;
  double potential = 0.0;
  double force = 0.0;
  double jx = 0.0;
  double jy = 0.0;
  double jz = 0.0;
  double ix = 0.0;
  double iy = 0.0;
  double iz = 0.0;
  double fx = 0.0;
  double fy = 0.0;
  double fz = 0.0;
  ap_int<32> j_idx = 0;
  double lj1 = 1.5;
  double lj2 = 2.0;
  for(int i = 0; i < 256; i++) {
    ix = position_x[i];
    //---
    iy = position_y[i];
    //---
    iz = position_z[i];
    //---
    fx = 0.0;
    fy = 0.0;
    fz = 0.0;
    for(int j = 0; j < 16; j++) {
      j_idx = nl[((i * 16) + j)];
      jx = position_x[j_idx];
      //---
      jy = position_y[j_idx];
      //---
      jz = position_z[j_idx];
      //---
      delx = (ix - jx);
      dely = (iy - jy);
      delz = (iz - jz);
      r2inv = (1.0 / ((delx * delx) + ((dely * dely) + (delz * delz))));
      r6inv = (r2inv * (r2inv * r2inv));
      potential = (r6inv * ((lj1 * r6inv) - lj2));
      force = (r2inv * potential);
      fx = (fx + (delx * force));
      fy = (fy + (dely * force));
      fz = (fz + (delz * force));
    }
    force_x[i] = fx;
    force_y[i] = fy;
    force_z[i] = fz;
  }
}
