#include "cfg.h"

 vector<string> max_same_vector( vector<string> v1,  vector<string> v2);
 vector<string> max_prefix( multimap <string,vector<string> > p, string nt);
 vector<string>  after_prefix(vector<string> prefix, vector<string> total);
 bool match_prefix( vector<string> prefix , vector<string> p);
 bool production_exists(multimap <string,vector<string> > p , string nt, vector<string> v );

/*
 * Constructor
 * Initializes the N T P S
 */
cfg::cfg(string filename)
{
	left_counter = 1;
	left_recur = "A";
	string s;
	ifstream fil(filename.c_str());
	
	for(int i=0;i<3;i++)
	{
		getline(fil,s);
		fill(s);
	}
	
	while(fil.good())
	{
		getline(fil,s);
		if(s.length() == 0)
			break;
		fill_productions(s);
	}
	cout<<"********************************Initial Produtions*********************\n";
	print_map(productions);
	
	
	eliminate_left_recursuion();
	cout<<"\n\n********************************After Left Recursion: Produtions*********************\n";
	print_map(productions);
	
	
	left_factoring(productions);
	first_sets();	
	follow_sets();
	ll1_parsing_table();
	
	
	cout<<"\n******************************** Non Terminals***************************************\n";	
	print_set(non_terminals);
	
	cout<<"\n\n********************************Terminals******************************************\n";
	print_set(terminals);
	
	cout<<"\n\n********************************After Left Factoring: Final Produtions*************\n";
	print_map(productions);
	
	cout<<"\n\n********************************Printing First sets********************************\n";
	print_map_set(first_of);
		
	cout<<"\n\n********************************Printing Follow sets*******************************\n";
	print_map_set(follow_of);
	cout<<"\n\n********************************Printing LL1 TABLE*********************************\n";
	print_ll1_table(ll1_table);
	cout<<"\n**********************************Parsing Input**************************************\n";
	
	fil.close();
	
	symbols.push("$");
	symbols.push(start_symbol);
}

/*
 * Prints the set 's' of strings
 */
void cfg::print_set(set <string> s)
{
	for(set<string>::iterator it=s.begin(); it !=s.end(); ++it)
	{
		cout<<"*"<<*it<<"*"<<"\t";
	}
	cout<<endl;
}

/*
 * Parses the input file
 * 	map<string, multimap<string,vector <string> > > ll1_table;

 */

void cfg::parseinput(string lookahead)
{
	string top_symbol = symbols.top();
	cout<<"top symbol is   :"<<top_symbol<<":  lookahead   :"<<lookahead<<endl;
	if(top_symbol == "$")
	{
		if(lookahead == "$")
		{
			cout<<"****************Accepted ***************** \n";
			return;
		}
		else
		{
			cout<<"Error : Empty Stack :(\n";
			exit(1);
		}
	}
	multimap<string,vector<string> > temp_mul = ll1_table[top_symbol];
	
	if(top_symbol == lookahead)
	{
		cout<<"	\nmatch "<<lookahead<<endl;
		symbols.pop();
		return;
	}
	else if(terminals.find(top_symbol)!=terminals.end())
	{
		cout<<"\nError : At "<<top_symbol<<"\tlookahead\t"<<lookahead<<endl;
		return;
	}
	else if(temp_mul.find(lookahead) == temp_mul.end())
	{
		cout<<"\nError : Invalid production "<<"top symbol is \t"<<top_symbol<<"\t***"<<lookahead<<"***"<<endl;
		return;
	}
	else
	{
		vector<string> temp_production;
		int check = 0;
			for (multimap <string,vector<string> >::iterator it=temp_mul.find(lookahead); it!=temp_mul.end(); ++it)
			{
				if(lookahead == (*it).first)
				{
					check++;
					//if(check>1)
					//{
					//	cout<<"\n Warning : Reduce Reduce conflict detected, taking default action\n";
					//}
				}
				else
				{
					if(check == 1)
					break;
				}
			}
		
		temp_production = temp_mul.find(lookahead)->second;
		cout<<top_symbol<<" --> ";
		print_vector(temp_production);
		cout<<endl;
		symbols.pop();
		push_production(temp_production);
		parseinput(lookahead);
	}
}

