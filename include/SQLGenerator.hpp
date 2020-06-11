/*
 * SQLGenerator.hpp
 * Created by David Baranyai
 * 2020.06.11
 */

#ifndef SQLGenerator_hpp
#define SQLGenerator_hpp

#include <iostream>
#include <fstream>
#include <string>
#include <ctime> 
#include <vector>
#include <filesystem>
#include "TFile.h"
#include "TTree.h"
#include "TTreeReader.h"
#include "TObjArray.h"

#define SQL_DEFAULT_DATATYPE "double"
#define SQL_INT_TYPE "int"

struct ColTypes
{
    std::string col_name;
    std::string col_type;
    double d_value;
    int i_value;
};

class SQLGenerator
{
private:
    /* data */
    TFile *inFile;
    std::ofstream outFile;
    TTree *theTree = nullptr;
    std::vector<std::filesystem::path> filePathList;
    std::vector<ColTypes> cols;

    std::string table = "USP";
    std::string database = "USP";
    bool useCreateDatabaseCommand = false;
    bool useCreateTableCommand = false;
    bool useFilenameAsDB = false;
    bool useIfnotExists = false;

    std::string GetCurrentFormattedDate();
    bool OpenTFile(std::filesystem::path);
    void Close();

    void GenerateCDCommand(); //CREATE DATABASE
    void GenerateCTCommand(); //CREATE TABLE
    void GenerateIICommand(); //INSERT INTO
public:
    SQLGenerator();
    ~SQLGenerator();

    void EnableCreateDatabaseCommand();
    void EnableCreateTableCommand();
    void EnableIfNotExists();
    void UseFilenameAsDB();
    void SetTableName(std::string);
    void SetDatabaseName(std::string);
    void OpenDirectory(std::string);
    void AppendFile(std::string);
    void GenerateSQL();
};


#endif //SQLGenerator.hpp