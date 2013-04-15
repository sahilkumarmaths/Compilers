/*
 * Author   : Sharath Reddy , Sahil Kumar, PVS Dileep
 * Version  : 1.0
 * Date		: 5/3/2013
 * Purpose  : Converts the regular expression to NFA to DFA and simulates the input on DFA
 * Note 	: It uses the power of DFA
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
#include <vector>
#include "lex.h"
#include <stdio.h>
#include <ctype.h>
#include <algorithm>

using namespace std;

int counter = 1;						// State counter

/*
 * NFA Structure
 */
struct nfa{
		int columns;					//Size of columns = alphabets + E + finite + state
		int rows;						// Rows = states+1
		set <string>alphabets;			//This will contain alphabets without epsilon
		string **table;					// Table for NFA transitions
};

/*
 * DFA Structure
 */
struct dfa
{
	int rows;							// 1 + unknown
	int columns;						// Alphabets + marked+ states +bool
	string **table;						// Table for DFA transitions
	set <string>alphabets;				// Alphabets of DFA same as of NFA
};

char* yytext = ""; 						// Lexeme (not '\0' terminated)
int yyleng   = 0;  						// Lexeme length. 
int yylineno = 0;  						// Input line number of grammer files
int line_counter=0;						//Line counter of input.c file

//For  Symbol Table
int id_counter = 0;							//Counts for total identifiers seen
map <string,int> id_map;					// Identifier and the list correspnding to use
vector<vector <int> > symbol_table(50);		//Will store the line numbers corresponding to the identifiers
map<string,string> mymap;					//Remember final states of token classes-NFA

struct nfa*    factor      ( void );
struct nfa*    term        ( void );
struct nfa*    expression  ( void );
struct nfa*    par	(void);					//Parenthesis
	
// Thompson constructions
struct nfa* create_a(string b);
struct nfa* create_ab(struct nfa *token_classa , struct nfa *b);
struct nfa* create_astar(struct nfa *a);
struct nfa* create_aorb(struct nfa *a , struct nfa *b);
void init_bool_aorb(struct nfa *c, struct nfa *a ,struct nfa *b);

//NFA
void nfa_display( struct nfa *a);
void nfa_delete(struct nfa *a );
struct nfa *create_table(int rows,int columns);
void init_bool(struct nfa *c, struct nfa *a ,struct nfa *b);
void merge_column(struct nfa *c1, struct nfa *a, struct nfa *b, int c , int d, int e,string symbol);
void merge_column_distinct(struct nfa *c, struct nfa *a , struct nfa *b, int d,  int e,string  a1,int up );

//NFA To DFA
struct dfa* nfa_to_dfa(struct nfa* c);
set<string> E_closure(set<string> T ,struct nfa* c );
set<string> move(set<string> S ,char next_char, struct nfa* c );

// Useful functions
string int_str(int a);
int find_symbol(struct nfa *a, string b);
int find_symbol_char(struct nfa *a, char b);
int alphabet_size(struct nfa *a, struct nfa *b);
string find_final_state_num(struct nfa* c);
int find_state(struct nfa * c , string state);
int find_start_state(struct nfa* c);
void print_symbol_table();
int find_final_state(struct nfa* c);
string char_to_string(char a);
void print(std::set<string> set);
void display_dfa(struct dfa *a);

//DFA States
struct dfa *create_table_dfa(int rows,int columns);
void dfa_set_bool(struct dfa *d, string a,int dfa_rows);
void dfa_insert_states(struct dfa *d, set <string> s, int i);
void dfa_delete(struct dfa *a ,int start,int rows, int columns);
void dfa_insert_transitions(struct dfa *d, set <string> u,int col,int dfa_row);

//Simulation
FILE *input;					//Input file pointer input_1 file
char nextchar();				//Next char to be simulated

//Dfa Simulation
string dfa_transition(struct dfa *df, string cur_state, string input);
int dfa_start_state(struct dfa *df);
int dfa_isaccept_state(struct dfa *df,string state );
void dfa_simulate(struct dfa *df);
void token_class(string cur_state , string buffer);