/*
 * Pushes the right hand side of production (myvector)
 */
void cfg::push_production(vector<string> myvector)
{
	for(int i= myvector.size()-1;i>=0;i--)
	{
		if(myvector[i]!="@")
		symbols.push(myvector[i]);
	}
}


/*
 * Fills S(start symbol) and the set of T, N
*/
void cfg::fill(string str)
{
	string tok;				//String tokenization
	string type;				//Recognize N, T, S
	set<string> temp;			//Temporary set
	stringstream stream(str);
	getline(stream,type,':');
	
	while( getline(stream, tok, ' ') )
	{
			temp.insert(tok);	
	}

	if(type[0]=='N')
	{
		non_terminals = temp;
	}
	else if(type[0]=='T')
	{
		terminals=temp;
	}
	else if(type[0]=='S')
	{
		start_symbol = *temp.begin();
	}
}

/*
 * Fill P(productions) in map <string, vector <string>>
 */
void cfg::fill_productions(string str)
{
	
	string tok;				//String tokenization
	string type;				//The Head of the production
	int separator = count(str.begin(),str.end(),'|');		// Total 
	vector<string> myvector;		//Temporary vector
	
	type = str.substr(0,str.find(" -"));
	int prev = str.find(" - ") +3;
	int next = 0;
	string temp;
	
	
	for(int i = 0; i <= separator; i++)
	{
		if(i == separator)
		{
			temp = str.substr(prev);
			prev = str.length();		// Out of loop
		}
		else
		{
			
			next = str.find(" |",prev);
			temp = str.substr(prev,next-prev);
			prev = next + 3 ;
		}
		
		stringstream stream(temp);
		while( getline(stream, tok, ' ') )
		{
			if(tok.length() != 0 )
				myvector.push_back(tok);		
		}
		productions.insert( std::pair<string, vector <string> >(type, myvector));
		myvector.clear();	
	}
	
	
}

/*
 * Prints any general vector
 */
void cfg::print_vector(vector<string> myvector)
{
	for (vector<string>::iterator it = myvector.begin() ; it != myvector.end(); ++it)
		cout << ' ' << *it;
}



/*
 * Prints any map<string, vector <string> >
 */
void cfg::print_map(multimap<string, vector <string> > mymap)
{
	for (multimap <string,vector<string> >::iterator it=mymap.begin(); it!=mymap.end(); ++it)
	{
		cout << (*it).first << " => ";
		print_vector((*it).second); 
		cout<<"\n";
	}
	cout<<"\n";
}

/*
 * Stores Union of set s1 and s2 in s3
 */
set<string> cfg:: union_set(set<string> s1, set<string> s2 ,int *chg )
{
	set<string> myset;
	for(set<string>::iterator it=s1.begin(); it !=s1.end(); ++it)
	{
		myset.insert(*it);
	}
	for(set<string>::iterator it=s2.begin(); it !=s2.end(); ++it)
	{
		myset.insert(*it);
	}
	if(s1.size()!=myset.size())
		*chg=1;
	return myset;
}

/*
 * Prints any map<string,set <string> >
 */
 
void cfg::print_map_set(map<string, set <string> > mymap)
{
	for (map <string,set<string> >::iterator it=mymap.begin(); it!=mymap.end(); ++it)
	{
		cout << it->first << " => ";
		print_set(it->second); 
		cout<<"\n";
	}

}
/*
 *Fill first_of of grammar symbols, in map<string,set <string> > first_of
 https://docs.google.com/viewer?a=v&q=cache:x7bOnwWfZk0J:www.clear.rice.edu/comp412/Lectures/L12TopDownII-2up.pdf+&hl=en&gl=in&pid=bl&srcid=ADGEEShqzd8ErmGFwhK6mdNOdho5uLDI7V-exuuNHCv-y3NIrKzcWRyej5w5UoF6Vabzaq9VPaG8X46wmGauCwz9JBqrcyRo6HIZGNftpxWkgpSOfHvd-JUt_tvrt2-QF5ZiXQDhJXNX&sig=AHIEtbQtsmiYJl7RI3gjdYQ16UIUY-4kLw
 */

