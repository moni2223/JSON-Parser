#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
using namespace std;

string convertKeyToString(char* a, int size)
{
	int i;
	string s = "";
	int firstdeli = 0;
	int seconddeli = 0;
	for (i = 0; i < size; i++) {
		if (a[i] >= 'A' && a[i] <= 'Z' || a[i] >= 'a' && a[i] <= 'z')
			s = s + a[i];
		
		else if (a[i] == '"' && firstdeli == 0)firstdeli = i;
		else if (a[i] == '"' && firstdeli != 0 && seconddeli == 0)seconddeli = i;
		else if (a[i] == '"' && firstdeli != 0 && seconddeli != 0)break;
		else continue;
	}
	return s;
}
string convertToString(char* a,int size)
{
	string s = "";
	for (int i = 0; i < size; i++) {
			s = s + a[i];
	}
	return s;
}
string SettinEmptyCharforPartofString(string s) 
{
	for (int i = 0; i < s.length(); i++)
	{
		if (s[i] >= 0 && s[i] <= 127)continue;
		else s[i] = '\0';
	}
	return s;
}

string input = "";
string searchedkey = "";
string searchedvalue = "";
string newinput = "";
string newinput1= "";
unsigned int swaplevel1 = 0;
unsigned int swaplevel2 = 0;
vector<string> searchedobj;
vector<string> searchedobjchildren;
int brackets = 0;


struct JSONonj 
{
	string key;
	string value;
	JSONonj& operator =(const int& a)
	{
	    key = a;
		return *this;
	}
	JSONonj(string key,string value):key(key),value(value){}


};

template <typename T>
struct FullNode
{
	T object;	
	vector<FullNode<T>*> children;

	FullNode(T object) : object(object) {}

};

template <typename T>
void print(FullNode<T>* root,int spaces)
{
	switch (spaces) 
	{
	case 0:cout << root->object.key << "->" << root->object.value << endl; break;
	case 1:cout <<"->  "<< root->object.key << "->" << root->object.value << endl; break;
	case 2:cout <<"->     "<< root->object.key << "->" << root->object.value << endl; break;
	case 3:cout << "->        " << root->object.key << "->" << root->object.value << endl; break;
	}

	for (int i = 0; i < root->children.size(); i++)
		print(root->children[i],spaces+1);
}

template<typename T>
void printonfile(FullNode<T>* root, int spaces, ofstream& infile)
{
	switch (spaces)
	{
	case 0:infile << root->object.key << ": " << root->object.value <<"\n"; break;
	case 1:infile << "  " << root->object.key << ": " << root->object.value << "\n"; break;
	case 2:infile << "     " << root->object.key << ": " << root->object.value << "\n"; break;
	case 3:infile << "        " << root->object.key << ": " << root->object.value << "\n"; break;
	}
	brackets = spaces;
	for (int i = 0; i < root->children.size(); i++)
	{
		printonfile(root->children[i], spaces + 1, infile);
	}
}

template <typename T>
void free(FullNode<T>* root)
{
	for (int i = 0; i < root->children.size(); i++)
		free(root->children[i]);

	delete root;
}

template <typename T>
bool contains(FullNode<T>* root)
{
	int size = root->children.size();

	if (root->object.key == input)
	{
		searchedobj.push_back(root->object.key);
		searchedobj.push_back(root->object.value);
		for (int i = 0; i < root->children.size(); i++)
		{
			searchedobjchildren.push_back(root->children[i]->object.key);
			searchedobjchildren.push_back(root->children[i]->object.value);
		}
		return 1;
	}

	for (int i = 0; i < size; i++)
	{
		if (contains(root->children[i]))
		{
			searchedobj.push_back((root->children[i])->object.key);
			searchedobj.push_back((root->children[i])->object.value);

			for (int j = 0; j < (root->children[i]->children).size(); j++)
			{
				searchedobjchildren.push_back((root->children[i])->children[j]->object.key);
				searchedobjchildren.push_back((root->children[i])->children[j]->object.key);
			}
			return 1;
		}
	}
	return 0;
}

