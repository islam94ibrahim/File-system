#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <time.h>
#include <algorithm>
#include <math.h> 
#include <map>
using namespace std;

void CreateIndexFileFile (char *filename, int numberOfRecords, int m);
int InsertNewRecordAtIndex (char *filename, int RecordID, int Reference);
void DeleteRecordFromIndex (char *filename, int RecordID);
void DisplayIndexFileContent (char *filename);
int SearchARecord (char *filename, int RecordID, int index);
int SearchNodeCapacity (char *filename,int index);
void sorting (char *filename,int index);
void InsertReArr (char *filename,int ParentIndex,int leafIndex,int RecordID, int Reference);
int numberOfRecords=10,m=3;
char arr[] = "Data.bin";
char* filename=arr;


int main(int argc, char** argv) {
	cout << "Chooser from the following:- ";
	cout << endl << "1- Create Index file" << endl << "2- Insert new record" << endl << "3- Delete a record" << endl
	<< "4- Search for a record" <<endl << "5- Display Index file content" <<endl << "6- Exit" <<endl << "Enter Choice: "; 
	int choice,RecordID;
	cin >> choice;
	while (choice != 6){
		if (choice==1){
			cout << "Enter number of records: ";
			cin >> numberOfRecords;
			cout << "Enter number of descendants: ";
			cin >> m;
			CreateIndexFileFile (filename, numberOfRecords, m);
			DisplayIndexFileContent (filename);
		}
		else if (choice==2){
			srand (time(NULL));
			int Reference=rand() % 100 + 1;
			cout << "Enter Record ID to insert: ";
			cin >>RecordID;
			int Insert;
			Insert=InsertNewRecordAtIndex (filename,RecordID, Reference);
			if (Insert == -1){
				cout << "No place to insert !!" << endl;
			}
			DisplayIndexFileContent (filename);
		}
		else if (choice==3){
		}
		else if (choice==4){
			DisplayIndexFileContent (filename);
			cout <<endl << "Enter Record ID to search for it: ";
			cin >> RecordID;
			int RecRef=SearchARecord (filename,RecordID,1);
			if (RecRef != -1){
				cout << "Record found .. Record reference= " << RecRef << endl;
			}
			else {
				cout << "Record not found !" << endl;
			}
		}
		else if (choice ==5){
			DisplayIndexFileContent (filename);
		}
		else {
			cout << "Wrong choice !!" << endl;
		}
		cout << endl << "1- Create Index file" << endl << "2- Insert new record" << endl << "3- Delete a record" << endl
		<< "4- Search for a record" <<endl << "5- Display Index file content" <<endl << "6- Exit" <<endl << "Enter Choice: "; 
		cin >> choice;
	}
	return 0;
}


void CreateIndexFileFile (char *filename, int numberOfRecords, int m){
	fstream file;
	file.open(filename,ios::binary|ios::out);
	for (int i=0;i<numberOfRecords;i++){
		int num1=1,num2=-1,num3=0,num4=i+1;
		if (i==0){
			file.write((char*)&(num2) ,sizeof(int));
			file.write((char*)&(num1) ,sizeof(int));
			file.write((char*)&(num2) ,sizeof(int));
			for (int i=0;i<(m*2)-2;i++){
				file.write((char*)&(num3) ,sizeof(int));
			}
		}
		else if (i>0 && i<numberOfRecords-1) {
			file.write((char*)&(num2) ,sizeof(int));
			file.write((char*)&(num4) ,sizeof(int));
			for (int i=0;i<(m*2)-1;i++){
				file.write((char*)&(num3) ,sizeof(int));
			}
		}
		else {
			file.write((char*)&(num2) ,sizeof(int));
			file.write((char*)&(num2) ,sizeof(int));
			for (int i=0;i<(m*2)-1;i++){
				file.write((char*)&(num3) ,sizeof(int));
			}
		
		}
	}
}

