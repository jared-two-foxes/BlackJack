#include <framework/database/database.hpp>

#include <iostream>

int main(int argc, char* argv[]) {
	db::database db;

	int ret;

	/* Create SQL statement */
    ret = db.exec("CREATE TABLE PERSON(ID INT PRIMARY KEY NOT NULL, NAME TEXT NOT NULL, USERNAME CHAR(50), PASSWORD CHAR(50));");
    if (ret) {
    	std::cout << db.error << std::endl;
    }

	
	ret = db.exec("INSERT INTO COMPANY (ID,NAME,USERNAME,PASSWORD) VALUES (1, 'Paul', 'California', 'password' );");
    if (ret) {
    	std::cout << db.error << std::endl;
    }

	return ret;
}

/*
"table PERSON {
	ID 		 INT PRIMARY KEY NOT NULL, 
	NAME 	 TEXT NOT NULL, 
	USERNAME CHAR(50), 
	PASSWORD CHAR(50)
};"
*/


"INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) "  
"VALUES (2, 'Allen', 25, 'Texas', 15000.00 ); "     

"INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY)" 
"VALUES (3, 'Teddy', 23, 'Norway', 20000.00 ); "

"INSERT INTO COMPANY (ID,NAME,AGE,ADDRESS,SALARY) " 
"VALUES (4, 'Mark', 25, 'Rich-Mond ', 65000.00 ) "