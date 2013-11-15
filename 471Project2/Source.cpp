#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

//Function Prototypes
void Optimal(int, int, vector<int>&);
void Fifo(int, int, vector<int>&);
void Lru(int, int, vector<int>&);
void Mru(int, int, vector<int>&);
void Display(int, int, string, double);

int main()
{
	ifstream infile;					//input file stream
	int PageSize=0;						//will loop through 512, 1024 and 2048
	int FrameSize=0;					//will loop through 4, 8, 12 
	int Address;						//address read from file
	vector<int> MemData;				//vector to hold address from file
	vector<int> PageData;				//vector to hold the page that corresponds to the virtual address 

	infile.open("testfile.dat");
	if (!infile)
	{
		cout<<"Failed to open testfile.dat"<<endl;
		system("pause");
		return 1;
	}

	//Load MemData vector with values from file
	//prime read
	infile>>Address;
	while(infile)
	{
		MemData.push_back(Address);
		infile>>Address;
	}

	//Loop through the 9 different possibilities
	for (PageSize=512; PageSize<=2048; PageSize=PageSize*2)		//Outerloop of PageSizes
	{
		for (FrameSize=4; FrameSize<=12; FrameSize=FrameSize+4)	//Innerloop of FrameSizes
		{
			for (unsigned x=0; x<MemData.size(); x++)			//Reload PageData Vector based on new PageSize
			{
				PageData.push_back(MemData.at(x)/PageSize);
			}
			Optimal(PageSize, FrameSize, PageData);				//Function call to Optimal
			Fifo(PageSize, FrameSize, PageData);				//Function call to Fifo
			Lru(PageSize, FrameSize, PageData);					//Function call to Lru
			Mru(PageSize, FrameSize, PageData);					//Function call to Mru
			PageData.clear();									//Clear out the PageData vector after each run
		}
	} 

	system("pause");
	return 0;
}

//****************************************************************************************
//**                          Optimal Function Starts Here                              **
//****************************************************************************************
void Optimal(int PageSize, int FrameSize, vector<int> & PageData)
{
	int PageFaults=0;							//Count the faults
	int MaxPageNum=0;							//Find the Max page address to determine number of pages
	int CurrentPosition=0;						//Current page position (Simulates what page must be loaded based on virtual address)
	int NextLoc=0;								//The next position of a Page in the PageData vector - used as a weight
	double FaultPercent=0.0;					//Holds the page fault percentage
	bool Hit=false;								//Track if page is already in a frame
	vector<pair<int,int>> Frame;				//Vector of pairs (Page, NextLoc)
	vector<int>::iterator PageItr;				//Iterator for PageData vector passed into function

	//This is to display the page numbers for debugging
	for (unsigned i=0; i<PageData.size(); i++)
	{
		cout<<"Page: "<<PageData.at(i)<<endl;
	}

	//Loop through the PageData vector, load frames, count faults
	while (CurrentPosition<PageData.size())
	{
		Hit=false; //reset to false on every loop

		//Keep track of the largest Page Number for # of pages
		if (PageData.at(CurrentPosition)>MaxPageNum)
		{
			MaxPageNum=PageData.at(CurrentPosition);
		}

		//Search for next location of page starting at CurrentPosition+1 
		PageItr=find(PageData.begin()+(CurrentPosition+1), PageData.end(), PageData.at(CurrentPosition));
		if (PageItr != PageData.end())
			NextLoc=PageItr-PageData.begin();	//If found NextLoc gets index as weight
		else
			NextLoc=PageData.size();			//If not found NextLoc gets PageData.size() (highest victim weight) 
		
		//First Possibility - There are still empty frames
		//Filled frames must first be checked for page hit, otherwise add to empty frame
		if (Frame.size()<FrameSize)
		{
			//Check if page is already in a loaded frame
			for (unsigned i=0; i<Frame.size(); i++)
			{
				if (Frame.at(i).first==PageData.at(CurrentPosition))
				{
					Hit=true;
					Frame.at(i).second=NextLoc;  //update weight
				}
			}
			//If not, add page to frame vector since there are empty frames
			if (!Hit)
			{
				Frame.push_back(make_pair(PageData.at(CurrentPosition),NextLoc));
				PageFaults=PageFaults+1;
			}
		}//end if

		//Second Possibility - All frames have something in them
		//Must update page weight (hit) or replace page (miss)
		else 
		{
			//Check if page is already in a loaded frame
			for (unsigned i=0; i<Frame.size(); i++)
			{
				if (Frame.at(i).first==PageData.at(CurrentPosition))
				{
					Hit=true;
					Frame.at(i).second=NextLoc;  //update weight
				}
			}
			//Otherwise find the victim and update it
			if(!Hit)
			{
				int VicPosition=0;
				int MaxWeight=0;
				for (unsigned i=0; i<Frame.size(); i++)
				{
					if (Frame.at(i).second>MaxWeight)
					{
						MaxWeight=Frame.at(i).second;
						VicPosition=i;
					}
				}
				Frame.at(VicPosition).first=PageData.at(CurrentPosition);
				Frame.at(VicPosition).second=NextLoc;
				PageFaults=PageFaults+1;
			}
		}//end else
				
		CurrentPosition=CurrentPosition+1; //move to next page needed by "program"

		//Display Frames for debugging
		for (unsigned i=0; i<Frame.size(); i++)
		{
			cout<<"Frame: "<<i<<" holds: "<<"("<<Frame.at(i).first<<","<<Frame.at(i).second<<")"<<endl;
		}
		cout<<"PageFaults: "<<PageFaults<<endl<<endl;
	}//end of while loop

	FaultPercent=(double)PageFaults/(double)PageData.size();

	cout<<endl<<"Final statistical data which will be feed to display function:"<<endl;
	cout<<"Page Size: "<<PageSize<<endl;
	cout<<"Number of Frames: "<<FrameSize<<endl;
	cout<<"# of Pages: "<<MaxPageNum+1<<endl;	//Add 1 to account for page 0
	cout<<"Page replacement ALG: Optimal"<<endl;
	cout<<"Page fault percentage: "<<FaultPercent<<endl;
}