int InsertNewRecordAtIndex (char *filename, int RecordID, int Reference){
	int NextEmptyOnHeader,Root,NextEmpty,Zero=0,NodeCapacity,leafNode,Header;
	fstream file;
	file.open(filename,ios::binary|ios::in|ios::out);
	file.seekg(4, file.beg);
	file.read((char*)&Header,sizeof(int));
	if (Header == -1){
		return -1;
	}
	file.seekg(8, file.beg);
	file.read((char*)&Root,sizeof(int));
	if (Root==-1){
		file.seekg(4, file.beg);
		file.read((char*)&NextEmptyOnHeader ,sizeof(int));
		file.seekg(((NextEmptyOnHeader*((m*2)+1))*4)+4, file.beg);
		file.read((char*)&NextEmpty,sizeof(int));
		file.seekg(4, file.beg);
		file.write((char*)&(NextEmpty) ,sizeof(int));
		file.write((char*)&(NextEmptyOnHeader) ,sizeof(int));
		file.seekg((NextEmptyOnHeader*((m*2)+1)*4), file.beg);
		file.write((char*)&(Zero) ,sizeof(int));
		file.write((char*)&(RecordID) ,sizeof(int));
		file.write((char*)&(Reference) ,sizeof(int));
	}
	else {
		file.seekg((Root*((m*2)+1)*4), file.beg);
		file.read((char*)&leafNode,sizeof(int));
		if (leafNode==0){
			NodeCapacity=SearchNodeCapacity (filename,Root);
			if (NodeCapacity!=-1){
					file.seekg(NodeCapacity, file.beg);
					file.write((char*)&(RecordID) ,sizeof(int));
					file.write((char*)&(Reference) ,sizeof(int));
					file.close();
					sorting (filename, Root);
			}
			else {
				int NodeIDs [m+1];
				int b=4;
				for (int i=0;i<m;i++){
					file.seekg((Root*((m*2)+1)*4)+b, file.beg);
					file.read((char*)&NodeIDs[i],sizeof(int));
					b=b+8;
				}
			//	NodeIDs [m]=RecordID;
				sort(NodeIDs,NodeIDs+(m+1));
				map <int, int> newNode1;
				map <int, int> newNode2;
				file.seekg((Root*((m*2)+1)*4)+4, file.beg);
				int NodeRoot=Root;
				int TempRecID,TempRecRef;
				int placeInMap;
				for (int i=0;i<m;i++){
				//		cout << NodeIDs[i] << " " <<  RecordID << endl;
					if (NodeIDs[i] > RecordID){
						placeInMap=i;
						break;
					}
					if ( i==m-1 && placeInMap == 0){
						placeInMap=m;
						break;
					}
				}
				if (placeInMap < round(m/2.0)){
				for (int i=0;i<m-round(m/2.0);i++){
					file.read((char*)&TempRecID,sizeof(int));
					file.read((char*)&TempRecRef,sizeof(int));
					newNode1 [TempRecID]=TempRecRef;
				}
				for (int i=0;i<((round(m/2.0)));i++){
					file.read((char*)&TempRecID,sizeof(int));
					file.read((char*)&TempRecRef,sizeof(int));
					newNode2 [TempRecID]=TempRecRef;
				}
				newNode1 [RecordID]=Reference;
				file.seekg(4, file.beg);
				file.read((char*)&NextEmptyOnHeader ,sizeof(int));
				int smallNode=NextEmptyOnHeader;
				file.seekg(((NextEmptyOnHeader*((m*2)+1))*4), file.beg);
				file.write((char*)&(Zero) ,sizeof(int));
			//	file.seekg(((NextEmptyOnHeader*((m*2)+1))*4)+4, file.beg);
				file.read((char*)&NextEmpty ,sizeof(int));
				int largNode=NextEmpty;
				file.seekg(((NextEmptyOnHeader*((m*2)+1))*4)+4, file.beg);
				for (map<int, int>::iterator i = newNode1.begin(); i != newNode1.end(); i++){
					file.write((char*)&(i->first) ,sizeof(int));
    				file.write((char*)&(i->second) ,sizeof(int));
				}
				file.seekg(((NextEmpty*((m*2)+1))*4)+4, file.beg);
				file.read((char*)&NextEmptyOnHeader ,sizeof(int));
				file.seekg(4, file.beg);
				file.write((char*)&(NextEmptyOnHeader) ,sizeof(int));
				file.seekg(((NextEmpty*((m*2)+1))*4), file.beg);
				file.write((char*)&(Zero) ,sizeof(int));
				for (map<int, int>::iterator i = newNode2.begin(); i != newNode2.end(); i++){
					file.write((char*)&(i->first) ,sizeof(int));
    				file.write((char*)&(i->second) ,sizeof(int));
				}
				file.seekg((NodeRoot*((m*2)+1)*4), file.beg);
				int One=1;
				file.write((char*)&(One) ,sizeof(int));
				file.write((char*)&(newNode1.rbegin()->first) ,sizeof(int));
				file.write((char*)&(smallNode) ,sizeof(int));
				file.write((char*)&(newNode2.rbegin()->first) ,sizeof(int));
				file.write((char*)&(largNode) ,sizeof(int));
			//	cout << (file.tellg()/4) << " " << (((NodeRoot+1)*((m*2)+1)*4))/4 << endl;
				for (int i=file.tellg() ; i< ((NodeRoot+1)*((m*2)+1)*4)+1; i=i+4){
					file.write((char*)&(Zero) ,sizeof(int));
				}	
				}
				else {
					for (int i=0;i<round(m/2.0);i++){
					file.read((char*)&TempRecID,sizeof(int));
					file.read((char*)&TempRecRef,sizeof(int));
					newNode1 [TempRecID]=TempRecRef;
				}
				for (int i=0;i<(m-(round(m/2.0)));i++){
					file.read((char*)&TempRecID,sizeof(int));
					file.read((char*)&TempRecRef,sizeof(int));
					newNode2 [TempRecID]=TempRecRef;
				}
				newNode2 [RecordID]=Reference;
				file.seekg(4, file.beg);
				file.read((char*)&NextEmptyOnHeader ,sizeof(int));
				int smallNode=NextEmptyOnHeader;
				file.seekg(((NextEmptyOnHeader*((m*2)+1))*4), file.beg);
				file.write((char*)&(Zero) ,sizeof(int));
			//	file.seekg(((NextEmptyOnHeader*((m*2)+1))*4)+4, file.beg);
				file.read((char*)&NextEmpty ,sizeof(int));
				int largNode=NextEmpty;
				file.seekg(((NextEmptyOnHeader*((m*2)+1))*4)+4, file.beg);
				for (map<int, int>::iterator i = newNode1.begin(); i != newNode1.end(); i++){
					file.write((char*)&(i->first) ,sizeof(int));
    				file.write((char*)&(i->second) ,sizeof(int));
				}
				file.seekg(((NextEmpty*((m*2)+1))*4)+4, file.beg);
				file.read((char*)&NextEmptyOnHeader ,sizeof(int));
				file.seekg(4, file.beg);
				file.write((char*)&(NextEmptyOnHeader) ,sizeof(int));
				file.seekg(((NextEmpty*((m*2)+1))*4), file.beg);
				file.write((char*)&(Zero) ,sizeof(int));
				for (map<int, int>::iterator i = newNode2.begin(); i != newNode2.end(); i++){
					file.write((char*)&(i->first) ,sizeof(int));
    				file.write((char*)&(i->second) ,sizeof(int));
				}
				file.seekg((NodeRoot*((m*2)+1)*4), file.beg);
				int One=1;
				file.write((char*)&(One) ,sizeof(int));
				file.write((char*)&(newNode1.rbegin()->first) ,sizeof(int));
				file.write((char*)&(smallNode) ,sizeof(int));
				file.write((char*)&(newNode2.rbegin()->first) ,sizeof(int));
				file.write((char*)&(largNode) ,sizeof(int));
			//	cout << (file.tellg()/4) << " " << (((NodeRoot+1)*((m*2)+1)*4))/4 << endl;
				for (int i=file.tellg() ; i< ((NodeRoot+1)*((m*2)+1)*4)+1; i=i+4){
					file.write((char*)&(Zero) ,sizeof(int));
				}
				}
				
			
			}
		}
		else {
			int Child;
			file.clear();
			file.seekg((Root*((m*2)+1)*4)+4, file.beg);
			int counter =1;
			while (true){
			//	cout << "hna" << endl;
				file.clear();
				file.read((char*)&Child,sizeof(int));
				int pos=file.tellg(),RecID,Index,RepKey;
				if (Child == 0 && counter!= m){
					
					file.seekg(pos-12, file.beg);
					file.read((char*)&RecID,sizeof(int));
					file.read((char*)&Index,sizeof(int));
					file.seekg(pos-12, file.beg);
					file.write((char*)&(RecordID) ,sizeof(int));
					file.seekg((Index*((m*2)+1)*4), file.beg);
					file.read((char*)&leafNode,sizeof(int));
					
					if (leafNode==0){
						NodeCapacity=SearchNodeCapacity (filename,Index);
						if (NodeCapacity!=-1){
							file.seekg(NodeCapacity, file.beg);
							file.write((char*)&(RecordID) ,sizeof(int));
							file.write((char*)&(Reference) ,sizeof(int));
							file.close();
							sorting (filename, Index);
							break;
						}
					}
					
				}
				else {
				//	cout << RecordID << " " <<  Child << endl;
					if (RecordID > Child && counter!= m){
						file.seekg(pos+4, file.beg);
					}
					else if (RecordID < Child) {
					//	cout << "hna" << endl;
						file.read((char*)&Index,sizeof(int));
						file.seekg((Index*((m*2)+1)*4), file.beg);
						file.read((char*)&leafNode,sizeof(int));
						if (leafNode==0){
							NodeCapacity=SearchNodeCapacity (filename,Index);
							if (NodeCapacity!=-1){
								file.seekg(NodeCapacity, file.beg);
								file.write((char*)&(RecordID) ,sizeof(int));
								file.write((char*)&(Reference) ,sizeof(int));
								file.close();
								sorting (filename, Index);
								break;
							}
							else {
							//	cout << "hna" << endl;
								file.close();
								InsertReArr (filename,Root, Index, RecordID,  Reference);
								break;
							}
						}
						else {
							
						}
					}
					else {
						file.close();
						InsertReArr (filename,Root,Root,RecordID,Reference);
						break;
					}
				}
				counter ++;
			}
		}
	}
	
}


