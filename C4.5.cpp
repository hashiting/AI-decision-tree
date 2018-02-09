#include<iostream>
#include<fstream>
#include<cstring>
#include<string>
#include <iomanip>
#include<sstream>
#include<map>
#include<vector>
#include<algorithm>
#include<math.h>
using namespace std;

struct data_type{
	vector<int> data;
	int type;
};

struct Decision_Tree{
	vector<Decision_Tree *> Pointer;//子代  没有了就赋空 
	int nextSplited_tag;//子代分类的标签是什么 如果没有了就变为-1 
	vector<double> Discr_split;//子代的分类边界  没有赋空 
	vector<data_type> data;//这一个结点包含的数据 数据只剩一个可以停止 
}; 

struct Splited_type{
	vector<vector<data_type>> Data_splited;//分好的数据 
	double shang;//shang 
	int tag;//分类的标签 
	int position;//原本的位置； 
};

bool Splited_type_cmd(Splited_type a,Splited_type b){
	return a.shang > b.shang;
}

double get_initial_shang(vector<data_type> d){//index  = location of tag;num_split = 分类个数 
	double sum = 0;
	for(int i = 0;i < d.size();i++){
		if(d[i].type == 0){
			sum++;
		}
	}
	double result;
	if(sum == 0 || sum == d.size())
	 	result = log2(1);
	else
		result = -(sum/d.size())*log2(sum/d.size()) - (((double)d.size()-sum)/d.size())*log2(((double)d.size()-sum)/d.size());
	return result;	
}

