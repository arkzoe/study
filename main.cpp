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

//���ͼ��
void addBook(const string& isbn, const string& title, const string& author, const string& publisher, int stock);
//����
void borrowBook(int user_id, const string& isbn, time_t borrow_date, time_t due_date);
//����
void returnBook(int user_id, const string& isbn);
//�������
void checkOverdue();
//��������
void overdue();
//ȷ���ļ�����
void ensureFile(const string& filename);
//��ȡbook
void loadBooksFromFile(const string& filename);
//��ȡrecord
void loadRecordsFromFile(const string& filename);
//����book
void saveBooksToFile(const string& filename);
//����record
void saveRecordsToFile(const string& filename);
//�˵�
void showMenu();
//����
void input();
//ͼ����Ϣ
void outputbook();
//���ļ�¼
void outpuborrow();

unordered_map<string, Book> books; // ͼ��

list<BorrowRecord> borrow_records; // ����

int main() {
    input();
    return 0;
}


void addBook(const string& isbn, const string& title, const string& author, const string& publisher, int stock) {
    if (books.find(isbn) != books.end()) {
        cout << "ͼ���Ѵ��ڣ����¿��������\n";
        books[isbn].stock += stock;
    }
    else {
        books[isbn] = Book(isbn, title, author, publisher, stock);
        cout << "ͼ��¼��ɹ���\n";
    }
}

void borrowBook(int user_id, const string& isbn, time_t borrow_date, time_t due_date) {
    auto it = books.find(isbn);
    if (it == books.end()) {
        cout << "ͼ�鲻���ڡ�\n";
        return;
    }
    if (it->second.stock <= 0) {
        cout << "��治�㣬�޷����ġ�\n";
        return;
    }
    it->second.stock -= 1;
    cout << "���ĳɹ�\n";

    borrow_records.push_back({user_id, isbn, borrow_date, due_date});
}

void returnBook(int user_id, const string& isbn) {
    for (auto it = borrow_records.begin(); it != borrow_records.end(); ++it) {
        if (it->user_id == user_id && it->isbn == isbn) {
            borrow_records.erase(it);
            cout << "�黹�ɹ������ļ�¼��ɾ����\n";
            auto book_it = books.find(isbn);
            if (book_it != books.end()) {
                book_it->second.stock += 1;
                cout << "�������1��\n";
            }
            return;
        }
    }
    cout << "δ�ҵ����ļ�¼��\n";
}

void overdue(){
	time_t now = time(nullptr);
	for (const auto& record : borrow_records) {
	    if (record.due_date < now) {
	        cout << "�������ѣ������ڼ�¼" << "\n"<< "\n";
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
            cout << "�������ѣ��û�ID: " << record.user_id 
				 << ", ͼ��ISBN: " << record.isbn
                 << ", Ӧ������: " << put_time(localtime(&record.due_date), "%Y-%m-%d %H:%M:%S") << "\n";
        }
    }
    if (!has_overdue) {
        cout << "û�г��ڵĽ��ļ�¼��\n";
    }
}

void ensureFile(const string& filename) {
    ifstream file(filename);
    if (!file) {
        ofstream newFile(filename);
        if (newFile) {
            cout << "�ļ������ڣ��Ѵ������ļ�: " << filename << "\n";
        }
        else {
            cerr << "�޷������ļ�: " << filename << "\n";
        }
        newFile.close();
    }
    file.close();
}

void loadBooksFromFile(const string& filename) {
    ensureFile(filename);
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "�޷���ͼ����Ϣ�ļ�: " << filename << "\n";
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
                cerr << "ͼ����Ϣ��ʽ���󣨿�治��������: " << line << "\n";
            }
        }
        else {
            cerr << "ͼ����Ϣ��ʽ����: " << line << "\n";
        }
    }
    file.close();
    cout << "ͼ����Ϣ������ɡ�\n";
}

void loadRecordsFromFile(const string& filename) {
    ensureFile(filename);
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "�޷��򿪽��ļ�¼�ļ�: " << filename << "\n";
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
                cerr << "���ļ�¼��ʽ�����û�ID��ʱ�������������: " << line << "\n";
            }
        }
        else {
            cerr << "���ļ�¼��ʽ����: " << line << "\n";
        }
    }
    file.close();
    cout << "���ļ�¼������ɡ�\n";
}