void InsertReArr (char *filename,int ParentIndex,int leafIndex,int RecordID, int Reference){
	
	fstream file;
	file.open(filename,ios::binary|ios::in|ios::out);
	map <int, int> newNode1;
	map <int, int> newNode2;
	int TempRecID,TempRecRef;
	int NodeIDs [m+1];
	int b=4,NodeCapacity;
	for (int i=0;i<m;i++){
		file.seekg((leafIndex*((m*2)+1)*4)+b, file.beg);
		file.read((char*)&NodeIDs[i],sizeof(int));
//		cout << NodeIDs[i] << endl;
		b=b+8;
	}
	sort(NodeIDs,NodeIDs+(m+1));
	int placeInMap;
	for (int i=0;i<m;i++){
//		cout << NodeIDs[i] << " " <<  RecordID << endl;
		if (NodeIDs[i] > RecordID){
			placeInMap=i;
			break;
		}
		if ( i==m-1 && placeInMap == 0){
			placeInMap=m;
			break;
		}
	}
//	cout << placeInMap<<endl;
	if (placeInMap < ceil(m/2.0)){
		file.clear();
		file.seekg((leafIndex*((m*2)+1)*4)+4, file.beg);
		
		for (int i=0;i<m-(ceil(m/2.0));i++){
			file.read((char*)&TempRecID,sizeof(int));
			file.read((char*)&TempRecRef,sizeof(int));
			newNode1 [TempRecID]=TempRecRef;
		//	cout << newNode1 [TempRecID] << endl;
		}
		for (int i=0;i<(ceil(m/2.0));i++){
			file.read((char*)&TempRecID,sizeof(int));
			file.read((char*)&TempRecRef,sizeof(int));
			newNode2 [TempRecID]=TempRecRef;
		//	cout << newNode2 [TempRecID] << endl;
		}
		int lastRemBigger = newNode2.rbegin()->first;
	//	cout << lastRemBigger << endl;
		newNode1 [RecordID]=Reference;	
		NodeCapacity=SearchNodeCapacity (filename,ParentIndex);
	//	cout << NodeCapacity << endl;
		if (NodeCapacity !=-1){
			file.seekg((ParentIndex*((m*2)+1)*4)+4, file.beg);
			b=4;
			int TempChild,pos,Node1Pos,NextEmpOnHeader,NextToBeOnHeader;
			while (true){
				file.clear();
				pos=file.tellg();
				file.read((char*)&TempChild,sizeof(int));
				if (TempChild == lastRemBigger){
					break;
				}
				b=b+8;
				file.seekg((ParentIndex*((m*2)+1)*4)+b, file.beg);
			}
			int Zero=0;
			file.seekg(4, file.beg);
			file.read((char*)&NextEmpOnHeader,sizeof(int));
			file.seekg(pos+4, file.beg);
			file.read((char*)&Node1Pos,sizeof(int));
			file.seekg(pos+4, file.beg);
			file.write((char*)&(NextEmpOnHeader) ,sizeof(int));
			file.seekg((Node1Pos*((m*2)+1)*4)+4, file.beg);
			for (map<int, int>::iterator i = newNode1.begin(); i != newNode1.end(); i++){
					file.write((char*)&(i->first) ,sizeof(int));
    				file.write((char*)&(i->second) ,sizeof(int));
			}
			
			for (int i=0 ; i< ((m*2)-(((m-(ceil(m/2.0)))*2)+2)); i++){
				file.write((char*)&(Zero) ,sizeof(int));
			}
			/*file.write((char*)&(Zero) ,sizeof(int));
			file.write((char*)&(Zero) ,sizeof(int));
			file.write((char*)&(Zero) ,sizeof(int));
			file.write((char*)&(Zero) ,sizeof(int));
			*/
			file.seekg((NextEmpOnHeader*((m*2)+1)*4)+4, file.beg);
			file.read((char*)&NextToBeOnHeader,sizeof(int));
			file.seekg(4, file.beg);
			file.write((char*)&(NextToBeOnHeader) ,sizeof(int));
			file.seekg((NextEmpOnHeader*((m*2)+1)*4), file.beg);
			file.write((char*)&(Zero) ,sizeof(int));
			for (map<int, int>::iterator i = newNode2.begin(); i != newNode2.end(); i++){
					file.write((char*)&(i->first) ,sizeof(int));
    				file.write((char*)&(i->second) ,sizeof(int));
			}
			for (int i=0 ; i< ((m*2)-(((ceil(m/2.0)))*2)); i++){
				file.write((char*)&(Zero) ,sizeof(int));
			}
			
			file.seekg(NodeCapacity, file.beg);
			file.write((char*)&(newNode1.rbegin()->first) ,sizeof(int));
			file.write((char*)&(leafIndex) ,sizeof(int));
			file.close();
			sorting (filename,ParentIndex);
		}
		
	}
	else if (placeInMap >= ceil(m/2.0)){
//		cout << "hna" << endl;
		file.clear();
		file.seekg((leafIndex*((m*2)+1)*4)+4, file.beg);
		
		for (int i=0;i<ceil(m/2.0);i++){
			file.read((char*)&TempRecID,sizeof(int));
			file.read((char*)&TempRecRef,sizeof(int));
			newNode1 [TempRecID]=TempRecRef;
		}
		for (int i=0;i<(m-(ceil(m/2.0)));i++){
			file.read((char*)&TempRecID,sizeof(int));
			file.read((char*)&TempRecRef,sizeof(int));
			newNode2 [TempRecID]=TempRecRef;
		}
		
		int lastRemBigger = newNode2.rbegin()->first;
		newNode2 [RecordID]=Reference;	
		NodeCapacity=SearchNodeCapacity (filename,ParentIndex);
		if (NodeCapacity != -1){
			file.seekg((ParentIndex*((m*2)+1)*4)+4, file.beg);
			b=4;
			int TempChild,pos,NextEmpOnHeader,NextToBeOnHeader;
			while (true){
				file.clear();
				pos=file.tellg();
				file.read((char*)&TempChild,sizeof(int));
				if (TempChild == lastRemBigger){
					break;
				}
				b=b+8;
				file.seekg((ParentIndex*((m*2)+1)*4)+b, file.beg);
			}
			int Zero=0;
			file.seekg(4, file.beg);
			file.read((char*)&NextEmpOnHeader,sizeof(int));
			file.seekg(pos, file.beg);
			file.write((char*)&(newNode1.rbegin()->first) ,sizeof(int));
			file.seekg((leafIndex*((m*2)+1)*4)+4, file.beg);
			
			for (map<int, int>::iterator i = newNode1.begin(); i != newNode1.end(); i++){
					file.write((char*)&(i->first) ,sizeof(int));
    				file.write((char*)&(i->second) ,sizeof(int));
    				cout << i->first << " " << i->second << endl;
			}
			for (int i=0 ; i< ((m*2)-(((ceil(m/2.0)))*2)); i++){
				file.write((char*)&(Zero) ,sizeof(int));
			}
			
			file.seekg((NextEmpOnHeader*((m*2)+1)*4)+4, file.beg);
			file.read((char*)&NextToBeOnHeader,sizeof(int));
			file.seekg(4, file.beg);
			file.write((char*)&(NextToBeOnHeader) ,sizeof(int));
			file.seekg((NextEmpOnHeader*((m*2)+1)*4), file.beg);
			file.write((char*)&(Zero) ,sizeof(int));
			for (map<int, int>::iterator i = newNode2.begin(); i != newNode2.end(); i++){
					file.write((char*)&(i->first) ,sizeof(int));
    				file.write((char*)&(i->second) ,sizeof(int));
			}
			for (int i=0 ; i< ((m*2)-(((m-(ceil(m/2.0)))*2)+2)); i++){
				file.write((char*)&(Zero) ,sizeof(int));
			}
			
			file.seekg(NodeCapacity, file.beg);
			file.write((char*)&(newNode2.rbegin()->first) ,sizeof(int));
			file.write((char*)&(NextEmpOnHeader) ,sizeof(int));
			file.close();
			sorting (filename,ParentIndex);
		}
		else {
			ParentIndex=1;
			leafIndex=1;
			cout <<ParentIndex <<  " hna" << endl;
			file.seekg((leafIndex*((m*2)+1)*4)+4, file.beg);
			newNode1.erase(newNode1.begin(),newNode1.end());
			newNode2.erase(newNode2.begin(),newNode2.end());
			for (int i=0;i<ceil(m/2.0);i++){
				file.read((char*)&TempRecID,sizeof(int));
				file.read((char*)&TempRecRef,sizeof(int));
				newNode1 [TempRecID]=TempRecRef;
			}
			for (int i=0;i<(m-(ceil(m/2.0)));i++){
				file.read((char*)&TempRecID,sizeof(int));
				file.read((char*)&TempRecRef,sizeof(int));
				newNode2 [TempRecID]=TempRecRef;
			}
			
	//		int lastRemBigger = newNode2.rbegin()->first;
			int AddToleaf;
			AddToleaf=newNode2 [lastRemBigger];
			cout << lastRemBigger << endl;
			cout << AddToleaf <<endl;
			if (AddToleaf == 0){
				
			}
			else if (AddToleaf !=0){
			newNode2 [RecordID]=Reference;	
			NodeCapacity=SearchNodeCapacity  (filename,AddToleaf);
			if (NodeCapacity != -1){
				
			}
			else {
				int ForMap1,ForMap2,ForMap3,ForMap4,Zero=0;
				file.seekg(4, file.beg);
				file.read((char*)&ForMap4,sizeof(int));
				file.seekg((AddToleaf*((m*2)+1)*4)+4, file.beg);
				map <int,int> newNode3;
				map <int,int> newNode4;
				
				for (int i=0;i<ceil(m/2.0);i++){
				file.read((char*)&TempRecID,sizeof(int));
				file.read((char*)&TempRecRef,sizeof(int));
				newNode3 [TempRecID]=TempRecRef;
				}
				for (int i=0;i<(m-(ceil(m/2.0)));i++){
					file.read((char*)&TempRecID,sizeof(int));
					file.read((char*)&TempRecRef,sizeof(int));
					newNode4 [TempRecID]=TempRecRef;
				}
				newNode4 [RecordID]=Reference;	
				file.seekg((AddToleaf*((m*2)+1)*4)+4, file.beg);
				for (map<int, int>::iterator i = newNode3.begin(); i != newNode3.end(); i++){
					file.write((char*)&(i->first) ,sizeof(int));
    				file.write((char*)&(i->second) ,sizeof(int));
    				//cout << i->first << " " << i->second << endl;
				}
				for (int i=0 ; i< ((m*2)-(((ceil(m/2.0)))*2)); i++){
				file.write((char*)&(Zero) ,sizeof(int));
				}
				
				file.seekg((ForMap4*((m*2)+1)*4), file.beg);
				file.write((char*)&(Zero) ,sizeof(int));
				file.read((char*)&ForMap1,sizeof(int));
				file.seekg((ForMap4*((m*2)+1)*4)+4, file.beg);
				for (map<int, int>::iterator i = newNode4.begin(); i != newNode4.end(); i++){
					file.write((char*)&(i->first) ,sizeof(int));
    				file.write((char*)&(i->second) ,sizeof(int));
    				//cout << i->first << " " << i->second << endl;
				}
				for (int i=0 ; i< ((m*2)-(((m-(ceil(m/2.0)))*2)+2)); i++){
				file.write((char*)&(Zero) ,sizeof(int));
				}
				
				int LastElemnt4 = newNode4.rbegin()->first;
				int One=1;
				file.seekg((ForMap1*((m*2)+1)*4), file.beg);
				file.write((char*)&(One) ,sizeof(int));
				file.read((char*)&ForMap2,sizeof(int));
				file.seekg((ForMap1*((m*2)+1)*4)+4, file.beg);
				for (map<int, int>::iterator i = newNode1.begin(); i != newNode1.end(); i++){
					file.write((char*)&(i->first) ,sizeof(int));
    				file.write((char*)&(i->second) ,sizeof(int));
    				//cout << i->first << " " << i->second << endl;
				}
				for (int i=0 ; i< ((m*2)-(((ceil(m/2.0)))*2)); i++){
				file.write((char*)&(Zero) ,sizeof(int));
				}
				
				file.seekg((ForMap2*((m*2)+1)*4), file.beg);
				file.write((char*)&(One) ,sizeof(int));
				int b=0,BiggerOfMap3F=newNode3.rbegin() ->first,BiggerOfMap3S=AddToleaf,
				BiggerOfMap4F= newNode4.rbegin() ->first,BiggerOfMap4S=ForMap4;
				map <int,int> LastNode;
			//	cout << ForMap3 << endl;
				LastNode[newNode2.begin()->first]=newNode2.begin()->second;
				LastNode[BiggerOfMap3F]=BiggerOfMap3S;
				LastNode[BiggerOfMap4F]=BiggerOfMap4S;
				
				for (map<int, int>::iterator i = LastNode.begin(); i != LastNode.end(); i++){
					
						file.write((char*)&(i->first) ,sizeof(int));
    					file.write((char*)&(i->second) ,sizeof(int));
    					//cout << i->first << " " << i->second << endl;
    				
				}
				for (int i=0 ;i< ((m*2)-(((m-(ceil(m/2.0)))*2)+2)); i++){
				file.write((char*)&(Zero) ,sizeof(int));
				}
				file.seekg(4, file.beg);
				int MinOne=-1;
				file.write((char*)&(MinOne) ,sizeof(int));
				file.seekg((leafIndex*((m*2)+1)*4)+4, file.beg);
				file.write((char*)&(newNode1.rbegin()->first) ,sizeof(int));
				file.write((char*)&(ForMap1) ,sizeof(int));
				file.write((char*)&(LastNode.rbegin()->first) ,sizeof(int));
				file.write((char*)&(ForMap2) ,sizeof(int));
				for (int i=0 ;i< ((m*2)-(((m-(round(m/2.0)))*2))); i++){
				file.write((char*)&(Zero) ,sizeof(int));
				}
				file.close();
			//	sorting (filename,leafIndex);
			//	sorting (filename,ForMap1);
			//	sorting (filename,ForMap2);
			}
		}
		}
	}
	
	
}

