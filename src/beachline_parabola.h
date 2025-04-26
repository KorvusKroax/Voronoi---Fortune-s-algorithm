#pragma once

#include "site.h"
#include "halfedge.h"

struct BeachlineParabola {
    Site site;
    HalfEdge* halfEdge_left = nullptr;
    HalfEdge* halfEdge_right = nullptr;

    BeachlineParabola() { }

    BeachlineParabola(Site site, HalfEdge* halfEdge_left = nullptr, HalfEdge* halfEdge_right = nullptr)
    {
        this->site = site;
        this->halfEdge_left = halfEdge_left;
        this->halfEdge_right = halfEdge_right;
    }
};
