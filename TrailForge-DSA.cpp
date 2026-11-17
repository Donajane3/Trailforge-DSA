#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>
#include <iomanip>
using namespace std;

struct Product {
    int id;
    string name, category, brand;
    double price;
    int stock;
};

// LAB 1: Dynamic Array
class Catalog {
public:
    Product* items;
    int count, capacity;

    Catalog() {
        count = 8;
        capacity = 10;
        items = new Product[capacity];

        Product p[] = {
            {1,"Hiking Backpack","Bags","Osprey",3500,15},
            {2,"Trekking Shoes","Footwear","Merrell",4200,20},
            {3,"Camping Tent","Camping","Columbia",5500,10},
            {4,"Rain Jacket","Clothing","The North Face",2800,18},
            {5,"Sleeping Bag","Camping","Columbia",2200,12},
            {6,"Hiking Pants","Clothing","The North Face",1800,25},
            {7,"Trail Running Shoes","Footwear","Merrell",3900,14},
            {8,"Travel Backpack","Bags","Osprey",2500,16}
        };

        for(int i=0;i<count;i++)
            items[i] = p[i];
    }

    void resize() {
        capacity *= 2;
        Product* temp = new Product[capacity];

        for(int i=0;i<count;i++)
            temp[i] = items[i];

        delete[] items;
        items = temp;

        cout << "Catalog memory successfully resized!\n";
    }

    void addProduct(Product p) {
        if(count == capacity) resize();
        items[count++] = p;
        cout << "Product added successfully!\n";
    }

    ~Catalog() {
        delete[] items;
    }
};

// LAB 2: Doubly Linked List - Shopping Cart
struct CartNode {
    int index, quantity;
    CartNode *prev, *next;

    CartNode(int i,int q) {
        index=i; quantity=q;
        prev=next=nullptr;
    }
};

class Cart {
public:
    CartNode *head=nullptr, *tail=nullptr;

    void add(int index,int quantity) {
        CartNode* temp=head;

        while(temp) {
            if(temp->index==index) {
                temp->quantity+=quantity;
                return;
            }
            temp=temp->next;
        }

        CartNode* node=new CartNode(index,quantity);
        if(!head) head=tail=node;
        else {
            tail->next=node;
            node->prev=tail;
            tail=node;
        }
    }

    void display(Catalog& c) {
        if(!head) {
            cout << "Cart is empty.\n";
            return;
        }

        double total=0;
        cout << "\n--- SHOPPING CART ---\n";

        for(CartNode* p=head;p;p=p->next) {
            double subtotal=c.items[p->index].price*p->quantity;
            cout << c.items[p->index].name
                 << " x " << p->quantity
                 << " = P" << subtotal << "\n";
            total+=subtotal;
        }

        cout << "TOTAL: P" << total << "\n";
    }

    void clear() {
        while(head) {
            CartNode* temp=head;
            head=head->next;
            delete temp;
        }
        tail=nullptr;
    }

    ~Cart() { clear(); }
};

// LAB 3: Circular Linked List - Brand Showcase
struct BrandNode {
    string name;
    BrandNode* next;
    BrandNode(string n) {
        name=n; next=nullptr;
    }
};

class BrandShowcase {
    BrandNode* head=nullptr;

public:
    void add(string name) {
        BrandNode* node=new BrandNode(name);

        if(!head) {
            head=node;
            node->next=head;
        } else {
            BrandNode* temp=head;
            while(temp->next!=head)
                temp=temp->next;

            temp->next=node;
            node->next=head;
        }
    }

    void display() {
        if(!head) return;

        BrandNode* temp=head;
        cout << "\n--- PARTNER BRANDS ---\n";

        do {
            cout << "[ " << temp->name << " ] ";
            temp=temp->next;
        } while(temp!=head);

        cout << "\n";
    }

    ~BrandShowcase() {
        if(!head) return;
        BrandNode* temp=head->next;

        while(temp!=head) {
            BrandNode* next=temp->next;
            delete temp;
            temp=next;
        }

        delete head;
    }
};

// LAB 4: Stack - Undo / Redo
struct Action {
    int index, before, after;
};

class InventoryHistory {
    vector<Action> undoStack, redoStack;

public:
    void update(Catalog& c,int index,int newStock) {
        Action a={index,c.items[index].stock,newStock};
        c.items[index].stock=newStock;
        undoStack.push_back(a);
        redoStack.clear();
    }

    void undo(Catalog& c) {
        if(undoStack.empty()) {
            cout << "Nothing to undo.\n";
            return;
        }

        Action a=undoStack.back();
        undoStack.pop_back();

        c.items[a.index].stock=a.before;
        redoStack.push_back(a);

        cout << "Undo successful!\n";
    }