void sorting (char *filename,int index){
//	cout << index << endl;
	fstream file;
	file.open(filename,ios::binary|ios::in|ios::out);
	map <int, int> SortingMap;
	file.seekg((index*((m*2)+1)*4)+4, file.beg);
	int TempRecRef,TempRecId;
	for (int i=0;i<(m);i++){
		file.read((char*)&TempRecId,sizeof(int));
		if (TempRecId==0) break;
		file.read((char*)&TempRecRef,sizeof(int));
		SortingMap [TempRecId]=TempRecRef;
	//	cout << SortingMap [TempRecId] << endl;
	}
	file.seekg((index*((m*2)+1)*4)+4, file.beg);
	for (map<int, int>::iterator i = SortingMap.begin(); i != SortingMap.end(); i++){
    	file.write((char*)&(i->first) ,sizeof(int));
    	file.write((char*)&(i->second) ,sizeof(int));
	}
	file.close();
}


int SearchNodeCapacity (char *filename,int index){
	fstream file;
	file.open(filename,ios::binary|ios::in|ios::out);
	int value;
//	cout << (index*((m*2)+1)*4)+4 << " " << ((index+1)*((m*2)+1)*4)+4 << endl;
	for (int i=(index*((m*2)+1)*4)+4;i<((index+1)*((m*2)+1)*4);i=i+8){
		file.seekg(i, file.beg);
		file.read((char*)&value,sizeof(int));
		if (value==0){
			return i;
		}
	}
	return -1;
}


