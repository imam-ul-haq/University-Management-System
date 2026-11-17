#include <iostream>
#include <string>
#include <limits>
#include <cstddef>
using namespace std;

struct Student {
    int id;
    string name;
    string department;
    int semester;
};

struct Node {
    Student data;
    Node* next;
};

Node* head = NULL;   // Linked list: active students
Node* front = NULL;  // Queue: first admission request
Node* rear = NULL;   // Queue: last admission request
Node* top = NULL;    // Stack: last deleted student

// Reject letters, extra characters and out-of-range numbers.
// Return -1 if the user closes the input.
int readNumber(string prompt, int minimum, int maximum) {
    int number;
    while (true) {
        cout << prompt;
        if (cin >> number) {
            string extra;
            getline(cin, extra);
            bool hasExtra = false;
            for (unsigned int i = 0; i < extra.length(); i++) {
                if (extra[i] != ' ' && extra[i] != '\t' && extra[i] != '\r')
                    hasExtra = true;
            }
            if (!hasExtra && number >= minimum && number <= maximum)
                return number;
        } else {
            if (cin.eof()) return -1;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cout << "Enter a whole number from " << minimum << " to " << maximum << ".\n";
    }
}

bool readText(string prompt, string& value) {
    while (true) {
        cout << prompt;
        if (!getline(cin, value)) return false;
        bool hasText = false;
        for (unsigned int i = 0; i < value.length(); i++) {
            if (value[i] != ' ' && value[i] != '\t' && value[i] != '\r')
                hasText = true;
        }
        if (hasText) return true;
        cout << "This field cannot be empty.\n";
    }
}

bool readStudent(Student& s, int id) {
    s.id = id;
    if (!readText("Name: ", s.name)) return false;
    if (!readText("Department: ", s.department)) return false;
    s.semester = readNumber("Semester (1-8): ", 1, 8);
    return s.semester != -1;
}

Node* makeNode(Student s) {
    Node* newNode = new Node;
    newNode->data = s;
    newNode->next = NULL;
    return newNode;
}

Node* findStudent(Node* start, int id) {
    while (start != NULL) {
        if (start->data.id == id) return start;
        start = start->next;
    }
    return NULL;
}

bool idInUse(int id) {
    return findStudent(head, id) != NULL || findStudent(front, id) != NULL;
}

void printStudent(Student s) {
    cout << "ID: " << s.id << " | Name: " << s.name
         << " | Department: " << s.department
         << " | Semester: " << s.semester << "\n";
}

// LINKED LIST: add a new student at the beginning.
void addStudent() {
    int id = readNumber("Student ID: ", 1, 999999999);
    if (id == -1) return;
    if (idInUse(id)) {
        cout << "ID already exists in students or pending admissions.\n";
        return;
    }
    Student s;
    if (!readStudent(s, id)) return;
    Node* newNode = makeNode(s);
    newNode->next = head;
    head = newNode;
    cout << "Student added.\n";
}

void displayStudents() {
    if (head == NULL) {
        cout << "No students found.\n";
        return;
    }
    cout << "\nACTIVE STUDENTS\n";
    Node* current = head;
    while (current != NULL) {
        printStudent(current->data);
        current = current->next;
    }
}

void searchStudent() {
    int id = readNumber("Student ID to search: ", 1, 999999999);
    if (id == -1) return;
    Node* result = findStudent(head, id);
    if (result == NULL) cout << "Student not found.\n";
    else printStudent(result->data);
}

void updateStudent() {
    int id = readNumber("Student ID to update: ", 1, 999999999);
    if (id == -1) return;
    Node* result = findStudent(head, id);
    if (result == NULL) {
        cout << "Student not found.\n";
        return;
    }
    Student updated;
    if (!readStudent(updated, id)) return;
    result->data = updated; // Keep the same ID.
    cout << "Student updated.\n";
}

// Remove the student from the list and PUSH it onto the stack.
void deleteStudent() {
    int id = readNumber("Student ID to delete: ", 1, 999999999);
    if (id == -1) return;
    Node* current = head;
    Node* previous = NULL;
    while (current != NULL && current->data.id != id) {
        previous = current;
        current = current->next;
    }
    if (current == NULL) {
        cout << "Student not found.\n";
        return;
    }
    if (previous == NULL) head = current->next;
    else previous->next = current->next;
    current->next = top;
    top = current;
    cout << "Student deleted. Use option 6 to undo.\n";
}

// STACK: POP the last deleted student and restore it.
void undoDelete() {
    if (top == NULL) {
        cout << "Nothing to undo.\n";
        return;
    }
    if (idInUse(top->data.id)) {
        cout << "Undo blocked: this ID is active or pending.\n";
        return;
    }
    Node* restored = top;
    top = top->next;
    restored->next = head;
    head = restored;
    cout << "Restored student ID " << restored->data.id << ".\n";
}

// QUEUE: ENQUEUE a new admission request at the rear.
void addAdmission() {
    int id = readNumber("Applicant student ID: ", 1, 999999999);
    if (id == -1) return;
    if (idInUse(id)) {
        cout << "ID already exists in students or pending admissions.\n";
        return;
    }
    Student s;
    if (!readStudent(s, id)) return;
    Node* newNode = makeNode(s);
    if (front == NULL) front = rear = newNode;
    else {
        rear->next = newNode;
        rear = newNode;
    }
    cout << "Admission request queued.\n";
}

// QUEUE: DEQUEUE the first request and move it to the student list.
void processAdmission() {
    if (front == NULL) {
        cout << "No pending admissions.\n";
        return;
    }
    Node* admitted = front;
    front = front->next;
    if (front == NULL) rear = NULL;
    admitted->next = head;
    head = admitted;
    cout << "Admitted student ID " << admitted->data.id << ".\n";
}

void displayAdmissions() {
    if (front == NULL) {
        cout << "No pending admissions.\n";
        return;
    }
    cout << "\nPENDING ADMISSIONS (first request first)\n";
    Node* current = front;
    while (current != NULL) {
        printStudent(current->data);
        current = current->next;
    }
}

// Release all remaining nodes before closing.
void freeNodes(Node* start) {
    while (start != NULL) {
        Node* old = start;
        start = start->next;
        delete old;
    }
}

int main() {
    int choice;
    cout << "UNIVERSITY MANAGEMENT SYSTEM\n";
    do {
        cout << "\n1. Add student\n2. Display students\n3. Search student\n";
        cout << "4. Update student\n5. Delete student\n6. Undo last deletion\n";
        cout << "7. Add admission request\n8. Process next admission\n";
        cout << "9. Display admission queue\n0. Exit\n";
        choice = readNumber("Choice: ", 0, 9);
        if (choice == -1) break;
        switch (choice) {
            case 1: addStudent(); break;
            case 2: displayStudents(); break;
            case 3: searchStudent(); break;
            case 4: updateStudent(); break;
            case 5: deleteStudent(); break;
            case 6: undoDelete(); break;
            case 7: addAdmission(); break;
            case 8: processAdmission(); break;
            case 9: displayAdmissions(); break;
        }
    } while (choice != 0);
    freeNodes(head);
    freeNodes(front);
    freeNodes(top);
    if (choice == -1) cout << "Input closed.\n";
    else cout << "Goodbye. Session data cleared.\n";
    return 0;
}
