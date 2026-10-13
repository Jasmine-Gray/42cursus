#ifndef PMERGEME_HPP
#define PMERGEME_HPP

#include <vector>
#include <deque>
#include <string>
#include <iostream>
#include <sys/time.h>

class PmergeMe {
private:
    std::vector<int> _vec;
    std::deque<int>  _deq;

    // Ford-Johnson Algorithm for std::vector
    void _sortVector(std::vector<int>& v);
    
    // Ford-Johnson Algorithm for std::deque
    void _sortDeque(std::deque<int>& d);

    // Jacobsthal number generator
    size_t _getJacobsthal(size_t n) const;

public:
    PmergeMe();
    PmergeMe(const PmergeMe& other);
    PmergeMe& operator=(const PmergeMe& other);
    ~PmergeMe();

    bool parseInput(int argc, char** argv);
    void execute();
};

#endif

