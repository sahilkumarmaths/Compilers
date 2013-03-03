/*
 * Author   : Sharath Reddy , Sahil Kumar, PVS Dileep
 * Version  : 1.0
 * Date		: 3/3/2013
 */

#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <set>
#include <string.h>
#include <fstream>
#include <math.h>
#include <map>
using namespace std;

int counter = 1;																																// State counter

/*
 * NFA Structure
 */
struct nfa{
		int columns;													// #columns = alphabets + E + finite + state	
		int rows;																// #Rows = states + 1
		set <string>alphabets;			// Contain alphabets without epsilon
		string **table;										// Contain the values
};

/*
 * DFA Structure
 */
struct dfa
{
	int rows;																		// Unknown so taken by default 500
	int columns;															// #columns = Alphabets + states + bool
	string **table;												// Contain the values
	set <string>alphabets;					// We know it from nfa
};
	
string int_str(int a);

// Creating regular expressions to NFA
struct nfa* create_a(string b);
struct nfa* create_astar(struct nfa *a);
struct nfa* create_table(int rows,int columns);
struct nfa* create_ab(struct nfa *a , struct nfa *b);
struct nfa* create_aorb(struct nfa *a , struct nfa *b);


void nfa_delete(struct nfa *a );
void nfa_display( struct nfa *a);
struct dfa* nfa_to_dfa(struct nfa* c);

void init_bool(struct nfa *c, struct nfa *a ,struct nfa *b);
void init_bool_aorb(struct nfa *c, struct nfa *a ,struct nfa *b);
void merge_column(struct nfa *c1, struct nfa *a, struct nfa *b, int c , int d, int e,string symbol);
void merge_column_distinct(struct nfa *c, struct nfa *a , struct nfa *b, int d,  int e,string  a1,int up );
int find_start_state(struct nfa* c);
int find_symbol(struct nfa *a, string b);
int find_symbol_char(struct nfa *a, char b);
int alphabet_size(struct nfa *a, struct nfa *b);
int find_state(struct nfa * c , string state);
set<string> E_closure(set<string> T ,struct nfa* c );
void dfa_set_bool(struct dfa *d, string a,int dfa_rows);

//Functions for DFA manipulation
void dfa_insert_states(struct dfa *d, set <string> s, int i);
void dfa_delete(struct dfa *a ,int start,int rows, int columns);
void dfa_insert_transitions(struct dfa *d, set <string> u,int col,int dfa_row);
struct dfa* dfa_create_table(int rows,int columns);

//Dfa Simulation
string dfa_transition(struct dfa *df, string cur_state, string input);
int dfa_start_state(struct dfa *df);
int dfa_isaccept_state(struct dfa *df,string state );
void dfa_simulate(struct dfa *df);

//Regex Parsing
struct nfa* nfa_create_range(string start,string end);
struct nfa* scan_regex();
struct nfa* ret_nfa(string word);
map<int,string> mymap;

int regex_pos;

//Simulation
FILE *input;
char nextchar();
void nfa_simulation(struct nfa * c);
int find_final_state(struct nfa* c);
set<string> move(set<string> S ,char next_char, struct nfa* c );
string char_to_string(char a);
void print(std::set<string> set);
void display_dfa(struct dfa *a);


struct info
{
	string temp_final;
	string data;
} 	ret[6];


int main()
{
		struct nfa *regex = scan_regex();
	 nfa_display(regex);
	
	 struct dfa *star = nfa_to_dfa(regex);
	 display_dfa(star);
	
	 dfa_simulate(star);
	
}

/*
 * Prints all elements of a set
 */
void print(std::set<string> set)
{
	std::set<string>::iterator it;
	
	for(it=set.begin(); it !=set.end(); ++it)
		{
				cout<<" "<<*it<<" ";
		}
		cout<<endl;
}


