#include "BitcoinExchange.hpp"
#include <stdexcept> // std::invalid_argument に使用

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
    
    if (file.fail()) {
        std::cerr << "Error: could not open database file." << std::endl;
        return;
    }

    std::string line;
    std::getline(file, line); 

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
    std::getline(file, line); 

    while (std::getline(file, line)) {
        // 空行をスキップ
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
            // DBに直近過去の日付が存在しない場合のエラーハンドリング
            std::cerr << "Error: bad input => " << date << std::endl;
        }
    }
}

bool BitcoinExchange::isValidDate(const std::string& date) const {
    // 形式チェック (YYYY-MM-DD)
    if (date.length() != 10 || date[4] != '-' || date[7] != '-') {
        return false;
    }

    // 数値としてパース
    int year = std::atoi(date.substr(0, 4).c_str());
    int month = std::atoi(date.substr(5, 2).c_str());
    int day = std::atoi(date.substr(8, 2).c_str());

    // 基本的な範囲チェック
    if (year < 0 || month < 1 || month > 12 || day < 1 || day > 31) {
        return false;
    }

    // 各月の日数（2月は一旦28日とする）
    int daysInMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    // 閏年判定 (4で割り切れ、かつ100で割り切れない、または400で割り切れる)
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        daysInMonth[2] = 29;
    }

    // その月の日数を超えていないかチェック
    if (day > daysInMonth[month]) {
        return false;
    }

    return true; 
}

bool BitcoinExchange::parseValue(const std::string& valueStr, float& value) const {
    // 空文字、または空白文字のみの場合はエラー
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

    // データベースの最初の日付より前の場合、lower_boundの仕様上itはbegin()を指す
    if (it == database.begin()) {
        throw std::invalid_argument("date too early"); // 例外を投げて呼び出し元でエラー表示
    }

    --it;
    return it->second;
}
