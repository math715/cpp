//
// Created by ruoshui on 1/15/19.
//

#include <cassert>
#include <iostream>
#include "a_star.h"


namespace algo {
    int Point::HeuristicCostEstimate(const algo::Point &point) const {
        return 10 * ( abs(point.x - x) + abs(point.y - y));
    }

    int Point::g_score(const algo::Point &point) {
        int dx = abs(x - point.x);
        int dy = abs(y - point.y);
        assert(dx <= 1 && dy <= 1);
        return dx + dy == 1? 10 : 14;
    }

    int OpenSet::Cost(const algo::Point &point) {
        assert(infotable.find(point) != infotable.end());
        return infotable[point]->f_score;
    }

    void OpenSet::push(std::shared_ptr<PointInfo> &info) {
        infotable[info->point] = info;
        openset_.insert(info);
    }

    bool OpenSet::empty() {
        return openset_.empty();
    }

    const std::shared_ptr<PointInfo>& OpenSet::top()  {
        assert(!openset_.empty());

        return *openset_.begin();
    }

    void OpenSet::pop() {
        assert(!openset_.empty());
        auto p = (*openset_.begin())->point;
//        std::cout << "delete " << p.x << "\t" << p.y << std::endl;
//        std::cout << "debug" << std::endl;
//        for (auto curr_info : openset_) {
//            std::cout << curr_info->point.x << "\t" << curr_info->point.y << "\t" << curr_info->g_score
//                      << "\t"<< curr_info->h_score << "\t" << curr_info->f_score << std::endl;
//        }
//        std::cout << "***********************" << std::endl;
        openset_.erase(openset_.begin());
//        for (auto curr_info : openset_) {
//            std::cout << curr_info->point.x << "\t" << curr_info->point.y << "\t" << curr_info->g_score
//                      << "\t"<< curr_info->h_score << "\t" << curr_info->f_score << std::endl;
//        }
//        std::cout << "end" << std::endl;

        infotable.erase(p);
    }

    bool OpenSet::contain(const algo::Point &point) {
        return infotable.find(point) != infotable.end();
    }

    std::shared_ptr<PointInfo> OpenSet::get(const algo::Point &point) {
        assert(contain(point));
        return infotable[point];
    }

    void OpenSet::clear() {
        openset_.clear();
        infotable.clear();
    }

    void OpenSet::replace(std::shared_ptr<algo::PointInfo> &old_info, std::shared_ptr<algo::PointInfo> &new_info) {
        assert(PointEqual()(old_info->point, new_info->point));
        // Bug opeset_.eaase remove all pointinfo f_score == old_info f_score
//        openset_.erase(old_info);
        auto it = openset_.begin();
        for (; it != openset_.end(); ++it) {
            if ((*it)->point == old_info->point){
                break;
            }
        }
        openset_.erase(it);
        openset_.insert(new_info);
        infotable[new_info->point] = new_info;
    }

    std::vector<Point> AStar::Search(const algo::Point &start, const algo::Point &end) {
        close_set_.clear();
        openset_.clear();
        fathertables.clear();
        InitOpenSet(start, end);
        while (!openset_.empty()) {
            auto curr_info = openset_.top();
            if (PointEqual()(curr_info->point, end)) {
                return getPath(start, end);
            }
//            std::cout << curr_info->point.x << "\t" << curr_info->point.y << "\t" << curr_info->g_score
//            << "\t"<< curr_info->h_score << "\t" << curr_info->f_score << std::endl;
            // del point from openset
            openset_.pop();
//            openset_.erase(openset_.begin());
            auto curr_point = curr_info->point;
            close_set_.insert(curr_info->point);
            for (auto neighbor_point : neighborNodes(curr_point) ) {
                // neighbor in close set skip
                if (close_set_.find(neighbor_point) != close_set_.end()) {
                    continue;
                }
                std::shared_ptr<PointInfo> new_info(new PointInfo);
                new_info->point = neighbor_point;
                int tentative_g_score = curr_info->g_score + curr_point.g_score(neighbor_point);
                if (openset_.contain(neighbor_point)) {
                    auto neighbor_info = openset_.get(neighbor_point);
                    if (tentative_g_score < neighbor_info->g_score ) {
                        new_info->g_score = tentative_g_score;
                        new_info->h_score = neighbor_info->h_score;
                        new_info->f_score = new_info->g_score + new_info->h_score;
//                        new_info->father = curr_point;
                        fathertables[neighbor_point] = curr_point;
                        openset_.replace(neighbor_info, new_info);
                    }
                } else {
                    new_info->g_score = tentative_g_score;
                    new_info->h_score = neighbor_point.HeuristicCostEstimate(end);
                    new_info->f_score = new_info->g_score + new_info->h_score;
                    fathertables[neighbor_point] = curr_point;
                    openset_.push(new_info);
                }
            }
        }
         std::vector<Point> vec;
        return vec;

    }

    void AStar::InitOpenSet(const algo::Point &start, const Point &end) {
        std::shared_ptr<PointInfo> info(new PointInfo());
        info->point = start;
        info->g_score = 0;
        info->h_score = start.HeuristicCostEstimate(end);
        info->f_score = info->g_score + info->h_score;
//        info->father = start;
        openset_.push(info);
        fathertables[start] = start;

    }

    std::vector<Point> AStar::neighborNodes(const algo::Point &point) {
        std::vector<Point> result;
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                if (dx == 0 && dy == 0){
                    continue;
                }
                Point p;
                p.x = point.x + dx;
                p.y = point.y + dy;
                if (p.x >= 0 && p.x < map_rows_ && p.y >= 0 && p.y < map_cols_) {
                    if (map_[p.x][p.y] == 0) {// can not arrival
                        result.push_back(p);
                    }
                }
            }
        }

        return result;
    }

    std::vector<Point> AStar::getPath(const algo::Point &start, const algo::Point &end) {
        std::vector<Point> result;
        auto p = end;
        result.push_back(end);
        int cnt = 0;
        while( !PointEqual()(start, p)) {
            cnt++;
            if (cnt > map_cols_ *map_rows_) {
                assert(true);
            }
            p = fathertables[p];
            result.insert(result.begin(), p);
        }
        return result;
    }
}