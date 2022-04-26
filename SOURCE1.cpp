#include <map>
#include <set>
#include <vector>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <fstream>
//#include <psapi.h>// memory cost need
//#pragma  comment(lib,"psapi.lib")
using namespace std;
#define K 600    
#define M 1000   
#define N 170000  
struct node         //nettree node
{
	int name;
	int min_leave, max_leave;
	vector <int> parent;
	vector <int> children;
	bool used;
	bool toleave;
};

struct occurrence   //出现
{
	vector <int > position;
};
struct sub_ptn_struct   //a[0,3]c => start[min,max]end
{
	char start, end;
	int min, max;
};
vector <occurrence> store;
sub_ptn_struct sub_ptn[M];  //pattern p[i]模式串 
int ptn_len = 0;  //the length of pattern//模式串的长度 
int seq_len = 0;
int maxgap = -1; //gap constraint 间隙约束 
char S[N];  //sequence 序列 

void dealrange(char *p, int len)      //确定范围 
//put sub-pattern "a[1,3]b" into sub_ptn and sub_ptn.start=a，sub_ptn.end=b, sub_ptn.min=1，sub_ptn.max=3
{
	sub_ptn[ptn_len].start = p[0];
	sub_ptn[ptn_len].end = p[len];
	if (len == 1)
	{
		sub_ptn[ptn_len].max = sub_ptn[ptn_len].min = 0;
	}
	else
	{
		int value = 0;
		int i;
		int flag = 0;
		for (i = 2;p[i] != ',';i++)
			if (p[i] == '-')
				flag = 1;
			else
				value = value * 10 + p[i] - 48;
		if (flag == 1)
			sub_ptn[ptn_len].min = -value;
		else
			sub_ptn[ptn_len].min = value;
		flag = 0;
		value = 0;
		for (int j = i + 1;p[j] != ']';j++)
			if (p[j] == '-')
				flag = 1;
			else
				value = value * 10 + p[j] - 48;
		if (flag == 1)
			sub_ptn[ptn_len].max = -value;
		else
			sub_ptn[ptn_len].max = value;
	}
	if (sub_ptn[ptn_len].max - sub_ptn[ptn_len].min + 1 > maxgap)
		maxgap = sub_ptn[ptn_len].max - sub_ptn[ptn_len].min + 1;
	ptn_len++;
}


void convert_p_to_ruler(char *p)     //transform p into sub_ptn
{
	char st, en;
	int l, r;
	int len = strlen(p);
	st = p[0];
	en = p[len - 1];
	if (isalpha(st) && isalpha(en))
	{
		l = 0;
		for (int i = 1;i < len;i++)
		{
			if (isalpha(p[i]))
			{
				r = i;
				dealrange(p + l, r - l);
				l = i;
			}
		}
	}
	else
	{
		cout << "irregular pattern.\n";
		exit(-1);
	}
}
void disp_pattern()         //display the array sub_ptn
{
	for (int i = 0;i < ptn_len;i++)
	{
		//printf("%c\t%d\t%d\t%c\n",sub_ptn[i].start ,sub_ptn[i].min , sub_ptn[i].max ,sub_ptn[i].end );
		cout << sub_ptn[i].start << "\t" << sub_ptn[i].min << "\t"
			<< sub_ptn[i].max << "\t" << sub_ptn[i].end << endl;
	}
}