    void redo(Catalog& c) {
        if(redoStack.empty()) {
            cout << "Nothing to redo.\n";
            return;
        }

        Action a=redoStack.back();
        redoStack.pop_back();

        c.items[a.index].stock=a.after;
        undoStack.push_back(a);

        cout << "Redo successful!\n";
    }
};

// LAB 5: Queue - FIFO Checkout
struct Order {
    int index, quantity;
};

void checkout(Catalog& c,Cart& cart,
              InventoryHistory& history) {
    if(!cart.head) {
        cout << "Your cart is empty!\n";
        return;
    }

    queue<Order> orders;
    bool valid=true;

    for(CartNode* p=cart.head;p;p=p->next) {
        if(p->quantity>c.items[p->index].stock) {
            cout << "Insufficient stock for "
                 << c.items[p->index].name << "\n";
            valid=false;
        }
        orders.push({p->index,p->quantity});
    }

    if(!valid) {
        cout << "Checkout cancelled. Please update your cart.\n";
        return;
    }

    double total=0;
    cout << "\n--- CUSTOMER CHECKOUT ---\n";

    while(!orders.empty()) {
        Order o=orders.front();
        orders.pop();

        Product& p=c.items[o.index];
        total+=p.price*o.quantity;

        history.update(c,o.index,p.stock-o.quantity);

        cout << "Processed: " << p.name
             << " x " << o.quantity << "\n";
    }

    cout << "Total Payment: P" << total << "\n";
    cout << "Order completed successfully!\n";

    cart.clear();
}

// LAB 6: Binary Search Tree - Price Index
struct TreeNode {
    double price;
    TreeNode *left,*right;

    TreeNode(double p) {
        price=p;
        left=right=nullptr;
    }
};

TreeNode* insert(TreeNode* root,double price) {
    if(!root) return new TreeNode(price);

    if(price<root->price)
        root->left=insert(root->left,price);
    else
        root->right=insert(root->right,price);

    return root;
}

void inorder(TreeNode* root) {
    if(!root) return;

    inorder(root->left);
    cout << "P" << root->price << " ";
    inorder(root->right);
}

