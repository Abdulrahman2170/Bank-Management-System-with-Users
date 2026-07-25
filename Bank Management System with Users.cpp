#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <vector>

using namespace std;

//=========================================
//              Constants
//=========================================

const string ClientsFileName = "Clients.txt";
const string UsersFileName = "Users.txt";

//=========================================
//              Structures
//=========================================

struct sClient
{
	string AccountNumber;
	string PinCode;
	string Name;
	string Phone;
	double AccountBalance = 0;
	bool MarkDelete = false;
};
struct sUser
{
	string Username;
	string Password;
	short Permission;
	bool MarkForDelete = false;
};
sUser CurrentUser;

//=========================================
//                Enums
//=========================================

enum enMainMenuOptions
{
	ShowClientList = 1,
	AddNewClientt = 2,
	DeleteClient = 3,
	UpdateClient = 4,
	FindClient = 5,
	Transactions = 6,
	ManageUsers = 7,
	Logout = 8
};
enum enTransactionsMenuOptions
{
	Deposit = 1,
	Withdraw = 2,
	TotalBalance = 3,
	MainMenu = 4
};
enum enMainMenuPermissions
{
	pAll = -1,
	pListClients = 1,
	pAddNewClient = 2,
	pDeleteClient = 4,
	pUpdateClients = 8,
	pFindClient = 16,
	pTransactions = 32,
	pManageUsers = 64
};
enum enManageUsersMenuOptions
{
	eListUsers = 1,
	eAddNewUser = 2,
	eDeleteUser = 3,
	eUpdateUser = 4,
	eFindUser = 5,
	eMainMenu = 6,
};

//=========================================
//          Helper Functions
//=========================================

vector<string> SplitString(string S1, string Delim)
{
	vector <string> vString;
	size_t pos = 0;
	string sWord;
	while ((pos = S1.find(Delim)) != std::string::npos)
	{
		sWord = S1.substr(0, pos);
		if (sWord != "")
		{
			vString.push_back(sWord);

		}
		S1.erase(0, pos + Delim.length());
	}
	if (S1 != "")
	{
		vString.push_back(S1);
	}
	return vString;
}
string ConvertClientRecordToLine(const sClient& Client, string Separator = "#//#")
{
	string stClientRecord;
	stClientRecord += Client.AccountNumber + Separator;
	stClientRecord += Client.PinCode + Separator;
	stClientRecord += Client.Name + Separator;
	stClientRecord += Client.Phone + Separator;
	stClientRecord += to_string(Client.AccountBalance);
	return stClientRecord;
}
string ConvertUserRecordToLine(const sUser& User, string Separator = "#//#")
{
	string stUserRecord;
	stUserRecord += User.Username + Separator;
	stUserRecord += User.Password + Separator;
	stUserRecord += to_string(User.Permission);
	return stUserRecord;
}
sClient ConvertLineToClientRecord(string Line, string Separator = "#//#")
{
	sClient Client;
	vector<string> vClientData;
	vClientData = SplitString(Line, Separator);
	Client.AccountNumber = vClientData[0];
	Client.PinCode = vClientData[1];
	Client.Name = vClientData[2];
	Client.Phone = vClientData[3];
	Client.AccountBalance = stod(vClientData[4]);
	return Client;
}
sUser ConvertLineToUserRecord(string Line, string Separator = "#//#")
{
	sUser User;
	vector<string> vUserData;
	vUserData = SplitString(Line, Separator);
	User.Username = vUserData[0];
	User.Password = vUserData[1];
	User.Permission = stod(vUserData[2]);
	return User;
}

//=========================================
//          Find Functions
//=========================================

bool FindClientByAccountNumber(string AccountNumber, const vector<sClient>& vClients, sClient& Client)
{
	for (const sClient& C : vClients)
	{
		if (C.AccountNumber == AccountNumber)
		{
			Client = C;
			return true;
		}
	}

	return false;
}
bool FindUserByUsername(string Username, const vector<sUser>& vUsers, sUser& User)
{
	for (const sUser& U : vUsers)
		if (U.Username == Username)
		{
			User = U;
			return true;
		}
	return false;
}

