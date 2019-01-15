//
// Created by ruoshui on 1/15/19.
//

#ifndef ALGO_A_STAR_H
#define ALGO_A_STAR_H

#include <set>
#include <map>
#include <unordered_set>
#include <queue>
#include <memory>
#include <unordered_map>

namespace  algo {


    struct Point {
        int x;
        int y;
        int HeuristicCostEstimate(const Point &point) const;
        // Adjacent point score 1
        // Diagonal point score 4
        int g_score(const Point &point);

    };
    inline bool operator==(const Point &lhs, const Point &rhs){
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }
//    Point dir[8] = {
//            {-1, 0},  //up
//            {1, 0},   //down
//            {0, -1},  //left
//            {0, 1 },  //right
//            {-1, -1}, //up left
//            {-1, 1},  //up right
//            {1, -1},  //down left
//            {1, 1},   //down right
//    };

    struct PointInfo {
        PointInfo() = default;
        Point point;
//        Point father;
        int g_score;  // g(n)
        int h_score; // h(n)
        int f_score; // f(n) = g(n) + h(n)
    };
    struct PointHash {
        std::size_t operator() (const algo::Point &point) const {
            return std::hash<int>{}(point.x)+ std::hash<int>{}(point.y);
        }
    };
    struct PointEqual {
        bool operator()(const Point& lhs, const Point& rhs) const
        {
            return lhs.x == rhs.x && lhs.y == rhs.y;
        }
    };
    struct PointCmp {
        bool operator()(const Point &lhs, const Point &rhs) const {
            return lhs.x < rhs.x || (lhs.x == rhs.x && lhs.y < rhs.y);
        }
    };

    struct PointInfoCmp {
        bool operator() (const std::shared_ptr<PointInfo> &lhs, const std::shared_ptr<PointInfo> &rhs) {
            return lhs->f_score < rhs->f_score;
        }
    };
    class OpenSet {
    public:
        int Cost(const Point &point);
        void push(std::shared_ptr<PointInfo> &info);
        bool empty();
        const std::shared_ptr<PointInfo> &top() ;
        void pop();
        bool contain(const Point &point);
        std::shared_ptr<PointInfo> get(const Point &point);
        void replace(std::shared_ptr<PointInfo> &old_info, std::shared_ptr<PointInfo> &new_info);
        int size(){
            return openset_.size();
        }
        void clear();
    private:
        std::unordered_map<Point,  std::shared_ptr<PointInfo>, PointHash, PointEqual> infotable;
        std::multiset<std::shared_ptr<PointInfo>, PointInfoCmp> openset_;
    };


    class AStar {
    public:
        AStar(const std::vector<std::vector<int>> &map):map_(map){
            map_rows_ = map.size();
            map_cols_ = map[0].size();
        }
        std::vector<Point> Search(const Point &start, const Point &end);

    private:
        void InitOpenSet(const algo::Point &start, const Point &end);
        std::vector<Point> neighborNodes(const Point &point);
        std::vector<Point> getPath(const Point&start, const Point &end);
        int map_rows_;
        int map_cols_;
        std::vector<std::vector<int>> map_;
        std::unordered_set<Point, PointHash, PointEqual> close_set_;
        OpenSet openset_;

        std::map<Point, Point, PointCmp> fathertables;

    };
}


#endif //ALGO_A_STAR_H
