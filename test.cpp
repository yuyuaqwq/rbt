// rbt.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <chrono>

#include <rbt/set.hpp>
#include <set>
#include <map>


static int RandInt() {
	return (rand() << 16) | rand();
}

static std::vector<int64_t> GenerateI64Vector(size_t count, bool order = true, bool dense = true) {
	std::vector<int64_t> res;
	res.reserve(count);
	for (size_t i = 0; i < count; i++) {
		int64_t data = i;
		//ReverseOrder(&data, sizeof(data));
		if (!dense) {
			data = ((int64_t)rand() << 48) + ((int64_t)rand() << 32) + ((int64_t)rand() << 16) + rand();
		}
		res.push_back(data);
	}

	if (order == false) {
		for (int64_t i = 0; i < count; i++) {
			std::swap(res[i], res[RandInt() % count]);
		}
	}

	return res;
}
static std::vector<std::string> GenerateStrVector(size_t count, bool order = true, bool dense = true) {
	std::vector<std::string> res;
	res.reserve(count);
	for (size_t i = 0; i < count; i++) {
		int64_t data = i;
		//ReverseOrder(&data, sizeof(data));
		if (!dense) {
			data = ((int64_t)rand() << 48) + ((int64_t)rand() << 32) + ((int64_t)rand() << 16) + rand();
		}
		res.push_back(std::to_string(data));
	}

	if (order == false) {
		for (int64_t i = 0; i < count; i++) {
			std::swap(res[i], res[RandInt() % count]);
		}
	}

	return res;
}

struct test {
	int a = 0;

	constexpr std::strong_ordering operator<=>(const test& rhs) const = default;
};



int main()
{
	using Type = int64_t;
    rbt::set<Type> rbt_set;
	std::set<Type> std_set;
	auto count = 1000000;

	auto data = GenerateI64Vector(count, false, true);
	//auto data = GenerateStrVector(count, true, true);


	{
		std::cout << "std::set::insert" << std::endl;
		auto start_time = std::chrono::high_resolution_clock::now();
		for (auto& d : data) {
			auto [res, success] = std_set.insert(d);
			if (!success) {
				printf("???");
			}
		}
		auto end_time = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
		std::cout << "time: " << duration.count() << "ms" << std::endl;
	}

	{
		std::cout << "rbt::set::insert" << std::endl;
		auto start_time = std::chrono::high_resolution_clock::now();
		for (auto& d : data) {
			auto [res, success] = rbt_set.insert(d);
			if (!success) {
				printf("???");
			}
		}
		auto end_time = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
		std::cout << "time: " << duration.count() << "ms" << std::endl;
	}


	{
		std::cout << "std::set::find" << std::endl;
		auto start_time = std::chrono::high_resolution_clock::now();
		for (auto& d : data) {
			auto iter = std_set.find(d);
			if (iter == std_set.end()) {
				printf("???");
			}
		}
		auto end_time = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
		std::cout << "time: " << duration.count() << "ms" << std::endl;
	}


	{
		std::cout << "rbt::set::find" << std::endl;
		auto start_time = std::chrono::high_resolution_clock::now();
		for (auto& d : data) {
			auto iter = rbt_set.find(d);
			if (iter == rbt_set.end()) {
				printf("???");
			}
		}
		auto end_time = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
		std::cout << "time: " << duration.count() << "ms" << std::endl;
	}


	//for (int64_t i = 0; i < count; i++) {
	//	std::swap(data[i], data[RandInt() % count]);
	//}




    std::map<int, int> aaa;
    aaa.insert(std::pair{ 1,2 });
    aaa.insert(std::pair{ 1,3 });
    std::cout << "Hello World!\n";
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
