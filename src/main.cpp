// CS1005 Discrete Structures - Fall 2026 - Course Project
// Finding network motifs in the email-Eu-core network
//
// Group leader : Shahzaib Afzal (25I-0924)
// Members      : Shahzaib Afzal (25I-0924), Muhammad Usman (25I-0537),
//                Hashim Amjad (25I-0814), Umair (25I-1009)

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "chart.hpp"
#include "graph.hpp"
#include "nullmodel.hpp"
#include "triad.hpp"
using namespace std;
// last four digits of each member's roll number, read as numbers
//   25I-0924 -> 924, 25I-0537 -> 537, 25I-0814 -> 814, 25I-1009 -> 1009
static const int ROLL_LAST4[] = {924, 537, 814, 1009};

struct Params {
    long long R;
    int pairNumber;
    int idxA, idxB;         
    int networks;           
    int attemptFactor;       
    unsigned long long seed;
};

static bool readPairs(const string& path, vector<pair<int, int> >& out) {
    ifstream in(path.c_str());
    if (!in) return false;
    string line;
    while (getline(in, line)) {
        if (line.empty() || line[0] == '#') continue;
        istringstream ss(line);
        int a, b;
        if (ss >> a >> b) out.push_back(make_pair(a, b));
    }
    return true;
}


static Params deriveParams() {
    Params p;
    p.R = 0;
    int members = sizeof(ROLL_LAST4) / sizeof(ROLL_LAST4[0]);
    for (int i = 0; i < members; i++) p.R += ROLL_LAST4[i];

    p.pairNumber = static_cast<int>(p.R % 190);

    int counter = 0;
    p.idxA = p.idxB = -1;
    for (int i = 0; i < 20 && p.idxA < 0; i++) {
        for (int j = i + 1; j < 20; j++) {
            if (counter == p.pairNumber) {
                p.idxA = i;
                p.idxB = j;
                break;
            }
            counter++;
        }
    }

    p.networks = 20 + static_cast<int>(p.R % 31);
    p.attemptFactor = 5 + static_cast<int>(p.R % 6);
    p.seed = static_cast<unsigned long long>(p.R);
    return p;
}


struct DeptGraph {
    int dept;
    int n;                      
    vector<Edge> edges;     
};

static DeptGraph buildDeptGraph(int dept, const vector<Edge>& allEdges,
                                const map<int, int>& deptOf) {
    DeptGraph g;
    g.dept = dept;

    map<int, int> local;   
    for (map<int, int>::const_iterator it = deptOf.begin(); it != deptOf.end(); ++it) {
        if (it->second == dept) {
            int id = static_cast<int>(local.size());
            local[it->first] = id;
        }
    }
    g.n = static_cast<int>(local.size());

    for (size_t i = 0; i < allEdges.size(); i++) {
        map<int, int>::const_iterator a = local.find(allEdges[i].first);
        map<int, int>::const_iterator b = local.find(allEdges[i].second);
        if (a != local.end() && b != local.end()) g.edges.push_back(Edge(a->second, b->second));
    }
    return g;
}


struct DeptResult {
    Census real;
    ZScores z;
    array<double, 13> mean, sd;
};

static DeptResult analyse(const DeptGraph& g, const Params& p) {
    DeptResult r;
    r.real = triad_census(g.n, g.edges);

    long long attempts = static_cast<long long>(p.attemptFactor) * static_cast<long long>(g.edges.size());
    vector<Census> random;
    for (int t = 0; t < p.networks; t++) {

        vector<Edge> swapped = randomize(g.edges, attempts, p.seed + t);
        random.push_back(triad_census(g.n, swapped));
        if ((t + 1) % 10 == 0 || t + 1 == p.networks)
            cout << "   random graph " << (t + 1) << "/" << p.networks << " done\n";
    }
    r.z = compute_z(r.real, random);
    mean_and_sd(random, r.mean, r.sd);
    return r;
}


