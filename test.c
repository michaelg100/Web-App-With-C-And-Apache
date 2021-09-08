#include <stdio.h>
int main()
{
    printf ("Content-Type: text/html\n\n");
    printf ("<html>");
    printf ("<head><title>C-Html sync</title></head>");
    printf ("<body>");    
    printf ("This page is created by a C program.<br />");
    printf ("I love to share my ideas to the world <strong>bold</strong> and <em>italic</em> text.");
    printf ("</body> </html>");
    return 0;
}