void create_nettree(vector <node> *nettree)
{
	int *start;
	start = new int[ptn_len + 1];
	int i;
	for (i = 0;i < ptn_len + 1;i++)
		start[i] = 0;
	for (i = 0;i < ptn_len + 1;i++)
		nettree[i].clear();
	for (int j = 0;j < ptn_len + 1;j++)
	{
		char cchar = j == ptn_len ? sub_ptn[j - 1].end : sub_ptn[j].start;
		if (j == 0)
		{
			for (i = 0;i < strlen(S);i++)
			{
				node anode;
				anode.name = i;
				//	anode.parent.resize (0);
				anode.parent.clear();
				//		anode.children .resize (0);
				anode.children.clear();
				anode.max_leave = anode.name;
				anode.min_leave = anode.name;
				anode.used = false;
				if (cchar == S[i])
				{
					int len = nettree[0].size();
					//	nettree[0].resize(len+1);
					anode.toleave = true;
					//	nettree[0][len]=anode;
					nettree[0].push_back(anode);
				}
			}
		}
		else
		{
			int p = nettree[j - 1].size();
			int n = nettree[j - 1][0].name;
			int m = nettree[j - 1][p - 1].name;

			if (sub_ptn[j - 1].min >= 0)
			{
				for (i = n + sub_ptn[j - 1].min + 1;i <= m + sub_ptn[j - 1].max + 1;i++)
				{
					node anode;
					anode.name = i;
					//	anode.parent.resize (0);
					//	anode.children .resize (0);
					anode.max_leave = anode.name;
					anode.min_leave = anode.name;
					anode.used = false;
					if (cchar == S[i])
					{
						int len = nettree[j].size();
						//	nettree[j].resize(len+1);
						anode.toleave = true;
						//	nettree[j][len]=anode;
						nettree[j].push_back(anode);
						for (int k = start[j - 1];k < p;k++)
						{
							int n = nettree[j - 1][k].name;
							if (i < n + sub_ptn[j - 1].min + 1)
								break;
							else if (i > n + sub_ptn[j - 1].max + 1)
								continue;
							else if (i >= n + sub_ptn[j - 1].min + 1 && i <= n + sub_ptn[j - 1].max + 1)
							{
								if (n != i)
								{
									//			int nc=nettree[j-1][k].children.size();
									//		    nettree[j-1][k].children.resize(nc+1);
									//		    nettree[j-1][k].children[nc] =len;
									nettree[j - 1][k].children.push_back(len);
									//		   int np=nettree[j][len].parent.size();
									//		   nettree[j][len].parent.resize(np+1);
									//		   nettree[j][len].parent[np]=k;
									nettree[j].back().parent.push_back(k);

								}
								else
									continue;
							}
						}

					}
				}
			}
			else if (sub_ptn[j - 1].max < 0)
			{
				for (i = n + sub_ptn[j - 1].min;i <= m + sub_ptn[j - 1].max;i++)
				{
					node anode;
					anode.name = i;
					//		anode.parent.resize (0);
					//		anode.children .resize (0);
					anode.max_leave = anode.name;
					anode.min_leave = anode.name;
					anode.used = false;
					if (cchar == S[i])
					{
						int len = nettree[j].size();
						//	nettree[j].resize(len+1);
						anode.toleave = true;
						//	nettree[j][len]=anode;
						nettree[j].push_back(anode);
						for (int k = start[j - 1];k < p;k++)
						{
							int n = nettree[j - 1][k].name;
							if (i < n + sub_ptn[j - 1].min)
								break;
							else if (i > n + sub_ptn[j - 1].max)
								continue;
							else if (i >= n + sub_ptn[j - 1].min&&i <= n + sub_ptn[j - 1].max)
							{
								if (n != i)
								{
									//	int nc=nettree[j-1][k].children.size();
								//	    nettree[j-1][k].children.resize(nc+1);
							//		    nettree[j-1][k].children[nc] =len;
									nettree[j - 1][k].children.push_back(len);
									//		   int np=nettree[j][len].parent.size();
								//			   nettree[j][len].parent.resize(np+1);
								//			   nettree[j][len].parent[np]=k;
									nettree[j].back().parent.push_back(k);

								}
								else
									continue;
							}
						}
					}
				}
			}
			else if (sub_ptn[j - 1].min < 0 && sub_ptn[j - 1].max >= 0)
			{
				for (i = n + sub_ptn[j - 1].min;i <= m + sub_ptn[j - 1].max + 1;i++)
				{
					node anode;
					anode.name = i;
					//			anode.parent.resize (0);
					//			anode.children .resize (0);
					anode.max_leave = anode.name;
					anode.min_leave = anode.name;
					anode.used = false;
					if (cchar == S[i])
					{
						int len = nettree[j].size();
						//		nettree[j].resize(len+1);
						anode.toleave = true;
						//		nettree[j][len]=anode;
						nettree[j].push_back(anode);
						for (int k = start[j - 1];k < p;k++)
						{
							int n = nettree[j - 1][k].name;
							if (i < n + sub_ptn[j - 1].min)
								break;
							else if (i > n + sub_ptn[j - 1].max + 1)
								continue;
							else if (i >= n + sub_ptn[j - 1].min&&i <= n + sub_ptn[j - 1].max + 1)
							{
								if (n != i)
								{
									//		int nc=nettree[j-1][k].children.size();
								//		    nettree[j-1][k].children.resize(nc+1);
								//		    nettree[j-1][k].children[nc] =len;
									nettree[j - 1][k].children.push_back(len);
									//			   int np=nettree[j][len].parent.size();
									//			   nettree[j][len].parent.resize(np+1);
									//			   nettree[j][len].parent[np]=k;
									nettree[j].back().parent.push_back(k);
								}
								else
									continue;
							}
						}
					}
				}
			}

		}

	}

}





