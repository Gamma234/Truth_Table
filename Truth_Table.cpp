#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <math.h>
#include <iomanip> // setw

using namespace std;

char gTrash = ' ';

int InputNumber();

struct Word {
	char character;
	int value;
}; // struct Word

struct Token {
	string token;
	Token *next;
	Token(string temp) { // constructor
		token = temp;
		next = NULL;
	} // Token()
}; // struct Token

class Truth_Table {

	private :
		Word list[30];
		int listSize;
		string str;
		stack<string> sBuffer;
		stack<int> iBuffer;
		Token *listHead, *listTail;

	public :
		Truth_Table(int num); // constructor
		bool transform;

		bool CheckError(); // check error
		void InfixToPostfix(); // Infix To Postfix
		void PostfixToValue(); // Postfix To Value
		bool ChangeListValue();

		bool ErrorMessage(int error, char ch); // 錯誤訊息
		void InsertIntoList(string temp);
		int ValueOfOperator(string opt);
		void Calculation(string opt);
		void EraseTheList(); // 刪除list所有資料
		void OutputList();
		char LastNotWhiteCharacter(int i); // 找到上一個非空白字元
		void OutputListValueLine();
		int CharacterValue(char ch);
		bool CharacterExist( char ch );

}; // class Truth_Table

int main() {

	cout << "Truth Table" << endl;
	cout << "----------------------" << endl;
	cout << "How many variables ? ( 1~26 )" << endl;

	int num = 0;
	
	while (true) {
		num = InputNumber();
		if (0 < num && num <= 26) break;
		else cerr << "(Wrong number !)" << endl << endl;
	} // while

	Truth_Table TT( num );

	do {
		TT.OutputListValueLine();
		if (! TT.transform) {
			TT.transform = true;
			TT.InfixToPostfix();
		} // if
		TT.PostfixToValue();
	} while (TT.ChangeListValue());

	system("pause");

} // main()

int InputNumber() {

	int num = -1;
	cout << "Input an integer : ";

	while (true) {

		cin >> num;

		if (cin.fail()) {	// 防止輸入錯誤, 例如: 輸入字元而非整數 			
			cerr << "(Wrong number !)" << endl;
			cin.clear();
			if (num > 100 && num != -100) cin.ignore();
			while (gTrash != '\n') gTrash = cin.get(); // 處理掉同行"錯誤輸入"
			gTrash = ' '; // 初始化 
		} // if
		else {
			while (gTrash != '\n') gTrash = cin.get(); // 處理掉同行"其他不需要的輸入"
			gTrash = ' '; // 初始化 
			cout << "Get your input : " << num << endl << endl;
			break;
		} // else

		cout << endl << "Input again : ";

	} // while

	return num;

} // InputNumber()