//=========================================
//          Read Functions
//=========================================

string ReadAccountNumber()
{
	string AccountNumber = "";
	cout << "\nPlease Enter Account Number? ";
	cin >> AccountNumber;
	return AccountNumber;
}
sClient ReadNewClient()
{
	sClient Client;
	cout << "Enter Account Number:";
	getline(cin >> ws, Client.AccountNumber);
	cout << "Enter PinCode:";
	getline(cin, Client.PinCode);
	cout << "Enter Name";
	getline(cin, Client.Name);
	cout << "Enter Phone:";
	getline(cin, Client.Phone);
	cout << "Enter Account Balanc:";
	cin >> Client.AccountBalance;
	return Client;
}
string ReadUsername()
{
	string Username;

	cout << "Please Enter Username: ";
	getline(cin >> ws, Username);

	return Username;
}
bool ReadAnswer(string Message)
{
	char Answer = 'n';

	cout << "\nDo you want to give access to:\n";
	cout << Message << "? y/n? ";
	cin >> Answer;

	return (Answer == 'Y' || Answer == 'y');
}
short ReadPermissionsToSet()
{
	short Permissions = 0;

	if (ReadAnswer("Full Access"))
		return pAll;

	if (ReadAnswer("Show Client List"))
		Permissions |= pListClients;

	if (ReadAnswer("Add New Client"))
		Permissions |= pAddNewClient;

	if (ReadAnswer("Delete Client"))
		Permissions |= pDeleteClient;

	if (ReadAnswer("Update Client"))
		Permissions |= pUpdateClients;

	if (ReadAnswer("Find Client"))
		Permissions |= pFindClient;

	if (ReadAnswer("Transactions"))
		Permissions |= pTransactions;

	if (ReadAnswer("Manage Users"))
		Permissions |= pManageUsers;

	return Permissions;
}
sUser ReadNewUser(const vector<sUser>& vUsers)
{
	sUser User;
	sUser TempUser;

	do
	{
		cout << "Enter Username: ";
		getline(cin >> ws, User.Username);

		if (FindUserByUsername(User.Username, vUsers, TempUser))
			cout << "\nUser with [" << User.Username << "] already exists, Enter another Username.\n";

	} while (FindUserByUsername(User.Username, vUsers, TempUser));

	cout << "Enter Password: ";
	getline(cin, User.Password);

	User.Permission = ReadPermissionsToSet();

	return User;
}
enManageUsersMenuOptions ReadManageUsersMenuoption()
{
	short Choice = 0;
	cout << "Choose what do you want to do? [ 1 to 6 ]?";
	cin >> Choice;
	return (enManageUsersMenuOptions)Choice;
}

//=========================================
//          File Functions
//=========================================

vector<sClient> LoadClientsDatafromFile(string FileName)
{
	vector<sClient> vClients;
	fstream MyFile;
	MyFile.open(FileName, ios::in);
	if (MyFile.is_open())
	{
		sClient Client;
		string Line;
		while (getline(MyFile, Line))
		{
			Client = ConvertLineToClientRecord(Line);
			vClients.push_back(Client);
		}

		MyFile.close();
	}
	return vClients;
}