//REGEX parsing
int regex_pos;					//
int line_pos;					//
struct nfa* total_nfa;			// Final NFA
int getlines(string);			//
char global_char;				// 


/*
 * Main Function
 */
int main(int argc , char *argv[])
{
	struct nfa* final_nfa;
	string reg=argv[2];						//Grammer file	
	
	int x=getlines(reg);					//Number of lines in the Grammer file
	ifstream ifile("id");					// Temp file storing only regular expressions	
	string istr;							// Temp variable
	
	string filename = argv[1];
	
	//Combining nfa of all the token classes	
	for(line_pos=0;line_pos<x;line_pos++)
	{	
		if(line_pos==0)
		{
			final_nfa = expression();
			ifile>>istr;
			mymap[int_str(counter-1)]=istr;
		}
		else
		{
			struct nfa *arg2 = expression();
			ifile>>istr;
			mymap[int_str(counter-1)]=istr;
			final_nfa = create_aorb(final_nfa,arg2);
		}
	}
	//nfa_display(final_nfa);
	input =fopen(filename.c_str(),"r");
	struct dfa *star = nfa_to_dfa(final_nfa);
	//display_dfa(star);
	
	dfa_simulate(star);
	print_symbol_table();
	
	cout<< "States of DFA"<< star->rows<<endl;
}
/*
 * Returns number of lines in token_class in grammar_1
 */
int getlines(string file)
{
	FILE* fp=fopen(file.c_str(),"r");
   int x=0;
   static char input_buffe[1000];
   string out;
	ofstream ofile("reg_lex");
	ofstream fil("id");
	while(fgets(input_buffe,1000,fp))
	{
		x++;
		string state;
		string temp = input_buffe;
		stringstream stream(temp);
		while( getline(stream, state, '\t') )
		{
			ofile<<state<<endl;
			getline(stream, state, '\t');
			fil<<state<<endl;
		}
	}
	fclose(fp);
	return x;
}

/*
 * Parses the Regular Expression
 */
int lex(void){
	FILE *fp;
	fp = fopen("reg_lex","r");
    static char input_buffer[1024];
    char        *current;

   current = yytext + yyleng; //Skip current lexeme      

	while(!*current ){
    	  current = input_buffer;
		 if(!gets(input_buffer))
		 {
			 *current = '\0' ;
			return EOI;
		 }
         ++yylineno;
	 }
      for(; *current; ++current)
      {
         /* Get the next token */
         yytext = current;
         yyleng = 1;
         switch( *current ){
           case '*':
	     		return STAR;
           case '(':
         	   return LP;
           case ')':
         	   return RP;
		   case '|':
			    return OR;
		   case '.':
			    return AND;
		   case '-':
				return DASH;
           default:
           {
               if(isalnum(*current))
               {
                  ++current;
                  yyleng = current - yytext;
			   }
               else if(*current=='\\')
               {
				   ++current;
				   if(*current=='n')
				   {
					   ++current;
					   yyleng = current - yytext;
					   *yytext='\n';
				   }
				   else if(*current=='t')
				   {
					   ++current;
					   yyleng = current - yytext;
					   *yytext='\t';
				   }
				   else if(*current=='.')
				   {
					   ++current;
					   yyleng = current - yytext;
					   *yytext='.';
				   }
				   else if(*current=='(')
				   {
					   ++current;
					   yyleng = current - yytext;
					   *yytext='(';
				   }
				   else if(*current==')')
				   {
					   ++current;
					   yyleng = current - yytext;
					   *yytext=')';
				   }
				   else if(*current=='|')
				   {
					   ++current;
					   yyleng = current - yytext;
					   *yytext='|';
				   }
				   else if(*current=='*')
				   {
					   ++current;
					   yyleng = current - yytext;
					   *yytext='*';
				   }
				   else if(*current == '-')
				   {
					   ++current;
					   yyleng = current - yytext;
					   *yytext='-';
				   }
			   }
			   else if(*current==' ')
			   {
				   ++current;
				   *yytext=' ';
				   yyleng = current - yytext;
			   }
			   return NUM_OR_ID;
            }
            break;
         }
      }
}