bool Truth_Table::CheckError() { // check error

	if (listHead != NULL) EraseTheList();

	cout << "\n-------------------------------" << endl;
	cout << "Set your formula : ";
	getline(cin, str);
	cout << "Get your formula : " << str << endl << endl;

	bool success = true;
	int openNum = 0, closeNum = 0; // openNum代表'('的數量, closeNum代表')'的數量
	string temp = ""; // 暫存現在的字元
	char last = '\0'; // 暫存上一個非空白字元

	int firstNotWhite = 0; // 第一個非空白字元 // 真正的開頭 
	while (str[firstNotWhite] == ' ' || str[firstNotWhite] == '\t') firstNotWhite++; // 找到第一個非空白字元 

	for (int i = firstNotWhite; i < str.length(); i++) {

		if (str[i] == ' ' || str[i] == '\t') continue;
		else last = LastNotWhiteCharacter(i - 1);  // 找到上一個非空白字元 

		if ('A' <= str[i] && str[i] <= 'Z') { // operand
			
			if (!CharacterExist( str[i] )) { // 是否為不存在的字
				success = ErrorMessage(5, str[i]); // 錯誤訊息 
				break;
			} // if
			
			if (i != firstNotWhite) { // 非開頭
				// if (str[i - 1] == ' ' || str[i - 1] == '\t') { // 前一格是空白 
					if ('A' <= last && last <= 'Z') { // 上一個非空白字元是數字 
						success = ErrorMessage(5, str[i]); // 錯誤訊息 
						break;
					} // if
				// } // if
				if (last == ')') { // 上一個字元是')' 
					success = ErrorMessage(5, str[i]); // 錯誤訊息 
					break;
				} // if
			} // if

		} // if
		else { // operator

			temp = temp + str[i];
			if (!ValueOfOperator(temp)) { // 非可用的 operator
				success = ErrorMessage(1, str[i]); // 錯誤訊息 
				break;
			} // if
			else { // 是可用的 operator 
				if (temp == "(") openNum++;
				if (temp == ")") closeNum++;
				if (openNum < closeNum) { // ')'比'('還多 
					success = ErrorMessage(3, str[i]); // 錯誤訊息 
					break;
				} // if
				if (i == firstNotWhite && temp != "(") { // 開頭為非'('的 operator
					success = ErrorMessage(4, str[i]); // 錯誤訊息 
					break;
				} // if
				else {
					if (i != firstNotWhite) { // 非開頭
						if (temp != "(" && last != ')' && last != '\'' &&  // 這個字元不是'(' 且 上一個非空白字元不是')' 且 上一個非空白字元不是'\''
							(last < 'A' || last > 'Z')) { // 且 上一個字元非數字 
							success = ErrorMessage(4, str[i]); // 錯誤訊息 
							break;
						} // if
						else if ((temp == "(" && ('A' <= last && last <= 'Z')) || // 這個字元是'(' 且 上一個非空白字元是數字 
							(temp == "(" && last == ')') ||   // 或 這個字元是'(' 且 上一個非空白字元是')'
							(temp == ")" && last == '(')) {  // 或 這個字元是')' 且 上一個非空白字元是'('
							success = ErrorMessage(4, str[i]); // 錯誤訊息 
							break;
						} // else if
					} // if
				} // else
			} // else
			temp = ""; // 初始化

		} // else

	} // for

	if (success) {
		if (openNum != closeNum) { // '('比')'還多
			success = ErrorMessage(2, '0'); // 錯誤訊息 
		} // if
		last = LastNotWhiteCharacter(str.length() - 1);
		if ( last != '\'' && last != ')' && (last < 'A' || last > 'Z')) { // 結尾不正確 
			success = ErrorMessage(4, '0'); // 錯誤訊息 
		} // if
	} // if

	if (success) cout << "It is a legitimate infix expression." << endl;
	cout << "----------------------------" << endl;

	if (success) return true;
	else return false;

} // Truth_Table::CheckError()

void Truth_Table::InfixToPostfix() { // Infix To Postfix

	// cout << "Your infix : " << str << endl << endl;

	string temp = "";

	for (int i = 0; i < str.length(); ) {

		if (str[i] == ' ' || str[i] == '\t') {
			i++;
			continue;
		} // if

		temp = "";

		if ('A' <= str[i] && str[i] <= 'Z') { // operand
			temp = temp + str[i++];
			InsertIntoList(temp);
		} // if
		else { // operator

			temp = temp + str[i++];

			if (temp == "(") sBuffer.push(temp); // ( 情況 
			else if (temp == ")") { // ')' 情況  
				temp = sBuffer.top();
				while (temp != "(") {
					InsertIntoList(temp);
					sBuffer.pop();
					temp = sBuffer.top();
				} // while
				sBuffer.pop();
			} // else if
			else { // 其他operator情況  
				while (!sBuffer.empty() && ValueOfOperator(temp) >= ValueOfOperator(sBuffer.top())) {
					InsertIntoList(sBuffer.top());
					sBuffer.pop();
				} // while
				sBuffer.push(temp);
			} // else

		} // else

	} // for

	while (!sBuffer.empty()) { // 將sBuffer內剩的東西取出
		InsertIntoList(sBuffer.top());
		sBuffer.pop();
	} // while

	// cout << "Postfix expression : ";
	// OutputList();

} // Truth_Table::InfixToPostfix()

bool Truth_Table::ChangeListValue() {

	for (int i = listSize - 1; i >= 0; i--) {
		list[i].value++;
		if (list[0].value == 2) return false;
		else if (list[i].value == 2) list[i].value -= 2;
		else break;
	} // for

	return true;

} // Truth_Table::ChangeListValue()

