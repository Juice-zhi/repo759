#include <omp.h>

#include <algorithm>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <random>
#include <string>

using std::chrono::high_resolution_clock;
using std::chrono::duration;

// Constants
const double G = 1.0;          // Gravitational constant
const double softening = 0.1;  // Softening length
const double dt = 0.01;        // Time step
const double board_size = 4.0; // Size of the board

// Function to calculate acceleration due to gravity
void getAcc(const double pos[][3], const double mass[], double acc[][3], int N) {
    // Each iteration writes only acc[i], so the particles can be split among
    // the threads. This loop takes almost all of the time, so it is the one that
    // uses the schedule chosen on the command line (schedule(runtime)).
#pragma omp parallel for schedule(runtime)
    for (int i = 0; i < N; i++) {
        // Sum the pull of every other particle j on particle i.
        double ax = 0.0;
        double ay = 0.0;
        double az = 0.0;
        for (int j = 0; j < N; j++) {
            if (i != j) {
                double dx = pos[j][0] - pos[i][0];
                double dy = pos[j][1] - pos[i][1];
                double dz = pos[j][2] - pos[i][2];
                double inv_r3 = std::pow(dx * dx + dy * dy + dz * dz + softening * softening, -1.5);
                ax += G * (dx * inv_r3) * mass[j];
                ay += G * (dy * inv_r3) * mass[j];
                az += G * (dz * inv_r3) * mass[j];
            }
        }
        acc[i][0] = ax;
        acc[i][1] = ay;
        acc[i][2] = az;
    }
}

// For debug: save positions to a CSV file
void savePositionsToCSV(const double pos[][3], int N, int step, const std::string &filename) {
    std::ofstream file;

    // Open the file in append mode
    file.open(filename, std::ios_base::app);

    if (file.is_open()) {
        file << step << ",[";
        for (int i = 0; i < N; i++) {
            if (i != N - 1)
                file << "[" << pos[i][0] << "," << pos[i][1] << "," << pos[i][2] << "],";
            else
                file << "[" << pos[i][0] << "," << pos[i][1] << "," << pos[i][2] << "]";
        }
        file << "]\n";  // Newline for separation between steps
        file.close();
    } else {
        std::cerr << "Unable to open file!" << std::endl;
    }
}

int main(int argc, char *argv[]) {
    high_resolution_clock::time_point start;
    high_resolution_clock::time_point end;
    duration<double, std::milli> duration_sec;
    start = high_resolution_clock::now();

    // Check if correct number of arguments are provided. The OpenMP schedule
    // for the force loop is optional and defaults to static.
    if (argc != 4 && argc != 5) {
        std::cerr << "Usage: " << argv[0]
                  << " <number_of_particles> <simulation_end_time> <num_threads> [static|dynamic|guided]"
                  << std::endl;
        return 1;
    }

    // Read N, tEnd and the number of threads from command line
    int N = std::stoi(argv[1]);           // Number of particles
    double tEnd = std::stod(argv[2]);     // Time at which simulation ends
    int num_threads = std::stoi(argv[3]); // Number of OpenMP threads
    std::string schedule = (argc == 5) ? argv[4] : "static";

    if (num_threads < 1) {
        std::cerr << "num_threads must be a positive integer." << std::endl;
        return 1;
    }
    omp_sched_t kind;
    if (schedule == "static") {
        kind = omp_sched_static;
    } else if (schedule == "dynamic") {
        kind = omp_sched_dynamic;
    } else if (schedule == "guided") {
        kind = omp_sched_guided;
    } else {
        std::cerr << "The schedule must be static, dynamic or guided." << std::endl;
        return 1;
    }
    omp_set_num_threads(num_threads);
    omp_set_schedule(kind, 0);  // chunk size 0: use the default chunk size

    // File to save positions
    std::string filename = "positions.csv";

    // Clear the file before starting simulation (optional)
    std::ofstream file;
    file.open(filename, std::ofstream::out | std::ofstream::trunc);
    file.close();

    // Allocate dynamic arrays based on N
    double* mass = new double[N];
    double(*pos)[3] = new double[N][3];
    double(*vel)[3] = new double[N][3];
    double(*acc)[3] = new double[N][3];

    // Create a random number engine
    std::mt19937 generator(std::random_device{}());

    // Create random distributions
    std::uniform_real_distribution<double> uniform_dist(0.0, 1.0);
    std::normal_distribution<double> normal_dist(0.0, 1.0);

    // Simulation parameters
    double t = 0.0;

    // Set initial masses and random positions/velocities
    for (int i = 0; i < N; i++) {
        mass[i] = uniform_dist(generator);

        pos[i][0] = normal_dist(generator);
        pos[i][1] = normal_dist(generator);
        pos[i][2] = normal_dist(generator);

        vel[i][0] = normal_dist(generator);
        vel[i][1] = normal_dist(generator);
        vel[i][2] = normal_dist(generator);
    }

    // Convert to Center-of-Mass frame
    double velCM[3] = {0.0, 0.0, 0.0};
    double totalMass = 0.0;
    for (int i = 0; i < N; i++) {
        velCM[0] += vel[i][0] * mass[i];
        velCM[1] += vel[i][1] * mass[i];
        velCM[2] += vel[i][2] * mass[i];
        totalMass += mass[i];
    }

    velCM[0] /= totalMass;
    velCM[1] /= totalMass;
    velCM[2] /= totalMass;

    for (int i = 0; i < N; i++) {
        vel[i][0] -= velCM[0];
        vel[i][1] -= velCM[1];
        vel[i][2] -= velCM[2];
    }

    // Initial accelerations
    getAcc(pos, mass, acc, N);

    // Number of timesteps
    int Nt = int(tEnd / dt);

    // Main simulation loop. The time steps depend on each other and stay
    // serial; the loops over particles inside a step are parallel. They do very
    // little work per particle, so they always use a static schedule.
    for (int step = 0; step < Nt; step++) {

        // (1/2) kick
#pragma omp parallel for schedule(static)
        for (int i = 0; i < N; i++) {
            for (int k = 0; k < 3; k++) {
                vel[i][k] += acc[i][k] * dt / 2.0;
            }
        }

        // Drift
#pragma omp parallel for schedule(static)
        for (int i = 0; i < N; i++) {
            for (int k = 0; k < 3; k++) {
                pos[i][k] += vel[i][k] * dt;
            }
        }

        // Ensure particles stay within the board limits
#pragma omp parallel for schedule(static)
        for (int i = 0; i < N; i++) {
            for (int k = 0; k < 3; k++) {
                pos[i][k] = std::clamp(pos[i][k], -board_size, board_size);
            }
        }

        // Update accelerations
        getAcc(pos, mass, acc, N);

        // (1/2) kick
#pragma omp parallel for schedule(static)
        for (int i = 0; i < N; i++) {
            for (int k = 0; k < 3; k++) {
                vel[i][k] += acc[i][k] * dt / 2.0;
            }
        }

        // Update time
        t += dt;

        // For debug: save positions to CSV at each step
        // savePositionsToCSV(pos, N, step, filename);
    }

    // Clean up dynamically allocated memory
    delete[] mass;
    delete[] pos;
    delete[] vel;
    delete[] acc;

    end = high_resolution_clock::now();
    duration_sec = std::chrono::duration_cast<duration<double, std::milli> >(end - start);
    std::cout << "time: " << duration_sec.count() << "ms\n";

    return 0;
}