//返回分类后的所有数据集以及熵 
Splited_type get_type_shang(vector<data_type> d,int tag,int position,vector<double> Discr_split){//tag = type Discr_split = 分界边界
	if(Discr_split.size() == 0)
	cout<<"bound size error"<<endl;  
	int num_type = Discr_split.size() + 1;
	double GR = 0;
	double shang = 0;
	double sshang = 0;
	Splited_type result;
	for(int i = 0;i <= Discr_split.size();i++){
		vector<data_type> temp;
		double num_fit = 0;
		double num_fit_zero = 0;
		if(i == 0){
			for(int j = 0;j < d.size();j++){
				if(d[j].data[tag] <= Discr_split[i]){
					temp.push_back(d[j]);
					num_fit++;
					if(d[j].type == 0){
						num_fit_zero++;
					}
				}
			}
		}
		else if(i != Discr_split.size()){
			for(int j = 0;j < d.size();j++){
				if(d[j].data[tag] <= Discr_split[i] && d[j].data[tag] > Discr_split[i-1]){
					temp.push_back(d[j]);
					num_fit++;
					if(d[j].type == 0){
						num_fit_zero++;
					}
				}
			}
		} 
		else{
		for(int j = 0;j < d.size();j++){
			if(d[j].data[tag] > Discr_split[i-1]){
				temp.push_back(d[j]);
				num_fit++;
				if(d[j].type == 0){
					num_fit_zero++;
				}
			}
		}
		}
		double temp_shang = 0;
		double temp_sshang = 0;
		if(num_fit_zero == 0 || num_fit_zero == num_fit){
			temp_shang = log2(1);
			temp_sshang = log2(1);	
		}
		else{
			temp_shang = (num_fit/d.size())*(-(num_fit_zero/num_fit)*log2(num_fit_zero/num_fit) - 
		((num_fit - num_fit_zero)/num_fit)*log2((num_fit - num_fit_zero)/num_fit));
		temp_sshang = -(num_fit/d.size())*log2(num_fit/d.size());
		}
		
		shang += temp_shang;
		sshang += temp_sshang;
		//if(temp.size() == 0)
		//cout<<"confused error"<<endl;
		result.Data_splited.push_back(temp);
	}
	result.shang  = (get_initial_shang(d)-shang)/sshang;
	result.tag = tag;
	result.position = position;
	return result;
}
//index 标签的位置 返回每两个数据的中点 
vector<double> Get_bound(vector<data_type> d,int tag){
	/*vector<double> all_num;
	for(int i = 0;i < d.size();i++){
		all_num.push_back(d[i].data[tag]);
	}
	sort(all_num.begin(),all_num.end());
	vector<double> result;
	for(int i = 0;i < all_num.size()-1;i++){
		if(all_num[i+1] != all_num[i]){
			result.push_back((all_num[i] + all_num[i+1])/2);
		}
	}
	if(result.size() == 0){
		result.push_back(all_num[0]);
	}
	return result;*/
	vector<double> result;
	result.push_back(2);
	result.push_back(4);
	result.push_back(6);
	result.push_back(8);
	return result;
} 
void data_initial(vector<data_type> &d,int num_tag,string a){//read file
	ifstream file1;
	file1.open(a);
	string tab = ",";
	while(!file1.eof()){
		char line[50];
		file1.getline(line,49);
		string thisline  = line;
		int found = thisline.find(tab);
		while(found!=-1){
			thisline.replace(found,1," ");
			found = thisline.find(tab);
		}
		stringstream ss(thisline);
		int count = 0;
		int each_num;
		vector<int> data;
		int type;
		while(ss>>each_num){
			count++;
			if(count <= num_tag)
			data.push_back(each_num);
			else
			type = each_num;
		} 
		data_type node;
		node.data = data;
		node.type = type;
		d.push_back(node);
	}
	file1.close();	
}
/**********************************************************************************************************
思路：
每一层应该有一个标签表 向量存 如果已经用了就可以用erase去掉 每一次先遍历所有还存在的标签的熵，选出最小的
接着将这个从标签表中去除 调用分好的数据集，分配指针 赋值这子代的分类依据，分类边界，子代的指针获得新的数据
接着对每一个指针递归 
递归停止条件 ：传下去的标签表为0或者传下去的该层数据的size为1个
构建完成;
***********************************************************************************************************/ 
int leaf = 0;
void Creat_DT(Decision_Tree *head,vector<int> tag_table){
	if(head->data.size() <= 1 || tag_table.size() == 0)
	{
		head->nextSplited_tag = -1;
		cout<<"data num:"<<head->data.size()<<endl;
		if(head->data.size() != 0)
		leaf++;
		return;
	}
	vector<Splited_type> each_type;
	for(int i = 0;i < tag_table.size();i++){
		each_type.push_back(get_type_shang(head->data,tag_table[i],i,Get_bound(head->data,tag_table[i])));
		cout<<each_type[i].shang<<endl;
	}
	sort(each_type.begin(),each_type.end(),Splited_type_cmd);
	head->nextSplited_tag = each_type[0].tag;
	head->Discr_split = Get_bound(head->data,each_type[0].tag);
	tag_table.erase(tag_table.begin() + each_type[0].position);
	for(int i = 0;i < each_type[0].Data_splited.size();i++){
		Decision_Tree *temp = new Decision_Tree();
		temp->data = each_type[0].Data_splited[i];
		head->Pointer.push_back(temp);
		Creat_DT(temp,tag_table);
	}
}
int get_result(vector<data_type> data){
	int zero = 0;
	int one = 0;
	for(int i = 0;i < data.size();i++){
		if(data[i].type == 0)
		zero++;
		else
		one++;
	}
	if(zero > one)
	return 0;
	return 1;
}
int main(){
	vector<data_type> all_data;
	string name = "train.csv";
	const int num_tag = 9;
	data_initial(all_data,num_tag,name);
	cout<<get_initial_shang(all_data)<<endl;
	
	vector<int> tag_table;
	for(int i = 0;i < num_tag;i++){
		tag_table.push_back(i);
	}
	Decision_Tree *head;
	head = new Decision_Tree();
	head->data = all_data;
	Creat_DT(head,tag_table);
	//finish creat 
	//begin test
	vector<data_type> test_data;
	name = "test.csv";
	data_initial(test_data,num_tag,name);
	int right = 0;
	int notfound = 0;
	double allone = 0;
	double allzero = 0;
	double onezero = 0;
	double zeroone = 0;	
	for(int i = 0;i < test_data.size();i++){
		Decision_Tree *temp;
		Decision_Tree *next_temp;
		temp = new Decision_Tree();
		next_temp = new Decision_Tree();
		temp = head;
		next_temp = temp;
		while(next_temp->nextSplited_tag != -1 && next_temp->data.size() != 0){
			int position;
			for(int j = 0;j <= next_temp->Discr_split.size();j++){
				if(j == 0){
					if(test_data[i].data[next_temp->nextSplited_tag] <= next_temp->Discr_split[j]){
						position = j;
						break;
					}	
				}
				else if(j != next_temp->Discr_split.size()){
					if(test_data[i].data[next_temp->nextSplited_tag] <= next_temp->Discr_split[j]&&test_data[i].data[next_temp->nextSplited_tag] > next_temp->Discr_split[j-1]){
						position = j;
						break;
					}
				}
				else{
					if(test_data[i].data[next_temp->nextSplited_tag] > next_temp->Discr_split[j-1]){
						position = j;
						break;
					}
				}
				
			}
			temp = next_temp;
			next_temp = next_temp->Pointer[position];
		}
		int result;
		if(next_temp->data.size() != 0){
			result = get_result(next_temp->data);
		}
		else{
			result = get_result(temp->data);
			notfound++;
		}
		if(result == test_data[i].type){
			right++;
			if(result == 1)
			allone++;
			else
			allzero++;
		}
		else
		{
			if(result == 1)
			zeroone++;
			else
			onezero++;
		}
		
	}
	double recall = allone/(allone + onezero);
	double pre = allone/(allone + zeroone);
	double F1 = 2*pre*recall/(pre + recall);
	cout<<right<<endl;
	cout<<recall<<endl;
	cout<<pre<<endl;
	cout<<F1<<endl;
	cout<<0.5 * leaf/all_data.size()<<endl;
}
