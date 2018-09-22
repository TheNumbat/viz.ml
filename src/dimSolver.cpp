#include <iostream>
#include <vector>

#include "data.h"

#include <bhtsne/sp_tsne.h>
#include <nlopt/nlopt.hpp>

std::vector<std::vector<f64>> read_data() {
	int n = 784;
	int d = 3;
	double* data;
	FILE *h;
	h = fopen("result.dat", "r");
	if (h == NULL) {
		printf("Error: could not open data file.\n");
		return;
	}
	fread(&n, sizeof(int), 1, h);
	fread(&d, sizeof(int), 1, h);
	fread(data, sizeof(double), n * d, h);
	std::vector<std::vector<f64>> v;
	std::vector<f64> v1,v2,v3;
	for (int i = 0; i<784; i++)
	{
		v1.push_back(data[i]);
		v2.push_back(data[784+i]);
		v3.push_back(data[784*2+i]);
	}
	v.push_back(v1);
	v.push_back(v2);
	v.push_back(v3);
}

#if 0
int count = 0;

>>>>>>> e8bab02b3aeebfc0142d779b4864fbf9ca335936
double mds_distance(int n, const double *x, f64 *grad, void *data)
{
	count++;
	std::cout << count << "\n";
    dataset *d = (dataset *) data;
    double mds_dist = 0;
    for(int i = 0; i < NUM_DATA_POINTS; i++) {
		for(int j = i+1; j < NUM_DATA_POINTS; j++) {
			f64 delta[NUM_PIXELS];
			for(int k = 0; k < NUM_PIXELS; k++) {
				delta[k] = d->pixels[i][k] - d->pixels[j][k];
			}

			f64 accum = 0.0f;
			for(int k = 0; k < NUM_PIXELS; k++) {
				accum += x[k] * pow(delta[k],2);
			}
			f64 sqrt_dist = d->distances[i][j] - sqrt(accum);
			mds_dist += pow((sqrt_dist),2);
			if(grad){
				for(int k = 0; k < NUM_PIXELS; k++) {
					grad[k] += sqrt_dist * x[k] * delta[k];
				}
			}
		}
	}
	return mds_dist;
}

double sammon_distance(int n, const double *x, double *grad, void *data)
{
    dataset *d = (dataset *) data;
    double sam_dist = 0;
    for(int i = 0; i < NUM_DATA_POINTS; i++) {
		for(int j = i+1; j < NUM_DATA_POINTS; j++) {
			f64 delta[NUM_PIXELS];
			for(int k = 0; k < NUM_PIXELS; k++) {
				delta[k] = d->pixels[i][k] - d->pixels[j][k];
			}

			f64 accum = 0.0f;
			for(int k = 0; k < NUM_PIXELS; k++) {
				accum += x[k] * pow(delta[k],2);
			}

			f64 sqrt_dist = d->distances[i][j] - sqrt(accum);
			sam_dist += pow((sqrt_dist),2)/d->distances[i][j];
			if(grad){
				for(int k = 0; k < NUM_PIXELS; k++) {
					grad[k] += sqrt_dist * x[k] * delta[k]/d->distances[i][j];
				}
			}
		}
	}
	return sam_dist;
}

std::vector<std::vector<f64>> run_nlopt(bool sammon, dataset *data){
	nlopt::opt opt(nlopt::LN_NEWUOA, NUM_PIXELS);
	if(!sammon){
		opt.set_min_objective((nlopt::func)sammon_distance, data);
	}
	else{
		opt.set_min_objective((nlopt::func)mds_distance, data);
	}
	opt.set_default_initial_step(5.0);
	opt.set_stopval(65);
	double minf;
	std::vector<std::vector<f64>> solution;
	std::vector<f64> x(NUM_PIXELS);

	std::cout << opt.get_stopval() << std::endl;

	for(int iter = 0; iter < 3; iter++){
		for (int i = 0; i < NUM_PIXELS; i++)
		{
			x[i] = 2. * randf64();
		}
		try
		{
			opt.optimize(x, minf);
			std::cout << "found minimum " << minf << std::endl;
			solution.push_back(x);
		}
		catch (std::exception &e)
		{
			std::cout << "nlopt failed: " << e.what() << std::endl;
		}
	}
	return solution;
}
#endif
#if 0
void run_bhtsne(dataset *data){
	i32 N = NUM_DATA_POINTS;
	//probably bad
	i32 k = 90;
	i32 perplexity = 50;
	// find k, eta, exageration, 
	i32 no_dims = 3;
	i32 iterations = 500;
	i32 verbose = 3;

	f64 theta = 0.5;
	//probably wrong
	f64 eta = 10;
	//probably wrong
	f64 exageration = 0;
	f64 *sorted_distances = NULL; 
	i32 *sorted_indices = NULL;

	TSNE* tsne = new TSNE();

	time_t start = clock();
	// Read the parameters and the dataset
	
	f64* Y = (f64*)malloc(N * no_dims * sizeof(f64));
	if (Y == NULL) { printf("Memory allocation failed on Y malloc\n"); exit(1); }

	// Now fire up the SNE implementation
	//f64* costs = (f64*)calloc(N, sizeof(f64));
	//if (costs == NULL) { printf("Memory allocation failed costs\n"); exit(1); }
	tsne->run(sorted_distances, sorted_indices, N, no_dims, k, perplexity, theta, eta, exageration, iterations, verbose, Y);

	// Save the results
	tsne->save_data(Y, N, no_dims, verbose);

	// Clean up the memory
	free(Y); Y = NULL;
	delete(tsne);
	time_t end = clock();
	if (verbose > 0) printf("T-sne required %f seconds (%f minutes) to run\n", float(end - start) / CLOCKS_PER_SEC, float(end - start) / (60 * CLOCKS_PER_SEC));
}
#endif