void displaynettree(vector <node> *nettree)
{
	cout << "The nettree is as follow:\n";
	cout << "--------------------------------------------------------------------------\n";
	for (int i = 0;i < ptn_len + 1;i++)
	{
		cout << i << " level:  ";
		for (int j = 0;j < nettree[i].size();j++)
			if (nettree[i][j].used == false)
			{
				cout << nettree[i][j].name << "\t";
				cout << nettree[i][j].name << ".parent.size()=" << nettree[i][j].parent.size() << "\t";
				cout << nettree[i][j].name << ".min_leave=" << nettree[i][j].min_leave << "\t";
				cout << nettree[i][j].name << ".max_leave=" << nettree[i][j].max_leave << "\t";
				// cout <<nettree[i][j].name <<".toleave="<< nettree[i][j].toleave<<"\t";
				cout << nettree[i][j].name << ".used=" << nettree[i][j].used << endl;
			}
		cout << "\n";
		cout << endl;
	}
}


void update_nettree(vector <node> *nettree)
{
	for (int i = ptn_len - 1; i >= 0; i--)
	{

		for (int j = nettree[i].size() - 1; j >= 0; j--)
		{
			bool flag = true;
			int size = nettree[i][j].children.size();
			for (int k = 0; k < size; k++)
			{
				int child = nettree[i][j].children[k];
				if (nettree[i + 1][child].toleave == true)
				{
					nettree[i][j].max_leave = nettree[i + 1][child].max_leave;

				}
				if (nettree[i + 1][child].used == false)
				{
					flag = false;

				}
			}
			for (int h = 0; h < size; h++)
			{
				int child = nettree[i][j].children[h];
				if (nettree[i + 1][child].toleave == true)
				{
					nettree[i][j].min_leave = nettree[i + 1][child].min_leave;
					break;

				}
			}
			nettree[i][j].used = flag;
			if (flag == true)
			{
				nettree[i][j].max_leave = nettree[i][j].name;
				nettree[i][j].min_leave = nettree[i][j].name;
				nettree[i][j].toleave = false;
			}

		}
	}
}

