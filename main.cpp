#include <iostream>
#include <climits>
#include <fstream>
#include <random>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cmath>
#include <sys/stat.h>
#ifdef _WIN32
#include <direct.h>
#endif
using namespace std;

// Function to create a directory if it does not exist
void create_directory(const string& directory) {
    struct stat info;
    if (stat(directory.c_str(), &info) != 0 || !(info.st_mode & S_IFDIR)) {
#ifdef _WIN32
        _mkdir(directory.c_str());
#else
        mkdir(directory.c_str(), 0755);
#endif
    }
}

void generate_datasets(const string& file_name, int& num_batches, int& batch_size, int& percentage_bad_batches, int& percentage_bad_chips, int& samples, mt19937& generator) {
    ifstream file(file_name);
    if (file.is_open()) {
        file >> num_batches >> batch_size >> percentage_bad_batches >> percentage_bad_chips >> samples;
    } else {
        cerr << "Unable to open the input file: " << file_name << endl;
        exit(1);
    }

    uniform_int_distribution<int> batch_dist(0, 99);

    for (int i = 1; i <= num_batches; ++i) {
        ofstream ds_file("output_files/ds" + to_string(i) + ".txt");
        bool is_bad_batch = batch_dist(generator) < percentage_bad_batches;

        if (is_bad_batch) {
            uniform_int_distribution<int> num_bad_chips_dist(0, percentage_bad_chips * batch_size / 100);
            int num_bad_chips = num_bad_chips_dist(generator);

            vector<int> chip_indices(batch_size);
            iota(chip_indices.begin(), chip_indices.end(), 0);
            shuffle(chip_indices.begin(), chip_indices.end(), generator);

            for (int j = 0; j < batch_size; ++j) {
                if (find(chip_indices.begin(), chip_indices.begin() + num_bad_chips, j) != chip_indices.begin() + num_bad_chips) {
                    ds_file << 'b' << endl;
                } else {
                    ds_file << 'g' << endl;
                }
            }
        } else {
            for (int j = 0; j < batch_size; ++j) {
                ds_file << 'g' << endl;
            }
        }
    }
}

void monte_carlo_algorithm(int num_batches, int batch_size, int percentage_bad_batches, int percentage_bad_chips, int samples, mt19937& generator) {
    uniform_int_distribution<int> chip_dist(0, batch_size - 1);
    int bad_batches_detected = 0;

    for (int i = 1; i <= num_batches; ++i) {
        ifstream ds_file("output_files/ds" + to_string(i) + ".txt");
        int bad_chips_detected = 0;

        for (int j = 0; j < samples; ++j) {
            char chip;
            ds_file >> chip;

            if (chip == 'b') {
                bad_chips_detected++;
            }
        }

        double probability_of_failure = pow(static_cast<double>(100 - percentage_bad_chips) / 100, samples);
        double probability_of_bad_batch = static_cast<double>(bad_chips_detected) / samples;
        if (probability_of_bad_batch > probability_of_failure) {
            bad_batches_detected++;
        }

        ds_file.close();
    }

    cout << "Percentage of bad batches detected: " << static_cast<double>(bad_batches_detected) / num_batches * 100
         << "%" << endl;
}

