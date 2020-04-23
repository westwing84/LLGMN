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


int main() {

	//�ϐ��錾
	int teaching_data_size = 0;			//���t�f�[�^�̐�
	int non_teaching_data_size = 0;		//���w�K�f�[�^�̐�
	int input_size = 2;					//���͂̌�(����)
	int k_class = 4;					//�N���X(=�o�͂̌�)
	int component = 2;					//�R���|�[�l���g
	double learning_rate = 0.01;		//�w�K��
	int learning_times = 10000;			//�ő�w�K��
	double efunc_min = 0.001;			//�]���֐��̎�������l
	int non_linear_input_size;			//����`�ϊ���̓��͂̌�

	//���͎����C�N���X���C�R���|�[�l���g���C�w�K���C�ő�w�K�񐔂����
	cout << "���̓f�[�^�̎���������͂��Ă�����: " ;
	cin >> input_size;
	cout << "�N���X������͂��Ă�����: ";
	cin >> k_class;
	cout << "�R���|�[�l���g������͂��Ă�����: ";
	cin >> component;
	cout << "�w�K������͂��Ă�����: ";
	cin >> learning_rate;
	cout << "�ő�w�K�񐔂���͂��Ă�����: ";
	cin >> learning_times;
	non_linear_input_size = 1 + input_size * (input_size + 3) / 2;

	//���t�f�[�^�̃f�[�^�����J�E���g
	string filename_t_in = "lea_sig.csv";
	ifstream ifs(filename_t_in);
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
	ifstream ifs_t_in(filename_t_in);
	if (!ifs_t_in) {
		printf("���t�f�[�^�t�@�C��(����)���J���܂���ł����D\n");
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

	//�o�̓f�[�^
	string filename_t_out = "lea_T_sig.csv";
	ifstream ifs_t_out(filename_t_out);
	if (!ifs_t_out) {
		printf("���t�f�[�^�t�@�C��(�o��)���J���܂���ł����D\n");
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


	//���w�K�f�[�^�̓ǂݍ���
	//�s���J�E���g
	string filename_nt_in = "dis_sig.csv";
	ifstream ifs2(filename_nt_in);
	if (!ifs2) {
		printf("���w�K�f�[�^�t�@�C�����J���܂���ł����D\n");
		return 0;
	}
	while (getline(ifs2, buf)) {
		non_teaching_data_size++;
	}
	ifs2.close();

	vector<Tdata> non_teaching_data(non_teaching_data_size, Tdata(input_size, k_class));	//���w�K�f�[�^

	//�ǂݍ���
	ifstream ifs_nt_in(filename_nt_in);
	if (!ifs_nt_in) {
		printf("���w�K�f�[�^�t�@�C�����J���܂���ł����D\n");
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
	cout << "���t�f�[�^���w�K���Ă��܂��D" << endl;
	learning(teaching_data, weight, In, Out, non_linear_input, output, teaching_data_size, input_size, k_class, component, learning_rate, learning_times, efunc_min);

	//���t�f�[�^��NN�ɓ��͂��������o�͂������邩�m�F
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
	cout << "���t�f�[�^����͂����Ƃ��̏o�̓f�[�^��" << filename_t_out2 << "�ɕۑ����܂����D" << endl;

	//���w�K�f�[�^��NN�ɓ���
	Out.resize(non_teaching_data_size);
	for (int ntdata_num = 0; ntdata_num < non_teaching_data_size; ntdata_num++) {
		forwardprop(weight, In, Out[ntdata_num], non_teaching_data[ntdata_num].input, non_linear_input[ntdata_num], non_teaching_data[ntdata_num].output, input_size, k_class, component);
	}

	//���ʌ��ʂ��t�@�C��dis_sig_out.csv�ɏo��
	string filename_nt_out = "dis_sig_out.csv";
	ofstream ofs(filename_nt_out);
	for (int n = 0; n < non_teaching_data_size; n++) {
		for (int k = 0; k < k_class; k++) {
			ofs << non_teaching_data[n].output[k] << ",";
		}
		ofs << "\n";
	}
	ofs.close();
	cout << "���w�K�f�[�^�̎��ʌ��ʂ�" << filename_nt_out << "�ɕۑ����܂����D" << endl;

	return 0;
}