void cfg:: first_sets()
{
	map <string,bool> nullable;
	int change=1;

	set<string> temp;
	for(set<string>::iterator it=terminals.begin(); it !=terminals.end(); ++it)
	{	
		temp.insert(*it);							
		first_of[*it] = temp;	
		temp.clear();
	}
	
	for (multimap <string,vector<string> >::iterator it=productions.begin(); it!=productions.end(); ++it)
	{	
		if( find ( ((*it).second).begin(), ((*it).second).end(), "@") != ((*it).second).end())
		{
			
			nullable[(*it).first] = true;
			
		}
	}
	int i=0;
	while(i<500 && change==1)
	{
		change=0;
		i++;
		for (multimap <string,vector<string> >::iterator it=productions.begin(); it!=productions.end(); ++it)
		{	
			vector<string> myvector = (*it).second;		//right hand side of the production
			set<string> rhs;
			if( find ( ((*it).second).begin(), ((*it).second).end(), "@") != ((*it).second).end())
			{
				first_of[(*it).first].insert("@");
				continue;			
			}
			else
			{
				int vect_size = myvector.size();
				int cur=0;
				for (vector<string>::iterator v1 = myvector.begin() ; v1 != myvector.end(); ++v1)
				{
					cur++;
					int thuss;
					rhs = union_set(rhs,first_of[*v1],&thuss);
					set<string>::iterator set_it;
					set_it=rhs.find("@");
					if(set_it!=rhs.end())
					{
						if(cur!=vect_size)
							rhs.erase(set_it);
						continue;
					}
					else
						break;
				}
				first_of[(*it).first] = union_set(first_of[(*it).first],rhs,&change);
			}
		}
	}
}


/*
 *Fill follow_of of grammar symbols, in map<string,set <string> > follow_of
 */ 
void cfg:: follow_sets()
{
	int change=1;
	follow_of[start_symbol].insert("$");
	while(change==1)
	{
		change=0;
		for (multimap <string,vector<string> >::iterator it=productions.begin(); it!=productions.end(); ++it)
		{	
			
			vector<string> myvector = (*it).second;		//right hand side of the production
			set<string> trailer = follow_of[(*it).first];
			for(int i= myvector.size()-1;i>=0;i--)
			{
				
				if(non_terminals.find(myvector[i]) != non_terminals.end())
				{
					follow_of[myvector[i]] = union_set(follow_of[myvector[i]], trailer, &change);
					
					if(first_of[myvector[i]].find("@") != first_of[myvector[i]].end())
					{
						int thuss = 0;
						set<string> buffer = first_of[myvector[i]];
						set<string>::iterator set_it;
						set_it=buffer.find("@");
						if(set_it!=buffer.end())
						{
							buffer.erase(set_it);
						}
						trailer = union_set( trailer ,buffer, &thuss);
					}
					else
					{
							trailer = first_of[myvector[i]];
					}
				}
				else
				{
					trailer.clear();
					trailer.insert(myvector[i]);
				}
				
			}
		}
	}

}


/*Prints the LL1 table
 * map<string, multimap<string,vector <string> > > ll1_table;
 */
void cfg:: print_ll1_table(map<string, multimap<string,vector <string> > > mymap)
{
	for (map <string, multimap<string,vector<string> > >::iterator it=mymap.begin(); it!=mymap.end(); ++it)
	{
		cout << (*it).first << " =>* \n";
		print_map((*it).second); 
		cout<<"\n";
	}
	cout<<"\n";
}


/*
 * Fills the predictive LL1 parsing table
 * map<string, multimap<string,vector <string> > > ll1_table;
 */