vector<sUser> LoadUsersDatafromFile(string FileName)
{
	vector<sUser> vUsers;
	fstream MyFile;
	MyFile.open(FileName, ios::in);
	if (MyFile.is_open())
	{
		sUser User;
		string Line;
		while (getline(MyFile, Line))
		{
			User = ConvertLineToUserRecord(Line);
			vUsers.push_back(User);
		}
		MyFile.close();
	}
	return vUsers;
}
void SaveClientsDataToFile(string FileName, const vector<sClient>& vClients)
{
	fstream MyFile;
	MyFile.open(FileName, ios::out);
	string DataLine;
	if (MyFile.is_open())
	{
		for (const sClient& C : vClients)
		{
			if (C.MarkDelete == false)
			{
				DataLine = ConvertClientRecordToLine(C);
				MyFile << DataLine << endl;
			}
		}
		MyFile.close();
	}

}
void SaveUsersDataToFile(string FileName, const vector<sUser>& vUsers)
{
	fstream MyFile;
	MyFile.open(FileName, ios::out);
	string DataLine;
	if (MyFile.is_open())
	{
		for (const sUser& U : vUsers)
		{
			if (U.MarkForDelete == false)
			{
				DataLine = ConvertUserRecordToLine(U);
				MyFile << DataLine << endl;
			}
		}
		MyFile.close();
	}

}
void AddDataLineToFile(string FileName, string stDataLine)
{
	fstream MyFile;
	MyFile.open(FileName, ios::out | ios::app);
	if (MyFile.is_open())
	{
		MyFile << stDataLine << endl;
		MyFile.close();
	}
}
void AddDataLineToUsersFile(string FileName, string stDataLine)
{
	fstream MyFile;
	MyFile.open(FileName, ios::out | ios::app);
	if (MyFile.is_open())
	{
		MyFile << stDataLine << endl;
		MyFile.close();
	}
}

//=========================================
//          More Find Functions
//=========================================

bool FindUserByUsernameAndPassword(string Username, string Password, sUser& User)
{
	vector<sUser> vUsers = LoadUsersDatafromFile(UsersFileName);

	for (const sUser& U : vUsers)
	{
		if (U.Username == Username &&
			U.Password == Password)
		{
			User = U;
			return true;
		}
	}

	return false;
}

//=========================================
//          Mark & Access Functions
//=========================================

bool MarkClientForDeleteByAccountNumber(string AccountNumber, vector <sClient>& vClients)
{
	for (sClient& C : vClients)
	{
		if (C.AccountNumber == AccountNumber)
		{
			C.MarkDelete = true;
			return true;
		}
	}
	return false;
}
bool MarkUserForDeleteByUsername(string Username, vector <sUser>& vUsers)
{
	for (sUser& U : vUsers)
	{
		if (U.Username == Username)
		{
			U.MarkForDelete = true;
			return true;
		}
	}
	return false;
}
bool CheckAccessPermission(enMainMenuPermissions Permission)
{
	if (CurrentUser.Permission == pAll)
		return true;

	return (CurrentUser.Permission & Permission);
}

//=========================================
//          Print Functions
//=========================================

