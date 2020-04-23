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


int main() {

	//変数宣言
	int teaching_data_size = 0;			//教師データの数
	int non_teaching_data_size = 0;		//未学習データの数
	int input_size = 2;					//入力の個数(次元)
	int k_class = 4;					//クラス(=出力の個数)
	int component = 2;					//コンポーネント
	double learning_rate = 0.01;		//学習率
	int learning_times = 10000;			//最大学習回数
	double efunc_min = 0.001;			//評価関数の収束判定値
	int non_linear_input_size;			//非線形変換後の入力の個数

	//入力次元，クラス数，コンポーネント数，学習率，最大学習回数を入力
	cout << "入力データの次元数を入力してださい: " ;
	cin >> input_size;
	cout << "クラス数を入力してださい: ";
	cin >> k_class;
	cout << "コンポーネント数を入力してださい: ";
	cin >> component;
	cout << "学習率を入力してださい: ";
	cin >> learning_rate;
	cout << "最大学習回数を入力してださい: ";
	cin >> learning_times;
	non_linear_input_size = 1 + input_size * (input_size + 3) / 2;

	//教師データのデータ数をカウント
	string filename_t_in = "lea_sig.csv";
	ifstream ifs(filename_t_in);
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
	ifstream ifs_t_in(filename_t_in);
	if (!ifs_t_in) {
		printf("教師データファイル(入力)を開けませんでした．\n");
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
	ifs_t_in.close();

	//出力データ
	string filename_t_out = "lea_T_sig.csv";
	ifstream ifs_t_out(filename_t_out);
	if (!ifs_t_out) {
		printf("教師データファイル(出力)を開けませんでした．\n");
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
	ifs_t_out.close();


	//未学習データの読み込み
	//行数カウント
	string filename_nt_in = "dis_sig.csv";
	ifstream ifs2(filename_nt_in);
	if (!ifs2) {
		printf("未学習データファイルを開けませんでした．\n");
		return 0;
	}
	while (getline(ifs2, buf)) {
		non_teaching_data_size++;
	}
	ifs2.close();

	vector<Tdata> non_teaching_data(non_teaching_data_size, Tdata(input_size, k_class));	//未学習データ

	//読み込み
	ifstream ifs_nt_in(filename_nt_in);
	if (!ifs_nt_in) {
		printf("未学習データファイルを開けませんでした．\n");
		return 0;
	}
	for (int n = 0; getline(ifs_nt_in, str); n++) {
		string tmp;
		stringstream stream;
		stream << str;
		for (int d = 0; getline(stream, tmp, ','); d++) {
			non_teaching_data[n].input[d] = atof(tmp.c_str());
		}
	}
	ifs_nt_in.close();

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
	cout << "教師データを学習しています．" << endl;
	learning(teaching_data, weight, In, Out, non_linear_input, output, teaching_data_size, input_size, k_class, component, learning_rate, learning_times, efunc_min);

	//教師データをNNに入力し正しい出力が得られるか確認
	for (int tdata_num = 0; tdata_num < teaching_data_size; tdata_num++) {
		forwardprop(weight, In, Out[tdata_num], teaching_data[tdata_num].input, non_linear_input[tdata_num], output[tdata_num], input_size, k_class, component);
	}
	string filename_t_out2 = "lea_sig_out.csv";
	ofstream ofs_t(filename_t_out2);
	for (int n = 0; n < non_teaching_data_size; n++) {
		for (int k = 0; k < k_class; k++) {
			ofs_t << output[n][k] << ",";
		}
		ofs_t << "\n";
	}
	ofs_t.close();
	cout << "教師データを入力したときの出力データを" << filename_t_out2 << "に保存しました．" << endl;

	//未学習データをNNに入力
	Out.resize(non_teaching_data_size);
	for (int ntdata_num = 0; ntdata_num < non_teaching_data_size; ntdata_num++) {
		forwardprop(weight, In, Out[ntdata_num], non_teaching_data[ntdata_num].input, non_linear_input[ntdata_num], non_teaching_data[ntdata_num].output, input_size, k_class, component);
	}

	//識別結果をファイルdis_sig_out.csvに出力
	string filename_nt_out = "dis_sig_out.csv";
	ofstream ofs(filename_nt_out);
	for (int n = 0; n < non_teaching_data_size; n++) {
		for (int k = 0; k < k_class; k++) {
			ofs << non_teaching_data[n].output[k] << ",";
		}
		ofs << "\n";
	}
	ofs.close();
	cout << "未学習データの識別結果を" << filename_nt_out << "に保存しました．" << endl;

	return 0;
}