#include "PmergeMe.hpp"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <climits>
#include <algorithm>

PmergeMe::PmergeMe() {}

PmergeMe::PmergeMe(const PmergeMe& other) : _vec(other._vec), _deq(other._deq) {}

PmergeMe& PmergeMe::operator=(const PmergeMe& other) {
    if (this != &other) {
        _vec = other._vec;
        _deq = other._deq;
    }
    return *this;
}

PmergeMe::~PmergeMe() {}

size_t PmergeMe::_getJacobsthal(size_t n) const {
    if (n == 0) return 0;
    if (n == 1) return 1;
    size_t prev2 = 0;
    size_t prev1 = 1;
    size_t current = 0;
    for (size_t i = 2; i <= n; ++i) {
        current = prev1 + 2 * prev2;
        prev2 = prev1;
        prev1 = current;
    }
    return current;
}

bool PmergeMe::parseInput(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::istringstream iss(argv[i]); // RAIIによりスコープ終了時に自動解放
        std::string token;
        while (iss >> token) {
            for (size_t j = 0; j < token.length(); ++j) {
                if (!std::isdigit(token[j])) {
                    return false;
                }
            }
            long val = std::atol(token.c_str());
            if (val <= 0 || val > INT_MAX) {
                return false;
            }
            _vec.push_back(static_cast<int>(val));
            _deq.push_back(static_cast<int>(val));
        }
        // ストリーム状態ビットのチェック (badbitの検出)
        if (iss.bad()) {
            return false;
        }
    }
    return !_vec.empty();
}

// ---------------------------------------------------------
// std::vector 用 Merge-Insert Sort
// ---------------------------------------------------------
void PmergeMe::_sortVector(std::vector<int>& v) {
    size_t n = v.size();
    if (n <= 1) return;

    bool hasStraggler = (n % 2 != 0);
    int straggler = 0;
    if (hasStraggler) {
        straggler = v.back();
        v.pop_back();
    }

    std::vector<std::pair<int, int> > pairs;
    for (size_t i = 0; i < v.size(); i += 2) {
        if (v[i] > v[i + 1])
            pairs.push_back(std::make_pair(v[i], v[i + 1]));
        else
            pairs.push_back(std::make_pair(v[i + 1], v[i]));
    }

    std::vector<int> mainChain;
    for (size_t i = 0; i < pairs.size(); ++i) {
        mainChain.push_back(pairs[i].first);
    }
    _sortVector(mainChain);

    std::vector<int> pend;
    std::vector<bool> used(pairs.size(), false);
    for (size_t i = 0; i < mainChain.size(); ++i) {
        for (size_t j = 0; j < pairs.size(); ++j) {
            if (!used[j] && pairs[j].first == mainChain[i]) {
                pend.push_back(pairs[j].second);
                used[j] = true;
                break;
            }
        }
    }

    if (!pend.empty()) {
        mainChain.insert(mainChain.begin(), pend[0]);
    }

    size_t pendSize = pend.size();
    if (pendSize > 1) {
        size_t insertedCount = 1; // pend[0]は挿入済み
        size_t k = 3;

        while (insertedCount < pendSize) {
            size_t jacob = _getJacobsthal(k);
            size_t targetIdx = (jacob < pendSize) ? jacob - 1 : pendSize - 1;

            if (targetIdx >= insertedCount) {
                for (size_t idx = targetIdx; idx >= insertedCount; --idx) {
                    int item = pend[idx];
                    std::vector<int>::iterator pos = std::lower_bound(mainChain.begin(), mainChain.end(), item);
                    mainChain.insert(pos, item);
                    if (idx == insertedCount) break; // size_tアンダーフロー防止
                }
                insertedCount = targetIdx + 1;
            }
            k++;
        }
    }

    if (hasStraggler) {
        std::vector<int>::iterator pos = std::lower_bound(mainChain.begin(), mainChain.end(), straggler);
        mainChain.insert(pos, straggler);
    }

    v = mainChain;
}

