#pragma once

#include "voronoi.h"
#include "site.h"

struct Node
{
    Site site;
    Node* next;
    Node* prev;

    Node() { }

    Node(Site site)
    {
        this->site = site;
        this->next = nullptr;
        this->prev = nullptr;
    }

    Node(Site site, Node* next, Node* prev)
    {
        this->site = site;
        this->next = next;
        this->prev = prev;
    }

    void setNext(Node* next) { this->next = next; }
    void setPrev(Node* prev) { this->prev = prev; }
};
