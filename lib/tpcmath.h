/* by Shuhei Ajimura */
#ifdef __cplusplus
extern "C" {
#endif

void fadc_calc_sigma(int, int, double *, double *);
void fadc_calc_sigmaD(int, double, double *, double *);
void fadc_solve_parabola(int *, int *, double *);
void fadc_solve_parabolaD(double *, double *, double *);

#ifdef __cplusplus
};
#endif