string GetPermissionName(short Permission)
{
	if (Permission == pAll)
		return "Full Access";
	else
		return "Custom";
}
void PrintClientRecord(const sClient& Client)
{
	cout << "| " << setw(15) << left << Client.AccountNumber;
	cout << "| " << setw(10) << left << Client.PinCode;
	cout << "| " << setw(40) << left << Client.Name;
	cout << "| " << setw(12) << left << Client.Phone;
	cout << "| " << setw(12) << left << Client.AccountBalance;
}
void PrintUserRecord(const sUser& User)
{
	cout << "| " << setw(15) << left << User.Username;
	cout << "| " << setw(20) << left << User.Password;
	cout << "| " << setw(15) << left << GetPermissionName(User.Permission);
}
void PrintClientRecordForTransections(sClient Client)
{

	cout << "\n--------------------------------------------\n";
	cout << "Account Number : " << Client.AccountNumber << endl;
	cout << "Pin Code       : " << Client.PinCode << endl;
	cout << "Name           : " << Client.Name << endl;
	cout << "Phone          : " << Client.Phone << endl;
	cout << "Balance        : " << Client.AccountBalance << endl;
	cout << "--------------------------------------------\n";
}
void PrintAllClientsData(const vector<sClient>& vClients)
{
	cout << "\n\t\t\t\t\tClient List (" << vClients.size() << ")Client(s).";
	cout << "\n_______________________________________________________";
	cout << "_________________________________________\n" << endl;
	cout << "| " << left << setw(15) << "Account Number";
	cout << "| " << left << setw(10) << "Pin Code";
	cout << "| " << left << setw(40) << "Client Name";
	cout << "| " << left << setw(12) << "Phone";
	cout << "| " << left << setw(12) << "Balance";
	cout << "\n_______________________________________________________";
	cout << "_________________________________________\n" << endl;
	for (const sClient& Client : vClients)
	{
		PrintClientRecord(Client);
		cout << endl;
	}
	cout << "\n_______________________________________________________";
	cout << "_________________________________________\n" << endl;

}
void PrintAllUsersData(const vector<sUser>& vUsers)
{
	cout << "\n\t\t\t\t\tUsers List (" << vUsers.size() << ") User(s).";
	cout << "\n_______________________________________________________";
	cout << "_________________________________________\n" << endl;
	cout << "| " << left << setw(15) << "Username";
	cout << "| " << left << setw(20) << "Password";
	cout << "| " << left << setw(20) << "Permissions";
	cout << "\n_______________________________________________________";
	cout << "_________________________________________\n" << endl;
	for (const sUser& User : vUsers)
	{
		PrintUserRecord(User);
		cout << endl;
	}
	cout << "\n_______________________________________________________";
	cout << "_________________________________________\n" << endl;

}
void ShowTotalBalances(const vector<sClient>& vClients)
{
	double TotalBalances = 0;

	cout << "===============================================\n";
	cout << "\t\t\tBalances List (" << vClients.size() << ") Client(s).";
	cout << "\n===============================================\n";

	cout << "_______________________________________________________\n";

	cout << "| " << left << setw(15) << "Account Number";
	cout << "| " << left << setw(40) << "Client Name";
	cout << "| " << left << setw(12) << "Balance";

	cout << "\n_______________________________________________________\n";

	for (const sClient& Client : vClients)
	{
		cout << "| " << left << setw(15) << Client.AccountNumber;
		cout << "| " << left << setw(40) << Client.Name;
		cout << "| " << left << setw(12) << Client.AccountBalance << endl;

		TotalBalances += Client.AccountBalance;
	}

	cout << "_______________________________________________________\n";

	cout << "\n\t\t\tTotal Balances = " << TotalBalances << endl;
	system("pause>0");
}

//=========================================
//          Update Helpers & Functions
//=========================================

sClient ChangeClientRecord(string AccountNumber)
{
	sClient Client;
	Client.AccountNumber = AccountNumber;
	cout << "\n\nEnter PinCode? ";
	getline(cin >> ws, Client.PinCode);
	cout << "Enter Name? ";
	getline(cin, Client.Name);
	cout << "Enter Phone? ";
	getline(cin, Client.Phone);
	cout << "Enter AccountBalance? ";
	cin >> Client.AccountBalance;
	return Client;
}
bool UpdateClientByAccountNumber(string AccountNumber, vector<sClient>& vClients)
{
	sClient Client;
	char Answer = 'n';
	if (FindClientByAccountNumber(AccountNumber, vClients, Client))
	{
		PrintClientRecord(Client);
		cout << "\n\nAre you sure you want update this client? y/n? ";
		cin >> Answer;
		if (Answer == 'y' || Answer == 'Y')
		{
			for (sClient& C : vClients)
			{
				if (C.AccountNumber == AccountNumber)
				{
					C = ChangeClientRecord(AccountNumber);
					break;
				}
			}
			SaveClientsDataToFile(ClientsFileName, vClients);
			cout << "\n\nClient Updated Successfully.";
			return true;
		}
		return false;
	}
	else
	{
		cout << "\nClient with Account Number (" << AccountNumber << ") is Not Found!";
		return false;
	}
}
void ReadUserInfo(sUser& User)
{
	cout << "\nEnter Password: ";
	getline(cin >> ws, User.Password);

	User.Permission = ReadPermissionsToSet();
}
void UpdateClientBalanceByAccountNumber(string AccountNumber, double Amount, vector<sClient>& vClients)
{
	for (sClient& C : vClients)
	{
		if (C.AccountNumber == AccountNumber)
		{
			C.AccountBalance += Amount;
			break;
		}
	}
}
bool UpdateUserByUsername(string Username, vector<sUser>& vUsers)
{
	sUser User;
	char Answer = 'n';

	if (FindUserByUsername(Username, vUsers, User))
	{
		if (User.Username == "Admin")
		{
			cout << "\nYou cannot update Admin Username.";
			return false;
		}

		PrintUserRecord(User);

		cout << "\n\nAre you sure you want to update this user? Y/N? ";
		cin >> Answer;

		if (Answer == 'Y' || Answer == 'y')
		{
			ReadUserInfo(User);

			for (sUser& U : vUsers)
			{
				if (U.Username == Username)
				{
					U = User;
					break;
				}
			}

			SaveUsersDataToFile(UsersFileName, vUsers);

			cout << "\n\nUser Updated Successfully.\n";
			return true;
		}

		return false;
	}

	cout << "\nUser with Username (" << Username << ") is Not Found!\n";
	return false;
}

