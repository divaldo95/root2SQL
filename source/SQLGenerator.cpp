/*
 * SQLGenerator.hpp
 * Created by David Baranyai
 * 2020.06.11
 */
#include "SQLGenerator.hpp"

SQLGenerator::SQLGenerator()
{
}

std::string SQLGenerator::GetCurrentFormattedDate()
{
    time_t rawtime;
    struct tm* timeinfo;
    char buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime (buffer,20,"%Y.%m.%d %T",timeinfo);
    return std::string(buffer);
}

SQLGenerator::~SQLGenerator()
{
}

bool SQLGenerator::OpenTFile(std::filesystem::path path)
{
    inFile = TFile::Open(path.c_str());
    path.replace_extension(".sql");
    outFile.open(path);
    path.replace_extension(".root");
    if(outFile.is_open())
    {
        outFile << "/*\nSQL file generated from " << path.filename() << " on " << GetCurrentFormattedDate() << "\n*/\nSTART TRANSACTION;\n";
        //return true;
    }
    else
    {
        std::cout << "Failed opening the output file for " << path.filename() << std::endl;
        return false;
    }
    ColTypes c;
    inFile->GetObject("qc_soh_tree", theTree);
    TObjArray *branchList = theTree->GetListOfBranches();
    for(int i = 0; i < branchList->GetEntries(); i++) //the last line is different
    {
        c.col_name = branchList->At(i)->GetName();
        if(c.col_name.find("timestamp") != std::string::npos) c.col_type = SQL_INT_TYPE;
        else c.col_type = SQL_DEFAULT_DATATYPE;
        cols.push_back(c);
    }
    return true;
}

void SQLGenerator::EnableCreateDatabaseCommand()
{
    useCreateDatabaseCommand = true;
    std::cout << "Enabled CREATE DATABASE generation" << std::endl;
}

void SQLGenerator::EnableCreateTableCommand()
{
    useCreateTableCommand = true;
    std::cout << "Enabled CREATE TABLE generation" << std::endl;
}

void SQLGenerator::EnableIfNotExists()
{
    useIfnotExists = true;
    std::cout << "Using IF NOT EXISTS on CREATE statements" << std::endl;
}

void SQLGenerator::UseFilenameAsDB()
{
    useFilenameAsDB = true;
    std::cout << "Using file name as table" << std::endl;
}

void SQLGenerator::SetTableName(std::string tablename)
{
    table = tablename;
    useFilenameAsDB = false;
    std::cout << "Using " << table << " as table name" << std::endl;
}

void SQLGenerator::SetDatabaseName(std::string dbname)
{
    database = dbname;
    std::cout << "Using " << dbname << " as database name" << std::endl;
}

void SQLGenerator::OpenDirectory(std::string path)
{
    if(!std::filesystem::exists(std::filesystem::path(path)))
    {
        std::cout << "Directory does not exists. Skipping." << std::endl;
        return;
    }

    for (const auto & entry : std::filesystem::directory_iterator(path))
    {
        //std::cout << entry.path() << std::endl;
        if(entry.path().extension().compare(".root") == 0) //only read ROOT files
        {
            //std::cout << "Extension check: " << entry.path() << std::endl;
            filePathList.push_back(entry.path());
        }
    }
}

void SQLGenerator::AppendFile(std::string filename)
{
    std::filesystem::path path(filename);
    if(!std::filesystem::exists(path))
    {
        std::cout << "File does not exists. Skipping." << std::endl;
        return;
    }
    if(path.extension().compare(".root") != 0)
    {
        std::cout << "WARNING: File extension is not .root" << std::endl;
    }
    filePathList.push_back(path);
}

void SQLGenerator::Close()
{
    if(inFile->IsOpen()) inFile->Close();
    if(outFile.is_open()) outFile.close();
}

void SQLGenerator::GenerateCDCommand()
{
    outFile << "CREATE DATABASE ";
    if(useIfnotExists) outFile << "IF NOT EXISTS ";
    outFile << database << ";" << std::endl;
}

void SQLGenerator::GenerateCTCommand()
{
    outFile << "CREATE TABLE ";
    if(useIfnotExists) outFile << "IF NOT EXISTS ";
    outFile << table << "(\n\tID INT AUTO_INCREMENT PRIMARY KEY,\n";
    for(int i = 0; i < cols.size() - 1; i++) //the last line is different
    {
        //std::cout << branchList->At(i)->GetName() << std::endl;
        outFile << "\t" << cols[i].col_name << " " << cols[i].col_type << ",\n";
    }
    outFile << "\t" << cols[cols.size() - 1].col_name << " " << cols[cols.size() - 1].col_type << "\n);\n";
}

void SQLGenerator::GenerateIICommand()
{
    for(int i = 0; i < cols.size(); i++)
    {
        if(cols[i].col_type.compare(SQL_INT_TYPE) == 0)
        {
            theTree->SetBranchAddress(cols[i].col_name.c_str(), &cols[i].i_value);
        }
        else
        {
            theTree->SetBranchAddress(cols[i].col_name.c_str(), &cols[i].d_value);
        }
    }
    for(int i = 0; i < theTree->GetEntries(); i++)
    {
        theTree->GetEntry(i);
        outFile << "INSERT INTO " << table << " (";
        for(int j = 0; j < cols.size() - 1; j++)
        {
            outFile << cols[j].col_name <<  ", ";
        }
        outFile << cols[cols.size() - 1].col_name << ")\n\t";
        outFile << " VALUES (";
        for(int j = 0; j < cols.size() - 1; j++)
        {
            if(cols[j].col_type.compare(SQL_INT_TYPE) == 0) outFile << cols[j].i_value << ", ";
            else outFile << cols[j].d_value << ", ";
        }
        if(cols[cols.size() - 1].col_type.compare(SQL_INT_TYPE) == 0) outFile << cols[cols.size() - 1].i_value << ");\n";
        else outFile << cols[cols.size() - 1].d_value << ");\n";
    }
}

void SQLGenerator::GenerateSQL()
{
    if(filePathList.size() == 0)
    {
        std::cout << "ROOT file list is empty" << std::endl;
        return;
    }
    for(int i = 0; i < filePathList.size(); i++)
    {
        if(!OpenTFile(filePathList[i]))
        {
            Close();
            continue; //skip file if can't be opened
        }
        if(useFilenameAsDB)
        {
            table = filePathList[i].stem();
        }
        if(useCreateDatabaseCommand)
        {
            GenerateCDCommand();
        }
        outFile << "USE " << database << ";\n"; //select the database
        if(useCreateTableCommand)
        {
            GenerateCTCommand();
        }
        GenerateIICommand();
        outFile << "COMMIT;";
        Close();
        theTree = nullptr;
        cols.clear();
    }
}