#ifndef VF2_H
#define VF2_H

#include "VF2Head.h"
#include "VF2Graph.h"
#include "Singletone.h"

class Graph;

class VF2
{
	IMPLEMENTATION_SINGLETONE(VF2);

public:
	struct State // State of dfs matching
	{
		vector<prii> s; // Store matched pairs
		// Same means with the paper
		int core1[VF2Graph::maxv]; // core1[n] = m or -1
		int core2[VF2Graph::maxv]; // core2[m] = n or -1
		bool in1[VF2Graph::maxv];
		bool in2[VF2Graph::maxv];
		bool out1[VF2Graph::maxv];
		bool out2[VF2Graph::maxv];
		State()
		{
			s.clear();
			memset(core1, -1, sizeof(core1));
			memset(core2, -1, sizeof(core2));
			memset(in1, 0, sizeof(in1));
			memset(in2, 0, sizeof(in2));
			memset(out1, 0, sizeof(out1));
			memset(out2, 0, sizeof(out2));
		}
	};

	struct Match // Store match
	{
		vector<prii> s; // match
		int id;         // graphDB id
		Match() {}
		Match(vector<prii> _s, int _id) : s(_s), id(_id) {}
	};

public:
	//���������, �������� �� mainGraph �������, ������� �������� ������������ ����� queryGraph
	bool isomorphic(Graph* queryGraph, Graph* mainGraph);      // Run VF2 on pat & g (main procedure)

private:
	//�������������� ���� � ������ VF2
	void convertToVF2(Graph& graph, VF2Graph& graphVF2);
	void GenRevGraph(const VF2Graph &src, VF2Graph &dst); // Generate reversed graph
	bool CheckPrev(const State &s, int a, int b);	// Prev
	bool CheckSucc(const State &s, int a, int b);   // Succ
	bool CheckIn(const State &s);     				// In
	bool CheckOut(const State &s);    				// Out
	bool CheckNew(const State &s);    				// New
	void CalDFSVec(const State &s);                 // Cal all temp vec per dfs
	void CalCheckVec(const State &s, int a, int b); // Cal all temp vec per check
	bool check(const State &s, int a, int b);       // Check feasibility
	void GenPairs(const State &s);                  // Generate all pairs to var allPairs
	void CheckPairs(const State &s);                // Check allPairs, return candiPairs
	void UpdateState(State &s, int a, int b);       // Update state ns with pair(a,b)
	bool FinalCheck(const State &s);									// Final check for answer
	bool dfs(const State &s);											// VF2 dfs (recursive)

private:
	// Temp var
	vector<prii> tlist;            // Store temp list of match
	vector<prii> allPairs;         // Generated pairs
	vector<prii> candiPairs;       // Candidate pairs pass check()
	bool flagIn, flagOut, flagAll; // Flags
	vector<int> pred1, pred2;      // Pred of a in pat & b in g
	vector<int> succ1, succ2;      // Succ of a in pat & b in g
	vector<int> m1, m2;            // M_1, M_2
	vector<int> tin1, tin2;        // Tin_1,Tin_2
	vector<int> tout1, tout2;      // Tout_1,Tout_2
	vector<int> n1, n2;            // N_1, N_2
	vector<int> ns1, ns2;          // Point set of pat & g
	vector<int> t1, t2;            // tin+tout

	VF2Graph pat, g;       // Pattern & DBGraph
	VF2Graph revpat, revg; // Reversed Pattern & DBGraph
};

#endif