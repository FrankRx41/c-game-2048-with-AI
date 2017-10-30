#include <stdio.h>
#include <string.h>

#define strequ(s1,s2)	(strcmp((s1),(s2))==0)

#define MAX	256

int getit(FILE * fp){
	char str[MAX];
	while(fgets(str,MAX,fp),str[0]!='\n' && str[0]!='#')
	{
		char delims[] = "=";
		char *result = NULL;
		result = strtok( str, delims );
		printf( "result is \"%s\"\n", result );
		result = strtok( NULL, delims );
		if(result[0] == '\"'){
			char str[MAX];
			sscanf(result,"%s",str);
			printf("str: %s\n",str);
		}else{
			int x = 0;
			sscanf(result,"%x",&x);
			printf("x = %x\n",x);
		}
	}
	printf("%d\n",__LINE__);
	return 0;
}

int gettablecolor(int x){
	int i=0;
	while(x>>=1,x!=0){
		i++;
	}
	return i;
}

int main(){

	FILE *fp = fopen("config.ini","r+");
	if(!fp){
		perror("fopen error: ");
		return 0;
	}
	char str[MAX] = {0},s[MAX] = {0};

	fseek(fp,0,SEEK_SET);

	while(!feof(fp)){
		//printf(" begin ");

		fgets(str,MAX,fp);
		//printf("%s",str);
		str[strlen(str)-1] = 0;
		
		if(strequ(str,"[FONT]"))
		{
			getit(fp);
			printf("%d\n",__LINE__);
		}
		else if(strequ(str,"[COLOR]"))
		{
			getit(fp);
		}
		else if(strequ(str,"[SCORE]"))
		{
			return (0);
			getit(fp);
		}
		else if(str[0] == '#')
		{
			printf("%d\n",__LINE__);
		}
	}
	return 0;
}

/*
#include <stdio.h>
#include <string.h>

#define strequ(s1,s2)	(strcmp((s1),(s2))==0)

#define MAX	256

int main(){
	FILE *fp = fopen("config.ini","a+");
	if(!fp){
		perror("fopen error: ");
		return 0;
	}
	char str[MAX] = {0};
	char a,s[MAX] = {0};

	while(!feof(fp)){
		fgets(str,MAX,fp);
		
		if(str[0]=='\n'){
			// next line must be like [***]
			// here i begin to scan the value

			fgets(str,MAX,fp);
			//int len = strlen(str)-3;
			//printf("len: %d\n",len);
			// ignore '['
			//sscanf(str,"%c",&a);
			//int i;
			//for(i=0;i<len;i++){
			//	sscanf(&str[1+i],"%c",&s[i]);
			//}
			//s[i] = 0;
			// fuck the last ']'
			//scanf("%c",&a);
			//sscanf(str,"%c%s%c",&a,s,&b);
			//sscanf(str,"%c%*s%c",&a,strlen(str)-3,s,&b);
			//printf("i get: %s\n",s);

			if(strequ(str,"[FONT]")){
				printf("%d\n",__LINE__);
			}else if(strequ(str,"[COLOR]")){
				// here i begin to read the value
				while(fgets(str,MAX,fp),str[0]!='\n'){
					//sscanf(str,"%s=%c",s,&a);
					// ignore '='
					//s[strlen(s)-1]=0;
					//printf("%s is %c\n",s,a);

					char delims[] = "="; 
					char *result = NULL;  
					
					result = strtok( str, delims );
					
					while( result != NULL )  
					{  
						printf( "result is \"%s\"\n", result );  
						result = strtok( NULL, delims );  
					}  

				}
			}else if(strequ(str,"[SCORE]")){

			}

		}
		//printf("%s",str);
	}
	return 0;
}
*/


/*
example:

#this is comment too
[FONT]
Name="Sans-serif","MSYaHei"
Size=

#this is comment
[COLOR]
0=0xFF00FF
2=77895
4=
6=
8=
16=
32=
64=
128=
256=
512=
1024=
2048=
4096=
8192=
16384=
32768=
65536=
Padding=

[SCORE]
High=

*/
