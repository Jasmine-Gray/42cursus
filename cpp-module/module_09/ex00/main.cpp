#include "BitcoinExchange.hpp"

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Error: could not open file." << std::endl;
        return 1;
    }

    BitcoinExchange btc;
    
    // データベースファイルは "data.csv" という名前で提供される前提[cite: 1, 2]
    btc.loadDatabase("data.csv");
    btc.processInput(argv[1]);

    return 0;
}