//=========================================
//          Add Functions
//=========================================

void AddNewClient()
{
	sClient Client = ReadNewClient();
	AddDataLineToFile(ClientsFileName, ConvertClientRecordToLine(Client));
}
void AddNewUser()
{
	vector<sUser> vUsers = LoadUsersDatafromFile(UsersFileName);

	sUser User = ReadNewUser(vUsers);

	AddDataLineToUsersFile(UsersFileName, ConvertUserRecordToLine(User));
}
void AddClients()
{
	char addmore = 'Y';
	do
	{
		system("cls");
		cout << "Adding New Client:\n\n";
		AddNewClient();
		cout << "\nUser Added Successfully, do you want to add more users ? Y / N ? ";
		cin >> addmore;
	} while (toupper(addmore) == 'Y');
}
void AddUsers()
{
	char addmore = 'Y';
	do
	{
		system("cls");
		cout << "Adding New User:";
		AddNewUser();
		cout << "User added successfully,do u want to add more User? Y/N?";
		cin >> addmore;
	} while (toupper(addmore) == 'Y');
}

//=========================================
//          Delete Functions
//=========================================

bool DeleteClientByAccountNumber(string AccountNumber, vector<sClient>& vClients)
{
	sClient Client;
	char Answer = 'n';
	if (FindClientByAccountNumber(AccountNumber, vClients, Client))
	{
		PrintClientRecord(Client);
		cout << "\n\nAre you sure you want delete this client? y/n ? ";
		cin >> Answer;
		if (Answer == 'y' || Answer == 'Y')
		{
			MarkClientForDeleteByAccountNumber(AccountNumber,
				vClients);
			SaveClientsDataToFile(ClientsFileName, vClients);

			vClients = LoadClientsDatafromFile(ClientsFileName);
			cout << "\n\nClient Deleted Successfully.";
			return true;
		}
		return false;
	}
	else
	{
		cout << "\nClient with Account Number (" << AccountNumber << ") is Not Found!";
		return false;
	}
}
bool DeleteUserByUsername(string Username, vector<sUser>& vUsers)
{
	sUser User;
	char Answer = 'n';
	if (FindUserByUsername(Username, vUsers, User))
	{
		if (User.Username == "Admin")
		{
			cout << "\nYou cannot delete this user.";
			return false;
		}
		PrintUserRecord(User);
		cout << "\n\nAre you sure you want to delete this user? Y/N? ";
		cin >> Answer;
		if (Answer == 'y' || Answer == 'Y')
		{
			MarkUserForDeleteByUsername(Username, vUsers);
			SaveUsersDataToFile(UsersFileName, vUsers);

			vUsers = LoadUsersDatafromFile(UsersFileName);
			cout << "\n\nUser Deleted Successfully.";
			return true;
		}
		return false;
	}
	else
	{
		cout << "\nUser with Username (" << Username << ") is Not Found!";
		return false;
	}
}

//=========================================
//          Transaction Functions
//=========================================