void nfa_simulation(struct nfa * c)
{
	char next_char;
	std::set<string>::iterator it;
	std::set<string> start_set;
	int start_state_row = find_start_state(c);
	start_set.insert(c->table[start_state_row][1]);
	std::set<string> S = E_closure(start_set , c);
	
	//cout<<"For frst time\n";
	std::set<string> temp;
	next_char = nextchar();
	
	//cout<<"next char is:\n"<<next_char<<endl;
	//print(S);
	int i = 0;
	//while(i<5)
	while(1)
	{
		temp = S;
		if(next_char == '\n')//change it to EOF finally
		{
			//cout<<"came to EOF\n";
			int final_state_row = find_final_state(c);
			it = temp.find(c->table[final_state_row][1]) ;
			if( it != temp.end() )
			{
				//cout<<"Found in prev temp\n";
				for(int j =0 ; j<2; j++)
				{
					//cout<<"Serchng for:"<<ret[j].temp_final<<"#j:"<<j<<endl;
					it = temp.find(ret[j].temp_final);
					
					if(it != temp.end())
					{
						//cout<<"temp final state is:"<<*it<<endl;
						cout<<"Its a :"<<ret[j].data<<endl;
					}
				}
			}
			exit(0);
		}
		S = E_closure(move(S,next_char,c), c);
		//cout<<"For character:"<<next_char<<endl;
		print(S);
		if(S.empty())
		{
			int final_state_row = find_final_state(c);
			it = temp.find(c->table[final_state_row][1]) ;
			if( it != temp.end() )
			{
				//cout<<"Found in prev temp\n";
				for(int j =0 ; j<2; j++)
				{
					//cout<<"Serchng for:"<<ret[j].temp_final<<"#j:"<<j<<endl;
					it = temp.find(ret[j].temp_final);
					
					if(it != temp.end())
					{
						//cout<<"temp final state is:"<<*it<<endl;
						cout<<"Its a :"<<ret[j].data<<endl;
					}
				}
			}
			fseek ( input , -1 , SEEK_CUR );
			S = E_closure(start_set , c);			
		}
		//cout<<"Taking next character\n";
		//i++;
		
		next_char = nextchar();
	}
}

/*
 * Returns the next input character after reading the file
 */
char nextchar()
{
	char c;
	c=fgetc(input);
	//if(c==EOF)
	//exit(0);
	return c;
}

/*
 * Converting NFA to DFA
 */
struct dfa* nfa_to_dfa(struct nfa* c)
{
	int dfa_rows = 1;																									//Counter for the rows of dfa 
	struct dfa *df;																										//DFA 
	set<string>::iterator it;
	int i=2;																																		//For filling the alphabets in DFA table from 2 
	
	string c_start_state = c->table[find_start_state(c)][1];			//Start State of NFA
 string c_final_state = c->table[find_final_state(c)][1];			//Final State of NFA
	
	//Allocating Space for DFA. It sets "States" and "Bool"
	//df = dfa_create_table(pow(2,c->rows -1) +1, c->columns-1);
	df = dfa_create_table(1000, c->columns-1);
	
	// Assigning rows and columns
	df->columns = c->columns-1;
	df->rows = 1000;
	//df->rows = pow(2,c->rows -1) + 1;
	
	//Setting the alphabets
	for(it = c->alphabets.begin(); it!= c->alphabets.end();++it)
	{
		df->table[0][i] = *it;
		i++;
	}
	
	set < set<string> > marked;					//This set will contain all marked sets
	set < set<string> > unmarked;			// This set will contain all unmarked sets
	
	// Start Sate E-Closure
	set<string> start_set , u;
	set<string> t;
	int start_state_row = find_start_state(c);
	start_set.insert(c->table[start_state_row][1]);
	
	start_set = E_closure(start_set,c);
	//cout<< "Printing set start_set "<<endl;
		//print(start_set);
		//cout<<endl;
	//Inserting the first set in unmarked set
	unmarked.insert(start_set);
	
	while(unmarked.size()!=0)
	{
		t = *(unmarked.begin());
		
		unmarked.erase(unmarked.find(t));
		marked.insert(t);
		
		//Inserting the first states in the Dfa table 
		dfa_insert_states(df, t, dfa_rows);
		if( t.find(c_start_state)!=t.end())
		{
			dfa_set_bool(df,"1",dfa_rows);
		}else if( t.find(c_final_state)!=t.end())
		{
			dfa_set_bool(df,"2",dfa_rows);
		}else
		{
			dfa_set_bool(df,"3",dfa_rows);
		}
		
		
		int col = 2;
		for(it = c->alphabets.begin(); it!= c->alphabets.end(); ++it)
		{
			
			u = E_closure(move(t,(*it)[0],c),c);
			if(!(marked.find(u)!= marked.end() || unmarked.find(u)!= unmarked.end()))
			{
				if(u.size()>0)
					unmarked.insert(u);
			}// If u is neither in the marked or unmarked states
			
			if(u.size()>0)
			{
				dfa_insert_transitions(df,u,col,dfa_rows);
			}
			col++;
		}
		dfa_rows++;
	}//while ends
	df->rows = dfa_rows;
	//nfa_delete(c );
	
