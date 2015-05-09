#include "Model.h"
#include <set>
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

Model::Model(const string &filename)
{
}

void constructTetrahedron(int base1, int base2, int base3, int top, vector<tuple<int, int, int>> *result) {
	result->emplace_back(make_tuple(base1, base2, top));
	result->emplace_back(make_tuple(base2, base3, top));
	result->emplace_back(make_tuple(base1, top, base3));
	result->emplace_back(make_tuple(base1, base3, base2));
}

vector<tuple<int, int, int>> convexHull(const vector<Pnt3f> &points) {
	int n = (int)points.size();
	vector<tuple<int, int, int>> ans;
	set<int> processed;
	for (int i = 0; i < n && ans.empty(); ++i)
		for (int j = i + 1; j < n && ans.empty(); ++j)
			for (int k = j + 1; k < n && ans.empty(); ++k)
				for (int l = k + 1; l < n && ans.empty(); ++l)
					if (!colinear(points[i], points[j], points[k])
						&& !colinear(points[i], points[j], points[l])
						&& !colinear(points[i], points[k], points[l])
						&& !colinear(points[j], points[k], points[l])) {
						constructTetrahedron(i, j, k, l, &ans);
						processed.insert(i);
						processed.insert(j);
						processed.insert(k);
						processed.insert(l);
					}

	// degeneration case
	if (ans.empty())
		return ans;

	while ((int) processed.size() < n) {
		int curr;
		for (int i = 0; i < n; ++i)
			if (processed.count(i) == 0) {
				curr = i;
				break;
			}
		processed.insert(curr);

		vector<tuple<int, int, int>> new_ans;
		for (const tuple<int, int, int> &ti : ans) {
			auto side1 = points[get<1>(ti)] - points[get<0>(ti)],
				side2 = points[get<2>(ti)] - points[get<1>(ti)];
			auto norm = side1 * side2, v = points[curr] - points[get<0>(ti)];
			if (fgt(dot(norm, v), 0))
				constructTetrahedron(get<0>(ti), get<1>(ti), get<2>(ti), curr, &new_ans);
			else
				new_ans.emplace_back(ti);
		}
		ans = new_ans;
	}

	return ans;
}