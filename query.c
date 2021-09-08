#include "stdio.h"
#include "web.h"

int main(){

	char **qs = web_get_query_string();

	char *user = web_get_from_query_string(qs, "user");
	char *color = web_get_from_query_string(qs, "color");
	char red[4] = "red";

	web_print_header();

	if(!color){
		color = red;
	}
	printf("<body bgcolor=\"%s\">", color);

	if(user){
		printf("<h1>Welcome: %s</h1>", user);
	}else{
		printf("<h1>Welcome!</h1>");
	}

	printf("</body>");

}//main*/
