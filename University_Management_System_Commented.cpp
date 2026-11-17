#include <iostream>  
#include <string>    
#include <limits>     // Provides numeric_limits for clearing invalid input.
#include <cstddef>    // Provides standard definitions such as NULL.
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

Node* head = NULL;   // Points to the first active student in the linked list.
Node* front = NULL;  // Points to the first admission request in the queue.
Node* rear = NULL;   // Points to the last admission request in the queue.
Node* top = NULL;    // Points to the most recently deleted student in the stack.

int readNumber(string prompt, int minimum, int maximum) {
    int number;

    while (true) {                         // Keep asking until valid input is entered.
        cout << prompt;                    // Display the input message.

        if (cin >> number) {               // Try to read an integer.
            string extra;
            getline(cin, extra);           // Read anything left on the same line.

            bool hasExtra = false;         // Assume there are no extra characters.
            for (unsigned int i = 0; i < extra.length(); i++) 
			{
                // Ignore ordinary whitespace , detect any other extra character.
                if (extra[i] != ' ' && extra[i] != '\t' && extra[i] != '\r')
                    hasExtra = true;
            }

            // Return the number only if the line is clean and the range is valid.
            if (!hasExtra && number >= minimum && number <= maximum)
                return number;
        } else {
            if (cin.eof()) return -1;      // Stop if the input stream has been closed.

            cin.clear();                   // Clear the input error state.
            
            // Remove the invalid input from the input buffer.
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        // Show an error message before asking again.
        cout << "Enter a whole number from " << minimum << " to " << maximum << ".\n";
    }
}

// Reads a non-empty text value from the user.
// Returns false if the input stream is closed.
bool readText(string prompt, string& value) 
{
    while (true) {                       
        cout << prompt;

        if (!getline(cin, value)) return false; // Read the complete line.

        bool hasText = false;              // Check whether the input contains text.
        for (unsigned int i = 0; i < value.length(); i++) {
            if (value[i] != ' ' && value[i] != '\t' && value[i] != '\r')
                hasText = true;
        }

        if (hasText) return true;          // Accept the value if it is not empty.
        cout << "This field cannot be empty.\n";
    }
}

// Taking all student details and storing them in the Student structure.
bool readStudent(Student& s, int id) {
    s.id = id;                             // Store the already-entered student ID.

    if (!readText("Name: ", s.name)) return false;
    if (!readText("Department: ", s.department)) return false;

    // Semester must be between 1 and 8.
    s.semester = readNumber("Semester (1-8): ", 1, 8);

    return s.semester != -1;               // Return false if input was closed.
}

// Creating a new node dynamically and storing the given student record in it.
Node* makeNode(Student s) 
{
    Node* newNode = new Node;              // Allocate memory for a new node.
    newNode->data = s;                     // Copy student details into the node.
    newNode->next = NULL;                  // The new node initially points to nothing.
    return newNode;                        // Return the address of the new node.
}

// Searches a linked list for a student with the given ID.
// Returns the node address if found; otherwise, returns NULL.
Node* findStudent(Node* start, int id)
 {
    while (start != NULL) {                // Continue until the end of the list.
        if (start->data.id == id)          // Compare the current student's ID.
            return start;                  // Return the node if the ID matches.

        start = start->next;               // Move to the next node.
    }

    return NULL;                           // Student was not found.
}

// Checks whether an ID is already used by an active student or pending applicant.
bool idInUse(int id) {
    return findStudent(head, id) != NULL || findStudent(front, id) != NULL;
}

// Displays the details of one student.
void printStudent(Student s)
 {
    cout << "ID: " << s.id << " | Name: " << s.name
         << " | Department: " << s.department
         << " | Semester: " << s.semester << "\n";
}

// LINKED LIST: Adds a new student at the beginning of the active-students list.
void addStudent() {
    int id = readNumber("Student ID: ", 1, 999999999);
    if (id == -1) return;                  // Stop if input is closed.

    if (idInUse(id)) {                     // Do not allow a duplicate ID.
        cout << "ID already exists in students or pending admissions.\n";
        return;
    }

    Student s;                             // Create a temporary Student record.
    if (!readStudent(s, id)) return;       // Read the student's remaining details.

    Node* newNode = makeNode(s);           // Create a node for this student.
    newNode->next = head;                  // Link it to the current first node.
    head = newNode;                        // Make the new node the first node.

    cout << "Student added.\n";
}

// Traverses the active-students linked list and displays every student.
void displayStudents() {
    if (head == NULL) {                    // Check whether the list is empty.
        cout << "No students found.\n";
        return;
    }

    cout << "\nACTIVE STUDENTS\n";
    Node* current = head;                  // Start from the first student.

    while (current != NULL) {              // Visit every node in the list.
        printStudent(current->data);       // Display the current student's details.
        current = current->next;           // Move to the next node.
    }
}

// Searches for an active student using the student's ID.
void searchStudent() {
    int id = readNumber("Student ID to search: ", 1, 999999999);
    if (id == -1) return;

    Node* result = findStudent(head, id);  // Search the active-students list.

    if (result == NULL)
        cout << "Student not found.\n";
    else
        printStudent(result->data);        // Display the student if found.
}

// Finds a student and updates the student's details.
void updateStudent() {
    int id = readNumber("Student ID to update: ", 1, 999999999);
    if (id == -1) return;

    Node* result = findStudent(head, id);  // Find the student by ID.

    if (result == NULL) {                  // Check whether the student exists.
        cout << "Student not found.\n";
        return;
    }

    Student updated;                       // Temporary variable for updated details.
    if (!readStudent(updated, id)) return; // Read the new details using the same ID.

    result->data = updated;                // Replace old details; keep the same ID.
    cout << "Student updated.\n";
}

// Removes a student from the linked list and PUSHES the node onto the stack.
// The stack makes it possible to undo the most recent deletion.
void deleteStudent() {
    int id = readNumber("Student ID to delete: ", 1, 999999999);
    if (id == -1) return;

    Node* current = head;                  // Start searching from the first node.
    Node* previous = NULL;                 // Stores the node before current.

    // Search for the student while keeping track of the previous node.
    while (current != NULL && current->data.id != id) {
        previous = current;                // Save the current node as previous.
        current = current->next;           // Move to the next node.
    }

    if (current == NULL) {                 // Student does not exist in the list.
        cout << "Student not found.\n";
        return;
    }

    // If previous is NULL, the student is the first node.
    if (previous == NULL)
        head = current->next;              // Move head to the second node.
    else
        previous->next = current->next;    // Skip the node being deleted.

    current->next = top;                   // Link deleted node to the old stack top.
    top = current;                         // PUSH: make deleted node the new stack top.

    cout << "Student deleted. Use option 6 to undo.\n";
}

// STACK: POPs the most recently deleted student and restores that student.
void undoDelete() {
    if (top == NULL) {                     // Check whether the stack is empty.
        cout << "Nothing to undo.\n";
        return;
    }

    // Do not restore if the same ID is already active or pending admission.
    if (idInUse(top->data.id)) {
        cout << "Undo blocked: this ID is active or pending.\n";
        return;
    }

    Node* restored = top;                  // Save the node at the top of the stack.
    top = top->next;                       // POP: move top to the next stack node.

    restored->next = head;                 // Link restored node to active students.
    head = restored;                       // Make the restored student the first node.

    cout << "Restored student ID " << restored->data.id << ".\n";
}

// QUEUE: Adds a new admission request at the rear (ENQUEUE operation).
void addAdmission() {
    int id = readNumber("Applicant student ID: ", 1, 999999999);
    if (id == -1) return;

    if (idInUse(id)) {                     // Prevent duplicate student IDs.
        cout << "ID already exists in students or pending admissions.\n";
        return;
    }

    Student s;
    if (!readStudent(s, id)) return;        // Read applicant details.

    Node* newNode = makeNode(s);           // Create a node for the admission request.

    if (front == NULL)
        front = rear = newNode;            // If empty, new node is both front and rear.
    else {
        rear->next = newNode;              // Attach new node after the current rear.
        rear = newNode;                    // Update rear to the new last node.
    }

    cout << "Admission request queued.\n";
}

// QUEUE: Removes the first request (DEQUEUE) and adds that student to the active list.
void processAdmission() {
    if (front == NULL) {                   // Check whether the admission queue is empty.
        cout << "No pending admissions.\n";
        return;
    }

    Node* admitted = front;                // Save the first request in the queue.
    front = front->next;                   // DEQUEUE: move front to the next request.

    if (front == NULL)
        rear = NULL;                       // If queue became empty, rear must also be NULL.

    admitted->next = head;                 // Link admitted student to active list.
    head = admitted;                       // Add admitted student at the beginning.

    cout << "Admitted student ID " << admitted->data.id << ".\n";
}

// Displays all pending admission requests in FIFO order.
void displayAdmissions() {
    if (front == NULL) {                   // Check whether the queue is empty.
        cout << "No pending admissions.\n";
        return;
    }

    cout << "\nPENDING ADMISSIONS (first request first)\n";
    Node* current = front;                 // Start from the first queued request.

    while (current != NULL) {              // Traverse the complete queue.
        printStudent(current->data);       // Display the current applicant.
        current = current->next;           // Move to the next request.
    }
}

// Deletes all nodes in a linked structure to release dynamically allocated memory.
void freeNodes(Node* start) {
    while (start != NULL) {                // Continue until all nodes are deleted.
        Node* old = start;                 // Keep the address of the current node.
        start = start->next;               // Move forward before deleting the node.
        delete old;                        // Release the node's allocated memory.
    }
}

// MAIN FUNCTION
int main() {
    int choice;

    cout << "UNIVERSITY MANAGEMENT SYSTEM\n";

    do {
        // Display all available operations.
        cout << "\n1. Add student\n2. Display students\n3. Search student\n";
        cout << "4. Update student\n5. Delete student\n6. Undo last deletion\n";
        cout << "7. Add admission request\n8. Process next admission\n";
        cout << "9. Display admission queue\n0. Exit\n";

        choice = readNumber("Choice: ", 0, 9); // Get a valid menu choice.
        if (choice == -1) break;             // Exit if input is closed.

        // Calling the functions
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

    // Free memory used by active students, pending admissions, and deleted students.
    freeNodes(head);
    freeNodes(front);
    freeNodes(top);

    if (choice == -1)
        cout << "Input closed.\n";
    else
        cout << "Goodbye. Session data cleared.\n";

    return 0;                               
}