bool ConfirmTransaction()
{
	char Answer;

	cout << "\nAre you sure you want to perform this transaction? (Y/N)? ";
	cin >> Answer;

	return (Answer == 'Y' || Answer == 'y');
}
void ShowDepositScreen()
{
	cout << "\n===========================================\n";
	cout << "\t\tDeposit Screen\n";
	cout << "===========================================\n";
}
void ShowWithdrawScreen()
{
	cout << "\n===========================================\n";
	cout << "\t\tWithdraw Screen\n";
	cout << "===========================================\n";
}
void DepositBalanceToclientByAccountNumber()
{
	ShowDepositScreen();
	string AccountNumber = ReadAccountNumber();
	vector<sClient> vClients = LoadClientsDatafromFile(ClientsFileName);
	sClient Client;
	if (!FindClientByAccountNumber(AccountNumber, vClients, Client))
	{
		cout << "\nClient with Account Number (" << AccountNumber << ") is Not Found!";
		return;
	}
	PrintClientRecordForTransections(Client);
	double Amount;
	cout << "Please Enter Deposit Amount?";
	cin >> Amount;

	if (ConfirmTransaction())
	{
		UpdateClientBalanceByAccountNumber(AccountNumber, Amount, vClients);

		FindClientByAccountNumber(AccountNumber, vClients, Client);

		PrintClientRecordForTransections(Client);

		SaveClientsDataToFile(ClientsFileName, vClients);

		cout << "\nDone Successfully.\n";
	}
}
void WithdrawBalanceToclientByAccountNumber()
{
	ShowWithdrawScreen();
	string AccountNumber = ReadAccountNumber();
	vector<sClient> vClients = LoadClientsDatafromFile(ClientsFileName);
	sClient Client;
	if (!FindClientByAccountNumber(AccountNumber, vClients, Client))
	{
		cout << "\nClient with Account Number (" << AccountNumber << ") is Not Found!";
		return;
	}
	PrintClientRecordForTransections(Client);
	double Amount;
	cout << "Please Enter Withdraw Amount? ";
	cin >> Amount;

	while (Amount > Client.AccountBalance)
	{
		cout << "\nAmount exceeds the balance, you can withdraw up to: "
			<< Client.AccountBalance << endl;

		cout << "Please Enter Withdraw Amount? ";
		cin >> Amount;
	}


	if (ConfirmTransaction())
	{
		UpdateClientBalanceByAccountNumber(AccountNumber, -Amount, vClients);

		FindClientByAccountNumber(AccountNumber, vClients, Client);

		PrintClientRecordForTransections(Client);

		SaveClientsDataToFile(ClientsFileName, vClients);

		cout << "\nDone Successfully.\n";
	}
}

//=========================================
//          Screen Functions
//=========================================