	return df;
	//return NULL;
}

/*
 * Inserts the set in the states in the ith row of DFA table
 */
void dfa_insert_states(struct dfa *d, set <string> s, int i)
{
	set <string>::iterator it;
	for(it = s.begin();it!= s.end() ;++it)
	{
		d->table[i][1] += *it + ","; 
	}
}

/*
 * Sets the bool states of DFA
 */
void dfa_set_bool(struct dfa *d, string a,int dfa_rows)
{
		d->table[dfa_rows][0] = a;
}

/*
 * Inserts the set u in the row = dfa_row and column = col in the DFA table 
 */
void dfa_insert_transitions(struct dfa *d, set <string> u,int col,int dfa_row)
{
	set <string>::iterator it;
	for(it = u.begin();it!= u.end() ;++it)
	{
		d->table[dfa_row][col] += *it + ","; 
	}
}

/*
 * Computes 'move' of states in S,  of the NFA c on input character next_char
 */
set<string> move(set<string> S ,char next_char, struct nfa* c )
{
	
	set<string> move_states;
	set<string>::iterator it;
	
	for(it=S.begin(); it !=S.end(); ++it)
		{
				int row_no = find_state(c ,*it);
				int column_no =find_symbol_char(c, next_char);
				string temp = c->table[row_no][column_no];	
				cout<<"Temp:"<<temp<<"###it:"<<*it<<"RowNo:"<<row_no<<"##ColNo:"<<column_no<<endl;		
				string state;
				stringstream stream(temp);
				 while( getline(stream, state, ',') )
				 {
						cout<<"Inserting:"<<state<<endl;
						move_states.insert(state);					 
				 }				
		}
return move_states;	
}

/*
 * Converts char to string
 */
string char_to_string(char a)
{
       stringstream ss;
       string s;
       char c = a;
       ss << c;
       ss >> s;
       return s;
}

/*
 * This function computes E closure of states in T,  of the NFA c
 */
set<string> E_closure(set<string> T ,struct nfa* c )
{
	set<string>::iterator it;
	string temp ;
	int row_no;	

	for(it=T.begin(); it !=T.end(); )
		{
				//cout<<"E closure current it:"<<*it<<endl;
				row_no = find_state(c , *it);
				temp = c->table[row_no][c->columns-1];			
				string state;
				stringstream stream(temp);
				++it;
				 while( getline(stream, state, ',') )
				 {
					 if(!(T.find(state)!= T.end()))
						{
							//cout<<"Inserting to E closure:"<<state<<endl;
							T.insert(state);
							it=T.begin();
							//cout<<"T.begin():"<<*it<<endl;
						}					 
				 }				
		}
return T;
}

/*
 * Returns true if two sets are equal, else false
 */
bool set_isequal(set<string> a, set<string> b)
{
	std::set<string>::iterator it1, it2;
	for (it1=a.begin(); it1!=a.end(); ++it1)
	{
		if(b.find(*it1) == b.end())return false;
	}
}

/*
 * Returns the row_no of the state , given state number in string format 
 */
int find_state(struct nfa *c , string state)
{
	for(int i=0 ; i < c->rows ;i++)
	{
		if(c->table[i][1] == state)
		{
			return i;
		}
	}
return -1;
}

/*
 * Returns the row_no of the start state of given nfa 
 */
int find_start_state(struct nfa* c)
{
	for(int i=0 ; i<c->rows ;i ++)
	{
		if(c->table[i][0] == "1" )
		return i;
	}
}

/*
 * Returns the row_no of the final state of given nfa 
 */
int find_final_state(struct nfa* c)
{
	for(int i=0 ; i<c->rows ;i ++)
	{
		if(c->table[i][0] == "2" )
		return i;
	}
}

/*
 * Creates the NFA for regular expression a
 * b = the alphabet
 * Initial state = 1;
 * Final State = 2;
 * Other = 3;
 */
struct nfa* create_a(string b)
{
		
		struct nfa *c = create_table(3,4);
		c->rows = 3;
		c->columns = 4;
		c->alphabets.insert(b);
	