struct nodep
{
	int level, position;
};
int bts(int j, int  child, int root, vector <node> *nettree, occurrence & occ, occurrence & occin)//回溯BackTracking Strategy 
{

	if (j <= 0)
	{
		return -1;
	}
	if (j > ptn_len)
	{
		return 1;
	}
	else
	{
		int t = 0;//尝试在这里定义 
		int parent = occin.position[j - 1];    //The position of the parent in nettree
		int cs = nettree[j - 1][parent].children.size();   //The number of children of the current node
		for (t = 0;t < cs;t++)
		{
			child = nettree[j - 1][parent].children[t];    //The position of the most left child
			int nodename = nettree[j][child].name;
			if (nettree[j][child].used == true)
				continue;
			if (nettree[j][child].used == false)
			{
				occin.position[j] = child;			//
				//occinname[j]=nettree[j][occin[j]].name ;
				nettree[j][child].used = true;
				occ.position[j] = nodename;
				//	nettree[j][child].toleave =false;
				//	recovery[ls].level =j;
				//	recovery[ls].position =child;
				//	ls++;
				int ret = bts(j + 1, child, root, nettree, occ, occin);
				if (ret == 1)
					break;
			}

		}
		if (t == cs)	// cannot find it//t是在for()循环中定义的此处已经出来了 
		{
			int ret = bts(j - 1, child, root, nettree, occ, occin);// backtracking

			if (ret == -1)
				return -1;
			else
				return 1;
		}
		return 1;
	}
}
bool can_out(occurrence &occ)
{
	bool right;
	int i;
	for (i = 0;i < ptn_len;i++)
	{
		int gap = occ.position[i + 1] == occ.position[i] ? 0 : (occ.position[i + 1] > occ.position[i] ? occ.position[i + 1] - occ.position[i] - 1 : occ.position[i + 1] - occ.position[i] + 1);
		//cout<<"gap="<<gap<<"\t";
		if (gap<sub_ptn[i].min || gap>sub_ptn[i].max)
		{
			right = false;
			break;
		}
		else
		{
			right = true;
		}
	}

	return right;
}

void displayocc(occurrence &occ)
{
	//cout <<"An occurrence is:";
	bool c_out;
	c_out = can_out(occ);
	if (c_out)
	{
		int i = 0;
		cout << "<";
		for (i = 0;i < ptn_len;i++)
			cout << occ.position[i] << ",\t";
		cout << occ.position[i];
		cout << ">" << endl;
	}

}
void nonoverlength()
{
	vector <node> *nettree = new vector <node>[ptn_len + 1];
	create_nettree(nettree);
	//displaynettree(nettree);
//	update_nettree(nettree);
//	cout<<"-------------------------------------------"<<endl;
//  displaynettree(nettree);
//	store.resize(0);
	store.clear();
	//	nodep * recovery;
	//	recovery=new nodep[100*ptn_len];
	for (int position = 0;position < nettree[0].size();position++)
	{
		//	cout<<"come in "<<position<<" node"<<endl;
		/*	if (nettree[0][position].toleave ==false)
			{
				continue;

			}*/

		int root = nettree[0][position].name;
		occurrence occ;
		occurrence occin;//在网树中的位置；
		occ.position.resize(ptn_len + 1);
		occin.position.resize(ptn_len + 1);
		occin.position[0] = position;
		occ.position[0] = nettree[0][position].name;
		nettree[0][position].used = true;
		//	nettree[0][position].toleave=false;
			//Looking down for the most left child
		int j = 1, ret = -1;
		//	int ls=0;
		ret = bts(j, position, root, nettree, occ, occin);	//BackTracking Strategy
		if (ret == 1)
		{
			//	int len=store.size ();
			//	store.resize (len+1);
			//	store[len]=occ;
			store.push_back(occ);
			//update_nettree_pc(nettree,occin);
		}

	}
	//	cout<<"-------------------------------------------"<<endl;
	  //  displaynettree(nettree);
	//	cout<<"-------------------------------------------"<<endl;
	//	delete []recovery;
	delete[]nettree;
}
void Inputstr(char *fn, char *str)
{
	FILE* fp = fopen(fn, "r+");
	if (NULL == fp)
	{
		cout << "Cannot find the file.\n";
		return;
	}
	fseek(fp, 0, SEEK_END);
	int len = ftell(fp);
	fseek(fp, 0, 0);
	fscanf(fp, "%s", str);
}

