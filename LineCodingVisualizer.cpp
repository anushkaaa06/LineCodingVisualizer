#include <iostream>
#include <vector>
#include <string>
#include <fstream>

// Abstract base class
class LineCoder {
public:
    virtual std::vector<double> encode(const std::string& data) = 0;
    virtual ~LineCoder() {}
};

class UnipolarNRZCoder : public LineCoder {
public:
    std::vector<double> encode(const std::string& data) override {
        std::vector<double> signal;
        for (char c : data) {
            signal.push_back(c == '1' ? 1.0 : 0.0);
        }
        return signal;
    }
};

class PolarNRZLCoder : public LineCoder {
public:
    std::vector<double> encode(const std::string& data) override {
        std::vector<double> signal;
        for (char c : data) {
            if (c == '1')
                signal.push_back(1.0);
            else
                signal.push_back(-1.0);
        }
        return signal;
    }
};

class BipolarAMICoder : public LineCoder {
private:
    int lastPulse = 1;

public:
    std::vector<double> encode(const std::string& data) override {
        std::vector<double> signal;
        for (char c : data) {
            if (c == '1') {
                signal.push_back(lastPulse);
                lastPulse = -lastPulse;
            } else {
                signal.push_back(0.0);
            }
        }
        return signal;
    }
};

class ManchesterCoder : public LineCoder {
public:
    std::vector<double> encode(const std::string& data) override {
        std::vector<double> signal;
        for (char c : data) {
            if (c == '1') {
                signal.push_back(1.0);
                signal.push_back(-1.0);
            } else {
                signal.push_back(-1.0);
                signal.push_back(1.0);
            }
        }
        return signal;
    }
};

class GnuplotVisualizer {
public:
    void generateData(const std::vector<double>& signal, const std::string& filename, bool manchester = false) {
        std::ofstream out(filename);
        if (!out.is_open()) {
            std::cerr << "Couldn't open file: " << filename << std::endl;
            return;
        }

        double t = 0.0;
        double dt = 1.0;

        if (manchester) {
            for (size_t i = 0; i < signal.size(); i += 2) {
                out << t << " " << signal[i] << "\n"
                    << t + dt/2.0 << " " << signal[i] << "\n"
                    << t + dt/2.0 << " " << signal[i+1] << "\n"
                    << t + dt << " " << signal[i+1] << "\n";
                t += dt;
            }
        } else {
            for (size_t i = 0; i < signal.size(); ++i) {
                out << t << " " << signal[i] << "\n"
                    << t + dt << " " << signal[i] << "\n";
                if (i+1 < signal.size())
                    out << t + dt << " " << signal[i+1] << "\n";
                t += dt;
            }
        }
    }

    void printScript(const std::string& datafile, const std::string& imgfile, double ymin, double ymax) {
        std::cout << "\nGnuplot Script:\n";
        std::cout << "set terminal png size 800,300\n";
        std::cout << "set output '" << imgfile << "'\n";
        std::cout << "set xlabel 'Time'\n";
        std::cout << "set ylabel 'Voltage'\n";
        std::cout << "set yrange [" << ymin << ":" << ymax << "]\n";
        std::cout << "set grid\n";
        std::cout << "plot '" << datafile << "' with lines title 'Signal'\n";
    }
};

int main() {
    std::string data;
    int choice;

    std::cout << "Enter binary data: ";
    std::cin >> data;

    std::cout << "Choose encoding scheme:\n";
    std::cout << "1. Unipolar NRZ\n";
    std::cout << "2. Polar NRZ-L\n";
    std::cout << "3. Bipolar AMI\n";
    std::cout << "4. Manchester\n";
    std::cout << "Your choice: ";
    std::cin >> choice;

    LineCoder* coder = nullptr;
    bool manchester = false;
    double ymin = -1.5, ymax = 1.5;

    if (choice == 1) {
        coder = new UnipolarNRZCoder();
        ymin = -0.5; ymax = 1.5;
    } else if (choice == 2) {
        coder = new PolarNRZLCoder();
    } else if (choice == 3) {
        coder = new BipolarAMICoder();
    } else if (choice == 4) {
        coder = new ManchesterCoder();
        manchester = true;
    } else {
        std::cout << "Invalid input." << std::endl;
        return 1;
    }

    GnuplotVisualizer viz;
    auto signal = coder->encode(data);

    viz.generateData(signal, "signal.dat", manchester);
    viz.printScript("signal.dat", "signal.png", ymin, ymax);

    delete coder;

    return 0;
}


// void visualize_signal(const std::vector<int>& signal) {
//     if (signal.empty()) {
//         std::cout << "No signal to visualize." << std::endl;
//         return;
//     }
//
//     int max_val = std::numeric_limits<int>::min();
//     int min_val = std::numeric_limits<int>::max();
//     for (int val : signal) {
//         max_val = std::max(max_val, val);
//         min_val = std::min(min_val, val);
//     }
//
//     int vertical_scale = std::max(std::abs(max_val), std::abs(min_val));
//     int signal_length = signal.size();
//
//     std::cout << "Time: |";
//     for (int i = 0; i < signal_length; ++i) {
//         std::cout << "----|";
//     }
//     std::cout << ">" << std::endl;
//
//     for (int level = vertical_scale; level >= -vertical_scale; --level) {
//         std::cout << std::setw(2) << level << "|";
//         for (int val : signal) {
//             if (val > level) {
//                 std::cout << "    |";
//             } else if (val < level) {
//                 std::cout << "    |";
//             } else if (val == level) {
//                 std::cout << " ***|";
//             }
//         }
//         std::cout << std::endl;
//     }
//
//     std::cout << "    ";
//     for (int i = 0; i < signal_length; ++i) {
//         std::cout << "Bit |";
//     }
//     std::cout << std::endl;
//     std::cout << "    ";
//     for (int i = 0; i < signal_length; ++i) {
//         std::cout << " " << i + 1 << "  ";
//     }
//     std::cout << std::endl;
// }

