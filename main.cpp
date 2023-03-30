#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <random>
#include <iomanip>

void generate_datasets(int num_batches, int batch_size, int bad_batch_percentage, int bad_chip_percentage, int items_sampled);
void analyze_datasets(int num_batches, int batch_size, int bad_batch_percentage, int bad_chip_percentage, int items_sampled);

int main() {
    std::vector<std::string> test_files = {"t1.txt", "t2.txt", "t3.txt", "t4.txt"};

    for (const auto& file : test_files) {
        std::ifstream input(file);
        int num_batches, batch_size, bad_batch_percentage, bad_chip_percentage, items_sampled;
        input >> num_batches >> batch_size >> bad_batch_percentage >> bad_chip_percentage >> items_sampled;

       // generate_datasets(num_batches, batch_size, bad_batch_percentage, bad_chip_percentage, items_sampled);
       // analyze_datasets(num_batches, batch_size, bad_batch_percentage, bad_chip_percentage, items_sampled);
        generate_datasets(200, 1000, 25, 15, 50);
        analyze_datasets(200, 1000, 25, 15, 50);
        generate_datasets(200, 1000, 10, 10, 40);
        analyze_datasets(200, 1000, 10, 10, 40);
        generate_datasets(200, 1000, 10, 5, 30);
        analyze_datasets(200, 1000, 10, 5, 30);
        generate_datasets(200, 1000, 10, 1, 20);
        analyze_datasets(200, 1000, 10, 1, 20);

    }

    return 0;
}

void generate_datasets(int num_batches, int batch_size, int bad_batch_percentage, int bad_chip_percentage, int items_sampled) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> batch_dist(0, 99);
    std::uniform_int_distribution<> chip_dist(0, 99);

    for (int i = 0; i < num_batches; i++) {
        std::ofstream output("ds" + std::to_string(i + 1) + ".txt");

        bool is_bad_batch = batch_dist(gen) < bad_batch_percentage;

        for (int j = 0; j < batch_size; j++) {
            bool is_bad_chip = is_bad_batch && (chip_dist(gen) < bad_chip_percentage);
            output << (is_bad_chip ? 'b' : 'g') << std::endl;
        }
    }
}

void analyze_datasets(int num_batches, int batch_size, int bad_batch_percentage, int bad_chip_percentage, int items_sampled) {
    // Implement the Monte Carlo algorithm to analyze the generated datasets and output the simulation results.
    // You can use the provided 'generate_datasets' function as a reference to read and sample the dataset files.
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> sample_dist(0, batch_size - 1);

    int bad_sets_detected = 0;
    int total_bad_sets = 0;

    for (int i = 0; i < num_batches; i++) {
        std::ifstream input("ds" + std::to_string(i + 1) + ".txt");

        std::vector<char> chips(batch_size);
        for (int j = 0; j < batch_size; j++) {
            input >> chips[j];
            if (chips[j] == 'b') {
                total_bad_sets++;
            }
        }

        int bad_chips_sampled = 0;
        for (int j = 0; j < items_sampled; j++) {
            int chip_index = sample_dist(gen);
            if (chips[chip_index] == 'b') {
                bad_chips_sampled++;
            }
        }

        if (bad_chips_sampled > 0) {
            bad_sets_detected++;
        }
    }

    double base = 1.0 - static_cast<double>(bad_chip_percentage) / 100.0;
    double p_failure = std::pow(base, items_sampled);
    double p_good = 1.0 - p_failure;
    double perc_bad_detected = static_cast<double>(bad_sets_detected) / total_bad_sets * 100;

    std::cout << "Analyzing Data Sets" << std::endl;
    std::cout << "Base = " << std::fixed << std::setprecision(6) << base << " exponent = " << items_sampled << std::endl;
    std::cout << "P(failure to detect bad item) = " << p_failure << std::endl;
    std::cout << "P(batch is good) = " << p_good << std::endl;
    std::cout << "Percentage of bad batches detected = " << std::setprecision(0) << perc_bad_detected << std::endl;
}
