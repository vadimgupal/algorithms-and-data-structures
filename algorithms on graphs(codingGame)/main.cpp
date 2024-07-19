#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <map>
#include <cmath>
#include <climits>



using namespace std;

struct edge//ребро
{
	int num = -1;//вершина в которую ведет ребро
	int weight = 1;
};

struct vertex//вершина
{
	int num = -1;
	std::vector<edge> adjacency_vector;//ребра из нее выходящие
};

struct cmpEdge
{
	bool operator()(const edge& a, const edge& b) const
	{
		return a.weight < b.weight;
	}
};



std::pair<std::map<int, int>, std::map<int, int>> dijkstra(std::map<int, vertex> graph, int start)
{
	std::map<int, int> distance;
	std::map<int, bool>visited;
	std::map<int, int> prev;

	for (auto it = graph.begin();it != graph.end();it++)
		distance[it->first] = INT_MAX;

	distance[start] = 0;

	for (int i = 0;i < graph.size();i++)
	{
		int nearest = -1;
		int min_distance = INT_MAX;
		for(auto it=graph.begin();it != graph.end();it++)
			if (!visited[it->first] && min_distance > distance[it->first])
			{
				nearest = it->first;
				min_distance = distance[it->first];
			}
		
		visited[nearest] = true;

		for (edge e : graph[nearest].adjacency_vector)
		{
			if (graph[e.num].num == -1) continue;

			if (!visited[e.num])
			{
				int newdist = distance[nearest] + e.weight;
				if (newdist < distance[e.num])
				{
					distance[e.num] = newdist;
					prev[e.num] = nearest;
				}
			}
		}
	}

	return { distance, prev };
}
std::deque<int> getPath(std::map<int, vertex>& graph, int start, int end, std::vector<std::pair<int, int>>& orks, std::map<int, std::pair<int, int>>& verts)
{
	std::pair<std::map<int, int>, std::map<int, int>> res;
	std::map<int, int> distance, prev;
	bool get_decision = false;
	bool impossible = false;

	while (!get_decision)
	{
		res = dijkstra(graph, start);
		distance = res.first;prev = res.second;

		int v = end;

		while (v != start && distance[v] != 0)
		{
			for (auto ork : orks)
			{
				std::pair<int, int>coord = verts[v];
				double dist = sqrt(pow(coord.first - ork.first, 2) + pow(coord.second - ork.second, 2));
				if (distance[v] >= dist)
				{
					impossible = true;
					break;
				}
			}
			if (impossible)break;
			v = prev[v];
		}

		if (v != start && distance[v] == 0)
		{
			impossible = true;
			get_decision = true;
			break;
		}
		else if (impossible)
		{
			impossible = false;
			for (auto it = verts.begin(); it != verts.end(); it++)
			{
				for (auto ork : orks)
				{
					auto coord = it->second;
					if (distance[v] >= sqrt(pow(coord.first - ork.first, 2) + pow(coord.second - ork.second, 2)))
					{
						distance[v] = INT_MAX;
						graph.erase(v);
					}
				}
			}
		}
		else if (v == start)
		{
			get_decision = true;
			break;
		}
	}


	std::deque<int> path;

	if (impossible) return path;

	while (end != start)
	{
		path.push_front(end);
		end = prev[end];
	}
	path.push_front(end);

	return path;
}


int main()
{
	int n;
	cin >> n; cin.ignore();
	int m;
	cin >> m; cin.ignore();
	int l;
	cin >> l; cin.ignore();

	std::map<int, std::pair<int, int>> verts;
	for (int i = 0; i < n; i++)
		cin >> verts[i].first >> verts[i].second;

	std::vector<std::pair<int, int>> orks;
	for (int i = 0; i < m; i++)
	{
		int xo, yo;
		cin >> xo >> yo;
		orks.push_back(make_pair(xo, yo));
	}

	std::map<int, vertex> graph;
	for (int i = 0; i < n; i++)
		graph[i] = { i, {} };

	for (int i = 0; i < l; i++)
	{
		int n1, n2;
		cin >> n1 >> n2;
		graph[n1].num = n1;
		graph[n1].adjacency_vector.push_back(edge{ n2, 1 });
	}

	int s;
	cin >> s; cin.ignore();
	int e;
	cin >> e; cin.ignore();

	auto res = getPath(graph, s, e, orks, verts);

	if (res.size() == 0)
		cout << "IMPOSSIBLE";
	else for (int i = 0; i < res.size(); i++)
	{
		cout << res[i];
		if (i != res.size() - 1)
			cout << " ";
	}


}