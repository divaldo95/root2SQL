# root2SQL

## Description
This script convert .root files into SQL scripts. Written for PHENIX experiment.

## Prerequisities
* CERN Root (tested on 6.20/04)
* C++17 compatible compiler
* CMake

## Parameters
Simply running the code doesn't do anything. You need to specify at least one file or a directory which contains root files. NOTHING will prevent you from running the same generated script multiple times, so be careful, because you will end up with duplicated values. The output (.sql) files are located near the original ones, this can't be changed this time. Simply import it. The current syntax is for MySQL/MariaDB.
- `-f rootfile.root` Specify a root file. Can be used multiple times to specify more than one file.
- `-d root_files_dir/` Specify a directory which contains the root files. Every file from that directory with .root extension will be opened and converted. Also can be used multiple times for adding more than one directory.
- `-n table_name` Specify the table name. Data will be inserted into that table. If CREATE TABLE statement is enabled, it will also use this name. This can be set to the file name with `-n filename`. In this case, every input file will use the same table as it's name. If you want to import multiple files into one table, set a common name. By default it is set to USP.
- `-s databasename` Set the database name to the given value. By default it is set to USP. CREATE DATABASE statement will also use this value.
- `-c` Enable generating CREATE DATABASE statements. If you don't have the database (schema) created, you should use this.
- `-t` Enable generating CREATE TABLE statements. If you don't have the tables created, you should use this.
- `-e` Enable IF NOT EXISTS statements on CREATE statements. If the database and tables are already present, this will prevent the script to fail. Better idea to turn off all CREATE statement after the database and tables are set, so it will not cause any trouble.

## Build
 - `git clone https://github.com/divaldo95/root2SQL.git`
 - `cd root2SQL && mkdir build && cd build`
 - `cmake ../`
 - `make -jN` Where N is the number of jobs to run simultaneously

## Sample runs
Generating an SQL script which uses 'test' as SCHEMA and uses 'testtable' as TABLE NAME with two input files set with -f
```
./root2SQL -f rootfile1.root -f rootfile2.root -n testtable -s test
```
Generating an SQL script which uses 'test' as SCHEMA and uses 'testtable' as TABLE NAME with files from a specified directory
```
./root2SQL -d rootfiles/ -n testtable -s test
```
Generating an SQL script which uses 'test' as SCHEMA and uses 'testtable' as TABLE NAME with files from a specified directory, with enabled CREATE statements and with IF NOT EXISTS statement.
```
./root2SQL -d rootfiles/ -n testtable -s test -c -t -e
```

# Changelog
## 2020-06-14
* Temporary branch using experimental/filesystem
* Fixed build on Ubuntu by CMakeLists changes (Root does not like c++17 standards set up by cmake, tested on Ubuntu 18.04)
* Added detector description table

## 2020-06-12
* Initial version