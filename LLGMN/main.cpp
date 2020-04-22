/****************************************
LLGMNの実装

******************************************/

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <random>
#include <cmath>
#include "tdata_class.h"
#include "function.h"

using namespace std;


//教師データクラスのコンストラクタ
Tdata::Tdata(int input_size, int output_size)
{
	input.resize(input_size, 0);
	output.resize(output_size, 0);
}


int main() {

	//変数宣言
	int teaching_data_size = 0;			//教師データの数
	const int input_size = 2;			//入力の個数(次元)
	const int k_class = 4;				//クラス(=出力の個数)
	const int component = 2;			//コンポーネント
	const double learning_rate = 0.01;	//学習率
	const int learning_times = 1000;	//最大学習回数
	const double efunc_min = 0.001;		//評価関数の収束判定値
	const int non_linear_input_size = 1 + input_size * (input_size + 3) / 2;	//非線形変換後の入力の個数

	//教師データのデータ数をカウント
	ifstream ifs("lea_sig.csv");
	if (!ifs) {
		printf("教師データファイルを開けませんでした．\n");
		return 0;
	}
	string buf;
	while (getline(ifs, buf)) {
		teaching_data_size++;
	}
	ifs.close();

	vector<Tdata> teaching_data(teaching_data_size, Tdata(input_size, k_class));	//教師データ

	//教師データファイルオープンおよび読み込み
	//入力データ
	ifstream ifs_t_in("lea_sig.csv");
	if (!ifs_t_in) {
		printf("教師データファイルを開けませんでした．\n");
		return 0;
	}
	string str;
	for (int n = 0; getline(ifs_t_in, str); n++) {
		string tmp;
		stringstream stream;
		stream << str;
		for (int d = 0; getline(stream, tmp, ','); d++) {
			teaching_data[n].input[d] = atof(tmp.c_str());
		}
	}

	//出力データ
	ifstream ifs_t_out("lea_T_sig.csv");
	if (!ifs_t_out) {
		printf("教師データファイルを開けませんでした．\n");
		return 0;
	}
	for (int n = 0; getline(ifs_t_out, str); n++) {
		string tmp;
		stringstream stream;
		stream << str;
		for (int d = 0; getline(stream, tmp, ','); d++) {
			teaching_data[n].output[d] = atof(tmp.c_str());
		}
	}


	/***********************************
	第1層から第2層へのブランチに対する重み．
	weight[h][k][m]: 1層目のh-1番目のノードから2層目のクラスk-1，コンポーネントm-1のノードへのブランチに対する重み．
	各要素数の範囲はそれぞれ，h: 0～non_linear_input_size-1, k: 0～k_class-1，m: 0～component-1．
	***********************************/
	vector<vector<vector<double>>> weight(non_linear_input_size, vector<vector<double>>(k_class, vector<double>(component)));

	/***********************************
	各ノードに対する入力．論文中におけるIに相当．
	In[l][k][m]:	l-1層目のクラスk-1，コンポーネントm-1のノードへの入力(2層目の場合)．
					l-1層目のk-1番目のノードへの入力(1,3層目の場合)．m=0の場合のみデータをもつ．
	l: 0～2, k: 0～max(non_linear_input_size,k_class)-1, m: 0～component-1.
	***********************************/
	vector<vector<vector<double>>> In(3, vector<vector<double>>(max(non_linear_input_size, k_class), vector<double>(component)));

	/***********************************
	各ノードに対する出力．論文中におけるOに相当．
	Out[n][l][k][m]:	n-1個目の入力データに対する出力．
						l-1層目のクラスk-1，コンポーネントm-1のノードからの出力(2層目の場合)．
						l-1層目のk-1番目のノードからの出力(1,3層目の場合)．m=0の場合のみデータをもつ．
	n: 0～teaching_data_size-1, l: 0～2, k: 0～max(non_linear_input_size,k_class)-1, m: 0～component-1.
	***********************************/
	vector<vector<vector<vector<double>>>> Out(teaching_data_size, vector<vector<vector<double>>>(3, vector<vector<double>>(max(non_linear_input_size, k_class), vector<double>(component))));

	/***********************************
	入力を非線形変換した後のデータ．
	non_linear_input[n][h]: n-1個目の入力データに対するh-1番目のデータ．
	n: 0～teaching_data_size-1, h: 0～non_linear_input_size-1.
	***********************************/
	vector<vector<double>> non_linear_input(teaching_data_size, vector<double>(non_linear_input_size));

	/***********************************
	NNからの出力．
	output[n][k]: n-1個目の入力データに対するk-1番目の出力．
	n: 0～teaching_data_size-1, k: 0～k_class.
	***********************************/
	vector<vector<double>> output(teaching_data_size, vector<double>(k_class));



	//重みの初期化
	init_weight(weight);

	//学習
	learning(teaching_data, weight, In, Out, non_linear_input, output, teaching_data_size, input_size, k_class, component, learning_rate, learning_times, efunc_min);

	return 0;
}