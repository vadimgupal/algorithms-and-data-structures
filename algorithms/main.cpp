#include<iostream>
#include<vector>



long long countred(int n)
{
	std::vector<long long>fib(n+1, -1);
	fib[0] = 1;
	fib[1] = 1;
	for (int i = 2;i <= n;i++)
		fib[i] = fib[i - 1] + fib[i - 2];
	return fib[n]-1;
}

long long countgreen(int n)
{
	std::vector<long long> fib(n + 1, -1);
	fib[0] = 1;
	fib[1] = 1;
	fib[2] = 1;
	for (int i = 3;i <= n;i++)
		fib[i] = fib[i - 1] + fib[i - 3];
	return fib[n]-1;
}

long long countblue(int n)
{
	std::vector<long long> fib(n + 1, -1);
	fib[0] = 1;
	fib[1] = 1;
	fib[2] = 1;
	fib[3] = 1;
	fib[4] = 2;
	for (int i = 5;i <= n;i++)
		fib[i] = fib[i - 1] + fib[i - 4];
	return fib[n] - 1;
}

long long counttiles(int n)
{
	return countred(n)+countgreen(n)+countblue(n);
}
using namespace std;

int knapsack(int w,const vector<int>& weight,const vector<int>& value) {
	int n = weight.size();
	vector<vector<int>> dp(n + 1, vector<int>(w + 1, 0));

	for(int i=1;i<=n;i++)
		for (int j = 1;j <= w;j++)
		{
			dp[i][j] = dp[i - 1][j];
			if (j >= weight[i-1] && dp[i - 1][j - weight[i-1]] + value[i-1] > dp[i][j])
				dp[i][j] = dp[i - 1][j - weight[i-1]] + value[i-1];
		}
	return dp[n][w];
}

//long long countwaystoshiftcoun(int n, vector<int>coins)
//{
//	vector<long long>res(n + 1,0);
//	
//	
//	for (int i = 0;i < coins.size();i++)
//		res[coins[i]] = 1;
//	res[0] = 0;
//
//	for (int i = 0;i < res.size();i++)
//	{
//		long long sum = 0;
//		for (int j = 0;j < coins.size();j++)
//			if(i - coins[j]>=0)
//				sum += res[i-coins[j]];
//		if(sum!=0)
//			res[i] = sum;
//	}
//
//	return res[n];
//}

long long countwaystoshiftcoun(int n, vector<int>coins)
{
	vector<vector<long long>> res(coins.size()+1,vector<long long>(n+1,0));
	res[0][0] = 1;
	for(int i=1;i<res.size();i++)
		for (int j = 0;j < res[i].size();j++)
		{
			res[i][j] = res[i - 1][j];
			if (j - coins[i - 1] >= 0)
				res[i][j] = res[i - 1][j] + res[i][j - coins[i - 1]];
		}
		
	return res[coins.size()][n];
}
int main()
{
	//number 1
	std::cout << counttiles(50)<<endl;

	//numbew2
	vector<int> weight{1,4,3};
	vector<int>value{1500,3000,2000};
	int w = 4;
	cout << knapsack(w, weight, value)<<endl;
	w = 13;
	weight = { 3,4,5,8,9 };
	value = { 1,6,4,7,6 };
	cout << knapsack(w, weight, value) << endl;

	//number 3
	cout << countwaystoshiftcoun(100, { 1, 5,10,20,50})<<endl;//на платформе прошли все тесты
}