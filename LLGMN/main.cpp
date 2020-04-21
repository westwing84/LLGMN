/****************************************
LLGMNの実装

******************************************/

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <random>

using namespace std;

//教師データクラス
class Tdata
{
public:
	vector<double> input;
	vector<double> output;
	Tdata(int input_size, int output_size);

private:

};

Tdata::Tdata(int input_size, int output_size)
{
	input.resize(input_size, 0);
	output.resize(output_size, 0);
}



int main() {
	
	return 0;
}

//順方向伝播
void forwardprop(
	vector<vector<vector<double>>>& weight,		//重み
	vector<vector<vector<double>>>& In,			//各ニューロンに対する入力
	vector<vector<vector<double>>>& Out,		//各ニューロンに対する出力
	vector<double>& input,		//NNへの入力
	vector<double>& output,		//NNへの出力
	int input_size,				//入力の個数(次元)
	int k_class,				//クラス
	int component				//コンポーネント(=出力の個数)
) {
	int non_linear_input_size = 1 + input_size * (input_size + 3) / 2;	//非線形変換後の入力の個数
	vector<double> non_linear_input(non_linear_input_size);				//非線形変換後の入力
	int count1 = 0, count2 = 0;

	//入力の非線形変換
	non_linear_input[0] = 1;
	for (int i = 0; i < input_size; i++) {
		non_linear_input[i + 1] = input[i];
	}
	for (int i = input_size + 1; i < non_linear_input_size; i++) {
		non_linear_input[i] = input[count1] * input[count2];
		count1++;
		if (count1 == input_size) {
			count2++;
			count1 = count2;
		}
	}

	//第1層の伝播
	for (int i = 0; i < non_linear_input_size; i++) {
		In[0][0][i] = non_linear_input[i];
		Out[0][0][i] = In[0][0][i];
	}

}