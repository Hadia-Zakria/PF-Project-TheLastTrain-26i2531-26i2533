#include<iostream>
#include<cstring>
#include"util.h"
#include<iomanip>
using namespace std;
int clampInt(int val,int minVal,int maxVal){
	if(val<minVal)
	   return minVal;
	else if(val>maxVal)
	   return maxVal;
	else
	  return val;
}

void trimWhitespace(char* str){
	int start=0;
	if(str==nullptr)
	  return;
	int end=strlen(str)-1;
	if(end<0)
	  return;
	while(str[start]==' ' || str[start]=='\t' || str[start]=='\n' || str[start]=='\r')
	    start++;
	while(end>=0 && (str[end]==' ' || str[end]=='\t' || str[end]=='\n' || str[end]=='\r'))
	    end--;
	int i;
	for(i=0;start<=end;i++){
		str[i]=str[start];
		start++;
	}
	str[i]='\0';
}

bool stringEquals(const char*a,const char*b){
	int i=0;
	if(a==nullptr || b==nullptr || strlen(a)!=strlen(b))
	   return false;;
	while(a[i]!='\0'){
	  if(a[i]!=b[i])
	    return false;
	    i++;
} 
    return true;
}

void printHeader(const char* title){
	cout<<left<<setfill('=')<<setw(30)<<""<<endl;
	cout<<endl;
	cout<<left<<setfill(' ')<<setw(17)<<right<<title<<endl;
	cout<<endl;
	cout<<left<<setfill('=')<<setw(30)<<""<<endl;
	cout<<setfill(' ')<<right;
}


void clearScreen(){
	system("cls");//remove
	system("clear");
}




