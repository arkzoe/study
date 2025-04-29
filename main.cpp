#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <unordered_map>
#include <list>
#include <string>
#include <ctime>
#include <fstream>
#include <sstream>
#include <iomanip>
//#include "list-hash.h"
using namespace std;

struct Book {
    string isbn;
    string title;
    string author;
    string publisher;
    int stock;
    Book() : isbn(""), title(""), author(""), publisher(""), stock(0) {}
    Book(string isbn, string title, string author, string publisher, int stock)
        : isbn(isbn), title(title), author(author), publisher(publisher), stock(stock) {}
};

struct BorrowRecord {
    int user_id;
    string isbn;
    time_t borrow_date;
    time_t due_date;

    BorrowRecord(int user_id, string isbn, time_t borrow_date, time_t due_date)
        : user_id(user_id), isbn(isbn), borrow_date(borrow_date), due_date(due_date) {}
};

//添加图书
void addBook(const string& isbn, const string& title, const string& author, const string& publisher, int stock);
//借书
void borrowBook(int user_id, const string& isbn, time_t borrow_date, time_t due_date);
//还书
void returnBook(int user_id, const string& isbn);
//检查逾期
void checkOverdue();
//逾期提醒
void overdue();
//确保文件存在
void ensureFile(const string& filename);
//读取book
void loadBooksFromFile(const string& filename);
//读取record
void loadRecordsFromFile(const string& filename);
//保存book
void saveBooksToFile(const string& filename);
//保存record
void saveRecordsToFile(const string& filename);
//菜单
void showMenu();
//输入
void input();
//图书信息
void outputbook();
//借阅记录
void outpuborrow();

unordered_map<string, Book> books; // 图书

list<BorrowRecord> borrow_records; // 借阅

int main() {
    input();
    return 0;
}


void addBook(const string& isbn, const string& title, const string& author, const string& publisher, int stock) {
    if (books.find(isbn) != books.end()) {
        cout << "图书已存在，更新库存数量。\n";
        books[isbn].stock += stock;
    }
    else {
        books[isbn] = Book(isbn, title, author, publisher, stock);
        cout << "图书录入成功。\n";
    }
}

void borrowBook(int user_id, const string& isbn, time_t borrow_date, time_t due_date) {
    auto it = books.find(isbn);
    if (it == books.end()) {
        cout << "图书不存在。\n";
        return;
    }
    if (it->second.stock <= 0) {
        cout << "库存不足，无法借阅。\n";
        return;
    }
    it->second.stock -= 1;
    cout << "借阅成功\n";

    borrow_records.push_back({user_id, isbn, borrow_date, due_date});
}

void returnBook(int user_id, const string& isbn) {
    for (auto it = borrow_records.begin(); it != borrow_records.end(); ++it) {
        if (it->user_id == user_id && it->isbn == isbn) {
            borrow_records.erase(it);
            cout << "归还成功，借阅记录已删除。\n";
            auto book_it = books.find(isbn);
            if (book_it != books.end()) {
                book_it->second.stock += 1;
                cout << "库存增加1。\n";
            }
            return;
        }
    }
    cout << "未找到借阅记录。\n";
}

void overdue(){
	time_t now = time(nullptr);
	for (const auto& record : borrow_records) {
	    if (record.due_date < now) {
	        cout << "超期提醒：有逾期记录" << "\n"<< "\n";
	        return ;
	    }
	}
}

void checkOverdue() {
    time_t now = time(nullptr);
    bool has_overdue = false;
    for (const auto& record : borrow_records) {
        if (record.due_date < now) {
            has_overdue = true;
            cout << "超期提醒：用户ID: " << record.user_id 
				 << ", 图书ISBN: " << record.isbn
                 << ", 应还日期: " << put_time(localtime(&record.due_date), "%Y-%m-%d %H:%M:%S") << "\n";
        }
    }
    if (!has_overdue) {
        cout << "没有超期的借阅记录。\n";
    }
}

void ensureFile(const string& filename) {
    ifstream file(filename);
    if (!file) {
        ofstream newFile(filename);
        if (newFile) {
            cout << "文件不存在，已创建新文件: " << filename << "\n";
        }
        else {
            cerr << "无法创建文件: " << filename << "\n";
        }
        newFile.close();
    }
    file.close();
}

void loadBooksFromFile(const string& filename) {
    ensureFile(filename);
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "无法打开图书信息文件: " << filename << "\n";
        return;
    }
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string isbn, title, author, publisher, stock_str;
        if (getline(ss, isbn, '|') && getline(ss, title, '|') && getline(ss, author, '|') && getline(ss, publisher, '|') && getline(ss, stock_str)) {
            try {
                int stock = stoi(stock_str);
                books[isbn] = Book(isbn, title, author, publisher, stock);
            }
            catch (const invalid_argument& e) {
                cerr << "图书信息格式错误（库存不是整数）: " << line << "\n";
            }
        }
        else {
            cerr << "图书信息格式错误: " << line << "\n";
        }
    }
    file.close();
    cout << "图书信息加载完成。\n";
}

