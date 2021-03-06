
int count = 0;

double mds_distance(int n, const double *x, f64 *grad, void *data)
{
	count++;
	std::cout << count << "\n";
    dataset *d = (dataset *) data;
    double mds_dist = 0;
    for(int i = 0; i < NUM_DATA_POINTS-1; i++) {
		for(int j = i+1; j < NUM_DATA_POINTS; j++) {
			f64 dist_r3 = sqrt(pow(x[3*i+0] - x[3*j+0],2) +
							   pow(x[3*i+1] - x[3*j+1],2) +
							   pow(x[3*i+2] - x[3*j+2],2));
			f64 dist_delta = d->distances[i][j]-dist_r3;
			mds_dist += pow(dist_delta, 2);
			if(grad){
				for(int k = 0; k < 3; k++) {
					grad[i+k] += 2 * dist_delta * x[3*i+k] - x[3*j+k]/dist_r3;
					grad[j+k] += 2 * dist_delta * x[3*i+k] - x[3*j+k]/dist_r3;
				}
			}
		}
	}
	return mds_dist;
}
double sammon_distance(int n, const double *x, f64 *grad, void *data)
{
	count++;
	std::cout << count << "\n";
    dataset *d = (dataset *) data;
    double sammon_dist = 0;
    for(int i = 0; i < NUM_DATA_POINTS-1; i++) {
		for(int j = i+1; j < NUM_DATA_POINTS; j++) {
			f64 dist_r3 = sqrt(pow(x[3*i+0] - x[3*j+0],2) +
							   pow(x[3*i+1] - x[3*j+1],2) +
							   pow(x[3*i+2] - x[3*j+2],2));
			f64 dist_delta = d->distances[i][j]-dist_r3;
			sammon_dist += pow(dist_delta, 2) / d->distances[i][j];
			if(grad){
				for(int k = 0; k < 3; k++) {
					grad[i+k] += 2 * dist_delta * x[3*i+k] - x[3*j+k]/dist_r3 /d->distances[i][j];
					grad[j+k] += 2 * dist_delta * x[3*i+k] - x[3*j+k]/dist_r3 /d->distances[i][j];
				}
			}
		}
	}
	return sammon_dist;
}

std::vector<f64> run_nlopt(bool sammon, dataset *data) {
	nlopt::opt opt(nlopt::LD_MMA, NUM_PIXELS);
	opt.set_min_objective((nlopt::func)mds_distance, data);
	//opt.set_default_initial_step(5.0);
	//opt.set_stopval(65);
	double minf;
	std::vector<f64> x(NUM_PIXELS*3);
	for (int i = 0; i < NUM_PIXELS*3; i++)
	{
			x[i] = randf64();
	}
	try
	{
		opt.optimize(x, minf);
		std::cout << "found minimum " << minf << std::endl;
	}
	catch (std::exception &e)
	{
		std::cout << "nlopt failed: " << e.what() << std::endl;
	}
	return x;
}
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