static int Lookahead = -1; // Lookahead-Stores the type of the nextchar in the parsing of the grammar_1 file

/*
 * Return true if "token" matches the current lookahead symbol.                
 */
int match(int token){
   if(Lookahead == -1)
      Lookahead = lex();
   return token == Lookahead;
}

/*
 *Advance the lookahead to the next input symbol.      
 */
void advance(void){
    Lookahead = lex();
}

/*
 * Least precedence to | operator 
 */
struct nfa* expression()
{
     //expression  -> expression or term	|term 
    struct nfa* or_nfa;
	struct nfa* temp=  term();
	or_nfa = temp; 
 
    while( match( OR ) || match(DASH) )
    {
		if(match(OR))
		{
			advance();
			or_nfa = create_aorb(or_nfa,term());
		}
		else if(match(DASH))
		{
			advance();
			int strt=global_char+1;
			int end=yytext[0];
			for(int i=strt;i<end;i++)
			{
				char tmp=i;
				struct nfa* d_temp;
				d_temp = create_a(char_to_string(tmp));
				or_nfa = create_aorb(or_nfa,d_temp);
			}
			or_nfa = create_aorb(or_nfa,term());
		}
    }
    return or_nfa;
}

/*
 * Next precedence to AND(.) operator 
 */
struct nfa*    term()
{
	//term ----> term.factor	|	factor
	struct nfa* and_nfa;
	struct nfa* temp=  factor();
	and_nfa = temp;	
    while( match( AND ) )
    {		
        advance();
        and_nfa = create_ab(and_nfa,factor());
    }
    return and_nfa;
}

/*
 * Next precedence to STAR(*) operator 
 */
struct nfa* factor()
{
	//factor------->	factor STAR par	|	par
	struct nfa* star_nfa;
	struct nfa* temp= par();
	star_nfa = temp;
	while(match(STAR))
	{
		star_nfa = create_astar(temp);
		advance();
	}
	return star_nfa;
}

/*
 * Highest precedence to ( ) 
 */
struct nfa*    par()
{
	//par ------>	CHAR |	(expression)
struct nfa* single_char;
    if( match(NUM_OR_ID) )
   {
	    char temp[2];
		temp[0] = yytext[0];
		temp[1] = '\0';
		global_char=yytext[0];
		string buff = temp;
        single_char = create_a(buff);
        advance();
   }
    else if( match(LP) )
    {
        advance();
        single_char = expression();
        if( match(RP) )
            advance();
        else
            fprintf( stderr, "%d: Mismatched parenthesis\n", yylineno );
    }
    else
	fprintf( stderr, "%d: Number or identifier expected\n", yylineno );
	
	return single_char;
}

/*
 * Prints elements of a set
 */
void print(std::set<string> set)
{
	std::set<string>::iterator it;
	
	for(it=set.begin(); it !=set.end(); ++it)
		{
				cout<<"*"<<*it<<"*";
		}
}

/*
 * Returns the next input character by reading the file
 */
char nextchar()
{
	char c;
	c=fgetc(input);
	return c;
}

/*
 * NFA to DFA Conversion
 */
struct dfa* nfa_to_dfa(struct nfa* c)
{
	int dfa_rows = 1;				//Counter for the rows of dfa 
	struct dfa *df;
	set<string>::iterator it;
	int i=2;
	
    string c_start_state = c->table[find_start_state(c)][1];
    string c_final_state = c->table[find_final_state(c)][1];
	
	//Creating table It sets States and Bool
	
	df = create_table_dfa(1000, c->columns-1);				// Maximum states the DFA can have is 1000
	
	// Assigning rows and columns
	df->columns = c->columns-1;
	df->rows = pow(2,c->rows -1) +1 ;
	