void loadRecordsFromFile(const string& filename) {
    ensureFile(filename);
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "无法打开借阅记录文件: " << filename << "\n";
        return;
    }
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string user_id_str, isbn, borrow_date_str, due_date_str;
        if (getline(ss, user_id_str, '|') && getline(ss, isbn, '|') && getline(ss, borrow_date_str, '|') && getline(ss, due_date_str)) {
            try {
                int user_id = stoi(user_id_str);
                time_t borrow_date = stol(borrow_date_str);
                time_t due_date = stol(due_date_str);
                borrow_records.emplace_back(user_id, isbn, borrow_date, due_date);
            }
            catch (const invalid_argument& e) {
                cerr << "借阅记录格式错误（用户ID或时间戳不是整数）: " << line << "\n";
            }
        }
        else {
            cerr << "借阅记录格式错误: " << line << "\n";
        }
    }
    file.close();
    cout << "借阅记录加载完成。\n";
}

void saveBooksToFile(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "无法打开文件: " << filename << "\n";
        return;
    }
    for (const auto& pair : books) {
        const Book& book = pair.second;
        file << book.isbn << "|" << book.title << "|" << book.author << "|" << book.publisher << "|" << book.stock << "\n";
    }
    file.close();
    cout << "图书信息已保存到文件: " << filename << "\n";
}

void saveRecordsToFile(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "无法打开文件: " << filename << "\n";
        return;
    }
    for (const auto& record : borrow_records) {
        file << record.user_id << "|" << record.isbn << "|" << record.borrow_date << "|" << record.due_date << "\n";
    }
    file.close();
    cout << "借阅记录已保存到文件: " << filename << "\n";
}

void showMenu() {
    cout << "欢迎使用图书管理系统\n";
    cout << "1. 录入图书\n";
    cout << "2. 借阅图书\n";
    cout << "3. 归还图书\n";
    cout << "4. 检查超期\n";
    cout << "5. 图书信息\n";
    cout << "6. 借阅记录\n";
    cout << "7. 退出\n";
    cout << "请输入选项: ";
}

void outputbook() {
    cout << "图书信息：" << endl;
    for (const auto& book : books) {
        cout << "ISBN: " << book.second.isbn << endl;
        cout << "标题: " << book.second.title << endl;
        cout << "作者: " << book.second.author << endl;
        cout << "出版社: " << book.second.publisher << endl;
        cout << "库存: " << book.second.stock << endl;
        cout << endl;
    }
}

void outpuborrow() {
    cout << "借阅记录：" << endl;
    for (const auto& record : borrow_records) {
        cout << "用户ID: " << record.user_id << endl;
        cout << "图书ISBN: " << record.isbn << endl;
        cout << "借阅日期: " << put_time(localtime(&record.borrow_date), "%Y-%m-%d %H:%M:%S") << endl;
        cout << "应还日期: " << put_time(localtime(&record.due_date), "%Y-%m-%d %H:%M:%S") << endl;
        cout << endl;
    }
}

void input() {
    loadBooksFromFile("books.txt");
    loadRecordsFromFile("borrow_records.txt");
    overdue();
    int choice;
    while (true) {
        showMenu();
        cin >> choice;
        if (choice == 1) { // 录入图书
            string isbn, title, author, publisher;
            int stock;
            cout << "请输入ISBN: ";
            cin >> isbn;
            cout << "请输入标题: ";
            cin.ignore(); // 忽略前面的换行符
            getline(cin, title);
            cout << "请输入作者: ";
            getline(cin, author);
            cout << "请输入出版社: ";
            getline(cin, publisher);
            cout << "请输入库存: ";
            cin >> stock;
            addBook(isbn, title, author, publisher, stock);
        }
        else if (choice == 2) { // 借阅图书
            int user_id;
            string isbn;
            cout << "请输入用户ID: ";
            cin >> user_id;
            cout << "请输入ISBN: ";
            cin >> isbn;
            time_t borrow_date = time(nullptr);
            time_t due_date = borrow_date + 604800; // 7天后
            borrowBook(user_id, isbn, borrow_date, due_date);
        }
        else if (choice == 3) { // 归还图书
            int user_id;
            string isbn;
            cout << "请输入用户ID: ";
            cin >> user_id;
            cout << "请输入ISBN: ";
            cin >> isbn;
            returnBook(user_id, isbn);
        }
        else if (choice == 4) { // 检查超期
            checkOverdue();
        }
        else if (choice == 5) { // 图书信息
            outputbook();
        }
        else if (choice == 6) { // 借阅记录
            outpuborrow();
        }
        else if (choice == 7) { // 退出
            saveBooksToFile("books.txt");
            saveRecordsToFile("borrow_records.txt");
            cout << "退出系统。\n";
            break;
        }
        else {
            cout << "无效选项，请重新输入。\n";
        }
        system("pause");
        system("cls");
    }
}
