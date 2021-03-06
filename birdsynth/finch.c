/*
 * Slightly edited by Paul Ecoffet, using the code from the DYNAMICAL SYSTEMS
 * LAB Physics Department, Universidad de Buenos Aires Changed so that the synth
 * receives its parameters from arguments and not files. I tried to make minimal
 * modifications to the code from the DYNAMICAL SYSTEMS LAB except from the I/O
 * part.
 * I have little understanding on how the synthesizer is implemented.
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "finch.h"
#include "rk4.h"

#ifndef VERBOSE
#define VERBOSE 0
#endif

#define PI2 6.2831853
#define IA 16807
#define IM 2147483647
#define AM (1.0 / IM)
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1 + (IM - 1) / NTAB)
#define EPS 1.2e-7
#define RNMX (1.0 - EPS)
#define Nforzante 100
#define Nforzados 100
#define N_NAME1_AUX 30
#define N_NAME2_AUX 30
#define S(x) 1 / (1 + exp(-(x / 0.015)))

#define c 35000

struct Par {
  double A1;
  double A2;
  double A3;
  double Ancho1;
  double Ancho2;
  double Ancho3;
  double r;
  double gamma2;
  double gamma3;
  double gm;
  double forcing1;
  double forcing2;
  double alfa1;
  double alfa2;
  double beta1;
  double beta2;
  double beta3;
  double alfa3;
  double RBoverLB;
  double s1overLG;
  double s1overLB;
  double s1overCH;
  double LGoverLB;
  double RB;
  double noise;
  double rdis;
  double amplitud;
} aa;

/**

Init aa with zeros everywhere because otherwise synth is inconsistent.
Indeed, otherwise the initial code from the university of Buenos Aires uses
unintialised values.

It is an quick and dirty fix of the code. I really don't want to change the
real code a lot.

-- Paul Ecoffet
*/
static void init_aa() {
  // aa is global.
  aa.A1 = 0;
  aa.A2 = 0;
  aa.A3 = 0;
  aa.Ancho1 = 0;
  aa.Ancho2 = 0;
  aa.Ancho3 = 0;
  aa.r = 0;
  aa.gamma2 = 0;
  aa.gamma3 = 0;
  aa.gm = 0;
  aa.forcing1 = 0;
  aa.forcing2 = 0;
  aa.alfa1 = 0;
  aa.alfa2 = 0;
  aa.beta1 = 0;
  aa.beta2 = 0;
  aa.beta3 = 0;
  aa.alfa3 = 0;
  aa.RBoverLB = 0;
  aa.s1overLG = 0;
  aa.s1overLB = 0;
  aa.s1overCH = 0;
  aa.LGoverLB = 0;
  aa.RB = 0;
  aa.noise = 0;
  aa.rdis = 0;
  aa.amplitud = 0;
}

static void print_aa() {
  printf("aa.A1 = %lf;\n", aa.A1);
  printf("aa.A2 = %lf;\n", aa.A2);
  printf("aa.A3 = %lf;\n", aa.A3);
  printf("aa.Ancho1 = %lf;\n", aa.Ancho1);
  printf("aa.Ancho2 = %lf;\n", aa.Ancho2);
  printf("aa.Ancho3 = %lf;\n", aa.Ancho3);
  printf("aa.r = %lf;\n", aa.r);
  printf("aa.gamma2 = %lf;\n", aa.gamma2);
  printf("aa.gamma3 = %lf;\n", aa.gamma3);
  printf("aa.gm = %lf;\n", aa.gm);
  printf("aa.forcing1 = %lf;\n", aa.forcing1);
  printf("aa.forcing2 = %lf;\n", aa.forcing2);
  printf("aa.alfa1 = %lf;\n", aa.alfa1);
  printf("aa.alfa2 = %lf;\n", aa.alfa2);
  printf("aa.beta1 = %lf;\n", aa.beta1);
  printf("aa.beta2 = %lf;\n", aa.beta2);
  printf("aa.beta3 = %lf;\n", aa.beta3);
  printf("aa.alfa3 = %lf;\n", aa.alfa3);
  printf("aa.RBoverLB = %lf;\n", aa.RBoverLB);
  printf("aa.s1overLG = %lf;\n", aa.s1overLG);
  printf("aa.s1overLB = %lf;\n", aa.s1overLB);
  printf("aa.s1overCH = %lf;\n", aa.s1overCH);
  printf("aa.LGoverLB = %lf;\n", aa.LGoverLB);
  printf("aa.RB = %lf;\n", aa.RB);
  printf("aa.noise = %lf;\n", aa.noise);
  printf("aa.rdis = %lf;\n", aa.rdis);
  printf("aa.amplitud = %lf;\n", aa.amplitud);
  printf("---------------------\n");
}