		c->table[0][0] = "Bool";
		c->table[0][1] = "States";
		c->table[0][2] = b;
		c->table[0][3] = 'E';
		
		c->table[1][0] = "1";																		//Bool Initial State
		c->table[1][1] = int_str(counter)	;				//State	
		c->table[1][2] = int_str(counter+1) + ",";			
		c->table[1][3] = "";
		c->table[2][0] = "2";																		// Bool Final State
		
		c->table[2][1] = int_str(counter+1);				//State
		c->table[2][2] = "";
		c->table[2][3] = "";
		counter += 2;
	return c;
}

/*
 * Creates NFA for a*
 */
 struct nfa* create_astar(struct nfa *a)
{
		//Creating table of size alphabets +3  
		struct nfa *c = create_table(a->rows +2, a->alphabets.size() + 3);
		c->rows = a->rows +2;
		c->columns = a->alphabets.size() + 3;
		
		c->alphabets = a->alphabets;
		
		int third = counter;							// This is the third state
		int fourth = counter+1;				// This is the Fourth State
		counter += 2;
		
		int i1;																				//i1= Initial state of a
		int f1;			 																// f1 == Final state of a
		
		//Setting Bool column
		for(int i=0; i< a->rows; i++)
		{
			if(a->table[i][0] == "1" )			// 1 Means start state
			{
				i1 = i;
				c->table[i][0] = "3";
			}else if( a->table[i][0] == "2" )
			{
				f1 = i;
				c->table[i][0] = "3";
			}else
			{
				c->table[i][0] = a->table[i][0];
			}
		}
		c->table[a->rows][0] = "1";
		c->table[a->rows +1][0] = "2";
		// Bool column filled
		
		//Setting States column
		for(int i=0; i<a->rows; i++)
		{
				c->table[i][1] = a->table[i][1];
		}
		c->table[a->rows][1] = int_str(third);
		c->table[a->rows +1][1] = int_str(fourth);
		// Settings column filled
		
		//Filling the alphabets column
		for(int i= 0 ; i< a->rows ; i++)
		{
				for(int j =2; j< 2+ a->alphabets.size()+1; j++)
				{
					c->table[i][j] = a->table[i][j];
				}
		}
		
		c->table[f1][c->columns-1] += int_str(fourth)+"," + a->table[i1][1]+",";
		c->table[a->rows][c->columns-1] += int_str(fourth)+"," + a->table[i1][1]+",";
		//Table filled
		nfa_delete(a);
		
	return c; 
}

/*
 * Creates the nfa for a|b by taking the nfas for a and b
 */
struct nfa* create_aorb(struct nfa *a , struct nfa *b)
{
		//Creating table of size alphabets +3  
		struct nfa *c = create_table(a->rows + b->rows -1 + 2,alphabet_size(a, b)+3);
		set <string>tempset;
		
		set<string>::iterator it;	
		//These will contain only the alphabets of a and b
		for(it=a->alphabets.begin(); it !=a->alphabets.end(); ++it)
		{
				c->alphabets.insert(*it);
				tempset.insert(*it);
		}
		for(it=b->alphabets.begin(); it !=b->alphabets.end(); ++it)
		{
				c->alphabets.insert(*it);
				tempset.insert(*it);
		}	
				
		//states column filling
		for(int i = 1 ; i < a->rows; i++)
		{
				c->table[i][1] = a->table[i][1];
		}
		for(int i = 0 ; i < b->rows-1; i++)
		{
				c->table[a->rows+i][1] = b->table[ i +1 ][1]; 
		}
		
		//extra two states filling
		c->table[c->rows-2][1] = int_str(counter);
		c->table[c->rows-1][1] = int_str(counter+1);
		counter += 2;
		
		int i=2;
		for(it = c->alphabets.begin(); it!=c->alphabets.end(); ++it )
		{
				if(a->alphabets.find(*it) != a->alphabets.end() && b->alphabets.find(*it) != b->alphabets.end())
				{
						merge_column(c, a, b, find_symbol(a, *it) , find_symbol(b, *it) , i, *it);
						i++;
				}else if( a->alphabets.find(*it) != a->alphabets.end() )
				{
						merge_column_distinct(c, a ,b, find_symbol(a, *it),  i,*it,1 );
						i++;
				}else
				{
					merge_column_distinct(c, a ,b, find_symbol(b, *it),  i,*it,0 );
					i++;
				}
		}
		init_bool_aorb(c, a ,b);
		nfa_delete(a);
		nfa_delete(b);
		return c;			
}