void cfg:: ll1_parsing_table()
{

	for (multimap <string,vector<string> >::iterator it=productions.begin(); it!=productions.end(); ++it)
		{
			
			//first step
			set<string> temp = first_of_string((*it).second);
			
			multimap< string, vector <string> > buff = ll1_table[(*it).first];
			for(set<string>::iterator set_it=temp.begin(); set_it !=temp.end(); ++set_it)
			{
				buff.insert(std::pair<string,vector<string> > ( *set_it,(*it).second));
			}
			
			ll1_table[(*it).first] = buff;
			
			
			//second step
			if(temp.find("@") != temp.end())
			{
					set<string> follow_a = follow_of[(*it).first];
					multimap< string, vector <string> > buff = ll1_table[(*it).first];
					for(set<string>::iterator set_it=follow_a.begin(); set_it !=follow_a.end(); ++set_it)
					{
					
						buff.insert(std::pair<string,vector<string> > ( *set_it,(*it).second));
					}
					
					ll1_table[(*it).first] = buff;
					
			}
			
		}
	
}

/*
 * Finds the first_of set for a string of terminals and non-terminals
 */
set<string> cfg:: first_of_string(vector <string> myvector)
{
	set<string> temp;
	for (vector<string>::iterator v1 = myvector.begin() ; v1 != myvector.end(); ++v1)
	{
		int thuss;
		if(first_of[*v1].find("@") == first_of[*v1].end())			//If @ is not found, then break the loop
		{		
				temp = union_set(temp,first_of[*v1], &thuss);
				break;
		}	
		temp = union_set(temp,first_of[*v1], &thuss);
	}
	
	return temp;
}



/*
 * Eliminates the left recursion
 */
void cfg::eliminate_left_recursuion()
{
	string temp ;
	
	int current_left_counter = left_counter;  // Current non terminal to be added in the grammar dynamically
	for(set<string>::iterator i=non_terminals.begin(); i !=non_terminals.end(); ++i)
	{	
		
		for(set<string>::iterator j=non_terminals.begin(); j != i; ++j)
		{
			multimap <string,vector<string> > rough;	
			rough.clear();
		
			for (multimap <string,vector<string> > :: iterator k = productions.begin(); k!=productions.end() ; ++k)
			{
					
					if((*k).first == *i && ((*k).second)[0] == *j)
					{
						
						vector <string> myvector = (*k).second; 
						myvector.erase(myvector.begin());			//Removed the first symbol
					
						for (multimap <string,vector<string> >::iterator l = productions.begin(); l!=productions.end(); ++l)
						{
							
							vector <string> finalr;
							if( (*l).first == *j)
							{
									if( ((*l).second)[0] != "@")
										finalr = (*l).second;
									
									finalr.insert(finalr.end(),myvector.begin(),myvector.end());
									if(finalr.size()!=0)
									{
										if(!production_exists(rough,*i, finalr) && finalr.size()!=0 )
											rough.insert( pair <string, vector <string> > (*i, finalr));
									}
							}
						}
						myvector.clear();
					}
			}//Productions loop ends

			bool change = true;
			while(change)
			{
				change = false;
				
				multimap <string,vector<string> > :: iterator k = productions.find(*i);
				while(k!= productions.end())
				{
					if( (*k).first == *i && k!= productions.end() && ((*k).second)[0] == *j)
					{
						cout<<"Erasing i " << *i<<" Erasing j "<<*j<<"\n";
						change = true;
						productions.erase(k);
					}
					++k;
				}
			}
			
			if(rough.size()!=0)
			{
				for (multimap <string,vector<string> >::iterator l=rough.begin(); l!=rough.end(); ++l)
				{
						if(!production_exists(productions,(*l).first, (*l).second) && ((*l).second).size()!=0)
							productions.insert( pair<string, vector <string> >((*l).first, (*l).second));
				}
				rough.clear();
			}
		}//j ends
		immediate_left_recursion(*i);
	}//i ends	

	int final_left_counter = left_counter;  // Current non terminal to be added in the grammar dynamically
			
	for(int i= current_left_counter; i< final_left_counter; i++)
	{
		stringstream ss;
		ss << i;//add number to the stream
		temp = left_recur + ss.str();
		non_terminals.insert(temp);
		ss.clear();
	}
}

/*
 * Removes immediate left recursion
 */
