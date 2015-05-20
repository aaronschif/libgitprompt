gitprompt: main.cpp
	g++ $< -o $@ -lgit2 -std=gnu++11 -lboost_filesystem -lboost_system

clean:
	rm gitprompt