void ShowTransactionsMenuScreen()
{
	cout << "============================================\n";
	cout << "\tTransactions Menu Screen\n";
	cout << "============================================\n";
	cout << "\t[1] Deposit.\n";
	cout << "\t[2] Withdraw.\n";
	cout << "\t[3] Total Balance.\n";
	cout << "\t[4] Main Menu.\n";
	cout << "============================================\n";
}
void ShowFindClientScreen(const vector<sClient>& vClients)
{
	sClient Client;
	string AccountNumber = ReadAccountNumber();
	if (FindClientByAccountNumber(AccountNumber, vClients, Client))
	{
		cout << "\nThe following are the client details:\n";
		PrintClientRecord(Client);
	}
	else
	{
		cout << "\nClient with Account Number (" << AccountNumber << ") is Not Found!";
	}
}
void ShowAddNewClientsScreen()
{
	cout << "\n-----------------------------------------\n";
	cout << "\tAdd New Clients Screen";
	cout << "\n-----------------------------------------\n";

	AddClients();
}
void ShowListUsersScreen()
{
	vector<sUser> vUsers = LoadUsersDatafromFile(UsersFileName);
	PrintAllUsersData(vUsers);
}
void ShowFindUserScreen()
{
	vector<sUser> vUsers = LoadUsersDatafromFile(UsersFileName);

	sUser User;
	string Username = ReadUsername();

	if (FindUserByUsername(Username, vUsers, User))
	{
		cout << "\nThe following are the user details:\n";
		PrintUserRecord(User);
	}
	else
	{
		cout << "\nUser with Username (" << Username << ") is Not Found!\n";
	}
}
void ShowAddNewUsersScreen()
{
	cout << "\n-----------------------------------\n";
	cout << "\tAdd New Users Screen";
	cout << "\n-----------------------------------\n";

	AddUsers();
}
void ShowAccessDeniedMessage()
{
	system("cls");

	cout << "\n------------------------------------\n";
	cout << "\tAccess Denied!\n";
	cout << "------------------------------------\n";
	cout << "You don't have permission to perform\n";
	cout << "this action.\n";
	cout << "Please contact your administrator.\n";
}
void ShowDeleteClientScreen(vector<sClient>& vClients)
{
	cout << "\n-------------------------------------\n";
	cout << "\tDelete Client Screen";
	cout << "\n-------------------------------------\n";

	string AccountNumber = ReadAccountNumber();
	DeleteClientByAccountNumber(AccountNumber, vClients);
}
void ShowUpdateClientScreen(vector<sClient>& vClients)
{
	cout << "\n-------------------------------------\n";
	cout << "\tUpdate Client Screen";
	cout << "\n-------------------------------------\n";

	string AccountNumber = ReadAccountNumber();

	UpdateClientByAccountNumber(AccountNumber, vClients);
}
void ShowDeleteUserScreen()
{
	cout << "\n-----------------------------------\n";
	cout << "\tDelete User Screen";
	cout << "\n-----------------------------------\n";

	vector<sUser> vUsers = LoadUsersDatafromFile(UsersFileName);

	string Username = ReadUsername();

	DeleteUserByUsername(Username, vUsers);
}
void ShowUpdateUserScreen()
{
	cout << "\n-----------------------------------\n";
	cout << "\tUpdate User Screen";
	cout << "\n-----------------------------------\n";

	vector<sUser> vUsers = LoadUsersDatafromFile(UsersFileName);
	string Username = ReadUsername();

	UpdateUserByUsername(Username, vUsers);
}
void ShowManageUsersMenuScreen()
{
	cout << "============================================\n";
	cout << "\t Manage Users Menu Screen\n";
	cout << "============================================\n";
	cout << "\t[1] List Users.\n";
	cout << "\t[2] Add New User.\n";
	cout << "\t[3] Delete User.\n";
	cout << "\t[4] Update User.\n";
	cout << "\t[5] Find User.\n";
	cout << "\t[6] Main Menu.\n";
	cout << "============================================\n";
}
void ShowEndScreen()
{
	cout << "\n---------------------------------\n";
	cout << "\tProgram Ends :-)";
	cout << "\n---------------------------------\n";
}

//=========================================
//          Menus
//=========================================

