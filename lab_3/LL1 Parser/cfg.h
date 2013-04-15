#include<iostream>
#include<set>
#include<vector>
#include<map>
#include<stdio.h>
#include<fstream>
#include<string.h>
#include<stdlib.h>
#include<sstream>
#include<vector>
#include<algorithm>
#include<stack>
using namespace std;

class cfg{
	private:
		int left_counter;
		string left_recur;
		string start_symbol;
		set<string> non_terminals;
		set<string> terminals;
		map<string,set <string> > first_of;
		map<string,set <string> > follow_of;
		multimap<string, vector <string> > productions;
		map<string, multimap<string,vector <string> > > ll1_table;
		stack<string> symbols;
	public:
		cfg(string filename) ;
		void print_set(set <string>);
		void fill(string str);
		void fill_productions(string);
		void print_vector(vector<string>);
		void print_map(multimap<string, vector <string> >);
		void print_map_set(map<string, set <string> > );
		void eliminate_left_recursuion();
		void first_sets();
		void follow_sets();
		void ll1_parsing_table();
		void print_ll1_table(map<string, multimap<string,vector <string> > > my_map);
		set<string> first_of_string(vector <string>);
		set<string> union_set(set<string> s1, set<string> s2,int *chg );
		void parseinput(string lookahead);
		void push_production(vector<string> myvector);

		void immediate_left_recursion(string );
		void left_factoring(multimap <string,vector<string> >);
		string next_symbol();
};
