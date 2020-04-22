/****************************************
LLGMN�̎���

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


//���t�f�[�^�N���X�̃R���X�g���N�^
Tdata::Tdata(int input_size, int output_size)
{
	input.resize(input_size, 0);
	output.resize(output_size, 0);
}


int main() {

	//�ϐ��錾
	int teaching_data_size = 0;			//���t�f�[�^�̐�
	const int input_size = 2;			//���͂̌�(����)
	const int k_class = 4;				//�N���X(=�o�͂̌�)
	const int component = 2;			//�R���|�[�l���g
	const double learning_rate = 0.01;	//�w�K��
	const int learning_times = 1000;	//�ő�w�K��
	const double efunc_min = 0.001;		//�]���֐��̎�������l
	const int non_linear_input_size = 1 + input_size * (input_size + 3) / 2;	//����`�ϊ���̓��͂̌�

	//���t�f�[�^�̃f�[�^�����J�E���g
	ifstream ifs("lea_sig.csv");
	if (!ifs) {
		printf("���t�f�[�^�t�@�C�����J���܂���ł����D\n");
		return 0;
	}
	string buf;
	while (getline(ifs, buf)) {
		teaching_data_size++;
	}
	ifs.close();

	vector<Tdata> teaching_data(teaching_data_size, Tdata(input_size, k_class));	//���t�f�[�^

	//���t�f�[�^�t�@�C���I�[�v������ѓǂݍ���
	//���̓f�[�^
	ifstream ifs_t_in("lea_sig.csv");
	if (!ifs_t_in) {
		printf("���t�f�[�^�t�@�C�����J���܂���ł����D\n");
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

	//�o�̓f�[�^
	ifstream ifs_t_out("lea_T_sig.csv");
	if (!ifs_t_out) {
		printf("���t�f�[�^�t�@�C�����J���܂���ł����D\n");
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
	��1�w�����2�w�ւ̃u�����`�ɑ΂���d�݁D
	weight[h][k][m]: 1�w�ڂ�h-1�Ԗڂ̃m�[�h����2�w�ڂ̃N���Xk-1�C�R���|�[�l���gm-1�̃m�[�h�ւ̃u�����`�ɑ΂���d�݁D
	�e�v�f���͈̔͂͂��ꂼ��Ch: 0�`non_linear_input_size-1, k: 0�`k_class-1�Cm: 0�`component-1�D
	***********************************/
	vector<vector<vector<double>>> weight(non_linear_input_size, vector<vector<double>>(k_class, vector<double>(component)));

	/***********************************
	�e�m�[�h�ɑ΂�����́D�_�����ɂ�����I�ɑ����D
	In[l][k][m]:	l-1�w�ڂ̃N���Xk-1�C�R���|�[�l���gm-1�̃m�[�h�ւ̓���(2�w�ڂ̏ꍇ)�D
					l-1�w�ڂ�k-1�Ԗڂ̃m�[�h�ւ̓���(1,3�w�ڂ̏ꍇ)�Dm=0�̏ꍇ�̂݃f�[�^�����D
	l: 0�`2, k: 0�`max(non_linear_input_size,k_class)-1, m: 0�`component-1.
	***********************************/
	vector<vector<vector<double>>> In(3, vector<vector<double>>(max(non_linear_input_size, k_class), vector<double>(component)));

	/***********************************
	�e�m�[�h�ɑ΂���o�́D�_�����ɂ�����O�ɑ����D
	Out[n][l][k][m]:	n-1�ڂ̓��̓f�[�^�ɑ΂���o�́D
						l-1�w�ڂ̃N���Xk-1�C�R���|�[�l���gm-1�̃m�[�h����̏o��(2�w�ڂ̏ꍇ)�D
						l-1�w�ڂ�k-1�Ԗڂ̃m�[�h����̏o��(1,3�w�ڂ̏ꍇ)�Dm=0�̏ꍇ�̂݃f�[�^�����D
	n: 0�`teaching_data_size-1, l: 0�`2, k: 0�`max(non_linear_input_size,k_class)-1, m: 0�`component-1.
	***********************************/
	vector<vector<vector<vector<double>>>> Out(teaching_data_size, vector<vector<vector<double>>>(3, vector<vector<double>>(max(non_linear_input_size, k_class), vector<double>(component))));

	/***********************************
	���͂����`�ϊ�������̃f�[�^�D
	non_linear_input[n][h]: n-1�ڂ̓��̓f�[�^�ɑ΂���h-1�Ԗڂ̃f�[�^�D
	n: 0�`teaching_data_size-1, h: 0�`non_linear_input_size-1.
	***********************************/
	vector<vector<double>> non_linear_input(teaching_data_size, vector<double>(non_linear_input_size));

	/***********************************
	NN����̏o�́D
	output[n][k]: n-1�ڂ̓��̓f�[�^�ɑ΂���k-1�Ԗڂ̏o�́D
	n: 0�`teaching_data_size-1, k: 0�`k_class.
	***********************************/
	vector<vector<double>> output(teaching_data_size, vector<double>(k_class));



	//�d�݂̏�����
	init_weight(weight);

	//�w�K
	learning(teaching_data, weight, In, Out, non_linear_input, output, teaching_data_size, input_size, k_class, component, learning_rate, learning_times, efunc_min);

	return 0;
}