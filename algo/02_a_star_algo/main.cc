//
// Created by ruoshui on 1/15/19.
//

#include <vector>
#include <iostream>
#include <cassert>
#include "a_star.h"

void test_openset() {
    algo::OpenSet openset;
    std::shared_ptr<algo::PointInfo> info(new algo::PointInfo);
    info->point = {1,1};
    info->f_score = 100;
    openset.push(info);
    std::shared_ptr<algo::PointInfo> info2(new algo::PointInfo);
    info2->point = {2,2};
    info2->f_score = 90;
    openset.push(info2);
    std::shared_ptr<algo::PointInfo> info3(new algo::PointInfo);
    info3->point = {3,3};
    info3->f_score = 80;
    openset.push(info3);

    assert(openset.size() == 3);

    auto t = openset.top();
    assert(t->f_score == 80);
    openset.pop();
    t = openset.top();
    assert(t->f_score == 90);
    openset.pop();

    t = openset.top();
    assert(t->f_score == 100);
    openset.pop();
    assert(openset.empty());
}
void test_push() {
    algo::OpenSet openset;
    std::shared_ptr<algo::PointInfo> info(new algo::PointInfo);
    info->point = {1,1};
    info->f_score = 100;
    openset.push(info);
    std::shared_ptr<algo::PointInfo> info2(new algo::PointInfo);
    info2->point = {2,2};
    info2->f_score = 100;
    openset.push(info2);
    assert(openset.size() == 2);
    openset.pop();
    assert(openset.size() == 1);

}

void print_path(std::vector<std::vector<int>> map, const std::vector<algo::Point> &vs) {
    for (auto v : vs) {
        map[v.x][v.y] = 2;
    }
    for (auto ms : map) {
        for (auto m : ms) {
            std::cout << m << "  ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
void test_a_star() {
        std::vector<std::vector<int>> map = {
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
            {0, 0, 1, 1, 1, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 1, 0, 0, 1, 0, 0},
            {0, 0, 0, 0, 1, 0, 0, 1, 0, 0},
            {0, 0, 0, 0, 1, 0, 0, 1, 0, 0},
            {0, 0, 0, 0, 1, 0, 0, 1, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 1, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };
    algo::AStar aStar(map);
    std::vector<algo::Point> vs;
    vs = aStar.Search({3,2}, {8, 8});
    print_path(map, vs);

    vs = aStar.Search({1,3}, {3,2});
    print_path(map, vs);
}
int main() {
//    test_openset();
//    test_push();
    test_a_star();


    return 0;
}