void saveBooksToFile(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "�޷����ļ�: " << filename << "\n";
        return;
    }
    for (const auto& pair : books) {
        const Book& book = pair.second;
        file << book.isbn << "|" << book.title << "|" << book.author << "|" << book.publisher << "|" << book.stock << "\n";
    }
    file.close();
    cout << "ͼ����Ϣ�ѱ��浽�ļ�: " << filename << "\n";
}

void saveRecordsToFile(const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "�޷����ļ�: " << filename << "\n";
        return;
    }
    for (const auto& record : borrow_records) {
        file << record.user_id << "|" << record.isbn << "|" << record.borrow_date << "|" << record.due_date << "\n";
    }
    file.close();
    cout << "���ļ�¼�ѱ��浽�ļ�: " << filename << "\n";
}

void showMenu() {
    cout << "��ӭʹ��ͼ�����ϵͳ\n";
    cout << "1. ¼��ͼ��\n";
    cout << "2. ����ͼ��\n";
    cout << "3. �黹ͼ��\n";
    cout << "4. ��鳬��\n";
    cout << "5. ͼ����Ϣ\n";
    cout << "6. ���ļ�¼\n";
    cout << "7. �˳�\n";
    cout << "������ѡ��: ";
}

void outputbook() {
    cout << "ͼ����Ϣ��" << endl;
    for (const auto& book : books) {
        cout << "ISBN: " << book.second.isbn << endl;
        cout << "����: " << book.second.title << endl;
        cout << "����: " << book.second.author << endl;
        cout << "������: " << book.second.publisher << endl;
        cout << "���: " << book.second.stock << endl;
        cout << endl;
    }
}

void outpuborrow() {
    cout << "���ļ�¼��" << endl;
    for (const auto& record : borrow_records) {
        cout << "�û�ID: " << record.user_id << endl;
        cout << "ͼ��ISBN: " << record.isbn << endl;
        cout << "��������: " << put_time(localtime(&record.borrow_date), "%Y-%m-%d %H:%M:%S") << endl;
        cout << "Ӧ������: " << put_time(localtime(&record.due_date), "%Y-%m-%d %H:%M:%S") << endl;
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
        if (choice == 1) { // ¼��ͼ��
            string isbn, title, author, publisher;
            int stock;
            cout << "������ISBN: ";
            cin >> isbn;
            cout << "���������: ";
            cin.ignore(); // ����ǰ��Ļ��з�
            getline(cin, title);
            cout << "����������: ";
            getline(cin, author);
            cout << "�����������: ";
            getline(cin, publisher);
            cout << "��������: ";
            cin >> stock;
            addBook(isbn, title, author, publisher, stock);
        }
        else if (choice == 2) { // ����ͼ��
            int user_id;
            string isbn;
            cout << "�������û�ID: ";
            cin >> user_id;
            cout << "������ISBN: ";
            cin >> isbn;
            time_t borrow_date = time(nullptr);
            time_t due_date = borrow_date + 604800; // 7���
            borrowBook(user_id, isbn, borrow_date, due_date);
        }
        else if (choice == 3) { // �黹ͼ��
            int user_id;
            string isbn;
            cout << "�������û�ID: ";
            cin >> user_id;
            cout << "������ISBN: ";
            cin >> isbn;
            returnBook(user_id, isbn);
        }
        else if (choice == 4) { // ��鳬��
            checkOverdue();
        }
        else if (choice == 5) { // ͼ����Ϣ
            outputbook();
        }
        else if (choice == 6) { // ���ļ�¼
            outpuborrow();
        }
        else if (choice == 7) { // �˳�
            saveBooksToFile("books.txt");
            saveRecordsToFile("borrow_records.txt");
            cout << "�˳�ϵͳ��\n";
            break;
        }
        else {
            cout << "��Чѡ����������롣\n";
        }
        system("pause");
        system("cls");
    }
}