int SearchARecord (char *filename, int RecordID, int Index){
	fstream file;
	file.open(filename,ios::binary|ios::in);
	file.seekg((Index*((m*2)+1)*4), file.beg);
	int leaf,CurrentRecID,RecordRef,next;
	file.read((char*)&leaf,sizeof(int));
//	cout << leaf << endl;
	if (leaf == 0){
		while (true){
			int pos=file.tellg();
			if (pos == ((Index+1)*((m*2)+1)*4)){
				return -1;
			}
			file.read((char*)&CurrentRecID,sizeof(int));
//			cout << CurrentRecID <<endl;
			if (CurrentRecID == RecordID){
//				cout << "d5lt" << endl;
				file.read((char*)&RecordRef,sizeof(int));
				return RecordRef;
			}
			file.seekg(pos+8, file.beg);
		}
	}
	else {
		while (true){
			int pos=file.tellg();
			if (pos == ((Index+1)*((m*2)+1)*4)){
					return -1;
			}
			file.read((char*)&CurrentRecID,sizeof(int));
			if (CurrentRecID >= RecordID){
				file.read((char*)&next,sizeof(int));
				return SearchARecord (filename, RecordID,next);
			}
			file.seekg(pos+8, file.beg);
		}
	}	
	file.close();
}

void DisplayIndexFileContent (char *filename){
	fstream file;
	file.open(filename,ios::binary|ios::in);
	int num,counter=1;
	
	while (file.read((char*)&num ,sizeof(int))){
		cout << num << " ";	
		if (counter==(m*2)+1){
			cout<<endl;
			counter=0;
		}
		counter++;
	}
	file.close();
	
}
