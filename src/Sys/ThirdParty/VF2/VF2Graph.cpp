#include "VF2Graph.h"

void VF2Graph::init()
{
	memset(head, -1, sizeof(head));
	vn = 0;
	en = 0;
}

void VF2Graph::addv(int id, int label)
{
	vtx[id] = VF2Vertex(id, label);
	vn++;
}

void VF2Graph::addse(int u, int v, int label)
{
	edge[en] = VF2Edge(u, v, label, head[u]);
	head[u] = en++;
}

void VF2Graph::adde(int u, int v, int label)
{
	addse(u, v, label);
	addse(v, u, label);
}

void VF2Graph::delse(int u, int v, int label)
{
	for (int i = head[u];~i;i = edge[i].next)
	{
		if (edge[i].u == u&&edge[i].v == v&&edge[i].label == label)
		{
			edge[i].del = 1;
			return;
		}
	}
}

void VF2Graph::dele(int u, int v, int label)
{
	for (int i = head[u];~i;i = edge[i].next)
	{
		if (edge[i].u == u&&edge[i].v == v&&edge[i].label == label)
		{
			edge[i].del = 1;
			edge[i ^ 1].del = 1;
			return;
		}
	}
	//delse(u, v, label);
	//delse(v, u, label);
}