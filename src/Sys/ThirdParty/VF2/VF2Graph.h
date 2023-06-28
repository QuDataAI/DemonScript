#ifndef VF2GRAPH_H
#define VF2GRAPH_H

#include "VF2Head.h"

struct VF2Vertex
{
	int id;
	int label;
	int seq;
	bool del;

	VF2Vertex(int _id = 0, int _label = 0) : id(_id), label(_label), seq(-1), del(0) {}
	~VF2Vertex() {}
};

struct VF2Edge
{
	int u;
	int v;
	int label;
	int next;
	bool del;

	VF2Edge(int _u = 0, int _v = 0, int _label = 0, int _next = -1) : u(_u), v(_v), label(_label), next(_next), del(0) {}
	~VF2Edge() {}

	bool operator == (const VF2Edge &o) const
	{
		return u == o.u&&v == o.v&&label == o.label;
	}
};

class VF2Graph
{
public:
	VF2Graph()
	{
		memset(head, -1, sizeof(head));
		vn = 0;
		en = 0;
	}
	~VF2Graph() {}

	void init();
	void addv(int id, int label);
	void addse(int u, int v, int label);
	void adde(int u, int v, int label);
	void delse(int u, int v, int label);
	void dele(int u, int v, int label);

public:
	const static int maxv = 250;
	const static int maxe = 510;

public:
	int head[maxv];
	int vn;
	int en;
	VF2Vertex vtx[maxv]; // 0 to vn-1
	VF2Edge edge[maxe]; // 0 to en-1
};

#endif