/*
 * Creates the combined nfa for a and b i.e ab
 */
struct nfa* create_ab(struct nfa *a , struct nfa *b)
{
		//Creating table of size alphabets +3  
		struct nfa *c = create_table(a->rows + b->rows -1,alphabet_size(a, b)+3);
		set <string>tempset;
		set<string>::iterator it;
		
		//These will contain only the alphabets of a and b
		for(it=a->alphabets.begin(); it !=a->alphabets.end(); ++it)
		{
				c->alphabets.insert(*it);
				tempset.insert(*it);
		}
		for(it=b->alphabets.begin(); it !=b->alphabets.end(); ++it)
		{
				c->alphabets.insert(*it);
				tempset.insert(*it);
		}
		
		for(int i = 1 ; i < a->rows; i++)
		{
				c->table[i][1] = a->table[i][1];
		}
		for(int i = 0 ; i < b->rows-1; i++)
		{
				c->table[a->rows+i][1] = b->table[ i +1 ][1]; 
		}
		
		int i = 2;
		for(it = c->alphabets.begin(); it!=c->alphabets.end(); ++it )
		{
				if(a->alphabets.find(*it) != a->alphabets.end() && b->alphabets.find(*it) != b->alphabets.end())
				{
						merge_column(c, a, b, find_symbol(a, *it) , find_symbol(b, *it) , i, *it);
						i++;
				}else if( a->alphabets.find(*it) != a->alphabets.end() )
				{
						merge_column_distinct(c, a ,b, find_symbol(a, *it),  i,*it,1 );
						i++;
						
				}else
				{
					merge_column_distinct(c, a ,b, find_symbol(b, *it),  i,*it,0 );
					i++;
				}
		}
		init_bool(c, a ,b);
		nfa_delete(a);
		nfa_delete(b);
	return c;
}

/*
 * Returns the size of the union of the alphabets of two nfa's
 */
int alphabet_size(struct nfa *a, struct nfa *b)
{
		set <string> c;
		set <string>:: iterator it;
	
		//These will contain only the alphabets of a and b
		for(it=a->alphabets.begin(); it !=a->alphabets.end(); ++it)
		{
				c.insert(*it);
		}
		for(it=b->alphabets.begin(); it !=b->alphabets.end(); ++it)
		{
				c.insert(*it);
		}
		return c.size();
}

/*
 * Deletes the space allocated to NFA
 */
void nfa_delete(struct nfa *a )
{
		/*for(int i=0;  i<a->rows; i++)
		{
				delete(a->table[i]);	
		}*/
		delete(a->table);
		delete(a);
}

/*
 * Deletes the space allocated to dfa from the starting 
 * start = starting row from where to deallocate
 * rows = number of rows to deallocate
 * columns = number of columns in the table
 */
void dfa_delete(struct dfa *a ,int start,int rows, int columns)
{
		for(int i=start;  i<rows; i++)
		{
				delete(a->table[start]);	
		}
		//delete(a->table);
		//delete(a);
}

/*
 * Allocates the space for NFA Table of size rows*columns
 */
struct nfa *create_table(int rows,int columns)
{
		struct nfa *c = new struct nfa;
		
		c->rows = rows;
		c->columns = columns;
		c->table = new string*[c->rows];
		for(int i=0; i<c->rows; i++)
		{
				c->table[i] = new string [c->columns];	
		}
		c->table[0][0] = "Bool";
		c->table[0][1] = "States";
		c->table[0][columns-1] = 'E';		
		return c;
}

/*
 * Allocates the space for DFA Table of size rows*columns
 */
struct dfa *dfa_create_table(int rows,int columns)
{
		struct dfa *c = new struct dfa;
		
		c->rows = rows;
		c->columns = columns;
		c->table = new string*[c->rows];
		for(int i=0; i<c->rows; i++)
		{
				c->table[i] = new string [c->columns];	
		}
		c->table[0][0] = "Bool";
		c->table[0][1] = "States";
		c->table[0][columns-1] = 'E';		
		return c;
}

/*
 * Returns the index of the alphabet b in NFA Table of a
 * Not found returns -1
 */