template <typename T>
bool change(FullNode<T>* root) 
{
	string oldkey = "";
	string oldvalue="";
	
	string newkey = "";
	string newvalue = "";

	for (int i = 0; i < input.length(); i++)
	{
		if (input[i] != '/')oldkey += input[i];
		else
		{
			for (int j = i + 1; j < input.length(); j++)
				oldvalue += input[j];
			break;
		}
	}

	for (int i = 0; i < newinput.length(); i++)
	{
		if (newinput[i] != '/')newkey += newinput[i];
		else 
		{
			for (int j = i+1; j < newinput.length(); j++)
				newvalue += newinput[j];
			break;
		}
	}

	int size = root->children.size();
	if (root->object.key == oldkey)
	{
		root->object.key = newkey;
		root->object.value = newvalue;
		cout << "Changed successfully" << endl;
		return 1;
	}
	for (int i = 0; i < size; i++)
	{
		if (change(root->children[i]))
		{
			(root->children[i])->object.key = newkey;
			(root->children[i])->object.value = newvalue;
			cout << "Changed successfully" << endl;
			return 1;
		}
	}
	return 0;
}

template<typename T>
bool remove(FullNode<T>* root)
{
	int size = root->children.size();

	if (root->object.key == input)
	{
		return 1;
	}
	for (int i = 0; i < size; i++)
	{
		if (remove(root->children[i])) 
		{
			(root->children[i])->object.key = "";
			(root->children[i])->object.value = "";
			(root->children[i])->children.clear();
			cout << "Removed successfully" << endl;
			return 1;
		}
	}
	return 0;
}

template<typename T>
bool getlevelofobject(FullNode<T>* root) 
{
	int size = root->children.size();
	if (root->object.key == newinput)
	{
		return 1;
	}
	for (int i = 0; i < size; i++)
	{
		if (swap(root->children[i])) 
		{
			swaplevel1++;
			return 1;
		}
	}
}

template<typename T>
bool createobject(FullNode<T>* root) 
{
	int size = root->children.size();

	if (root->object.key == input)
	{
		return 1;
	}
	for (int i = 0; i < size; i++)
	{
		if (createobject(root->children[i])) 
		{
			root->children.push_back(new FullNode<JSONonj>({ newinput,newinput1 }));
			return 1;
		}
	}
	return 0;
}


//Global variables
FullNode<JSONonj>* root = new FullNode<JSONonj>({ "Unknown","Unknown"});
static int currentlevel = -1;
int parentpos = -1;


void RootLevel(string key1, string value1)
{
	root->object.key = key1;
	root->object.value = value1;
}
void NextLevel(string key1, string value1, int pos) 
{
	if (currentlevel >= 2) 
	{
		(root->children[parentpos]->children).push_back(new FullNode<JSONonj>({ key1,value1 }));
	}
	else 
	{
		(root->children).push_back(new FullNode<JSONonj>({ key1,value1 }));
		parentpos++;
	} 
}