short ReadMainMenuOption()
{
	short Choice;

	cout << "Choose what do you want to do? [1 to 7]? ";
	cin >> Choice;

	return Choice;
}
short ReadTransactionsMenuOption()
{
	short num;
	cout << "Choose what do You want to do? [ 1 to 4 ] ?";
	cin >> num;
	return num;
}
void ShowTransectionsMenu(vector<sClient>& vClients)
{
	short choice = 0;

	do
	{
		system("cls");
		ShowTransactionsMenuScreen();

		choice = ReadTransactionsMenuOption();

		switch (choice)
		{
		case Deposit:
			system("cls");
			DepositBalanceToclientByAccountNumber();
			break;

		case Withdraw:
			system("cls");
			WithdrawBalanceToclientByAccountNumber();
			break;

		case TotalBalance:
			system("cls");
			ShowTotalBalances(vClients);
			break;

		case MainMenu:
			break;
		}

	} while (choice != MainMenu);
}
void PerformManageUsersMenuOption(enManageUsersMenuOptions Choice)
{
	switch (Choice)
	{
	case eListUsers:
		system("cls");
		ShowListUsersScreen();
		break;
	case eAddNewUser:
		system("cls");
		ShowAddNewUsersScreen();
		break;
	case eDeleteUser:
		system("cls");
		ShowDeleteUserScreen();
		break;
	case eUpdateUser:
		system("cls");
		ShowUpdateUserScreen();
		break;
	case eFindUser:
		system("cls");
		ShowFindUserScreen();
		break;
	case eMainMenu:
		return;
	}
}
void showManageUserMenu()
{
	short Choice;
	do
	{
		system("cls");
		ShowManageUsersMenuScreen();
		Choice = ReadManageUsersMenuoption();
		if (Choice == eMainMenu)
			break;

		PerformManageUsersMenuOption((enManageUsersMenuOptions)Choice);

		cout << "\n\nPress any key to go back to Manage Users Menu...";
		system("pause>0");

	} while (true);
}
void PerformMainMenuOption(enMainMenuOptions Choice)
{
	vector<sClient> vClients = LoadClientsDatafromFile(ClientsFileName);

	switch (Choice)
	{
	case ShowClientList:

		if (CheckAccessPermission(pListClients))
		{
			system("cls");
			PrintAllClientsData(vClients);
		}
		else
			ShowAccessDeniedMessage();
		break;

	case AddNewClientt:

		if (CheckAccessPermission(pAddNewClient))
			ShowAddNewClientsScreen();
		else
			ShowAccessDeniedMessage();
		break;

	case DeleteClient:

		if (CheckAccessPermission(pDeleteClient))
		{
			system("cls");
			ShowDeleteClientScreen(vClients);
		}
		else
			ShowAccessDeniedMessage();
		break;

	case UpdateClient:

		if (CheckAccessPermission(pUpdateClients))
		{
			system("cls");
			ShowUpdateClientScreen(vClients);
		}
		else
			ShowAccessDeniedMessage();
		break;

	case FindClient:

		if (CheckAccessPermission(pFindClient))
		{
			system("cls");
			ShowFindClientScreen(vClients);
		}
		else
			ShowAccessDeniedMessage();
		break;

	case Transactions:

		if (CheckAccessPermission(pTransactions))
			ShowTransectionsMenu(vClients);
		else
			ShowAccessDeniedMessage();
		break;

	case ManageUsers:

		if (CheckAccessPermission(pManageUsers))
			showManageUserMenu();
		else
			ShowAccessDeniedMessage();
		break;

	case Logout:
		return;
	}
}
void ShowMainMenu()
{
	short Choice;

	do
	{
		system("cls");

		cout << "===========================================\n";
		cout << "\t\tMain Menu Screen\n";
		cout << "===========================================\n";

		cout << "\t[1] Show Client List.\n";
		cout << "\t[2] Add New Client.\n";
		cout << "\t[3] Delete Client.\n";
		cout << "\t[4] Update Client Info.\n";
		cout << "\t[5] Find Client.\n";
		cout << "\t[6] Transactions.\n";
		cout << "\t[7] Manage Users.\n";
		cout << "\t[8] Logout.\n";

		cout << "===========================================\n";

		Choice = ReadMainMenuOption();

		if (Choice == Logout)
			break;

		PerformMainMenuOption((enMainMenuOptions)Choice);

		// Transactions and ManageUsers are sub-menus that handle their own
		// navigation back to the Main Menu; don't show an extra pause for them.
		if (Choice != Transactions && Choice != ManageUsers)
		{
			cout << "\n\nPress any key to go back to Main Menu...";
			system("pause>0");
		}

	} while (true);
}

//=========================================
//          Login & Main
//=========================================

void login()
{
	bool loginFaild = false;
	string Username, Password;
	do
	{
		system("cls");

		cout << "\n---------------------------------\n";
		cout << "\tLogin Screen";
		cout << "\n---------------------------------\n";

		if (loginFaild)
			cout << "\nInvaild Username/Password!\n";

		cout << "\nEnter UserName:";
		getline(cin >> ws, Username);

		cout << "\nEnter Password:";
		getline(cin, Password);

		loginFaild = !FindUserByUsernameAndPassword(Username, Password, CurrentUser);
	} while (loginFaild);

	ShowMainMenu();
}

int main()
{
	while (true)
	{
		login();
	}

	return 0;
}

