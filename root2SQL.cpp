//
//  DB_Track.cpp
//  
//
//  Created by Baranyai David on 2018. 06. 05..
//

#include <stdio.h>
#include <iostream>
#include <experimental/filesystem>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "SQLGenerator.hpp"
#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TObjArray.h"

void print_usage()
{
    std::cout << "Use -f to specify a file to convert." << std::endl;
    std::cout << "Use -d to specify a directory to convert all root files from it. (Opens only .root files)" << std::endl;
    std::cout << "-f and -d can be combined and can be used multiple times to specify more than one file or directory." << std::endl;
    std::cout << "Use \'-n filename\' to use file name as table, or replace \'filename\' with the name you want." << std::endl;
    std::cout << "Use -s databasename to set the database name (schema)." << std::endl;
    std::cout << "Use -c to enable CREATE DATABASE statements." << std::endl;
    std::cout << "Use -e to enable IF NOT EXISTS statements." << std::endl;
    std::cout << "Use -t to enable CREATE TABLE statements." << std::endl;
}

int main(int argc, char** argv)
{
    SQLGenerator sqlGen;

    int c, index;

    while((c = getopt(argc, argv, "f:d:s:n:cteh")) != -1)
    {
        switch (c)
        {
        case 'n':
            if(std::string(optarg).compare("filename") == 0)
            {
                sqlGen.UseFilenameAsDB();
            }
            else
            {
                sqlGen.SetTableName(std::string(optarg));
            }
            break;
        case 's':
            sqlGen.SetDatabaseName(std::string(optarg));
            break;
        case 'e':
            sqlGen.EnableIfNotExists();
            break;
        case 'c':
            sqlGen.EnableCreateDatabaseCommand();
            break;
        case 't':
            sqlGen.EnableCreateTableCommand();
            break;
        case 'f':
            sqlGen.AppendFile(std::string(optarg));
            break;
        case 'd':
            sqlGen.OpenDirectory(std::string(optarg));
            break;
        case 'h':
            print_usage();
            break;
        default:
            abort();
        }
    }

    for (index = optind; index < argc; index++) printf ("Non-option argument %s\n", argv[index]);
    sqlGen.GenerateSQL();
    return 0;
}