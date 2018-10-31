// aapt list -a SpareParts.apk
// aapt dump xmltree SpareParts.apk AndroidManifest.xml
// A: android:name(0x01010003)="SpareParts" (Raw: "SpareParts")
// aapt dump badging SpareParts.apk
// launchable activity name='com.android.spare_parts.SpareParts'label='' icon=''

#include <math.h>
#include <string.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <stdio.h>

#define BSIZE 512

using namespace std;

char valid[200];
char invalid[200];

template<class T>
inline std :: string to_string ( const T &t )
{
	std :: stringstream ss;
	ss << t;
	return ss.str ();
}

std::string exec(char* cmd) {
	FILE* pipe = popen(cmd, "r");
	if (!pipe) return "ERROR";
	char buffer[BSIZE];
	std::string result = "";
	while(!feof(pipe)) {
		if(fgets(buffer, BSIZE, pipe) != NULL)
		result += buffer;
	}
	pclose(pipe);
	return result;
}

string get_tag(string val, string retval)
{
	size_t found = retval.find(val);
	if(found==string::npos)
	{
		return "ERROR";
	}
	string ret_val = retval.substr(retval.find(val)+strlen(val.c_str())+2, strlen(retval.c_str()) - (retval.find(val)+strlen(val.c_str())+2));
	string aval = ret_val.substr(0,ret_val.find("'"));
	if(aval.length()>100)
	{
		return aval.substr(0,100);
	}
	if( 0 == strncmp ( aval.c_str ( ) , "attribute: attribute is not a string value" , 42 ) )
	{
		return "ERROR";	
	}
	return aval;
}

string rep(string val, char o, char n)
{
	replace(val.begin(),val.end(),o,n);
	return val;
}

string validate_fn(string fn)
{ 
	int ln=strlen(invalid);
	for(int i=0;i<ln;i++)
	{
		fn=rep(fn,invalid[i],valid[i]);
	}
	return fn;
}

void trim(string& str)
{
	string::size_type pos = str.find_last_not_of(' ');
	if(pos != string::npos)
	{
		str.erase(pos + 1);
		pos = str.find_first_not_of(' ');
		if(pos != string::npos)
		{
			str.erase(0,pos);
		}
	}
	else
	{
		str.erase(str.begin(),str.end());
	}
}

void check_null(char *buf)
{
	if(buf==NULL)
	{
		ofstream    myOUTfile ( "diffoutp.txt" );
		myOUTfile << "Cannot Allocate Memory. Aborted." << endl;
		myOUTfile.close ();                  
	}
}

void display ( double k , long val , double total )
{
	system( "clear" );
	cout << "\n\n\t" << val << "% from " << total << " files, current " << k << endl;			
}

int show_progress ( double k , double total , int max )
{
	long val = (long) ( ( (double) k / (double) total ) * 100.0 );

	if ( ( val >= max ) && ( val <= max + 5 ) )
	{
		display ( k , val , total );
    	max = max + 5;
    	return max;
	}

	if ( val > max + 5 )
	{
         double dif = val - ( max + 5 );
         double fac = ( max + 5 + dif ) / 5.0;
         max = (int) ( floor ( fac ) * max ) + 5;
	}

	return max;
}

int main ()
{
	time_t      starttime, endtime, dif;
	struct tm   *timeinfo;
	char        buffer[80];
	time ( &starttime );
	timeinfo = localtime ( &starttime );
	strftime ( buffer, 80, "%X", timeinfo );

	int c=0;
	int pos=0;
	int max=0;
	bool nline=false;
	FILE * pFile = fopen ("diffchar.txt","rb");
	if (pFile!=NULL)
	{
		do
		{
			c = fgetc (pFile);
			if(c!=EOF)
			{
				if(c==13)
				{
					c = fgetc (pFile);
					if(c==10)
					{
						nline=true;
						invalid[pos]=0;
						pos=0;
						c = fgetc (pFile);
					}
				}
				if(nline) valid[pos]=c;
				else invalid[pos]=c;
				pos++;
			}
		}
		while (c != EOF);
		valid[pos]=0;
		fclose (pFile);
	}
	else
	{
		strncpy(invalid,"!*:;,|^~ªº+§/?’¿›¨‹¡»¹²³¦£¢„¤¶\\",109);
		strncpy(  valid,"_______________________________",109);
	}
	
	ifstream    myINfile ( "difflist.txt" );
	ifstream    myCTfile ( "difflist.txt" );
	ofstream    myOUTfile ( "diffoutp.txt" );

	myOUTfile << "APKNamer version 18.10.31.01.30" << endl;

	myOUTfile << "invalid SET: " << invalid << endl;
	myOUTfile << "valid   SET: " << valid << endl;
	
	myOUTfile << "Start time: " << buffer << endl;
	myOUTfile << "Starting comparison..." << endl;

	unsigned long   k;
	unsigned long   total;
	unsigned long   counter = 0;
	string          line1;
	string          str_cmd;
	string          retval;
	string          newname;
	char            bfer[]="000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";

	total = count(istreambuf_iterator<char>(myCTfile), istreambuf_iterator<char>(), '\n') +1;
	cout << "\n\n\t0% from " << total << " files" << endl;
	
	k = 0;
	if ( myINfile.is_open () )
	{
		do
		{
			getline ( myINfile, line1 );
			k++;
			trim(line1);
			if(line1.length()>0)
			{
				str_cmd = "aapt dump badging \"" + line1 +"\" 2>&1";
				strcpy(bfer,str_cmd.c_str());
				retval=exec(bfer);
				if (retval != "ERROR")
				{
					counter++;
					newname = "[" + get_tag("label", retval) + " " + get_tag("versionName", retval) + "] [" + get_tag("name",retval) + "] [_" + to_string ( counter ) + "_].apk";                    
					newname=validate_fn(newname);
					long    line1_slash = line1.find_last_of ( "/" );
					string  line1_pathName = line1.substr ( 0, line1_slash + 1 );
					newname = line1_pathName + newname;
					rename( line1.c_str (), newname.c_str() );
				}
			}			
			max = show_progress ( k , total , max );
		}
		while ( !myINfile.eof () );
		//free(valid);
		//free(invalid);
		myINfile.close ();
		myOUTfile << counter << " files renamed!" << endl;
		system("clear");
		cout << "\n\n\t100% from " << total << " files" << endl;
	}
	else
	{
		myOUTfile << "Could not open input file..." << endl;
	}

	myOUTfile << "Finishing execution..." << endl;
	myOUTfile << "Done." << endl;

	time ( &endtime );
	timeinfo = localtime ( &endtime );
	strftime ( buffer, 80, "%X", timeinfo );

	myOUTfile << "End time: " << buffer << endl;

	dif = endtime - starttime;

	timeinfo = localtime ( &dif );
	strftime ( buffer, 80, "%M:%S", timeinfo );

	myOUTfile << "Total time: " << buffer << endl;
	cout << "\n\tTotal Time: " << buffer << endl;

	myOUTfile.close ();
	myCTfile.close ();
	cout << endl;
	std::cout << "Press enter to continue ..."; 
	std::cin.get(); 
	return ( 0 );
}