static void print_v(double v[]) {
  int i = 0;
  for (i = 0; i < 10; i++) {
    printf("v[%d] = %.30e;\n", i, v[i]);
  }
}

void takens(int n, double v[], double dv[], double t) {
  extern double PRESSURE, presion, acople;
  double x, y, i1, i2, i3, w2, alfa = 50, x2, y2, x3, y3;

  x = v[0];
  y = v[1], i1 = v[2], i2 = v[3];
  i3 = v[4];
  x2 = v[5], y2 = v[6], w2 = v[7];
  x3 = v[8];
  y3 = v[9];

  dv[0] = y;
  dv[1] = aa.alfa1 * aa.gm * aa.gm +
          aa.beta1 * aa.gm * aa.gm * (x)-aa.gm * aa.gm * x * x * x -
          aa.gm * x * x * y + aa.gm * aa.gm * x * x - aa.gm * x * y;

  dv[2] = i2;
  dv[3] = -aa.s1overLG * aa.s1overCH * i1 -
          aa.rdis * (aa.s1overLB + aa.s1overLG) * i2 +
          i3 * (aa.s1overLG * aa.s1overCH -
                aa.rdis * aa.RB * aa.s1overLG * aa.s1overLB) +
          (aa.s1overLG) * aa.forcing2 +
          aa.rdis * aa.s1overLG * aa.s1overLB * aa.forcing1;
  dv[4] = -(1 / aa.s1overLG) * aa.s1overLB * i2 - aa.RB * aa.s1overLB * i3 +
          aa.s1overLB * aa.forcing1;

  dv[5] = y2;
  dv[6] = aa.alfa2 * aa.gm * aa.gm +
          aa.beta2 * aa.gm * aa.gm * (x2)-aa.gm * aa.gm * x2 * x2 * x2 -
          aa.gm * x2 * x2 * y2 + aa.gm * aa.gm * x2 * x2 - aa.gm * x2 * y2;
  dv[7] = 0.;

  dv[8] = y3;
  dv[9] = -1. * aa.gamma2 * aa.gamma2 * aa.alfa3 * x3 +
          aa.gamma2 * aa.beta3 * y3 -
          aa.gamma3 * aa.gamma2 * 2.0 * (40) * (40) * x3 * x3 * y3;

  return;
}

float ran1(long *idum) {
  int j;
  long k;
  static long iy = 0;
  static long iv[NTAB];
  float temp;

  if (*idum <= 0 || !iy) {
    if (-(*idum) < 1)
      *idum = 1;
    else
      *idum = -(*idum);
    for (j = NTAB + 7; j >= 0; j--) {
      k = (*idum) / IQ;
      *idum = IA * (*idum - k * IQ) - IR * k;
      if (*idum < 0)
        *idum += IM;
      if (j < NTAB)
        iv[j] = *idum;
    }
    iy = iv[0];
  }
  k = (*idum) / IQ;
  *idum = IA * (*idum - k * IQ) - IR * k;
  if (*idum < 0)
    *idum += IM;
  j = iy / NDIV;
  iy = iv[j];
  iv[j] = *idum;
  if ((temp = AM * iy) > RNMX)
    return RNMX;
  else
    return temp;
}

double PRESSURE = 0.0;
double presion, acople;