// ---------------------------------------------------------
// std::deque 用 Merge-Insert Sort
// ---------------------------------------------------------
void PmergeMe::_sortDeque(std::deque<int>& d) {
    size_t n = d.size();
    if (n <= 1) return;

    bool hasStraggler = (n % 2 != 0);
    int straggler = 0;
    if (hasStraggler) {
        straggler = d.back();
        d.pop_back();
    }

    std::deque<std::pair<int, int> > pairs;
    for (size_t i = 0; i < d.size(); i += 2) {
        if (d[i] > d[i + 1])
            pairs.push_back(std::make_pair(d[i], d[i + 1]));
        else
            pairs.push_back(std::make_pair(d[i + 1], d[i]));
    }

    std::deque<int> mainChain;
    for (size_t i = 0; i < pairs.size(); ++i) {
        mainChain.push_back(pairs[i].first);
    }
    _sortDeque(mainChain);

    std::deque<int> pend;
    std::vector<bool> used(pairs.size(), false);
    for (size_t i = 0; i < mainChain.size(); ++i) {
        for (size_t j = 0; j < pairs.size(); ++j) {
            if (!used[j] && pairs[j].first == mainChain[i]) {
                pend.push_back(pairs[j].second);
                used[j] = true;
                break;
            }
        }
    }

    if (!pend.empty()) {
        mainChain.push_front(pend[0]);
    }

    size_t pendSize = pend.size();
    if (pendSize > 1) {
        size_t insertedCount = 1;
        size_t k = 3;

        while (insertedCount < pendSize) {
            size_t jacob = _getJacobsthal(k);
            size_t targetIdx = (jacob < pendSize) ? jacob - 1 : pendSize - 1;

            if (targetIdx >= insertedCount) {
                for (size_t idx = targetIdx; idx >= insertedCount; --idx) {
                    int item = pend[idx];
                    std::deque<int>::iterator pos = std::lower_bound(mainChain.begin(), mainChain.end(), item);
                    mainChain.insert(pos, item);
                    if (idx == insertedCount) break; // size_tアンダーフロー防止
                }
                insertedCount = targetIdx + 1;
            }
            k++;
        }
    }

    if (hasStraggler) {
        std::deque<int>::iterator pos = std::lower_bound(mainChain.begin(), mainChain.end(), straggler);
        mainChain.insert(pos, straggler);
    }

    d = mainChain;
}

void PmergeMe::execute() {
    // Before の出力（要素数が多い場合は省略）
    std::cout << "Before: ";
    size_t printLimit = 5; // 表示する最大要素数（例: 5個まで表示）
    for (size_t i = 0; i < _vec.size(); ++i) {
        if (i >= printLimit) {
            std::cout << "[...]";
            break;
        }
        std::cout << _vec[i] << (i + 1 == _vec.size() ? "" : " ");
    }
    std::cout << std::endl;

    // vector での時間計測とソート
    struct timeval startVec, endVec;
    gettimeofday(&startVec, NULL);
    _sortVector(_vec);
    gettimeofday(&endVec, NULL);

    // deque での時間計測とソート
    struct timeval startDeq, endDeq;
    gettimeofday(&startDeq, NULL);
    _sortDeque(_deq);
    gettimeofday(&endDeq, NULL);

    // After の出力（要素数が多い場合は省略）
    std::cout << "After:  ";
    for (size_t i = 0; i < _vec.size(); ++i) {
        if (i >= printLimit) {
            std::cout << "[...]";
            break;
        }
        std::cout << _vec[i] << (i + 1 == _vec.size() ? "" : " ");
    }
    std::cout << std::endl;

    // 時間計算 (マイクロ秒 us)
    double timeVec = (endVec.tv_sec - startVec.tv_sec) * 1000000.0 + (endVec.tv_usec - startVec.tv_usec);
    double timeDeq = (endDeq.tv_sec - startDeq.tv_sec) * 1000000.0 + (endDeq.tv_usec - startDeq.tv_usec);

    std::cout << "Time to process a range of " << _vec.size()
              << " elements with std::vector : " << timeVec << " us" << std::endl;
    std::cout << "Time to process a range of " << _deq.size()
              << " elements with std::deque  : " << timeDeq << " us" << std::endl;
}