int main() {
    create_directory("output_files");
    vector<string> test_files = {"../output_files/t1.txt", "../output_files/t2.txt", "../output_files/t3.txt",
                                 "../output_files/t4.txt"};
    mt19937 generator(static_cast<unsigned int>(time(nullptr)));

    for (size_t i = 0; i < test_files.size(); ++i) {
        cout << "Simulation #" << i + 1 << endl;

        int num_batches, batch_size, percentage_bad_batches, percentage_bad_chips, samples;
        generate_datasets(test_files[i], num_batches, batch_size, percentage_bad_batches, percentage_bad_chips, samples,
                          generator);
        cout << "Generating data sets:" << endl;

        int total_bad_sets = 0;
        int max_bad_chips_in_bad_set = 0;
        int min_bad_chips_in_bad_set = INT_MAX;
        int sum_bad_chips_in_bad_sets = 0;

        for (int j = 1; j <= num_batches; ++j) {
            ifstream ds_file("output_files/ds" + to_string(j) + ".txt");
            int num_bad_chips = 0;

            for (int k = 0; k < batch_size; ++k) {
                char chip;
                ds_file >> chip;

                if (chip == 'b') {
                    ++num_bad_chips;
                }
            }

            if (num_bad_chips > 0) {
                ++total_bad_sets;
                max_bad_chips_in_bad_set = max(max_bad_chips_in_bad_set, num_bad_chips);
                min_bad_chips_in_bad_set = min(min_bad_chips_in_bad_set, num_bad_chips);
                sum_bad_chips_in_bad_sets += num_bad_chips;
            }

            ds_file.close();
        }

        double avg_bad_chips_in_bad_sets = static_cast<double>(sum_bad_chips_in_bad_sets) / total_bad_sets;

        cout << "\tTotal bad sets = " << total_bad_sets << endl;
        cout << "\tMax number of bad items in a bad set = " << max_bad_chips_in_bad_set << endl;
        cout << "\tMin number of bad items in the bad set  = " << min_bad_chips_in_bad_set << endl;
        cout << "\tAverage number of bad items in a bad set  = " << avg_bad_chips_in_bad_sets << endl << endl;

        cout << "\tNumber of batches of items:\t\t" << num_batches << endl;
        cout << "\tNumber of items in each batch:\t\t" << batch_size << endl;
        cout << "\tPercentage of batches containing bad items:\t" << percentage_bad_batches << "%" << endl;
        cout << "\tPercentage of items that are bad in a bad set:\t" << percentage_bad_chips << "%" << endl;
        cout << "\tItems sampled from each set:\t\t" << samples << endl;

        cout << endl << "Analyzing Data Sets" << endl;
        monte_carlo_algorithm(num_batches, batch_size, percentage_bad_batches, percentage_bad_chips, samples,
                              generator);

        if (i < test_files.size() - 1) {
            cout << endl;
        }
    }

    return 0;
}
/*
#include <iostream>
#include <fstream>
#include <random>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cmath>

using namespace std;

void generate_datasets(const string& file_name, int& num_batches, int& batch_size, int& percentage_bad_batches, int& percentage_bad_chips, int& samples, mt19937& generator) {
    ifstream file(file_name);
    if (file.is_open()) {
        file >> num_batches >> batch_size >> percentage_bad_batches >> percentage_bad_chips >> samples;
    } else {
        cerr << "Unable to open the input file: " << file_name << endl;
        exit(1);
    }

    uniform_int_distribution<int> batch_dist(0, 99);

    for (int i = 1; i <= num_batches; ++i) {
        ofstream ds_file("ds" + to_string(i) + ".txt");
        bool is_bad_batch = batch_dist(generator) < percentage_bad_batches;

        if (is_bad_batch) {
            // generate random number of bad chips within a range based on percentage of bad chips
            uniform_int_distribution<int> num_bad_chips_dist(0, percentage_bad_chips * batch_size / 100);
            int num_bad_chips = num_bad_chips_dist(generator);

            // generate set of random indices for bad chips
            vector<int> chip_indices(batch_size);
            iota(chip_indices.begin(), chip_indices.end(), 0);
            shuffle(chip_indices.begin(), chip_indices.end(), generator);

            // mark chips as bad or good depending on their index
            for (int j = 0; j < batch_size; ++j) {
                if (find(chip_indices.begin(), chip_indices.begin() + num_bad_chips, j) != chip_indices.begin() + num_bad_chips) {
                    ds_file << 'b' << endl;
                } else {
                    ds_file << 'g' << endl;
                }
            }
        } else {
            // mark all chips as good
            for (int j = 0; j < batch_size; ++j) {
                ds_file << 'g' << endl;
            }
        }
    }
}


void monte_carlo_algorithm(int num_batches, int batch_size, int percentage_bad_batches, int percentage_bad_chips, int samples, mt19937& generator) {
    uniform_int_distribution<int> chip_dist(0, batch_size - 1);
    int bad_batches_detected = 0;

    for (int i = 1; i <= num_batches; ++i) {
        ifstream ds_file("ds" + to_string(i) + ".txt");
        int bad_chips_detected = 0;

        for (int j = 0; j < samples; ++j) {
            char chip;
            ds_file >> chip;

            if (chip == 'b') {
                bad_chips_detected++;
            }
        }

        double probability_of_failure = pow(static_cast<double>(100 - percentage_bad_chips) / 100, samples);
        double probability_of_bad_batch = static_cast<double>(bad_chips_detected) / samples;

        if (probability_of_bad_batch > probability_of_failure) {
            bad_batches_detected++;
        }

        ds_file.close();
    }

    cout << "Percentage of bad batches detected: " << static_cast<double>(bad_batches_detected) / num_batches * 100
         << "%" << endl;
}

int main() {
    vector<string> test_files = {"t1.txt", "t2.txt", "t3.txt", "t4.txt"};
    mt19937 generator(static_cast<unsigned int>(time(nullptr)));

    for (size_t i = 0; i < test_files.size(); ++i) {
        cout << "Simulation #" << i + 1 << endl;

        int num_batches, batch_size, percentage_bad_batches, percentage_bad_chips, samples;
        generate_datasets(test_files[i], num_batches, batch_size, percentage_bad_batches, percentage_bad_chips, samples, generator);
        cout << "Generating data sets:" << endl;

        int total_bad_sets = 0;
        int max_bad_chips_in_bad_set = 0;
        int min_bad_chips_in_bad_set = INT_MAX;
        int sum_bad_chips_in_bad_sets = 0;

        for (int j = 1; j <= num_batches; ++j) {
            ifstream ds_file("ds" + to_string(j) + ".txt");
            int num_bad_chips = 0;

            for (int k = 0; k < batch_size; ++k) {
                char chip;
                ds_file >> chip;

                if (chip == 'b') {
                    ++num_bad_chips;
                }
            }

            if (num_bad_chips > 0) {
                ++total_bad_sets;
                max_bad_chips_in_bad_set = max(max_bad_chips_in_bad_set, num_bad_chips);
                min_bad_chips_in_bad_set = min(min_bad_chips_in_bad_set, num_bad_chips);
                sum_bad_chips_in_bad_sets += num_bad_chips;
            }

            ds_file.close();
        }

        double avg_bad_chips_in_bad_sets = static_cast<double>(sum_bad_chips_in_bad_sets) / total_bad_sets;

        cout << "\tTotal bad sets = " << total_bad_sets << endl;
        cout << "\tMax number of bad items in a bad set = " << max_bad_chips_in_bad_set << endl;
        cout << "\tMin number of bad items in the bad set  = " << min_bad_chips_in_bad_set << endl;
        cout << "\tAverage number of bad items in a bad set  = " << avg_bad_chips_in_bad_sets << endl << endl;

        cout << "\tNumber of batches of items:\t\t" << num_batches << endl;
        cout << "\tNumber of items in each batch:\t\t" << batch_size << endl;
        cout << "\tPercentage of batches containing bad items:\t" << percentage_bad_batches << "%" << endl;
        cout << "\tPercentage of items that are bad in a bad set:\t" << percentage_bad_chips << "%" << endl;
        cout << "\tItems sampled from each set:\t\t" << samples << endl;

        cout << endl << "Analyzing Data Sets" << endl;

        int num_bad_sets = 0;
        int max_bad_chips = 0;
        int min_bad_chips = INT_MAX;
        int total_bad_chips = 0;

        for (int j = 1; j <= num_batches; ++j) {
            ifstream ds_file("ds" + to_string(j) + ".txt");
            int num_bad_chips = 0;

            for (int k = 0; k < samples; ++k) {
                char chip;
                ds_file >> chip;

                if (chip == 'b') {
                    ++num_bad_chips;
                }
            }

            if (num_bad_chips > 0) {
                ++num_bad_sets;
                max_bad_chips = max(max_bad_chips, num_bad_chips);
                min_bad_chips = min(min_bad_chips, num_bad_chips);
                total_bad_chips += num_bad_chips;
            }

            ds_file.close();
        }

        double avg_bad_chips = static_cast<double>(total_bad_chips) / num_bad_sets;
        double base = static_cast<double>(100 - percentage_bad_chips) / 100;
        double exponent = static_cast<double>(samples);
        double p_failure = pow(base, exponent);
        double p_good_batch = 1 - (static_cast<double>(num_bad_sets) / num_batches);

        cout << "\tBase = " << base << ", exponent = " << exponent << endl;
        cout << "\tP(failure to detect bad item) = " << p_failure << endl;
        cout << "\tP(batch is good) = " << p_good_batch << endl;
        cout << "\tPercentage of bad batches detected = " << (1 - p_good_batch) * 100 << "%" << endl;

        if (i < test_files.size() - 1) {
            cout << endl;
        }
    }

    return 0;
}
*/
