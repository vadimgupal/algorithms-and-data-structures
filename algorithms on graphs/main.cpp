#include <iostream>
#include<fstream>
#include<algorithm>
#include<vector>
#include<chrono>
#include <random>
#include<numeric>


#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>


using namespace boost;
// ���������� ���� ������ ��� ������ �����
typedef adjacency_list<listS, vecS, directedS, no_property, property<edge_weight_t, int> > Graph;//listS ��� ������ ���������, vecS ��� ������ ������, directedS ��� ������ ��� ���� ������������, no_property ��� ������ ��� � ������ ��� �������, property<edge_weight_t, int> �������� ��� ����� ����� �������� ����
typedef graph_traits<Graph>::vertex_descriptor Vertex;//������� � �����
typedef std::pair<int, int> Edge;//����� � �����


int INF = INT_MAX;



std::vector<int> dijkstra(std::vector<std::vector<int>> matr, int start)
{
	std::vector<int> dist(matr.size(), INF);
	std::vector<bool>visited(matr.size());
	dist[start] = 0;

	for (int i=0;i < matr.size();i++)
	{
		int nearest = -1;//��������� �� ������������ �������

		for (int j = 0;j < matr.size();j++)
			if (!visited[j] && (nearest == -1 || dist[nearest] > dist[j]))
				nearest = j;

		visited[nearest] = true;

		for (int j = 0;j < matr[nearest].size();j++)
			if (dist[nearest] + matr[nearest][j] < dist[j] && dist[nearest] != INF && matr[nearest][j]!=0)
				dist[j] = dist[nearest] + matr[nearest][j];
	}
	return dist;
}

void generateDotFile(std::vector<std::vector<int>> matr, const std::string& path)//������� �� ������� ���� ������ ��� ����������� �����
{
	std::ofstream ofs(path);
	ofs << "digraph G {\n";
	for (int i = 0; i < matr.size(); ++i)
		for (int j = 0; j < matr.size(); ++j)
			if (matr[i][j] != 0 && i != j)
				ofs << " " << i << " -> " << j << " [label=\"" << matr[i][j] << "\"];\n";

	ofs << "}\n";
	ofs.close();
}


// ������� ��������� ��������� ������� ��������� ���������� ������
std::vector<std::vector<int>> generateRandomCostMatrix(int size, int maxCost) {
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> uni(0, maxCost);

	std::vector<std::vector<int>> matrix(size, std::vector<int>(size));
	for (int i = 0; i < size; ++i) {
		for (int j = 0; j < size; ++j) {
			matrix[i][j] = uni(rng);
		}
	}
	return matrix;
}

// ������� ��� ����������� ������� � ����
std::pair<Graph, std::vector<int>> convertMatrixToGraph(const std::vector<std::vector<int>>& costMatrix) {
	std::vector<Edge> edges;
	std::vector<int> weights;

	for (int i = 0; i < costMatrix.size(); ++i) {
		for (int j = 0; j < costMatrix[i].size(); ++j) {
			if (i != j && costMatrix[i][j] != 0) {
				edges.push_back(Edge(i, j));
				weights.push_back(costMatrix[i][j]);
			}
		}
	}

	return { Graph(edges.begin(), edges.end(), weights.begin(), costMatrix.size()), weights };
}


void runTests(int testCount, int size, int maxCost) {
	double customAlgorithmTotalTime = 0.0;
	double boostAlgorithmTotalTime = 0.0;

	for (int i = 0; i < testCount; i++)
	{
		// ��������� ������� ���������
		std::vector<std::vector<int>> costMatrix = generateRandomCostMatrix(size, maxCost);

		int startPoint = rand() % size; // ��������� ��������� ��������� �����

		// ������ � ����� ������� ��� ������ ���������
		auto start = std::chrono::high_resolution_clock::now();
		std::vector<int> dists = dijkstra(costMatrix, startPoint);
		auto end = std::chrono::high_resolution_clock::now();
		customAlgorithmTotalTime += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

		// ����������� ������� ��������� � ���� ��� ������������� ���������
		std::pair<Graph, std::vector<int>> graph_and_weights = convertMatrixToGraph(costMatrix);
		Graph g = graph_and_weights.first;
		std::vector<int> weights = graph_and_weights.second;

		std::vector<Vertex> p(num_vertices(g));
		std::vector<int> d(num_vertices(g));
		Vertex s = vertex(startPoint, g);

		// ������ � ����� ������� ��� ������������� ���������
		start = std::chrono::high_resolution_clock::now();
		dijkstra_shortest_paths(g, s, predecessor_map(&p[0]).distance_map(&d[0]));
		end = std::chrono::high_resolution_clock::now();
		boostAlgorithmTotalTime += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	}

	std::cout << "������� ����� ���������� ������ ��������� ��������: " << customAlgorithmTotalTime / testCount << "ms\n";
	std::cout << "������� ����� ���������� ������������� ��������� ��������: " << boostAlgorithmTotalTime / testCount << "ms\n";
}

int main()
{
	setlocale(LC_ALL, "ru");
	std::vector<std::vector<int>>matr(5, std::vector<int>(5,0));
	matr[0][1] = 3;
	matr[0][2] = 6;
	matr[0][3] = 1;
	
	matr[1][4] = 8;

	matr[2][4] = 4;

	matr[3][2] = 4;
	matr[3][4] = 12;

	std::vector<int>dist = dijkstra(matr, 0);//0 3 5 1 9
	for (auto x : dist)
		std::cout << x << " ";
	std::cout << std::endl;

	generateDotFile(matr, "graph.dot");



	
	// �������������� ����� � �� ����
	const Edge edges[] = { Edge(0, 1), Edge(0, 2), Edge(0, 3), Edge(1, 4), Edge(2, 4), Edge(3, 2), Edge(3, 4) };
	const int weights[] = { 3, 6, 1, 8, 4, 4, 12 };

	Graph g(edges, edges + sizeof(edges) / sizeof(Edge), weights, 5);

	// �������������� �������� ����� ��� ���������� ��������� ��������
	std::vector<Vertex> p(num_vertices(g));
	std::vector<int> d(num_vertices(g));
	Vertex s = vertex(0, g);

	// ��������� �������� ��������
	dijkstra_shortest_paths(g, s, predecessor_map(&p[0]).distance_map(&d[0]));

	for (auto x : d)
		std::cout << x << " ";
	std::cout << std::endl;


	runTests(1000, 50, 100);
}

/*
0 3 5 1 9
0 3 5 1 9
������� ����� ���������� ������ ��������� ��������: 729.413ms
������� ����� ���������� ������������� ��������� ��������: 1002.24ms
*/