//****************************************************************************************
//**                            LRU Function Starts Here                                **
//****************************************************************************************
void Lru(int PageSize, int FrameSize, vector<int> & PageData)
{
	int PageFaults=0;							//Count the faults
	int MaxPageNum=0;							//Find the Max page address to determine number of pages
	int CurrentPosition=0;						//Current page position (Simulates what page must be loaded based on virtual address)
	double FaultPercent=0.0;					//Holds the page fault percentage
	bool Hit=false;								//Track if page is already in a frame
	vector<pair<int,int>> Frame;				//Vector of pairs (Page, Index)

	//This is to display the page numbers for debugging
	for (unsigned i=0; i<PageData.size(); i++)
	{
		cout<<"Page: "<<PageData.at(i)<<endl;
	}

	//Loop through the PageData vector, load frames, count faults, etc.
	while (CurrentPosition<PageData.size())
	{
		Hit=false; //reset to false on every loop

		//Find the largest Page Number for # of pages
		if (PageData.at(CurrentPosition)>MaxPageNum)
		{
			MaxPageNum=PageData.at(CurrentPosition);
		}

		//First Possibility - There are still empty frames
		//Filled frames must first be checked for page hit, otherwise add to empty frame
		if (Frame.size()<FrameSize)
		{
			//Check if page is already in a loaded frame
			for (unsigned i=0; i<Frame.size(); i++)
			{
				if (Frame.at(i).first==PageData.at(CurrentPosition))
				{
					Hit=true;
					Frame.at(i).second=CurrentPosition;  //update index
				}
			}
			//If not, add page to frame vector since there are empty frames
			if (!Hit)
			{
				Frame.push_back(make_pair(PageData.at(CurrentPosition), CurrentPosition));
				PageFaults=PageFaults+1;
			}
		}//end if

		//Second Possibility - All frames have something in them
		//Must update page index (hit) or replace page (miss)
		else 
		{
			//Check if page is already in a loaded frame
			for (unsigned i=0; i<Frame.size(); i++)
			{
				if (Frame.at(i).first==PageData.at(CurrentPosition))
				{
					Hit=true;
					Frame.at(i).second=CurrentPosition;  //update index
				}
			}
			//Otherwise find the victim and update it
			if(!Hit)
			{
				int VicPosition=0;
				int MinIndex=PageData.size(); //Set to largest possible index, then find smallest as victim
				for (unsigned i=0; i<Frame.size(); i++)
				{
					if (Frame.at(i).second < MinIndex)
					{
						MinIndex=Frame.at(i).second;
						VicPosition=i;
					}
				}
				Frame.at(VicPosition).first=PageData.at(CurrentPosition);
				Frame.at(VicPosition).second=CurrentPosition;
				PageFaults=PageFaults+1;
			}
		}//end else

		CurrentPosition=CurrentPosition+1; //move to next page needed by "program"

		//Display Frames for debugging
		for (unsigned i=0; i<Frame.size(); i++)
		{
			cout<<"Frame: "<<i<<" holds: "<<"("<<Frame.at(i).first<<","<<Frame.at(i).second<<")"<<endl;
		}
		cout<<"PageFaults: "<<PageFaults<<endl<<endl;
	}//end while

	FaultPercent=(double)PageFaults/(double)PageData.size();

	cout<<endl<<"Final statistical data which will be feed to display function:"<<endl;
	cout<<"Page Size: "<<PageSize<<endl;
	cout<<"Number of Frames: "<<FrameSize<<endl;
	cout<<"# of Pages: "<<MaxPageNum+1<<endl;	//Add 1 to account for page 0
	cout<<"Page replacement ALG: LRU"<<endl;
	cout<<"Page fault percentage: "<<FaultPercent<<endl;
}