void Truth_Table::PostfixToValue() { // Postfix To Value

	for (Token *temp = listHead; temp != NULL; temp = temp->next) {
		if ('A' <= temp->token[0] && temp->token[0] <= 'Z')
			iBuffer.push( CharacterValue(temp->token[0]) );
		else Calculation(temp->token);
	} // for

	cout << setw( str.length() ) << ( iBuffer.top() == 0 ? 0 : 1 ) << endl;
	iBuffer.pop();

} // Truth_Table::PostfixToValue()

bool Truth_Table::ErrorMessage(int error, char ch) { // 錯誤訊息 

	switch (error) {
	case 1:
		cout << "Error 1 : " << ch << " is not a legitimate character." << endl; break;
	case 2:
		cout << "Error 2 : there is one extra open parenthesis." << endl; break;
	case 3:
		cout << "Error 2 : there is one extra close parenthesis." << endl; break;
	case 4:
		cout << "Error 3 : there is one extra operator." << endl; break;
	case 5:
		cout << "Error 3 : there is one extra operand." << endl; break;
	default:;
	} // switch

	return false;

} // Truth_Table::ErrorMessage()

void Truth_Table::InsertIntoList(string temp) {

	Token *newToken = new Token(temp);

	if (listHead == NULL) {
		listHead = newToken;
		listTail = listHead;
	} // if
	else {
		listTail->next = newToken;
		listTail = newToken;
	} // else

} // Truth_Table::InsertIntoList()

int Truth_Table::ValueOfOperator(string opt) {

	if (opt == "+") return 3;
	else if (opt == "*" || opt == "'") return 2;
	else if (opt == "(" || opt == ")") return 4;
	else return 0;

} // Truth_Table::ValueOfOperator()

void Truth_Table::Calculation(string opt) {

	int num1, num2;

	if (opt == "+" || opt == "*") {
		num1 = iBuffer.top(); iBuffer.pop();
		num2 = iBuffer.top(); iBuffer.pop();
	} // if
	else {
		num1 = iBuffer.top(); iBuffer.pop();
	} // else

	switch (opt[0]) {
		case '+':
			iBuffer.push(num2 + num1); break;
		case '*':
			iBuffer.push(num2 * num1); break;
		case '\'' :
			if ( num1 == 0 ) iBuffer.push( 1 );
			else iBuffer.push( 0 );
			break;
		default:;
	} // switch

} // Truth_Table::Calculation()

void Truth_Table::EraseTheList() { // 刪除list所有資料 

	for (Token * temp = listHead->next; listHead != NULL; temp = temp->next) {
		delete listHead;
		listHead = temp;
		if (listHead == NULL) break;
	} // for

} // Truth_Table::EraseTheList()

void Truth_Table::OutputList() {

	for (Token *temp = listHead; temp != NULL; temp = temp->next) {
		cout << temp->token;
		if (temp->next != NULL) cout << ",";
	} // for

	cout << endl;

} // Truth_Table::OutputList()

char Truth_Table::LastNotWhiteCharacter(int i) { // 找到上一個非空白字元  

	for (int j = i; j >= 0; j--) {
		if (str[j] != ' ' && str[j] != '\t') return str[j];
	} // for

	return ' ';

} // Truth_Table::LastNotWhiteCharacter()

int Truth_Table::CharacterValue(char ch) {

	for (int i = 0; i < listSize; i++) {
		if (list[i].character == ch)
			return list[i].value;
	} // for

} // Truth_Table::CharacterValue()

void Truth_Table::OutputListValueLine() {

	for (int i = 0; i < listSize; i++) {
		cout << list[i].value << "  ";
	} // for
	cout << "  ";

} // Truth_Table::OutputListValueLine()

Truth_Table::Truth_Table(int num) {

	transform = false;
	listHead = listTail = NULL;
	listSize = num;

	for (int i = 0, ch = 65; i < num; i++, ch++) {
		list[i].character = (char)ch;
		list[i].value = 0;
	} // for

	while ( true ) {
		if (CheckError()) break;
	} // while

	for (int i = 0; i < listSize; i++) {
		cout << list[i].character << "  ";
	} // for
	cout << "  ";

	cout << str << endl;
	cout << "----------------------------------------" << endl;

} // Truth_Table::Truth_Table()

bool Truth_Table::CharacterExist(char ch) {

	for (int i = 0; i < listSize; i++) {
		if (ch == list[i].character) return true;
	} // for

	return false;

} // Truth_Table::CharacterExist()