void deleteTree(TreeNode* root) {
    if(!root) return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

void showPriceIndex(Catalog& c) {
    TreeNode* root=nullptr;

    for(int i=0;i<c.count;i++)
        root=insert(root,c.items[i].price);

    cout << "\nBST Price Index (Ascending):\n";
    inorder(root);
    cout << "\n";

    deleteTree(root);
}

// LAB 7: Linear Search
void linearSearch(Catalog& c) {
    string keyword;
    bool found=false;

    cout << "Enter product keyword: ";
    getline(cin,keyword);

    for(int i=0;i<c.count;i++) {
        if(c.items[i].name.find(keyword)!=string::npos ||
           c.items[i].category.find(keyword)!=string::npos ||
           c.items[i].brand.find(keyword)!=string::npos) {

            cout << c.items[i].name << " - "
                 << c.items[i].brand << " - P"
                 << c.items[i].price << "\n";
            found=true;
        }
    }

    if(!found) cout << "No matching product found.\n";
}

// Binary Search - Price Range
void binaryPriceSearch(Catalog& c) {
    vector<Product> sorted(c.items,c.items+c.count);

    sort(sorted.begin(),sorted.end(),
        [](Product a,Product b) {
            return a.price<b.price;
        });

    double low,high;
    cout << "Minimum price: ";
    cin >> low;
    cout << "Maximum price: ";
    cin >> high;

    auto first=lower_bound(sorted.begin(),sorted.end(),low,
        [](Product p,double value) {
            return p.price<value;
        });

    auto last=upper_bound(sorted.begin(),sorted.end(),high,
        [](double value,Product p) {
            return value<p.price;
        });

    if(first==last) {
        cout << "No products in this price range.\n";
        return;
    }

    for(auto it=first;it!=last;it++)
        cout << it->name << " - P" << it->price << "\n";
}

// LAB 8: Insertion Sort and Quick Sort
void insertionSort(vector<Product>& a) {
    for(int i=1;i<(int)a.size();i++) {
        Product key=a[i];
        int j=i-1;

        while(j>=0 && a[j].price>key.price) {
            a[j+1]=a[j];
            j--;
        }

        a[j+1]=key;
    }
}

int partition(vector<Product>& a,int low,int high) {
    double pivot=a[high].price;
    int i=low-1;

    for(int j=low;j<high;j++) {
        if(a[j].price<=pivot) {
            i++;
            swap(a[i],a[j]);
        }
    }

    swap(a[i+1],a[high]);
    return i+1;
}

void quickSort(vector<Product>& a,int low,int high) {
    if(low<high) {
        int pi=partition(a,low,high);
        quickSort(a,low,pi-1);
        quickSort(a,pi+1,high);
    }
}

void displaySorted(Catalog& c) {
    vector<Product> a(c.items,c.items+c.count);
    int choice;

    cout << "1. Insertion Sort\n";
    cout << "2. Quick Sort\n";
    cout << "Choose sorting method: ";
    cin >> choice;

    if(choice==1)
        insertionSort(a);
    else if(choice==2)
        quickSort(a,0,(int)a.size()-1);
    else {
        cout << "Invalid choice.\n";
        return;
    }

    for(auto p:a)
        cout << p.name << " | " << p.brand
             << " | P" << p.price << "\n";
}

// Main Program
int main() {
    Catalog catalog;
    Cart cart;
    BrandShowcase brands;
    InventoryHistory history;

    brands.add("The North Face");
    brands.add("Columbia");
    brands.add("Merrell");
    brands.add("Osprey");

    int choice;

    do {
        cout << "\n====================================\n";
        cout << "       TRAILFORGE OUTDOOR GEAR\n";
        cout << "  E-COMMERCE INVENTORY & SEARCH\n";
        cout << "====================================\n";
        cout << "1. View Complete Catalog\n";
        cout << "2. Filter by Official Brand\n";
        cout << "3. Search Products by Keyword\n";
        cout << "4. Filter Products by Price Range\n";
        cout << "5. Add Item to Shopping Cart\n";
        cout << "6. Process Customer Checkout\n";
        cout << "7. Admin Undo / Redo Inventory\n";
        cout << "8. Display Partner Brand Showcase\n";
        cout << "9. Sort Inventory Display\n";
        cout << "10. Add New Product\n";
        cout << "11. Exit\n";
        cout << "====================================\n";
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore();

        switch(choice) {
            case 1:
                cout << "\nID | PRODUCT | BRAND | PRICE | STOCK\n";
                for(int i=0;i<catalog.count;i++) {
                    Product p=catalog.items[i];
                    cout << p.id << " | " << p.name
                         << " | " << p.brand
                         << " | P" << p.price
                         << " | " << p.stock << "\n";
                }
                break;

            case 2: {
                string brand;
                cout << "Enter brand name: ";
                getline(cin,brand);
                bool found=false;

                for(int i=0;i<catalog.count;i++) {
                    Product p=catalog.items[i];
                    if(p.brand==brand) {
                        cout << p.name << " - P"
                             << p.price << "\n";
                        found=true;
                    }
                }

                if(!found) cout << "Brand not found.\n";
                break;
            }

            case 3:
                linearSearch(catalog);
                break;

            case 4:
                binaryPriceSearch(catalog);
                break;

            case 5: {
                int id,qty,index=-1;
                cout << "Enter Product ID: ";
                cin >> id;
                cout << "Enter quantity: ";
                cin >> qty;

                for(int i=0;i<catalog.count;i++)
                    if(catalog.items[i].id==id)
                        index=i;

                if(index==-1)
                    cout << "Product not found.\n";
                else if(qty<=0 || qty>catalog.items[index].stock)
                    cout << "Invalid quantity or insufficient stock.\n";
                else {
                    cart.add(index,qty);
                    cout << "Added to cart!\n";
                }

                cart.display(catalog);
                break;
            }

            case 6:
                checkout(catalog,cart,history);
                break;

            case 7: {
                int action;
                cout << "1. Undo Inventory Change\n";
                cout << "2. Redo Inventory Change\n";
                cout << "Choose: ";
                cin >> action;

                if(action==1) history.undo(catalog);
                else if(action==2) history.redo(catalog);
                else cout << "Invalid choice.\n";
                break;
            }

            case 8:
                brands.display();
                break;

            case 9:
                displaySorted(catalog);
                break;

            case 10: {
                Product p;
                p.id=100+catalog.count;

                cout << "Product name: ";
                getline(cin,p.name);
                cout << "Category: ";
                getline(cin,p.category);
                cout << "Partner brand: ";
                getline(cin,p.brand);
                cout << "Price: ";
                cin >> p.price;
                cout << "Stock quantity: ";
                cin >> p.stock;

                if(p.price<0 || p.stock<0) {
                    cout << "Invalid price or stock.\n";
                    break;
                }

                catalog.addProduct(p);
                break;
            }

            case 11:
                cout << "Thank you for visiting TrailForge!\n";
                break;

            default:
                cout << "Invalid choice. Try again.\n";
        }

    } while(choice!=11);

    return 0;
}