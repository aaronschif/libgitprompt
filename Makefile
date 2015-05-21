libgitprompt: main.cpp format.cpp
	g++ $< -o $@ -lgit2 -std=gnu++11 -lboost_filesystem -lboost_system

install: libgitprompt
	install $< /usr/bin/

clean:
	rm gitprompt