int find_symbol(struct nfa *a, string b)
{
	for(int i= 2; i < a->columns; i++)
	{
			if(a->table[0][i]==b)
			{
				return i;
			}
	}
	return -1;
}

/*
 * Finding symbol char?? why we made this
 */
int find_symbol_char(struct nfa *a, char b)
{
	char temp[1];
	temp[0]=b;
	for(int i= 2; i < a->columns; i++)
	{
		int arg1=b;
		string temp=a->table[0][i];
		int arg2=temp[0];
			if(arg1 == arg2)
			{	
				return i;
			}
	}
	return -1;
}

/*
 * This will be called if the alphabet is same
 * Here the column c of a is merged with column d of b and put in column e of c.
 * Keep the states in the mind
 */
void merge_column(struct nfa *c1, struct nfa *a, struct nfa *b, int c , int d, int e,string symbol)
{
	c1->table[0][e] =  symbol;
	for(int i = 1 ; i< a->rows; i++)
	{
			c1->table[i][e] = a->table[i][c];
	}
	for(int i = 0 ; i< b->rows -1; i++)
	{
			c1->table[i+a->rows][e] = b->table[i+1][d];
	}
}

/*
 * This will be called if the alphabet is not same
 * Here the column c of a is put in colun e of c with string symbol a1 and accoring to up.
 * Keep the states in the mind
 * if up == 1 means upper is there and lower is to be set blank
 * if up == 0 means lower is there and upper is to be set blank
 * Up = 1 .. means filling d th column of a in eth column of c ..and up
 * Up = 0 .. means filling d th column of b in eth column of c .. and down
 */
void merge_column_distinct(struct nfa *c, struct nfa *a , struct nfa *b, int d,  int e,string  a1,int up )
{
	c->table[0][e] =  a1;	
	if(up == 1)
	{
		for(int i = 1 ; i< a->rows; i++)
		{
				c->table[i][e] = a->table[i][d];
		}
		
	}else
	{
		for(int i = 0 ; i< b->rows -1; i++)
		{
				c->table[i+a->rows][e] = b->table[i+1][d];
		}
	}
}

/*
 * This function initializes the column corresponding to bool column and
 * Epsilon Transitions And sets the final state of a and initial state of b as nothing 
 * And This sets the final state of a to initial state of b via E- Transitions 
 */
void init_bool(struct nfa *c, struct nfa *a ,struct nfa *b)
{
		int f1;
		string i1;
		for(int i = 1 ; i< a->rows; i++)
		{
				if(a->table[i][0] == "2")
				{
					f1 = i;
					c->table[i][0] = "3";
				}
				else	
					c->table[i][0] = a->table[i][0];
				
					c->table[i][c->columns-1] = a->table[i][a->columns-1];
		}
		
		for(int i = 0 ; i< b->rows -1; i++)
		{
				if(b->table[i+1][0] == "1")
				{
					i1 = b->table[i+1][1];
					c->table[i+a->rows][0] = "3" ;
				}
				else	
					c->table[i+a->rows][0] = b->table[i+1][0];
					
					c->table[i+a->rows][c->columns-1] = b->table[i+1][b->columns-1];
		}
		c->table[f1][c->columns-1] +=  i1 + ","; 
}

/*
 * Initializes the column corresponding to bool column and
 * Epsilon Transitions
 * Sets the E transitions for 2 extra states
 */
void init_bool_aorb(struct nfa *c, struct nfa *a ,struct nfa *b)
{
		for(int i = 1 ; i< a->rows; i++)
		{
				if(a->table[i][0] == "2")
				{
					c->table[i][0] = "3";
					c->table[i][c->columns-1] = c->table[c->rows-1][1] + ",";
				}
				else if(a->table[i][0] == "1")
				{
					c->table[i][0] = "3";
					c->table[c->rows-2][c->columns-1] += (a->table[i][1]+",");
					c->table[i][c->columns-1] += a->table[i][a->columns-1];
				}
				else
				{	
					c->table[i][0] = a->table[i][0];				//bool column
					c->table[i][c->columns-1] = a->table[i][a->columns-1];	//E column
				}
		}
		
		for(int i = 0 ; i< b->rows -1; i++)
		{
				if(b->table[i+1][0] == "1")
				{
					c->table[i+a->rows][0] = "3" ;
					c->table[c->rows-2][c->columns-1] += (b->table[i+1][1] + ",");
					c->table[i+a->rows][c->columns-1] += b->table[i+1][b->columns-1];
				}
				else if(b->table[i+1][0] == "2")
				{
					c->table[i+a->rows][0] = "3";
					c->table[i+a->rows][c->columns-1] = c->table[c->rows-1][1] + ",";
				}
				else
				{	
					c->table[i+a->rows][0] = b->table[i+1][0];
					c->table[i+a->rows][c->columns-1] = b->table[i+1][b->columns-1];	//E column
				}
		}
		c->table[c->rows-1][0] = "2";
		c->table[c->rows-2][0] = "1";
}