//****************************************************************************************
//**                            MRU Function Starts Here                                **
//****************************************************************************************
void Mru(int PageSize, int FrameSize, vector<int> & PageData)
{
	int PageFaults=0;							//Count the faults
	int MaxPageNum=0;							//Find the Max page address to determine number of pages
	int CurrentPosition=0;						//Current page position (Simulates what page must be loaded based on virtual address)
	double FaultPercent=0.0;					//Holds the page fault percentage
	bool Hit=false;								//Track if page is already in a frame
	vector<pair<int,int>> Frame;				//Vector of pairs (Page, Index)

	//This is to display the page numbers for debugging
	for (unsigned i=0; i<PageData.size(); i++)
	{
		cout<<"Page: "<<PageData.at(i)<<endl;
	}

	//Loop through the PageData vector, load frames, count faults, etc.
	while (CurrentPosition<PageData.size())
	{
		Hit=false; //reset to false on every loop

		//Find the largest Page Number for # of pages
		if (PageData.at(CurrentPosition)>MaxPageNum)
		{
			MaxPageNum=PageData.at(CurrentPosition);
		}

		//First Possibility - There are still empty frames
		//Filled frames must first be checked for page hit, otherwise add to empty frame
		if (Frame.size()<FrameSize)
		{
			//Check if page is already in a loaded frame
			for (unsigned i=0; i<Frame.size(); i++)
			{
				if (Frame.at(i).first==PageData.at(CurrentPosition))
				{
					Hit=true;
					Frame.at(i).second=CurrentPosition;  //update index
				}
			}
			//If not, add page to frame vector since there are empty frames
			if (!Hit)
			{
				Frame.push_back(make_pair(PageData.at(CurrentPosition), CurrentPosition));
				PageFaults=PageFaults+1;
			}
		}//end if

		//Second Possibility - All frames have something in them
		//Must update page index (hit) or replace page (miss)
		else 
		{
			//Check if page is already in a loaded frame
			for (unsigned i=0; i<Frame.size(); i++)
			{
				if (Frame.at(i).first==PageData.at(CurrentPosition))
				{
					Hit=true;
					Frame.at(i).second=CurrentPosition;  //update index
				}
			}
			//Otherwise find the victim and update it
			if(!Hit)
			{
				int VicPosition=0;
				int MaxIndex=0; //Set to lowest possbile index then find largest
				for (unsigned i=0; i<Frame.size(); i++)
				{
					if (Frame.at(i).second > MaxIndex)
					{
						MaxIndex=Frame.at(i).second;
						VicPosition=i;
					}
				}
				Frame.at(VicPosition).first=PageData.at(CurrentPosition);
				Frame.at(VicPosition).second=CurrentPosition;
				PageFaults=PageFaults+1;
			}
		}//end else

		CurrentPosition=CurrentPosition+1; //move to next page needed by "program"

		//Display Frames for debugging
		for (unsigned i=0; i<Frame.size(); i++)
		{
			cout<<"Frame: "<<i<<" holds: "<<"("<<Frame.at(i).first<<","<<Frame.at(i).second<<")"<<endl;
		}
		cout<<"PageFaults: "<<PageFaults<<endl<<endl;
	}//end while

	FaultPercent=(double)PageFaults/(double)PageData.size();

	cout<<endl<<"Final statistical data which will be feed to display function:"<<endl;
	cout<<"Page Size: "<<PageSize<<endl;
	cout<<"Number of Frames: "<<FrameSize<<endl;
	cout<<"# of Pages: "<<MaxPageNum+1<<endl;	//Add 1 to account for page 0
	cout<<"Page replacement ALG: MRU"<<endl;
	cout<<"Page fault percentage: "<<FaultPercent<<endl;
}

