#ifndef BRAIN_HPP
#define BRAIN_HPP

#include <iostream>
#include <string>

class Brain {
private:
    std::string ideas[100]; // 100個のstd::stringの配列 

public:
    Brain();
    Brain(const Brain& other);
    Brain& operator=(const Brain& other);
    ~Brain();

    // テスト用にアイデアを操作・取得する関数
    void setIdea(int index, const std::string& idea);
    std::string getIdea(int index) const;
};

#endif