bool readingfromJsonFile(bool read)
{

	char filename[] = "example.json";
	ifstream file(filename);
	if (file.fail())
	{
		cout << "Error occured while opening the file." << endl;
		return 0;
	}
	
    int countofopenbrac = 0;
	int countofclosedbrac = 0;
	int opened = 0;

	while (!file.eof())
	{
		char buff[1024];
		file.getline(buff, 1024);
		int tempdbrac = 0;
		int brac = 1;//count of "
		string temp;
		if (read == 0)//Validation 1 
		{
			for (int i = 0; i < strlen(buff); i++)
			{
				if (buff[i] == '{') 
					countofopenbrac++;

				if (buff[i] == '"')
					tempdbrac++;

				if (buff[i] == '}') 
				{
					countofclosedbrac++;
					if (buff[i + 1] != ',' && i!=strlen(buff)-1) 
					{
						cout << "Invalid ending of object" << endl;
						return 0;
					}
				}

				if (buff[i] == '"' && buff[i + 1] != ':' && tempdbrac == 1 && !isalpha(buff[i+1])) 
				{
					cout << "Wrong syntax when declaring key->value" << endl;
					return 0;
				}
			}
		}

		else //here we store them
		{
			if (opened == 0) 
			{
				cout << filename << " is opened." << endl;
				cout << endl;
				opened++;
			}

			if (buff[0] == '{')//the start of the JSON File 
				currentlevel++;

				int buff_size = sizeof(buff) / sizeof(char);
				string keyinput = convertKeyToString(buff, buff_size);//key
				//we need to extract value or go to next level
				//(convert buff to array and extract the value or the "{" for next level)
				string valueinput = convertToString(buff, buff_size);//value
				string inputvalue = "";
				int colon = valueinput.find(":");
				
				valueinput=SettinEmptyCharforPartofString(valueinput);

				for (int i = 0; i < strlen(buff); i++)//if we find }
				{
					if (buff[i] == '}')
					{
						currentlevel--;
						break;
					}
				}

				if (colon < 0)//when : is not found
					continue;

				valueinput = valueinput.substr(colon, valueinput.length());//here we take everything that is right from the colon

				for (int i = 0; i < valueinput.length(); i++)
				{
					if (valueinput[i] == '\"')
					{
						for (int j = i; j < valueinput.length(); j++)
						{
							if (isalpha(valueinput[j]) || valueinput[j]==' ' || valueinput[j] == '/' || valueinput[j] == '_')
								inputvalue += valueinput[j];
							
							if (valueinput[j] == '\0')break;
						}
						NextLevel(keyinput, inputvalue, 0);
						break;
					}
					else if (valueinput[i] == '{') 
					{
						if (currentlevel > 0 && root->object.key != "Unknown") //(1)
						{
							NextLevel(keyinput, inputvalue, 0);
							currentlevel++;
							break;
						}
						
					}
					else if(isdigit(valueinput[i]))
					{
						for (int j = i; j < valueinput.length(); j++)
						{
							if (isdigit(valueinput[j])) 
								inputvalue = inputvalue + valueinput[j];
							
							if (valueinput[j] == '\0')break;
						}
						NextLevel(keyinput, inputvalue, 0);
						break;
					}
					else if (valueinput[i] == '\0')break;
				}
				
				memset(buff, 0, 1024);//clearing the buff array
				
				if (root->object.key == "Unknown")//we check if this is the first object to be entered 
				{
					RootLevel(keyinput, inputvalue);
					currentlevel++;
					continue;
				}
				keyinput = "";
				inputvalue = "";
		}
	}

	if (countofopenbrac != countofclosedbrac) //Validation 2
	{
		cout << "Wrong syntax! Maybe you forgot to close some bracket?" << endl;
		return 0;
	}
	
	countofclosedbrac = countofopenbrac = 0;//free

	file.close();
	return 1;
}

void FoundObject(int state) 
{
	for (int i = 0; i < searchedobjchildren.size() / 2; i++)
	{
		searchedobjchildren.pop_back();
		searchedobjchildren.pop_back();
	}

	if (state == 1) 
	{
		searchedobj.pop_back();//deleting \n from the vector
		searchedobj.pop_back();
		cout << "It's ready" << endl;
		for (int i = 0; i < searchedobj.size()-1; i++)
		{
			cout << searchedobj.at(i) << "->";
		}
		cout << endl;
		cout << "And its children: " << endl;
		for (int i = 0; i < searchedobjchildren.size(); i++)
		{
			cout << searchedobjchildren.at(i) << "->";
		}
	}
	if (state == 2) {
		
		ofstream myfile;
		myfile.open("example2.json");
		myfile << "{ " << "\"" << searchedobj.at(0) << "\"" << ": ";
		if (searchedobj.at(1) == "")
		{
			myfile << "{" << endl;
			for (int i = 0; i < searchedobjchildren.size(); i += 2)
			{
				myfile << "   " << "\"" << searchedobjchildren.at(i) << "\"" << ": " << "\"" << searchedobjchildren.at(i + 1) << "\"" << endl;
			}
		}
		else myfile << "\"" << searchedobj.at(1) << "\"" << "," << endl;
	}
}