int main(int argc, char** argv) {
    string edgeFile = "data/email-Eu-core.txt";
    string labelFile = "data/email-Eu-core-department-labels.txt";
    if (argc >= 2) edgeFile = argv[1];
    if (argc >= 3) labelFile = argv[2];

    int members = sizeof(ROLL_LAST4) / sizeof(ROLL_LAST4[0]);
   
    vector<pair<int, int> > rawEdges, rawLabels;
    if (!readPairs(edgeFile, rawEdges)) {
        cerr << "Cannot open " << edgeFile << " (see data/README.txt)\n";
        return 1;
    }
    if (!readPairs(labelFile, rawLabels)) {
        cerr << "Cannot open " << labelFile << " (see data/README.txt)\n";
        return 1;
    }

    set<pair<int, int> > seen;
    vector<Edge> edges;
    long long loops = 0, dups = 0;
    for (size_t i = 0; i < rawEdges.size(); i++) {
        if (rawEdges[i].first == rawEdges[i].second) { loops++; continue; }
        if (!seen.insert(rawEdges[i]).second) { dups++; continue; }
        edges.push_back(rawEdges[i]);
    }

    map<int, int> deptOf;
    for (size_t i = 0; i < rawLabels.size(); i++) deptOf[rawLabels[i].first] = rawLabels[i].second;

    map<int, long long> internal;
    for (map<int, int>::iterator it = deptOf.begin(); it != deptOf.end(); ++it)
        internal[it->second] += 0;   
    for (size_t i = 0; i < edges.size(); i++) {
        map<int, int>::iterator a = deptOf.find(edges[i].first);
        map<int, int>::iterator b = deptOf.find(edges[i].second);
        if (a != deptOf.end() && b != deptOf.end() && a->second == b->second)
            internal[a->second]++;
    }

    vector<pair<long long, int> > ranking;   
    for (map<int, long long>::iterator it = internal.begin(); it != internal.end(); ++it)
        ranking.push_back(make_pair(-it->second, it->first));
    sort(ranking.begin(), ranking.end());
    if (ranking.size() < 20) {
        cerr << "Need at least 20 departments, found " << ranking.size() << "\n";
        return 1;
    }

    Params p = deriveParams();
    int deptA = ranking[p.idxA].second;
    int deptB = ranking[p.idxB].second;

    cout << "================ T1: parameters ================\n";
    cout << "raw edge lines          : " << rawEdges.size() << "\n";
    cout << "self-loops removed      : " << loops << "\n";
    cout << "duplicate edges removed : " << dups << "\n";
    cout << "edges in G (simple)     : " << edges.size() << "\n";
    cout << "nodes with a label      : " << deptOf.size() << ", departments: " << internal.size() << "\n\n";
    cout << "R = ";
    for (int i = 0; i < members; i++) cout << (i ? " + " : "") << ROLL_LAST4[i];
    cout << " = " << p.R << "\n";
    cout << "pair number  = R mod 190 = " << p.pairNumber << "  ->  (i, j) = (" << p.idxA << ", " << p.idxB << ")\n";
    cout << "random graphs = 20 + (R mod 31) = " << p.networks << "\n";
    cout << "attempts      = (5 + (R mod 6)) * edges = " << p.attemptFactor << " * edges\n";
    cout << "seed          = " << p.seed << " (graph t uses seed + t)\n\n";

    cout << "Top-20 departments by internal edges:\n";
    for (int i = 0; i < 20; i++)
        cout << "  [" << setw(2) << i << "] dept " << setw(2) << ranking[i].second
                  << "  internal edges = " << -ranking[i].first << (i == p.idxA ? "   <- Department A" : "")
                  << (i == p.idxB ? "   <- Department B" : "") << "\n";
    cout << "\nDepartment A = " << deptA << ", Department B = " << deptB << "\n\n";


    DeptGraph gA = buildDeptGraph(deptA, edges, deptOf);
    DeptGraph gB = buildDeptGraph(deptB, edges, deptOf);
    cout << "================ T2: department subgraphs ================\n";
    cout << "Department A (" << deptA << "): " << gA.n << " nodes, " << gA.edges.size() << " edges\n";
    cout << "Department B (" << deptB << "): " << gB.n << " nodes, " << gB.edges.size() << " edges\n\n";


    {
        // a -> b, b -> c  (a path)   with a=0, b=1, c=2
        vector<Edge> e;
        e.push_back(Edge(0, 1));
        e.push_back(Edge(1, 2));
        EdgeSet S = makeEdgeSet(e);
        cout << "================ T3: hand example ================\n";
        cout << "triad 0->1, 1->2 : canonical code = " << canonical_code(S, 0, 1, 2)
                  << ", class = " << triad_class(S, 0, 1, 2) << "  (worked out by hand in README.md)\n\n";
    }


    cout << "================ running Department A ================\n";
    DeptResult rA = analyse(gA, p);
    cout << "================ running Department B ================\n";
    DeptResult rB = analyse(gB, p);

    cout << "\n================ T4: triad counts ================\n";
    cout << "Class | Dept " << setw(2) << deptA << " (A) | Dept " << setw(2) << deptB << " (B)\n";
    for (int k = 0; k < 13; k++)
        cout << setw(5) << k + 1 << " | " << setw(10) << rA.real[k] << "   | " << setw(10)
                  << rB.real[k] << "\n";

    cout << "\n================ T6: Z-scores ================\n";
    cout << fixed << setprecision(2);
    cout << "Class |   Z (A)   |   Z (B)   | A says  | B says\n";
    for (int k = 0; k < 13; k++) {
        const char* la = rA.z[k] > 2 ? "motif" : (rA.z[k] < -2 ? "anti" : "-");
        const char* lb = rB.z[k] > 2 ? "motif" : (rB.z[k] < -2 ? "anti" : "-");
        cout << setw(5) << k + 1 << " | " << setw(9) << rA.z[k] << " | " << setw(9) << rB.z[k]
                  << " | " << setw(7) << la << " | " << lb << "\n";
    }


    string csvPath = "results/results_R" + to_string(p.R) + ".csv";
    ofstream csv(csvPath.c_str());
    if (csv) {
        csv << "class,code,N_A,N_B,mean_A,sd_A,Z_A,mean_B,sd_B,Z_B\n";
        for (int k = 0; k < 13; k++)
            csv << k + 1 << "," << CLASS_CODES[k] << "," << rA.real[k] << "," << rB.real[k] << "," << rA.mean[k]
                << "," << rA.sd[k] << "," << rA.z[k] << "," << rB.mean[k] << "," << rB.sd[k] << "," << rB.z[k] << "\n";
        cout << "\nsaved " << csvPath << "\n";
    } else {
        cout << "\n(could not write " << csvPath << " - does the results/ folder exist?)\n";
    }

    string svgPath = "results/zscores_R" + to_string(p.R) + ".svg";
    ostringstream title;
    title << "Triad Z-scores, R = " << p.R << " (Dept " << deptA << " vs Dept " << deptB << ")";
    write_chart(svgPath, title.str(), "Dept " + to_string(deptA) + " (A)",
                "Dept " + to_string(deptB) + " (B)", rA.z, rB.z);
    cout << "saved " << svgPath << "\n";
    return 0;
}