void cfg::immediate_left_recursion(string nt)
{

	multimap <string,vector<string> > ter, nonter;
	
	for (multimap <string,vector<string> >::iterator it=productions.begin(); it!=productions.end(); ++it)
	{
			if((*it).first == nt && (*it).second[0] == nt )
			{
				if(!production_exists(nonter , nt, (*it).second) && ((*it).second).size()!=0)
					nonter.insert( pair<string, vector <string> >(nt, (*it).second));
			}else if( (*it).first == nt && (*it).second[0] != nt)
			{
				if(!production_exists(ter , nt, (*it).second) && ((*it).second).size()!=0)
					ter.insert( pair<string, vector <string> >(nt, (*it).second));
			}
	}
	
	if(nonter.size()!=0)
	{
		//Erasing the productions of the form A-> AB or A-> ter
		bool change = true;
		while(change)
		{
			change = false;
			if(productions.find(nt) != productions.end())
			{
				change = true;
				productions.erase(productions.find(nt));
			}	
		}
		stringstream ss;									//create a stringstream
		ss << left_counter;								//add number to the stream
		left_counter++;
		string new_symbol = left_recur + ss.str();
		
		if(ter.size()!= 0)
		{
			for (multimap <string,vector<string> >::iterator it=ter.begin(); it!=ter.end(); ++it)
			{
				if( ((*it).second)[0] == "@")
					((*it).second).erase(((*it).second).begin());
			
				((*it).second).push_back(new_symbol);
				if(!production_exists(productions , nt, (*it).second) && ((*it).second).size()!=0  )
					productions.insert(pair<string, vector <string> >(nt, (*it).second));
			}
		}
	
		for (multimap <string,vector<string> >::iterator it=nonter.begin(); it!=nonter.end(); ++it)
		{
			((*it).second).erase(((*it).second).begin());
			((*it).second).push_back(new_symbol);
			
			if(!production_exists(productions , new_symbol, (*it).second) && ((*it).second).size()!=0 && new_symbol != ((*it).second)[0])
				productions.insert( pair<string, vector <string> >(new_symbol, (*it).second));
		}
		vector <string> temp_vec;
		temp_vec.push_back("@");
		
		if(!production_exists(productions , new_symbol, temp_vec) && temp_vec.size()!=0)
			productions.insert( pair<string, vector <string> >(new_symbol, temp_vec));
			
	}
	
}

/*
 * Left factoring
 */