/*
 * Function converts the string to int
 */
string int_str(int a)
{
	stringstream sstm;	
	sstm<< a;
	return sstm.str();
}

/*
 * Displays the NFA
 */
void nfa_display( struct nfa *a)
{
	for(int i=0; i<a->rows; i++)
	{
			for(int j=0;j<a->columns; j++)
			{
					cout<<a->table[i][j]<<"\t";
			}
			cout<< endl;
	}
	cout<< endl;
}

/*
 * Displays the DFA
 */
void display_dfa( struct dfa *a)
{
	for(int i=0; i<a->rows; i++)
	{
			for(int j=0;j<a->columns; j++)
			{
					cout<<a->table[i][j]<<"\t";
			}
			cout<< endl;
	}
	cout<< endl;
}

/*
 * 
 */
struct nfa* scan_regex()
{
	ifstream infile("regex");
	if(infile.good())
	{
		struct nfa* complete;
		int iter=0;
		while(!infile.eof())
		{
			string word;
			getline(infile,word,'\t');
			int len=word.length();
			
			string firstchar,secondchar;
			struct nfa* base;
			struct nfa* temp;
			
			int i=0;
			if(word[i]=='[')
			{
				firstchar = word.substr (1,1);
				i++;i++;
			}
			else
				break;
			if(word[i]=='-')
			{
				secondchar = word.substr(i+1,1);
				base = nfa_create_range(firstchar,secondchar);
				while(1)
				{
					if(word[i+2]==']')
					{
						i=i+3;
						if(word[i]=='*')
						{
							base = create_astar(base);
							i++;
						}
						break;
					}
					else 
					{
						firstchar = word.substr(i+2,1);
						if(word[i+3]=='-')
						{
							secondchar=word.substr(i+4,1);
							base = create_ab(base,nfa_create_range(firstchar,secondchar));
							i = i+3;
						}
					}
				}
			}
			else
			{
				base=create_a(firstchar);
				if(word[i]=='*')
				{
					base = create_astar(base);
					i++;
				}
			}	
			//end of first parse
			
			for(;i<len;i++)
			{
				int check=0;
				if(word[i]=='[')
				{
					check=1;
					firstchar = word.substr (i+1,1);
					i++;i++;
				}
				if(word[i]=='|')
				{
					struct nfa *arg1,*arg2;
					arg1=base;
					arg2=ret_nfa(word.substr(i+1,word.length()));
					i = i+regex_pos;
					if(word[i]==']')
					{
						base = create_aorb(arg1,arg2);
					}
					else
					{
						while(1)
						{
							if(word[i]==']')
							{
								i=i+1;
								break;
							}
							else 
							{
								firstchar = word.substr(i,1);
								if(word[i+1]=='-')
								{
									secondchar=word.substr(i+2,1);
									arg2 = create_ab(arg2,nfa_create_range(firstchar,secondchar));
									i = i+3;
								}
							}
						}
						base = create_aorb(arg1,arg2);
					}
				}
				else if(word[i]=='-')
				{
					secondchar = word.substr(i+1,1);
					base = create_ab(base,nfa_create_range(firstchar,secondchar));
					while(1)
					{
						if(word[i+2]==']')
						{
							i=i+3;
							break;
						}
						else 
						{
							firstchar = word.substr(i+2,1);
							if(word[i+3]=='-')
							{
								secondchar=word.substr(i+4,1);
								base = create_ab(base,nfa_create_range(firstchar,secondchar));
								i = i+3;
							}
						}
					}
				}
				else
				{
					if(check==1)
					{
						base=create_ab(base,create_a(firstchar));
					}
				}
			}
			getline(infile,word,'\n');
			if(iter!=0)
				complete=create_aorb(complete,base);
			else
				complete=base;
			iter++;
			mymap[find_final_state(base)]=word;
		}
		return complete;
	}
}

