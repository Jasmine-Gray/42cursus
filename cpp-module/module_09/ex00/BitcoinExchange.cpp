#include "BitcoinExchange.hpp"

BitcoinExchange::BitcoinExchange() {}

BitcoinExchange::BitcoinExchange(const BitcoinExchange& other) : database(other.database) {}

BitcoinExchange& BitcoinExchange::operator=(const BitcoinExchange& other) {
    if (this != &other) {
        database = other.database;
    }
    return *this;
}

BitcoinExchange::~BitcoinExchange() {}

void BitcoinExchange::loadDatabase(const std::string& filename) {
    std::ifstream file(filename.c_str());
    
    // 【NOTE対応】ファイルオープンの失敗を .fail() で検知
    if (file.fail()) {
        std::cerr << "Error: could not open database file." << std::endl;
        return;
    }

    std::string line;
    // データベースのヘッダーを読み飛ばす（空なら早期リターン）
    if (!std::getline(file, line)) {
        std::cerr << "Error: database file is empty." << std::endl;
        return;
    }

    while (std::getline(file, line)) {
        size_t commaPos = line.find(',');
        if (commaPos != std::string::npos) {
            std::string date = line.substr(0, commaPos);
            std::string rateStr = line.substr(commaPos + 1);
            database[date] = std::atof(rateStr.c_str());
        }
    }
}

void BitcoinExchange::processInput(const std::string& filename) const {
    std::ifstream file(filename.c_str());

    if (file.fail()) {
        std::cerr << "Error: could not open file." << std::endl;
        return;
    }

    std::string line;
    if (!std::getline(file, line)) {
        std::cerr << "Error: file is empty of could not be read." << std::endl;
        return;
    }

    // 1行目のヘッダーフォーマット検証 ("date | value" 以外はエラーを出力して次の行へ)
    if (line != "date | value") {
        std::cerr << "Error: invalid file format." << std::endl;
        std::cerr << "Usage: The first line of the input file must be exactly 'date | value'." << std::endl;
        return;
    }

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        size_t delimPos = line.find(" | ");
        if (delimPos == std::string::npos) {
            std::cerr << "Error: bad input => " << line << std::endl;
            continue;
        }

        std::string date = line.substr(0, delimPos);
        std::string valueStr = line.substr(delimPos + 3);

        if (!isValidDate(date)) {
            std::cerr << "Error: bad input => " << date << std::endl;
            continue;
        }

        float value;
        if (!parseValue(valueStr, value)) {
            continue; 
        }

        try {
            float rate = getExchangeRate(date);
            std::cout << date << " => " << valueStr << " = " << (value * rate) << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error: bad input => " << date << std::endl;
        }
    }
    
    // 【NOTE対応】ループアウト後に致命的なストリームエラーがないか確認
    if (file.bad()) {
        std::cerr << "Error: fatal error reading the file." << std::endl;
    }
}

bool BitcoinExchange::isValidDate(const std::string& date) const {
    if (date.length() != 10 || date[4] != '-' || date[7] != '-') {
        return false;
    }

    int year = std::atoi(date.substr(0, 4).c_str());
    int month = std::atoi(date.substr(5, 2).c_str());
    int day = std::atoi(date.substr(8, 2).c_str());

    if (year < 0 || month < 1 || month > 12 || day < 1 || day > 31) {
        return false;
    }

    int daysInMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        daysInMonth[2] = 29;
    }

    if (day > daysInMonth[month]) {
        return false;
    }

    return true; 
}

bool BitcoinExchange::parseValue(const std::string& valueStr, float& value) const {
    if (valueStr.empty() || valueStr.find_first_not_of(" \t\n\v\f\r") == std::string::npos) {
        std::cerr << "Error: bad input => " << valueStr << std::endl;
        return false;
    }

    char* endptr;
    double val = std::strtod(valueStr.c_str(), &endptr);

    if (*endptr != '\0' && *endptr != '\n' && *endptr != '\r') {
         std::cerr << "Error: bad input => " << valueStr << std::endl;
         return false;
    }

    if (val < 0) {
        std::cerr << "Error: not a positive number." << std::endl;
        return false;
    }
    
    if (val > 1000) {
        std::cerr << "Error: too large a number." << std::endl;
        return false;
    }

    value = static_cast<float>(val);
    return true;
}

float BitcoinExchange::getExchangeRate(const std::string& date) const {
    std::map<std::string, float>::const_iterator it = database.lower_bound(date);

    if (it != database.end() && it->first == date) {
        return it->second; 
    }

    if (it == database.begin()) {
        throw std::invalid_argument("date too early");
    }

    --it;
    return it->second;
}
