//-----------------------------------------------------------------
// Definition of channel #
// channel = 1: B+ -> J/psi K+
// channel = 2: B0 -> J/psi K*
// channel = 3: B0 -> J/psi Ks
// channel = 4: Bs -> J/psi phi
// channel = 5: Jpsi + pipi
// channel = 6: Lambda_b -> Jpsi + Lambda
//-----------------------------------------------------------------

//All functions and scripts must read the bins from here. Do not duplicate the definition of the bins.

double ntkp_pt_bins[]=    {10, 16, 20, 25, 30, 35, 42, 50, 60, 70, 90, 120};
double ntkstar_pt_bins[]= {10, 16, 20, 25, 30, 35, 42, 50, 60, 70, 90};
double ntks_pt_bins[]=    {10, 16, 20, 25, 30, 35, 42, 50, 60, 70, 90};
double ntphi_pt_bins[]=   {10, 16, 20, 25, 30, 35, 42, 50, 60, 70, 90};
double ntmix_pt_bins[]=   {10, 16, 20, 25, 30, 35, 42, 50, 60, 70, 90};
double ntlambda_pt_bins[]={10, 16, 20, 25, 30, 35, 42, 50, 60, 70, 90};

double ntkp_y_bins[]=    {0.00, 0.25, 0.50, 0.75, 1.00, 1.25, 1.50, 1.75, 2.00, 2.25}; //{0.00, 0.50, 1.00, 1.50, 2.25};
double ntkstar_y_bins[]= {0.00, 0.25, 0.50, 0.75, 1.00, 1.25, 1.50, 1.75, 2.00, 2.25}; //{0.00, 0.50, 1.00, 1.50, 2.25};
double ntks_y_bins[]=    {0.00, 0.50, 1.00, 1.50, 2.25};
double ntphi_y_bins[]=   {0.00, 0.25, 0.50, 0.75, 1.00, 1.25, 1.50, 1.75, 2.00, 2.25}; //{0.00, 0.50, 1.00, 1.50, 2.25};
double ntmix_y_bins[]=   {0.00, 0.50, 1.00, 1.50, 2.25};
double ntlambda_y_bins[]={0.00, 0.50, 1.00, 1.50, 2.25};

double ratio_pt_bins[]={10, 16, 20, 25, 30, 35, 42, 50, 60, 70, 90};
double ratio_y_bins[]={0.00, 0.25, 0.50, 0.75, 1.00, 1.25, 1.50, 1.75, 2.00, 2.25};

double total_pt_bin_edges[]={10, 90};
double total_y_bin_edges[]={0.00, 2.25};

//Averages
double ntkp_pt_bins_average_2016[] =    {13.26, 17.84, 22.23, 27.24, 32.26, 38.08, 45.47, 54.26, 64.30, 77.60, 100.8};
double ntkstar_pt_bins_average_2016[] = {14.13, 17.95, 22.30, 27.27, 32.27, 38.06, 45.46, 54.25, 64.31, 77.68};
double ntphi_pt_bins_average_2016[] =   {14.11, 17.94, 22.26, 27.22, 32.26, 38.03, 45.44, 54.29, 64.34, 77.49};
double ntkp_y_bins_average_2016[] =     {0.1266, 0.3776, 0.6282, 0.8741, 1.122, 1.362, 1.612, 1.863, 2.103};
double ntkstar_y_bins_average_2016[] =  {0.1268, 0.3766, 0.6269, 0.8729, 1.118, 1.361, 1.612, 1.858, 2.098};
double ntphi_y_bins_average_2016[] =    {0.1268, 0.3766, 0.6264, 0.8734, 1.118, 1.360, 1.613, 1.860, 2.100};

double ntkp_pt_bins_average_2015[] =    {13.28, 17.86, 22.24, 27.24, 32.25, 38.06, 45.49, 54.27, 64.30, 77.53, 100.4};
double ntkstar_pt_bins_average_2015[] = {14.13, 17.96, 22.30, 27.28, 32.27, 38.09, 45.44, 54.25, 64.34, 77.43};
double ntphi_pt_bins_average_2015[] =   {14.11, 17.96, 22.25, 27.22, 32.20, 38.07, 45.42, 54.24, 64.28, 76.96};
double ntkp_y_bins_average_2015[] =     {0.1263, 0.3784, 0.6274, 0.8743, 1.124, 1.365, 1.612, 1.864, 2.104};
double ntkstar_y_bins_average_2015[] =  {0.1278, 0.3774, 0.6270, 0.8725, 1.121, 1.366, 1.614, 1.859, 2.099};
double ntphi_y_bins_average_2015[] =    {0.1266, 0.3786, 0.6262, 0.8724, 1.121, 1.365, 1.614, 1.861, 2.099};