int  main()
{

	//    char p[M]="a[-5,6]c[-4,7]g[-3,8]t";
	 //   char p[M]="a[-2,4]b[-1,1]c[-1,2]d";

		//char p[M]="c[-1,2]a[-2,3]c[-3,4]g";
		//char p[M]="t[-2,2]c[-2,2]a";
		//char p[M]="a[-5,6]c[-4,7]g[-3,8]t";
		//char p[M]="g[0,2]t[0,3]c[-3,4]a";
		//char p[M]="g[-1,5]t[0,6]a[-2,7]g[-3,9]t[-2,5]a[-4,9]g[-1,8]t[-2,9]a";
		//char p[M]="g[-1,9]t[-1,9]a[-1,9]g[-1,9]t[-1,9]a[-1,9]g[-1,9]t[-1,9]a[-1,9]g[-1,9]t";
		//char p[M]="g[-1,5]t[0,6]a[-2,7]g[-3,9]t[-2,5]a[-4,9]g[-1,8]t[-2,9]a[-1,9]g[-1,9]t";
		//char p[M]="g[-1,7]t[-1,7]a[-1,7]g[-1,7]t[-1,7]a[-1,7]g";
		//char p[M]="g[-1,5]t[-1,5]a[-1,5]g[-1,5]t[-1,5]a[-1,5]g";
	  //  char p[M]="a[-2,2]a[-2,2]a[-2,2]a";

		 //strcpy(S,"aaa");
		//strcpy(S,"gctacctttctgacatga");
			//gctacctttctgacatga");
			//atctgtacctacttcgcgctacctttctgacatga");
			//aggaatccagcgagacacttagaataacaattgcatctgtacctacttcgcgctacctttctgacatga");
			//tacctttctgacatgaccctcgaggaaatgtcacgagactggttcatgctcatgcctagg");
			//aggaatccagcgagacacttagaataacaattgcatctgtacctacttcgcgctacctttctgacatgaccctcgaggaaatgtcacgagactggttcatgctcatgcctagg");
			//aggaatccagcgagacacttagaataacaattgcatctgtacctacttcgcgctacctttctgacatgaccctcgaggaaatgtcacgagactggttcatgctcatgcctaggcaaaagataataggccctctttgcgtgcgattggaccaggcggtcatggaaaagaacatagtactgaaagcgaacttcagtgtaatctttaaccgttagaga");
			//atggactccaacaccatgtcaagctttcaggtagactgtttcctttggcatatccgcaagcgatttgcagacaatggattgggtgatgccccattccttgatcggctccgccgagatcaaaagtccttaaaaggaagaggcaacacccttggcctcgatatcgaaacagccactcttgttgggaaacaaatcgtggaatggatcttgaaagaggaatccagcgagacacttagaataacaattgcatctgtacctacttcgcgctacctttctgacatgaccctcgaggaaatgtcacgagactggttcatgctcatgcctaggcaaaagataataggccctctttgcgtgcgattggaccaggcggtcatggaaaagaacatagtactgaaagcgaacttcagtgtaatctttaaccgttagaga");
			//atggactccaacaccatgtcaagctttcaggtagactgtttcctttggcatatccgcaagcgatttgcagacaatggattgggtgatgccccattccttgatcggctccgccgagatcaaaagtccttaaaaggaagaggcaacacccttggcctcgatatcgaaacagccactcttgttgggaaacaaatcgtggaatggatcttgaaagaggaatccagcgagacacttagaataacaattgcatctgtacctacttcgcgctacctttctgacatgaccctcgaggaaatgtcacgagactggttcatgctcatgcctaggcaaaagataataggccctctttgcgtgcgattggaccaggcggtcatggaaaagaacatagtactgaaagcgaacttcagtgtaatctttaaccgttagagaccttgatactactaagggctttcactgaggagggagcaatagttggagaaatttcaccattaccttctcttccaggacatacttatgaggatgtcaaaaatgcagttggggtcctcatcggaggacttgaatggaatggtaacacggttcgagtctctgaaaatatacagagattcgcttggagaaactgtgatgagaatgggagaccttcactacctccagagcagaaatgaaaagtggcgagagcaattgggacagaaatttgaggaaataaggtggttaattgaagaaatgcggcacagattgaaagcgacagagaatagtttcgaacaaataacatttatgcaagccttacaactactgcttgaagtagaacaagagataagagctttctcgtttcagcttatttaatgataa");
	//		strcpy(S,"atatata");
	//		char p[M]="a[0,1]t[-1,1]a[0,1]t";
	// strcpy(S,"aaaa");
	//  char p[M]="a[-2,6]c[-1,3]t[-2,2]a";
		//char p[M]="a[-3,3]a[-3,3]a[-3,3]a";
		//strcpy(S,"aaaaaaaa");
		//strcpy(S,"cgtactcttcga");
		 //char p[M]="t[-1,2]c[-2,2]a[-2,2]g";
		//   strcpy(S,"aaaa");
		// char p[M]="a[-1,1]a[-2,1]a[-2,1]a";
		//char p[M]="a[-1,2]b[-2,2]c[-1,1]d";
		//strcpy(S,"dacbabcbdcd"); 
		//char p[M]="g[-1,2]t[0,2]c[-1,4]a";
		//strcpy(S,"agctacttgga"); 

		//char p[M]="a[0,4]b[-1,3]c";
		//strcpy(S,"acbacbabcc");

	//	char p[M]="a[-1,2]b[0,4]c[-1,4]d";
	//	 char p[M]="a[0,1]t[-1,1]a[0,1]t";
	//	strcpy(S,"atatat");

	//    char p[M]="b[-1,2]a[-1,2]c[-1,2]b[-1,2]d";
	//	char p[M]="p[-10,10]n[-10,10]f[-10,10]d[-10,10]q";
	char p[M] = "p[0,10]n[0,10]f[0,10]d[0,10]q";

	//agctcttcagaataactgta
	//	char p[M]="c[-1,2]a[-2,3]c[-3,4]g";
	//	char p[M]="g[1,2]t[0,3]c[-3,4]g";
	//	char p[M]="t[-2,2]c[-2,2]a[-2,2]t";
	//	char p[M]="t[-2,2]c[-2,2]a[-2,2]t[-2,2]c";
	//	char p[M]="g[-1,5]t[0,6]a[-2,7]g[-3,9]t[-2,5]a[-4,9]g[-1,8]t[-2,9]a";
	//	char p[M]="g[-1,9]t[-1,9]a[-1,9]g[-1,9]t[-1,9]a[-1,9]g[-1,9]t[-1,9]a[-1,9]g[-1,9]t";
	//	char p[M]="g[-1,5]t[0,6]a[-2,7]g[-3,9]t[-2,5]a[-4,9]g[-1,8]t[-2,9]a[-1,9]g[-1,9]t";
	//	char p[M]="g[-1,7]t[-1,7]a[-1,7]g[-1,7]t[-1,7]a[-1,7]g";
	//	char p[M]="g[-1,9]t[-1,9]a[-1,9]g[-1,9]t[-1,9]a[-1,9]g";
	char fsn[] = "./DataSet/68.txt";
	Inputstr(fsn, S);
	//	char p[M]="r[-2,7]g";
	convert_p_to_ruler(p);
	cout << "The pattern can written as follows:\n";
	disp_pattern();
	cout << p << endl;
	DWORD begintime = GetTickCount();
	//	for(int t=0;t<20;t++)	
	//	{
	nonoverlength();
	//	}
	DWORD endtime = GetTickCount();
	for (int i = 0;i < store.size();i++)
		displayocc(store[i]);
	//	cout <<"耗时"<<(endtime-begintime)/20.0<<"ms. \n";
	cout << "耗时" << endtime - begintime << "ms. \n";
	cout << "共有" << store.size() << "个出现\n";
	return 0;
}