void Writeonfile()
{
	std::ofstream outfile(input);
	cout << "Do you want to save the whole JSONobj or only part of it(1 || 2)" << endl;
	cin >> newinput;
	if (newinput == "1")
	{
		outfile << "{";
		printonfile(root, 0, outfile);
		for (int i = 1; i < brackets; i++)
		{
			outfile << "}" << endl;
		}
	}

	else if (newinput == "2")
	{
		cout << "Input the object's key you want to write onto the file:" << endl;
		cin >> input;
		contains(root);
		outfile << "{" << searchedobj.at(0) << ":" << searchedobj.at(1) << "{" << "\n";
		for (int i = 0; i < searchedobjchildren.size() / 2; i += 2)
		{
			outfile << searchedobjchildren[i] << ":" << searchedobjchildren[i + 1] << ",\n";
		}
		outfile << "} }";
	}
}

void Helpmenu(int number) 
{
	switch (number)
	{
	case 1: {
		cout << "Opening file..." << endl;
		if (readingfromJsonFile(0) == 1)
		{
			readingfromJsonFile(1);
		} break;
	};
	case 2:print(root, 0); break;
	case 3: {cout << "Input the key you want to find: " << endl;
		cin >> input;
		if (contains(root) == 1)
	{
		int foundobj = 0;
		cout << "We have found what you asked for: " << endl;
		cout << "What do you want to do with the found object?(Type 1 for array, type 2 to write the data in new JSON file)" << endl;
		cin >> foundobj;
		FoundObject(foundobj);
	}
		  else
		cout << "We couldn't find anything. Are you sure that you wrote the correct key?" << endl;
	}; break;

	case 4: {cout << "Firstly input the old key/old value,and then the key/value you want to replace it with (Example debug/on then neshto/ne" << endl;
		cin >> input;
		cin >> newinput;
		if (change(root) == 0) cout << "No valid key or value." << endl;
		  else cout << "Changed successfully" << endl;
	}; break;
	case 5: {cout << "Firstly input the key to which the object will be created,then it's key and value" << endl;
		cin >> input;
		cin >> newinput;
		cin >> newinput1;
		if (createobject(root) == 0)cout << "Something went wrong.Maybe the old key is not in the file?" << endl;
		else cout<<"The object is created successfully" << endl;
}break;
	
	case 6: {cout << "Input the object's key you want to remove" << endl;
		cin >> input;
		remove(root); } break;
	case 7: {cout << "How do you want to name your file with the extension you want(Example newexample.json):" << endl;
		cin >> input;
		Writeonfile();
	}
	case 8:free(root); exit(0);
	default:break;
	}
}


int main()
{
	cout << "Welcome to our JSON Parser!" << endl;
	cout << "Choose any option from our menu *but first you should open file):" << endl;
	cout << "1. Open file" << endl;
	cout << "2. Printing the JSON file in read-friendly way." << endl;
	cout << "3. Search by giving key" << endl;
	cout << "4. Change by giving key" << endl;
	cout << "5. Create new object next to the given key" << endl;
	cout << "6. Delete any object by giving key" << endl;
	cout << "7. Save" << endl;
	cout << "8. Exit" << endl;
	cout << "Type the number of the function you want: ";

	int number = 0;
	cin >> number;
	Helpmenu(number);

    cout << endl;
	cout << "Type the number of the function you want: ";
	cin >> number;
	Helpmenu(number);
	cout << endl;
	cout << "Type the number of the function you want: ";
	cin >> number;
	Helpmenu(number);
	cout << endl;
	cout << "Type the number of the function you want: ";
	cin >> number;
	Helpmenu(number);
	cout << endl;
	cout << "Type the number of the function you want: ";
	cin >> number;
	Helpmenu(number);
	cout << endl;
	cout << "Type the number of the function you want: ";
	cin >> number;
	Helpmenu(number);
	cout << endl;
	return 0;
}

/*
Input example:
1                   
2
4
debug/on
neshto/novo
6
window
7
kaktoiskash.json
1
8
*/


/*
First we open file
Then print it
Then change object
Then delete object
Save the whole JSON object
Then exit the program
\*/