#include <iostream>
#include <string>
#include "whatever.hpp"

int main(void) {
    int a = 2;
    int b = 3;

    ::swap(a, b);
    std::cout << "a = " << a << ", b = " << b << std::endl;
    std::cout << "min(a, b) = " << ::min(a, b) << std::endl;
    std::cout << "max(a, b) = " << ::max(a, b) << std::endl;

    std::string c = "chaine1";
    std::string d = "chaine2";

    ::swap(c, d);
    std::cout << "c = " << c << ", d = " << d << std::endl;
    std::cout << "min(c, d) = " << ::min(c, d) << std::endl;
    std::cout << "max(c, d) = " << ::max(c, d) << std::endl;

    return 0;
}


// #include <iostream>
// #include <string>
// #include "whatever.hpp"
// 
// // テンプレートの動作を証明するためのテスト用カスタムクラス
// class MiniCar {
// private:
//     std::string model;
//     int         lidarRange;
// 
// public:
//     MiniCar(std::string m, int range) : model(m), lidarRange(range) {}
//     
//     std::string getModel() const { return model; }
//     int         getRange() const { return lidarRange; }
// 
//     // テンプレート関数で使用される比較演算子のオーバーロード
//     bool operator<=(const MiniCar& other) const { return lidarRange <= other.lidarRange; }
//     bool operator>=(const MiniCar& other) const { return lidarRange >= other.lidarRange; }
//     bool operator<(const MiniCar& other) const { return lidarRange < other.lidarRange; }
//     bool operator>(const MiniCar& other) const { return lidarRange > other.lidarRange; }
//     bool operator==(const MiniCar& other) const { return lidarRange == other.lidarRange; }
//     bool operator!=(const MiniCar& other) const { return lidarRange != other.lidarRange; }
// };
// 
// // 出力ストリーム用のオーバーロード
// std::ostream& operator<<(std::ostream& os, const MiniCar& car) {
//     os << car.getModel() << " [LiDAR: " << car.getRange() << "]";
//     return os;
// }
// 
// int main(void) {
//     std::cout << "--- Subject provided tests (int & std::string) ---" << std::endl;
//     int a = 2;
//     int b = 3;
// 
//     ::swap(a, b);
//     std::cout << "a = " << a << ", b = " << b << std::endl;
//     std::cout << "min(a, b) = " << ::min(a, b) << std::endl;
//     std::cout << "max(a, b) = " << ::max(a, b) << std::endl;
// 
//     std::string c = "chaine1";
//     std::string d = "chaine2";
// 
//     ::swap(c, d);
//     std::cout << "c = " << c << ", d = " << d << std::endl;
//     std::cout << "min(c, d) = " << ::min(c, d) << std::endl;
//     std::cout << "max(c, d) = " << ::max(c, d) << std::endl;
// 
//     std::cout << "\n--- Custom tests (double) ---" << std::endl;
//     double e = 42.42;
//     double f = -21.21;
//     ::swap(e, f);
//     std::cout << "e = " << e << ", f = " << f << std::endl;
//     std::cout << "min(e, f) = " << ::min(e, f) << std::endl;
//     std::cout << "max(e, f) = " << ::max(e, f) << std::endl;
// 
//     std::cout << "\n--- Custom tests (Custom Class: MiniCar) ---" << std::endl;
//     MiniCar car1("ROS2-Bot", 150);
//     MiniCar car2("RasPi-Racer", 300);
// 
//     ::swap(car1, car2);
//     std::cout << "car1 = " << car1 << ", car2 = " << car2 << std::endl;
//     std::cout << "min(car1, car2) = " << ::min(car1, car2) << std::endl;
//     std::cout << "max(car1, car2) = " << ::max(car1, car2) << std::endl;
// 
//     return 0;
// }
// 