//****************************************************************************************
//**                            FIFO Function Starts Here                               **
//****************************************************************************************
void Fifo(int PageSize, int FrameSize, vector<int> & PageData)
{
	int PageFaults=0;							//Count the faults
	int MaxPageNum=0;							//Find the Max page address to determine number of pages
	int CurrentPosition=0;						//Current page position (Simulates what page must be loaded based on virtual address)
	double FaultPercent=0.0;					//Holds the page fault percentage
	bool Hit=false;								//Track if page is already in a frame
	vector<int> Frame;							//Vector of Pages in Frames

	//This is to display the page numbers for debugging
	for (unsigned i=0; i<PageData.size(); i++)
	{
		cout<<"Page: "<<PageData.at(i)<<endl;
	}

	//Loop through the PageData vector, load frames, count faults, etc.
	while (CurrentPosition<PageData.size())
	{
		Hit=false; //reset to false on every loop

		//Find the largest Page Number for # of pages
		if (PageData.at(CurrentPosition)>MaxPageNum)
		{
			MaxPageNum=PageData.at(CurrentPosition);
		}

		//First Possibility - There are still empty frames
		//Filled frames must first be checked for page hit, otherwise add to empty frame
		if (Frame.size()<FrameSize)
		{
			//Check if page is already in a loaded frame
			for (unsigned i=0; i<Frame.size(); i++)
			{
				if (Frame.at(i)==PageData.at(CurrentPosition))
				{
					Hit=true;
				}
			}
			//If not, add page to frame vector since there are empty frames
			if (!Hit)
			{
				Frame.push_back(PageData.at(CurrentPosition));
				PageFaults=PageFaults+1;
			}
		}//end if   

		//Second Possibility - All frames have something in them
		else 
		{
			//Check if page is already in a loaded frame
			for (unsigned i=0; i<Frame.size(); i++)
			{
				if (Frame.at(i)==PageData.at(CurrentPosition))
				{
					Hit=true;
				}
			}
			//Otherwise, push new one in the bottom and pop the first one in from top
			if(!Hit)
			{
			Frame.push_back(PageData.at(CurrentPosition));
			Frame.erase(Frame.begin()); //pop the top

			PageFaults=PageFaults+1;
			}
		}//end else

		CurrentPosition=CurrentPosition+1; //move to next page needed by "program"

		//Display Frames for debugging
		for (unsigned i=0; i<Frame.size(); i++)
		{
			cout<<"Frame: "<<i<<" holds: "<<"("<<Frame.at(i)<<")"<<endl;
		}
		cout<<"PageFaults: "<<PageFaults<<endl<<endl;
	}//end while

	FaultPercent=(double)PageFaults/(double)PageData.size();

	cout<<endl<<"Final statistical data which will be feed to display function:"<<endl;
	cout<<"Page Size: "<<PageSize<<endl;
	cout<<"Number of Frames: "<<FrameSize<<endl;
	cout<<"# of Pages: "<<MaxPageNum+1<<endl;	//Add 1 to account for page 0
	cout<<"Page replacement ALG: FIFO"<<endl;
	cout<<"Page fault percentage: "<<FaultPercent<<endl;
}

//****************************************************************************************
//**                          Dispaly Function Starts Here                              **
//****************************************************************************************
void Display(int PageSize, int FrameSize, string Algorithm, double FaultPercent)
{
	//Heading should print in maing before calling functions
cout<<left<<setw(11)<<"Page Size"<<setw(12)<<"# of pages"<<setw(22)<<"Page replacement ALG"<<setw(23)<<"Page fault percentage"<<endl;
}














