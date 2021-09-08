FROM httpd:2.4
COPY ./test.c /usr/local/apache2/cgi-bin/
COPY ./test.pl /usr/local/apache2/cgi-bin/
COPY ./httpd.conf /usr/local/apache2/conf/httpd.conf
RUN apt-get update

RUN apt-get -y install \
    gcc \
    vim 

RUN gcc -o /usr/local/apache2/cgi-bin/index.out /usr/local/apache2/cgi-bin/test.c
# build:

# docker build -t my-apache2 .
# docker run -dit --name my-running-app -p 8080:80 my-apache2

# exec:

# docker exec -it my-running-app /bin/bash



#other