void cfg::left_factoring(multimap <string,vector<string> > p)
{
	set <string> old_non_terminal = non_terminals;
	set <string> new_non_terminal = non_terminals;
	set <string> checked_non_terminals;
	
	multimap <string,vector<string> > old_production = p; 
	multimap <string,vector<string> > new_production;
	
	multimap <string,vector<string> > common;
	vector <string> prefix;
	vector<string> prev;
	
	bool change = true;
	
	
	while(change)
	{
		change = false;
		new_production.clear();
		checked_non_terminals.clear();
	
		for(set<string>::iterator i=old_non_terminal.begin(); i!=old_non_terminal.end(); ++i)
		{
			prefix.clear();
			prefix = max_prefix(old_production, *i);
			
			if(prefix.size() == 0)
			{
					for(multimap <string,vector<string> >::iterator j=old_production.begin(); j!=old_production.end(); ++j)
					{
							if((*j).first == *i && checked_non_terminals.find(*i) == checked_non_terminals.end())
							{
									if(!production_exists(new_production , (*j).first, (*j).second ) &&  ((*j).second).size()!=0 )
										new_production.insert(pair<string, vector <string> >((*j).first, (*j).second));
							}
					}
					continue;
			}
			else 
			{
				change = true;
				common.clear();
				for(multimap <string,vector<string> >::iterator j= old_production.begin(); j!= old_production.end(); ++j)
				{
					if( (*j).first == *i  && match_prefix(prefix, (*j).second))
					{
							if(!production_exists(common , (*j).first, (*j).second ) && ((*j).second).size()!=0)
								common.insert(pair<string, vector <string> >((*j).first, (*j).second ));
					}else if((*j).first == *i)// all else in result
					{
							if(!production_exists(new_production , (*j).first, (*j).second ) && ((*j).second).size()!=0 && checked_non_terminals.find( (*j).first) == checked_non_terminals.end())
								new_production.insert(pair<string, vector <string> >((*j).first, (*j).second ));
					}
				}
				// Now to handle the common
				vector <string> temp;
				string n_symbol = next_symbol();
				
				//Pushing 	A-> prefixA'
				temp.insert(temp.end(), prefix.begin(), prefix.end());
				temp.push_back(n_symbol);
				
				
				if(!production_exists(new_production , *i, temp ) && temp.size()!=0)
					new_production.insert(pair<string, vector <string> >(*i, temp ));
				
				for(multimap <string,vector<string> >::iterator j= common.begin(); j!= common.end(); ++j)
				{
					temp.clear();
					temp = after_prefix(prefix, (*j).second);
					
					if(!production_exists(new_production , n_symbol, temp) && temp.size()!=0)
						new_production.insert(pair<string, vector <string> >(n_symbol, temp ));
					else if(temp.size()==0)
					{
						temp.push_back("@");
						new_production.insert(pair<string, vector <string> >(n_symbol, temp ));
					}
				}
				new_non_terminal.insert(n_symbol);
			}
			
			checked_non_terminals.insert(*i);
			
		}//for loop ends
		
		old_non_terminal = new_non_terminal;
		old_production = new_production;
	}// While loop ends
	productions = new_production;
	non_terminals = new_non_terminal;
	
}
/*
 * This sends the vector of the maximal length same in 
 * the given two vectors
 */
 vector<string> max_same_vector( vector<string> v1,  vector<string> v2)
 {
	 
 	vector<string> prefix;
	
	int size = v1.size() < v2.size() ? v1.size() : v2.size();
 	for(int i=0; i <size; i++)
 	{
 		if(v1[i] == v2[i])
 		{
 			prefix.push_back(v1[i]);
 		}else
 		{
 			break;
 		}
 	}
 	return prefix;
 }

/*
 * Sends the maximum prefix array in the multimap corresponding to 
 * the non terminal string
 */
 vector<string> max_prefix( multimap <string,vector<string> > p, string nt)
 {
 	vector<string> prefix;
	vector<string> prev;
	for(multimap <string,vector<string> >::iterator i=p.begin(); i!=p.end(); ++i)
	{
		if((*i).first!= nt)
		continue;
		
		for(multimap <string,vector<string> >::iterator j=i; j!=p.end(); ++j)
		{
			if((*j).first != nt || i==j)
				continue;		
			prev = max_same_vector((*i).second,(*j).second);
			if(prev.size() > prefix.size())
				prefix = prev;
		}
	}
 	return prefix;
 }

/* 
 * Returns the next symbol
 * Does not inserts in the Non terminals table
 */
string cfg::next_symbol()
{
	stringstream ss;
	ss << left_counter;//add number to the stream
	left_counter++;
	string temp = left_recur + ss.str();
	return temp;
}
/* 
 * Returns the vector of string after the prefix
 */
vector<string>  after_prefix(vector<string> prefix, vector<string> total)
{
	vector<string> after;
	vector<string> :: iterator  t1 = total.begin();
	
	for(vector<string> :: iterator t= prefix.begin();t!= prefix.end() ; t++ )
	{
		++t1;
	}
	
	after.insert(after.begin(), t1 ,  total.end());
	return after;
}

/*
 * Returns true if prefix is prefix of the other vector
 */
bool match_prefix( vector<string> prefix , vector<string> p)
 {
 	
 	if(p.size()< prefix.size())
		return false;
		
 	for(int i=0 ; i < prefix.size(); i++)
 	{
		if(prefix[i] != p[i])
			return false;
	}
 	return true;
 }
 
 /*
  * Sends true if the nt and the exact vector is found in the productions
  */
 bool production_exists(multimap <string,vector<string> > p , string nt, vector<string> v )
 {
	for(multimap <string,vector<string> >::iterator i=p.begin(); i!=p.end(); ++i)
	{	
		if( (*i).first == nt && ((*i).second).size() == v.size() && match_prefix(v,(*i).second) )
			return true;
	}
	return false;
 }

