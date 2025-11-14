HinLIBS Deliverable 1 - README
Team #: 89
Course: COMP 3004 Fall 2025
Project: Hintonville Library Information and Borrowing System (HinLIBS)
Date: 2025-11-13

Team Members

Xiaoqian Dou - 101192697
Jamal Ismail - 101240336
Henry Farnsworth - 101299604


## Installation, Build, Launch Instructions
### 1. Extract the Project
run the following in terminal

unzip COMP3004GroupProject.zip
cd COMP3004GroupProject


### 2. Open in Qt Creator
1. Launch Qt Creator on the course VM
2. Click **File -> Open File or Project**
3. Navigate to the extracted folder
4. Select the `COMP3004GroupProjectLocal.pro` file
5. Click **Open**

### 3. Build the Project
1. In Qt Creator, click **Build -> Build Project**
2. Wait for compilation to complete

### 4. Run the Application
1. Click **Build -> Run**
2. The HinLIBS application window should launch



### Pre-loaded Users (7 total)

#### Patrons (5)
1. **Name:** Alice
2. **Name:** Bob
3. **Name:** Carmen
4. **Name:** Diego
5. **Name:** Eva

#### Librarian (1)
- **Name:** Liam

#### System Administrator (1)
- **Name:** Sara



## User Guide

To start
1. Launch the application
2. Enter a valid username or full name from the list above
3. Click **Login** or press Enter
4. The appropriate interface will load based on user type

Once logged in, navigate the library catalogue to view available books and perform actions permitted for your role (eg, borrowing, returning, or managing inventory).


## Design Pattern Implementation

This project implements the observer design pattern. For details on the pattern selection, justification, and UML class diagram, please refer to the accompanying  document: `D1 - Design Pattern and Class Diagram.pdf`