/*
 * 
 */
struct nfa* ret_nfa(string word)
{
	int i=0;
	string firstchar,secondchar;
	struct nfa* base;
	struct nfa* temp;
	
	//first parse
	if(word[i]=='[')
	{
		firstchar = word.substr (1,1);
		i++;i++;
	}
	if(word[i]=='-')
	{
		secondchar = word.substr(i+1,1);
		base = nfa_create_range(firstchar,secondchar);
		while(1)
		{
			if(word[i+2]==']')
			{
				i=i+3;
				regex_pos=i;
				break;
			}
			else 
			{
				firstchar = word.substr(i+2,1);
				if(word[i+3]=='-')
				{
					secondchar=word.substr(i+4,1);
					base = create_ab(base,nfa_create_range(firstchar,secondchar));
					i = i+3;
				}
			}
		}
	}
	else
	{
		base=create_a(firstchar);
		regex_pos=i+1;
	}
	return base;
}

/*
 * 
 */
struct nfa* nfa_create_range(string firstchar,string secondchar)
{
	struct nfa *temp,*base;
	int j=firstchar[0];
	char arg=j;
	 base = create_a(char_to_string(arg));
	j++;
	for(;j<=secondchar[0];j++)
	{
		char arg = j;
		temp = create_a(char_to_string(arg	));
		base = create_aorb(base,temp);
	}
	return base;
}

/*
 * Simulating input on DFA
 */
void dfa_simulate(struct dfa *df)
{
	char nc;																																																			 	//next char
	string starting_state = df->table[dfa_start_state(df)][1];			// Starting state
	string cur_state = starting_state;																											//Curent State set to start state
	string next_state;																																											//Next State
	std::set<string>::iterator it;
	string buffer="";
	
	while(1)
	{
			nc = nextchar();
			if(nc == 10)
			{
				if(dfa_isaccept_state(df,cur_state) == 1)
				{
					cout<<buffer<<endl;
				}else
				{
					//Error
					cout<<"Error<<" << buffer;
				}
				break;
			}
			next_state = dfa_transition(df,cur_state,char_to_string(nc));
			
			if(dfa_isaccept_state(df,next_state)==1)
			{
					cur_state = next_state;
					buffer+= nc;
					continue;
			}else if(next_state != "")		
			{
					
					if(dfa_isaccept_state(df,cur_state)==1 )
					{
							cout<< buffer<<endl;
							buffer= "";
							//Accept string and continue parsing
							cur_state = starting_state;
							fseek(input,-1,SEEK_CUR);
							continue;
					}else
					{
						cur_state = next_state;
						buffer+= nc;
						continue;
					}
			}else if(next_state == "")
			{
					if(dfa_isaccept_state(df,cur_state)==1 )
					{
							//Accept string and continue parsing
							cout<< buffer<<endl;
							buffer= "";
							cur_state = starting_state;
							fseek(input,-1,SEEK_CUR);
							continue;
					}else
					{
						//Error
						cout<<"Error<<" << buffer;
						exit(1);
					}
			}
	}//while ends
}

/*
 * Returns the row number corresponding to the start state of dfa df 
 */
int dfa_start_state(struct dfa *df)
{
	for(int i=0 ; i<df->rows ;i ++)
	{
		if(df->table[i][0] == "1" )
			return i;
	}
}

/*
 * Returns the next state corresponding to the start state of dfa df 
 */
string dfa_transition(struct dfa *df, string cur_state, string input)
{
	int row = -1;
	int column =-1 ;
	
	for(int i= 0; i <df->columns ; i++)
	{
		if(df->table[0][i] == input)
		{
				column = i;
				break;
		}
	}
	for(int i= 0; i< df->rows ; i++)
	{
		if(df->table[i][1] == cur_state)
		{
				row = i;
				break;
		}
	}
	
	if(row == -1 || column == -1)
	{
			return "";
	}else
		return df->table[row][column];
}

/*
 * Returns 1= if accept state
 * Return 0 = if not accept state
 */
int dfa_isaccept_state(struct dfa *df,string state )
{
	for(int i= 0; i<df->rows ; i++)
	{
		if(df->table[i][1] == state && df->table[i][0]== "2")
			return 1;
	}
	return 0;
}
