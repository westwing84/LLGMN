#pragma once
#include <iostream>
#include <vector>

using namespace std;


//教師データクラス
class Tdata
{
public:
	vector<double> input;		//入力
	vector<double> output;		//出力
	Tdata(int input_size, int output_size);
};

//コンストラクタ
Tdata::Tdata(int input_size, int output_size)
{
	input.resize(input_size, 0);
	output.resize(output_size, 0);
}