void finch(double *alpha, double *beta, int size, double *out) {

  long idum = (-1);
  int i, j, taux, iii, ik, ikp;
  int t, to, maxtau, tau1, tau2, tau3;
  double r12, r23, r21, r32, t12, t21, t23, t32, r34, t34, tiempot, tnoise,
      tcount;
  double *a, *bf, *bb, *cf, *cb, *df, *db;
  double ancho1, ancho2, ancho3, largo1, largo2, largo3;
  double dt, ddb, dbold, nada, atenua;
  double v[10];
  double preout;
  char FileName[19];
  int i_cur = 0;

  char nombre_archivo1[N_NAME1_AUX];
  init_aa();
  for (ik = 1; ik < 2; ik++) {
    for (ikp = 1; ikp < 2; ikp++) {
      //*
      v[0] = .000000000005;
      v[1] = 0.00000000001;
      v[2] = .000000000001;
      v[3] = 0.00000000001;
      v[4] = .000000000001;

      v[5] = .0000000000045;
      v[6] = .000000000000001;

      v[7] = 0.;
      v[8] = 0.1;
      v[9] = 0.0;
      //*/
      /*
      v[0] = 6.305617549019946199706510014948e-01;
      v[1] = -6.019296353020359902075142599642e+01;
      v[2] = 2.555291041683124261172071600007e-07;
      v[3] = 7.742143094569151062778189498204e-03;
      v[4] = -5.082011184634870809836110822804e-10;
      v[5] = 1.189966633002718543465903167089e+00;
      v[6] = 3.339289417720148453816419110514e-11;
      v[7] = 0.000000000000000000000000000000e+00;
      v[8] = 1.043894773575649356460243774515e-26;
      v[9] = -6.753616673193740042845268300017e-23;
      //*/
      aa.forcing1 = 0.;
      aa.forcing2 = 0.;
      tiempot = 0.;
      tcount = 0;
      aa.noise = 0;
      tnoise = 0;
      aa.r = 0.4;
      aa.rdis = 7000;
      aa.gamma2 = 1.;
      aa.gamma3 = 1.;
      atenua = 1;
      dt = 1. / (882000);

      to = size * 20;

      a = (double *)calloc(to, sizeof(double));
      if (a == NULL) {
        fprintf(stderr, "Allocation error, impossible to alloc %ld bytes for a",
                to * sizeof(double));
        exit(-1);
      }
      bf = (double *)calloc(to, sizeof(double));
      if (bf == NULL) {
        fprintf(stderr,
                "Allocation error, impossible to alloc %ld bytes for bf",
                to * sizeof(double));
        exit(-1);
      }
      bb = (double *)calloc(to, sizeof(double));
      if (bb == NULL) {
        fprintf(stderr,
                "Allocation error, impossible to alloc %ld bytes for bb",
                to * sizeof(double));
        exit(-1);
      }
      cf = (double *)calloc(to, sizeof(double));
      if (cf == NULL) {
        fprintf(stderr,
                "Allocation error, impossible to alloc %ld bytes for cf",
                to * sizeof(double));
        exit(-1);
      }
      cb = (double *)calloc(to, sizeof(double));
      if (cb == NULL) {
        fprintf(stderr,
                "Allocation error, impossible to alloc %ld bytes for cb",
                to * sizeof(double));
        exit(-1);
      }
      db = (double *)calloc(to, sizeof(double));
      if (db == NULL) {
        fprintf(stderr,
                "Allocation error, impossible to alloc %ld bytes for db",
                to * sizeof(double));
        exit(-1);
      }
      df = (double *)calloc(to, sizeof(double));
      if (cf == NULL) {
        fprintf(stderr, "Allocation error, impossible to alloc %ld bytes for a",
                to * sizeof(double));
        exit(-1);
      }

      aa.Ancho1 = 0.2;
      aa.Ancho2 = .2;
      aa.Ancho3 = 0.2;
      ancho1 = aa.Ancho1;
      ancho2 = aa.Ancho2;
      ancho3 = ancho2;
      largo1 = 1.5;
      largo2 = 1.5;
      largo3 = 1.;

      r12 = (ancho1 - ancho2) / (ancho1 + ancho2);
      r21 = -r12;
      r23 = (ancho2 - ancho3) / (ancho2 + ancho3);
      r32 = -r23;
      t12 = 1 + r12;
      t21 = 1 + r21;
      t23 = 1 + r23;
      t32 = 1 + r32;

      tau1 = rint(largo1 / (c * dt + 0.0));
      tau2 = rint(largo2 / (c * dt + 0.0));
      tau3 = rint(largo3 / (c * dt + 0.0));

      maxtau = tau3;
      if (tau1 >= tau2 && tau1 >= tau3)
        maxtau = tau1;
      if (tau2 >= tau1 && tau2 >= tau3)
        maxtau = tau2;
      t = 0;

      for (j = 0; j <= maxtau; j++) {
        bf[j] = 0;
        bb[j] = 0;
        cf[j] = 0;
        cb[j] = 0;
        df[j] = 0;
        db[j] = 0;
      }

      t = maxtau;
      taux = 0;

      aa.alfa2 = .150;
      aa.beta2 = .150;
      aa.alfa3 = 10000000;
      aa.beta3 = -200;

      aa.s1overCH = (36 * 2.5 * 2 / 25.) * 1e09;
      aa.s1overLB = 1. * 1e-04;
      aa.s1overLG = (50 * 1e-03) / .5;
      aa.RB = 1 * 1e07;

      while ((t < to) && (v[1] > -5000000)) {
        dbold = db[t];

        a[t] = (.50) * (1.01 * (1. * (aa.A1 * v[1] + aa.A2 * v[6] +
                                      aa.A3 * v[9] / 10.))) +
               bb[t - tau1];
        bb[t] = r12 * a[t - tau1] + t21 * cb[t - tau2];
        bf[t] = t12 * a[t - tau1] + r21 * cb[t - tau2];
        cb[t] = r23 * bf[t - tau2] + t32 * db[t - tau3];
        cf[t] = t23 * bf[t - tau2] + r32 * db[t - tau3];
        db[t] = -aa.r * cf[t - tau3];

        ddb = (db[t] - dbold) / dt;
        aa.forcing1 = db[t];
        aa.forcing2 = ddb;

        PRESSURE = a[t];

        tiempot = tiempot + dt;

        aa.gm = 24000.;

        ancho1 = aa.Ancho1;
        ancho2 = aa.Ancho2;
        r12 = (ancho1 - ancho2) / (ancho1 + ancho2);
        r21 = -r12;
        r23 = (ancho2 - ancho3) / (ancho2 + ancho3);
        r32 = -r23;
        t12 = 1 + r12;
        t21 = 1 + r21;
        t23 = 1 + r23;
        t32 = 1 + r32;

        rk4(takens, v, 10, t + 0.0, dt);
        aa.noise = 0;
        preout = aa.RB * v[4];

        if (taux == 20) {
          /* Does not take data from file but from the argument data */
          aa.amplitud = alpha[i_cur];
          aa.beta1 = beta[i_cur];
          // fprintf(stderr, "%ld: %lf %lf\n", i_cur, alpha[i_cur],
          // beta[i_cur]);
          out[i_cur] = preout * 10;
          taux = 0;
          i_cur++;
          if ((i_cur == 3000 || i_cur == 21103 || i_cur == 25000 ||
               i_cur == 25001) &&
              VERBOSE) {
            printf("----- %d ------\n", i_cur);
            print_aa();
            print_v(v);
            printf("a[t] = %e;\n",a[t]);
            printf("bb[t] = %e;\n",bb[t]);
            printf("bf[t] = %e;\n",bf[t]);
            printf("cb[t] = %e;\n",cb[t]);
            printf("cf[t] = %e;\n",cf[t]);
            printf("db[t] = %e;\n",db[t]);
          }
        }
        taux++;

        if (tiempot > 0.00) {

          aa.alfa1 = -0.15 - 0.00 * aa.amplitud;
          aa.alfa2 = 0.15;
          aa.beta2 = 0.1;
          aa.r = 0.1;
          aa.noise = .21 * (ran1(&idum) - 0.5);
          aa.beta1 = aa.beta1 + 0.01 * aa.noise;
          aa.s1overCH = (360 / 0.8) * 1e08;
          aa.s1overLB = 1. * 1e-04;
          aa.s1overLG = (1 / 82.);
          aa.RB = (.5) * 1e07;

          aa.rdis = (300. / 5.) * (10000.);
          aa.A1 = (aa.amplitud) + 0.5 * aa.noise;
          /* DELETED ARTIFICIAL ARGUMENTS
          if ((tiempot > 0.1) && (tiempot < 0.165))
            aa.A1 = sqrt(aa.amplitud) + 10.5 * aa.noise;
          if ((tiempot > 0.215) && (tiempot < 0.315))
            aa.A1 = sqrt(aa.amplitud) + 10.5 * aa.noise;
          if ((tiempot > 0.3425) && (tiempot < 0.499))
            aa.A1 = sqrt(aa.amplitud) + 10.5 * aa.noise;
          if ((tiempot > 0.214) && (tiempot < 0.255))
            aa.A1 = aa.A1 / 2.;
          */
          aa.A2 = aa.amplitud * 0.0;
          aa.A3 = 0.;
        }

        t++;
      }

      free(a);
      free(bf);
      free(bb);
      free(cf);
      free(cb);
      free(df);
      free(db);
    }
  }
}
