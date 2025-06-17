#pragma once

#include <vector>
#include <iostream>
#include <string>

#include "edge.h"

struct Site
{
    double x, y;
    std::vector<Edge*> edges;

    void addEdge(HalfEdge* halfEdge, bool isBorderCuttedEdge = false, double min_x = 0, double min_y = 0, double max_x = 0, double max_y = 0)
    {
        int otherIndex = -1;
        for (int i = 0; i < this->edges.size(); i++) {
            if (edges[i] == halfEdge->otherHalf) {
                otherIndex = i;
                break;
            };
        }

        if (otherIndex == -1) {
            edges.push_back(halfEdge);
            return;
        }

        this->edges.erase(this->edges.begin() + otherIndex);

        double x1 = halfEdge->otherHalf->x + halfEdge->otherHalf->dir_x;
        double y1 = halfEdge->otherHalf->y + halfEdge->otherHalf->dir_y;
        double x2 = halfEdge->x + halfEdge->dir_x;
        double y2 = halfEdge->y + halfEdge->dir_y;
        // if (std::abs(x1 - x2) < 1e-10 && std::abs(y1 - y2) < 1e-10) return;
        // if (x1 == x2 && y1 == y2) return;
        edges.push_back(
            cross(x1, y1, x2, y2, this->x, this->y) < 0 ?
                new Edge(x1, y1, x2, y2) :
                new Edge(x2, y2, x1, y1)
        );



        if (!isBorderCuttedEdge) return;

        if (x1 == min_x || x1 == max_x || y1 == min_y || y1 == max_y) {
            // it is a starting edge
            // it must be signed somehow to start the edge arranging with this
            // ...
        }
    }

    void halfEdgesToEdges()
    {
        std::vector<Edge*> newEdges;

        for (Edge* edge : this->edges) {
            if (HalfEdge* halfEdge = dynamic_cast<HalfEdge*>(edge)) {
                double x1 = halfEdge->x;
                double y1 = halfEdge->y;
                double x2 = halfEdge->x + halfEdge->dir_x;
                double y2 = halfEdge->y + halfEdge->dir_y;
                newEdges.push_back(
                    cross(x1, y1, x2, y2, this->x, this->y) < 0 ?
                        new Edge(x1, y1, x2, y2) :
                        new Edge(x2, y2, x1, y1)
                );
            } else {
                newEdges.push_back(edge);
            }
        }

        this->edges = std::move(newEdges);
    }

    inline double cross(double x1, double y1, double x2, double y2, double px, double py) { return (x2 - x1) * (py - y1) - (y2 - y1) * (px - x1); }
    // positive: p is left, zero: p is on line, negative: p is right



    // void arrangeEdgesToContinuousPath()
    // {
    //     // // Remove zero-length edges
    //     // this->edges.erase(
    //     //     std::remove_if(
    //     //         this->edges.begin(),
    //     //         this->edges.end(),
    //     //         [](Edge* edge) {
    //     //             return
    //     //                 std::abs(edge->x1 - edge->x2) < 1e-10 &&
    //     //                 std::abs(edge->y1 - edge->y2) < 1e-10;
    //     //         }
    //     //     ),
    //     //     this->edges.end()
    //     // );

    //     std::string log = "";
    //     for (int i = 0; i < this->edges.size(); i++) log += std::to_string(i) + ": x1:" + std::to_string(this->edges[i]->x1) + ", y1:" + std::to_string(this->edges[i]->y1) + ", x2:" + std::to_string(this->edges[i]->x2) + ", y2:" + std::to_string(this->edges[i]->y2) + "\n";



    //     std::deque<Edge*> newEdges;

    //     int firstEdgeIndex = getFirstEdgeIndex();
    //     newEdges.push_back(this->edges[firstEdgeIndex]);
    //     edges.erase(this->edges.begin() + firstEdgeIndex);

    //     log += "start:" + std::to_string(firstEdgeIndex) + "\n";



    //     int index, count = 1000;
    //     while (this->edges.size() > 0 && count > 0) {
    //         Edge* lastEdge = newEdges.back();

    //         index = -1;
    //         for (size_t i = 0; i < this->edges.size(); i++) {
    //             if (std::abs(lastEdge->x2 - this->edges[i]->x1) < 1e-10 && std::abs(lastEdge->y2 - this->edges[i]->y1) < 1e-10) {
    //                 index = i;
    //                 break;
    //             }
    //         }
    //         if (index == -1) break; // no matching edge found

    //         newEdges.push_back(this->edges[index]);
    //         this->edges.erase(this->edges.begin() + index);

    //         count--;
    //     }
    //     if (count == 0) std::cout << "while count reached 1000" << std::endl;

    //     // count = 1000;
    //     // while (this->edges.size() > 0 && count > 0) {
    //     //     Edge* first = newEdges.front();

    //     //     index = -1;
    //     //     for (size_t i = 0; i < this->edges.size(); i++) {
    //     //         if (std::abs(first->x1 - this->edges[i]->x2) < 1e-10 && std::abs(first->y1 - this->edges[i]->y2) < 1e-10) {
    //     //             index = i;
    //     //             break;
    //     //         }
    //     //     }
    //     //     if (index == -1) break; // no matching edge found

    //     //     newEdges.push_front(this->edges[index]);
    //     //     this->edges.erase(this->edges.begin() + index);

    //     //     count--;
    //     // }
    //     // if (count == 0) std::cout << "while count reached 1000" << std::endl;

    //     if (this->edges.size() > 0) {
    //         for (int i = 0; i < this->edges.size(); i++) log += "remains - x1:" + std::to_string(this->edges[i]->x1) + ", y1:" + std::to_string(this->edges[i]->y1) + ", x2:" + std::to_string(this->edges[i]->x2) + ", y2:" + std::to_string(this->edges[i]->y2) + "\n";
    //         std::cout << "edges cannot be continous (remains:" << this->edges.size() << ")\n" << log << std::endl;
    //     }

    //     this->edges = std::move(newEdges);
    // }

    // int getFirstEdgeIndex()
    // {
    //     for (size_t i = 0; i < edges.size(); i++) {
    //         bool hasConnecting = false;
    //         for (size_t j = 0; j < edges.size(); j++) {
    //             if (i == j) continue;
    //             if (std::abs(edges[j]->x2 - edges[i]->x1) < 1e-10 &&
    //                 std::abs(edges[j]->y2 - edges[i]->y1) < 1e-10) {
    //                 hasConnecting = true;
    //                 break;
    //             }
    //         }
    //         if (!hasConnecting) return i;
    //     }
    //     return 0;
    // }
};