	//Setting the alphabets
	for(it = c->alphabets.begin(); it!= c->alphabets.end();++it)
	{
		df->table[0][i] = *it;
		i++;
	}
	
	set < set<string> > marked;				//Contains all marked sets
	set < set<string> > unmarked;			//Contains all unmarked sets
	
	// Start Sate E- Closure
	set<string> start_set,u;
	set<string> t;
	int start_state_row = find_start_state(c);
	start_set.insert(c->table[start_state_row][1]);
	
	start_set = E_closure(start_set,c);
	
	//Inserting the first set in unmarked set
	unmarked.insert(start_set);
	
	while(unmarked.size()!=0)
	{
		t = *( unmarked.begin());
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
}

/*
 * This function just inserts the set given in the next row in states in dfa table
 * Insert in ith row 
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
 * This function sets the bool states of Dfa
 */
void dfa_set_bool(struct dfa *d, string a,int dfa_rows)
{
		d->table[dfa_rows][0] = a;
}

/*
 * This function just inserts the set given in the next row in states in dfa table
 * Insert in ith row 
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
 * This function computes MOVE of states in S,  of the NFA c on input character next_char
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
				string state;
				stringstream stream(temp);
				 while( getline(stream, state, ',') )
				 {
						move_states.insert(state);					 
				 }				
		}
	
return move_states;
}

/* 
 * C-Char to C++ String Will not work for space
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
				row_no = find_state(c , *it);
				temp = c->table[row_no][c->columns-1];			
				string state;
				stringstream stream(temp);
				++it;
				 while( getline(stream, state, ',') )
				 {
					 if(!(T.find(state)!= T.end()))
					{
						T.insert(state);
						it=T.begin();
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
 *Returns the row_no of the state , given state number in string format 
 */
int find_state(struct nfa *c , string state)
{
	for(int i=0 ; i < c->rows ;i++)
	{
		if(c->table[i][1]==state)
		{
			return i;
		}
	}
return -1;
}

/*
 *This function returns the row_no of the start state of given nfa 
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
 *This function returns the row_no of the final state of given nfa 
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
 *This function returns the final state of given nfa 
 */
string find_final_state_num(struct nfa* c)
{
	for(int i=0 ; i<c->rows ;i ++)
	{
		if(c->table[i][0] == "2" )
		return c->table[i][1] ;
	}
}

/*
 * Creates the NFA for character b
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
		
		c->table[1][0] = "1";								//Bool Initial State
		c->table[1][1] = int_str(counter)	;				//State	
		c->table[1][2] = int_str(counter+1) + ",";			//
		c->table[1][3] = "";
		c->table[2][0] = "2";								// Bool Final State
		
		c->table[2][1] = int_str(counter+1);				//State
		c->table[2][2] = "";
		c->table[2][3] = "";
		counter += 2;

	return c;
}

/*
 * Takes one nfa a and makes nfa table for a*
 */
 struct nfa* create_astar(struct nfa *a)
{
		//Creating table of size alphabets +3  
		struct nfa *c = create_table(a->rows +2, a->alphabets.size() + 3);
		c->rows = a->rows +2;
		c->columns = a->alphabets.size() + 3;
		
		c->alphabets = a->alphabets;
		
		int third = counter;				// This is the third state
		int fourth = counter+1;				// This is the Fourth State
		counter += 2;
		
