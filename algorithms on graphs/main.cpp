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
// Определяем типы данных для нашего графа
typedef adjacency_list<listS, vecS, directedS, no_property, property<edge_weight_t, int> > Graph;//listS это список смежности, vecS это вектор вершин, directedS это значит что граф направленный, no_property это значит что у вершин нет свойств, property<edge_weight_t, int> означает что ребра имеют свойство веса
typedef graph_traits<Graph>::vertex_descriptor Vertex;//вершины в графе
typedef std::pair<int, int> Edge;//ребра в графе


int INF = INT_MAX;



std::vector<int> dijkstra(std::vector<std::vector<int>> matr, int start)
{
	std::vector<int> dist(matr.size(), INF);
	std::vector<bool>visited(matr.size());
	dist[start] = 0;

	for (int i=0;i < matr.size();i++)
	{
		int nearest = -1;//ближайшая не обработанная вершина

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

void generateDotFile(std::vector<std::vector<int>> matr, const std::string& path)//создает по матрице файл нужный для изображения графа
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


// Функция генерации случайной матрицы стоимости соединений вершин
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

// Функция для конвертации матрицы в граф
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
		// Генерация матрицы стоимости
		std::vector<std::vector<int>> costMatrix = generateRandomCostMatrix(size, maxCost);

		int startPoint = rand() % size; // Генерация случайной начальной точки

		// Запуск и замер времени для вашего алгоритма
		auto start = std::chrono::high_resolution_clock::now();
		std::vector<int> dists = dijkstra(costMatrix, startPoint);
		auto end = std::chrono::high_resolution_clock::now();
		customAlgorithmTotalTime += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

		// Конвертация матрицы стоимости в граф для библиотечного алгоритма
		std::pair<Graph, std::vector<int>> graph_and_weights = convertMatrixToGraph(costMatrix);
		Graph g = graph_and_weights.first;
		std::vector<int> weights = graph_and_weights.second;

		std::vector<Vertex> p(num_vertices(g));
		std::vector<int> d(num_vertices(g));
		Vertex s = vertex(startPoint, g);

		// Запуск и замер времени для библиотечного алгоритма
		start = std::chrono::high_resolution_clock::now();
		dijkstra_shortest_paths(g, s, predecessor_map(&p[0]).distance_map(&d[0]));
		end = std::chrono::high_resolution_clock::now();
		boostAlgorithmTotalTime += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	}

	std::cout << "Среднее время выполнения своего алгоритма Дейкстры: " << customAlgorithmTotalTime / testCount << "ms\n";
	std::cout << "Среднее время выполнения библиотечного алгоритма Дейкстры: " << boostAlgorithmTotalTime / testCount << "ms\n";
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



	
	// Инициализируем ребра и их веса
	const Edge edges[] = { Edge(0, 1), Edge(0, 2), Edge(0, 3), Edge(1, 4), Edge(2, 4), Edge(3, 2), Edge(3, 4) };
	const int weights[] = { 3, 6, 1, 8, 4, 4, 12 };

	Graph g(edges, edges + sizeof(edges) / sizeof(Edge), weights, 5);

	// Инициализируем свойства графа для выполнения алгоритма Дейкстры
	std::vector<Vertex> p(num_vertices(g));
	std::vector<int> d(num_vertices(g));
	Vertex s = vertex(0, g);

	// Выполняем алгоритм Дейкстры
	dijkstra_shortest_paths(g, s, predecessor_map(&p[0]).distance_map(&d[0]));

	for (auto x : d)
		std::cout << x << " ";
	std::cout << std::endl;


	runTests(1000, 50, 100);
}

/*
0 3 5 1 9
0 3 5 1 9
Среднее время выполнения своего алгоритма Дейкстры: 729.413ms
Среднее время выполнения библиотечного алгоритма Дейкстры: 1002.24ms
*/