		int i1;								//i1= Initial state of a
		int  f1;							// f1 == Final state of a
		
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
		nfa_delete(a);
	return c; 
}

/*
 * This is a fucntion that takes one nfa
 * And it makes  nfa table for a|b
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
 * This is a fucntion that takes two nfa a and b 
 * and returns the combined nfa for and b 
 * That is it makes  nfa table for ab
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
 * Returns the size of the union of the alphabets
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
 * Deletes the space allocated to nfa
 * This is not good deallocator .. some errors are still there
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
 * This function deletes the space allocated to dfa from the starting 
 * start = starting row from where to deallocate
 * rows = number of rows to deallocate
 * columns = number of columns in the table
 * This also has some error
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
 * This function creates the Table of rows and columns and allocates
 * space for them
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
 * This function creates the Table of rows and columns and allocates
 * space for them
 */
struct dfa *create_table_dfa(int rows,int columns)
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
 * Find the index of the alphabet b in nfa Table of A
 * if not found returns -1
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
 * Returns the column number of the char in the NFA TABLE
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
 * Here the column c of a is merged with column d of b and put in colun e of c.
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
 * Epsilon Transitions
 * And sets the final state of a and initial stae of b as nothing 
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
 * This function initializes the column corresponding to bool column and
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
					c->table[i][0] = a->table[i][0];					//bool column
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
 * This is a function that returns the string of the int
 */
string int_str(int a)
{
	stringstream sstm;	
	sstm<< a;
	return sstm.str();
}

/*
 * Displays the NFA Table
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
 * Displays the DFA Table
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
 * Simulating input on DFA
 */
void dfa_simulate(struct dfa *df)
{
	char nc;															//next char
	string starting_state = df->table[dfa_start_state(df)][1];			// Starting state
	string cur_state = starting_state;									//Curent State set to start state
	string next_state;													//Next State
	std::set<string>::iterator it;
	
	string buffer="";
	
	while(1)
	{
			nc = nextchar();
			if(nc == -1)
			{
				if(dfa_isaccept_state(df,cur_state) == 1)
				{
					token_class(cur_state,buffer);
					
				}else
				{
					//Error
					cout<<"Error<<" << buffer<<endl;
				}
				break;
			}
			char temp[2];
			temp[0] = nc;
			temp[1] = '\0';
			string buff = temp;
			next_state = dfa_transition(df,cur_state,buff);
			
			if(dfa_isaccept_state(df,next_state)==1)
			{
					cur_state = next_state;
					buffer+= nc;
					continue;
			}else if(next_state != "")		
			{
					if(dfa_isaccept_state(df,cur_state)==1 )
					{
							// Here maximal munch 
							// If there is transition then take that transition
							cur_state = next_state;
							buffer+= nc;
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
							token_class(cur_state,buffer);
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
 * Find the corresponding token class and decide what is the output of lexical analysis
 * If class = ID : Insert in symbol table
 * If class = space : do nothing
 * */
void token_class(string cur_state , string buffer)
{
	string token_class="";
	string state;
	stringstream stream(cur_state);
	string minimum = "999999";
	
	while( getline(stream, state, ',') )
	{
		token_class = mymap[state];
		if(token_class != "")
		{
			
			if( atoi(state.c_str())< atoi(minimum.c_str()))
			{
				minimum = state;
				
			}
		}					 
	}
	
	token_class = mymap[minimum];
	//Printing the data
	if(token_class != "WS")
	{
		cout<< "< "<< token_class <<" , "<<buffer<<" >\n";
	}else
	{
		cout<< "< "<< token_class <<" , "<<" >\n";
	}

	map <string,int > :: iterator temp; 
	if(token_class == "ID")
	{
		
		if( (temp = id_map.find(buffer) ) != id_map.end()) // Entry is already there
		{
				symbol_table[temp->second].push_back(line_counter);	
		}else
		{
			id_map[buffer] = id_counter;
			symbol_table[id_counter].push_back(line_counter);	
			id_counter++;
		}
	}
	int enter = 0;
	// Increasing the line count number
	if( (enter = count(buffer.begin(), buffer.end(), '\n')) > 0)
	{
		line_counter += enter;
	}
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

/*
 * Prints the Symbol Table
 */
void print_symbol_table()
{
		map  <string,int> :: iterator it;
		vector <int> :: iterator it1;
		
		for(it = id_map.begin(); it!= id_map.end(); ++it)
		{
				cout<< it->first << "\t ";
				for(it1 = symbol_table[it->second].begin(); it1 != symbol_table[it->second].end(); ++it1)
				{
					cout<< *it1 <<"\t";
				}
				cout<<endl;
		}
}
