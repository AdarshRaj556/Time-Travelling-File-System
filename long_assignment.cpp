#include<bits/stdc++.h>
using namespace std;
    class TreeNode{
        public:
            bool is_snapshot;
            int version_id;
            string content;
            string message;
            time_t created_timestamp;
            time_t snapshot_timestamp; 
            TreeNode* parent;
            vector<TreeNode*> children ;
        TreeNode(string content="",int version_id=0,TreeNode* parent=nullptr,string message=""){
            is_snapshot=false;
            this->version_id=version_id;
            this->content=content;
            this->message=message;
            created_timestamp=time(nullptr);
            this->parent=parent;
        }
        ~TreeNode() {
        for (TreeNode* child : children) {
            delete child;
        }
        children.clear();
    }
    };

class MyHashMap{
    struct Node{
        int key;
        TreeNode* value;
        Node* next;
        Node(int k,TreeNode* v){
            key=k;
            value=v;
            next=nullptr;
        }
    };
    vector<Node*> table;
    int capacity;
    int count;
    const double loadFactor=0.75;
    int hash(int key){
        return key%capacity;
    }
    void rehash(){
        int oldCapacity=capacity;
        vector<Node*> oldTable=table;
        capacity*=2;
        table.assign(capacity,nullptr);
        count=0;
        for(int i=0;i<oldCapacity;i++){
            Node* curr=oldTable[i];
            while(curr){
                put(curr->key,curr->value);
                Node* temp=curr;
                curr=curr->next;
                delete temp;
            }
        }
    }
public:
    MyHashMap(int initialSize=100){
        capacity=initialSize;
        table.assign(capacity,nullptr);
        count=0;
    }
    void put(int key,TreeNode* value){
        int idx=hash(key);
        Node* curr=table[idx];
        while(curr){
            if(curr->key==key){
                curr->value=value;
                return;
            }
            curr=curr->next;
        }
        Node* newNode=new Node(key,value);
        newNode->next=table[idx];
        table[idx]=newNode;
        count++;
        if((double)count/capacity>loadFactor)rehash();
    }
    TreeNode* get(int key){
        int idx=hash(key);
        Node* curr=table[idx];
        while(curr){
            if(curr->key==key)return curr->value;
            curr=curr->next;
        }
        return nullptr;
    }
    void remove(int key){
        int idx=hash(key);
        Node* curr=table[idx];
        Node* prev=nullptr;
        while(curr){
            if(curr->key==key){
                if(prev)prev->next=curr->next;
                else table[idx]=curr->next;
                delete curr;
                count--;
                return;
            }
            prev=curr;
            curr=curr->next;
        }
    }
    ~MyHashMap() {
        for (Node* head : table) {
            while(head){
                Node* tmp = head;
                head = head->next;
                delete tmp;
            }
        }
    }
};

    class file_struct{
        public:
            TreeNode* root;
            string filename;
            TreeNode* active_version;
            MyHashMap mp;
            int total_versions;
            time_t last_updated;
            int idx1;
            int idx2;
        file_struct(){
            root=nullptr;
            active_version=root;
            total_versions=0;
            idx1=0;
            idx2=0;
            filename="";
        }
        void name_file(string val){
            filename=val;
        }
        void create(){
            if(root==nullptr){
                root=new TreeNode;
                mp.put(total_versions,root);
                active_version=root;
                root->is_snapshot=true;
                root->message="FILE CREATED";
                total_versions++;
                last_updated=time(nullptr);
            }
        }
        string read(){
            if(active_version->content.size()==0){
                string str="Empty File";
                return str;
            }else{
                if(active_version){
                    return active_version->content;
                }else{
                    return "";
                }
            }
        }
        string get_filename(){
            return filename;
        }
        void insert(string cont){
            if(!active_version) return;
            if(active_version->is_snapshot==false){
                // active_version->content+=" ";
                active_version->content+=cont;
            }else{
                string str=active_version->content;
                // str+=" ";
                str+=cont;
                TreeNode* temp=new TreeNode(str,total_versions);
                temp->parent=active_version;
                active_version->children.push_back(temp);
                mp.put(total_versions,temp);
                active_version=temp;
                total_versions++;
            }
            last_updated=time(nullptr);
        }
        void update(string cont){
            if(!active_version) return;
            if(active_version->is_snapshot==false){
                active_version->content=cont;
            }else{
                TreeNode* temp=new TreeNode(cont,total_versions);
                temp->parent=active_version;
                active_version->children.push_back(temp);
                mp.put(total_versions,temp);
                active_version=temp;
                total_versions++;
            }
            last_updated=time(nullptr);
        }
        void snapshot(string msg){
            if(!active_version) return;
            if(active_version->is_snapshot){
                cout<<"Already snapshotted"<<endl;
                return;
            }
            if(msg.size()==0){
                cout<<"Message must be provided to snapshot"<<endl;
            }
            active_version->message=msg;
            active_version->snapshot_timestamp=time(nullptr);
            active_version->is_snapshot=true;
        }
        void rollback(int version_id=-1){
            if(!active_version) return;
            if(version_id==-1){
                if(active_version->parent) active_version=active_version->parent;
                else { cout << "Parent does not exist" << endl;}
            }else{
                TreeNode* target=mp.get(version_id);
                if(target){
                    active_version=target;
                }else{
                    cout<<"Total number of version are only "<<total_versions<<endl;
                }
            }
        }
        void history(){
            vector<TreeNode*> temp;
            TreeNode *curr=active_version;
            if(curr->is_snapshot){
                temp.push_back(curr);
            }
            while(curr->parent){
                if(curr->parent->is_snapshot){
                    temp.push_back(curr->parent);
                }
                curr=curr->parent;
            }
            for(int i=temp.size()-1;i>=0;i--){
                cout<<"Version ID:"<<temp[i]->version_id<<" TimeStamp:"<<temp[i]->created_timestamp<<" Snap Shot message:"<<temp[i]->message<<endl;
            }
            if(temp.size()==0){
                cout<<"NO HISTORY"<<endl;
            }
        }
        ~file_struct() {
            delete root;
            root = nullptr;
            active_version = nullptr;
        }
    };

class HashMap {
    struct Node {
        string key;
        file_struct* value;
        Node* next;
        Node(string k, file_struct* v) {
            key=k;
            value=v;
            next=nullptr; 
        }
    };
    vector<Node*> table;
    int capacity;
    int count;
    double loadFactorThreshold=0.75;
    int hash(const string &key, int mod) {
        const int p=131;
        long long hash_val=0,p_pow=1;
        for(unsigned char c:key){
            hash_val=(hash_val+(c+1)*p_pow)%mod;
            p_pow=(p_pow*p)%mod;
        }
        return (int)(hash_val%mod);
    }
    void insertWithoutRehash(const string &key, file_struct* value) {
        int idx=hash(key,capacity);
        Node* newNode=new Node(key,value);
        newNode->next=table[idx];
        table[idx]=newNode;
        count++;
    }
    void rehash() {
        int oldCapacity=capacity;
        vector<Node*> oldTable=table;
        capacity*=2;
        table.assign(capacity,nullptr);
        count=0;
        for(int i=0;i<oldCapacity;i++){
            Node* head=oldTable[i];
            while(head){
                insertWithoutRehash(head->key,head->value);
                Node* temp=head;
                head=head->next;
                delete temp;
            }
        }
    }
    public:
        HashMap(int initialCapacity=100) {
            capacity=initialCapacity;
            table.resize(capacity,nullptr);
            count=0;
        }
        void put(const string &key, file_struct* value) {
            int idx=hash(key,capacity);
            for(Node* curr=table[idx];curr;curr=curr->next){
                if(curr->key==key){
                    curr->value=value;
                    return;
                }
            }
            insertWithoutRehash(key,value);
            if((double)count/capacity>loadFactorThreshold){
                rehash();
            }
        }
        file_struct* get(const string &key) {
            int idx=hash(key,capacity);
            for(Node* curr=table[idx];curr;curr=curr->next){
                if(curr->key==key) return curr->value;
            }
            return nullptr;
        }
        
        ~HashMap() {
            for(Node* head:table){
                while(head){
                    Node* temp=head;
                    head=head->next;
                    delete temp->value;
                    delete temp;
                }
            }
        }
    };
class MyHeap {
public:
    vector<file_struct*> heap1;
    vector<file_struct*> heap2;
    int size1, size2;
    MyHeap() {
        size1 = 0;
        size2 = 0;
    }
    
    void push1(file_struct* val) {
        heap1.push_back(val);
        val->idx1 = size1;
        size1++;
        while (val->idx1 > 0 && val->total_versions > heap1[(val->idx1 - 1) / 2]->total_versions) {
            int parentIdx = (val->idx1 - 1) / 2;
            file_struct* parentNode = heap1[parentIdx];
            swap(heap1[val->idx1], heap1[parentIdx]);
            int oldIdx = val->idx1;
            val->idx1 = parentIdx;
            parentNode->idx1 = oldIdx;
        }
    }
    void push2(file_struct* val) {
        heap2.push_back(val);
        val->idx2 = size2;
        size2++;
        while (val->idx2 > 0 && val->last_updated > heap2[(val->idx2 - 1) / 2]->last_updated) {
            int parentIdx = (val->idx2 - 1) / 2;
            file_struct* parentNode = heap2[parentIdx];
            swap(heap2[val->idx2], heap2[parentIdx]);
            int oldIdx = val->idx2;
            val->idx2 = parentIdx;
            parentNode->idx2 = oldIdx;
        }
    }
    void update1(file_struct* val) {
        while (val->idx1 > 0 && val->total_versions > heap1[(val->idx1 - 1) / 2]->total_versions) {
            int parentIdx = (val->idx1 - 1) / 2;
            file_struct* parentNode = heap1[parentIdx];
            swap(heap1[val->idx1], heap1[parentIdx]);
            int oldIdx = val->idx1;
            val->idx1 = parentIdx;
            parentNode->idx1 = oldIdx;
        }
    }
    void update2(file_struct* val) {
        while (val->idx2 > 0 && val->last_updated > heap2[(val->idx2 - 1) / 2]->last_updated) {
            int parentIdx = (val->idx2 - 1) / 2;
            file_struct* parentNode = heap2[parentIdx];
            swap(heap2[val->idx2], heap2[parentIdx]);
            int oldIdx = val->idx2;
            val->idx2 = parentIdx;
            parentNode->idx2 = oldIdx;
        }
    }
    void heapify_down1(file_struct* val) {
        int n1 = heap1.size();
        int idx1 = val->idx1;
        while (true) {
            int left = 2 * idx1 + 1;
            int right = 2 * idx1 + 2;
            int largest = idx1;
            if (left < n1 && heap1[left]->total_versions > heap1[largest]->total_versions) largest = left;
            if (right < n1 && heap1[right]->total_versions > heap1[largest]->total_versions) largest = right;
            if (largest != idx1) {
                swap(heap1[idx1], heap1[largest]);
                heap1[idx1]->idx1 = idx1;
                heap1[largest]->idx1 = largest;
                idx1 = largest;
            } else break;
        }
    }
    void heapify_down2(file_struct* val) {
        int n2 = heap2.size();
        int idx2 = val->idx2;
        while (true) {
            int left = 2 * idx2 + 1;
            int right = 2 * idx2 + 2;
            int newest = idx2;
            if (left < n2 && heap2[left]->last_updated > heap2[newest]->last_updated) newest = left;
            if (right < n2 && heap2[right]->last_updated > heap2[newest]->last_updated) newest = right;
            if (newest != idx2) {
                swap(heap2[idx2], heap2[newest]);
                heap2[idx2]->idx2 = idx2;
                heap2[newest]->idx2 = newest;
                idx2 = newest;
            } else break;
        }
    }
    file_struct* pop1() {
        if (heap1.empty()) return nullptr;
        file_struct* top = heap1[0];
        swap(heap1[0], heap1.back());
        heap1[0]->idx1 = 0;
        heap1.pop_back();
        size1--;
        if (!heap1.empty()) heapify_down1(heap1[0]);
        return top;
    }
    file_struct* pop2() {
        if (heap2.empty()) return nullptr;
        file_struct* top = heap2[0];
        swap(heap2[0], heap2.back());
        heap2[0]->idx2 = 0;
        heap2.pop_back();
        size2--;
        if (!heap2.empty()) heapify_down2(heap2[0]);
        return top;
    }
    ~MyHeap() {
        heap1.clear();
        heap2.clear();
    }
};
int main() {
    cout<<"TIME TRAVELLING FILE SYSTEM"<<endl;
    string data;
    HashMap root_map;
    MyHeap system_wide;
    int total_files=0;
    while(true){
        getline(cin,data);
        int j=0;
        string cmd="",filename="",rest="";
        bool flag=true;
        for(int i=0;i<data.size();i++){
            if(data[i]==' ' && flag){
                j++;
                if(j>1){
                    flag=false;
                }
                continue;
            }
            if(j==0){
                cmd+=data[i];
            }
            if(j==1){
                filename+=data[i];
            }
            if(j>1){
                rest+=data[i];
            }
        }
        if(cmd=="CREATE"){
            if(root_map.get(filename)!=nullptr){
                cout<<filename<<" File already exists"<<endl;
                continue;
            }
            if(filename.size()==0){
                cout<<"Give the name of file"<<endl;
                continue;
            }
            if(rest.size()!=0){
                cout<<"Extra argument is given"<<endl;
                continue;
            }
            file_struct* f1=new file_struct;
            f1->name_file(filename);
            f1->create();
            root_map.put(filename,f1);
            system_wide.push1(f1);
            system_wide.push2(f1);
            total_files++;
            cout<<"File is created with file name "<<filename<<endl;   
        }else if(cmd=="READ"){
            if(filename.size()==0){
                cout<<"Give the name of file"<<endl;
                continue;
            }
            if(rest.size()!=0){
                cout<<"Extra argument is given"<<endl;
                continue;
            }
            file_struct* f=root_map.get(filename);
            if(f){
                cout<<f->read()<<endl;
            }else{
                cout<<"File with filename "<<filename<<" doesn't exist"<<endl;
            }
        }else if(cmd=="INSERT"){
            if(filename.size()==0){
                cout<<"Give the name of file"<<endl;
                continue;
            }
            file_struct* f=root_map.get(filename);
            if(f){
                if((int)rest.size()==0){
                    cout<<"No content is provided tha's why no insertion takes place"<<endl;
                }else{
                    f->insert(rest);
                    system_wide.update1(f);
                    system_wide.update2(f);
                }
            }else{
                cout<<"File with filename "<<filename<<" doesn't exist"<<endl;
            }
        }else if(cmd=="UPDATE"){
            if(filename.size()==0){
                cout<<"Give the name of file"<<endl;
                continue;
            }
            file_struct* f=root_map.get(filename);
            if(f){
                if(rest.size()==0){
                    cout<<"No content is provided that's why no updation takes place"<<endl;
                }else{
                    f->update(rest);
                    system_wide.update1(f);
                    system_wide.update2(f);
                }
            }else{
                cout<<"File with filename "<<filename<<" doesn't exist"<<endl;
            }
        }else if(cmd=="SNAPSHOT"){
            if(filename.size()==0){
                cout<<"Give the name of file"<<endl;
                continue;
            }
            file_struct* f=root_map.get(filename);
            if(f){
                if(rest.size()==0){
                    cout<<"No message is provided that'why snapshot is not happened"<<endl;
                    continue;
                }else{
                    f->snapshot(rest);
                }
            }else{
                cout<<"File with filename "<<filename<<" doesn't exist"<<endl;
            }
        }else if(cmd=="ROLLBACK"){
            if(filename.size()==0){
                cout<<"Give the name of file"<<endl;
                continue;
            }
            file_struct* f=root_map.get(filename);
            if(f){
                if(rest.size()==0){
                    f->rollback();
                }else{
                    int x;
                    bool flag=1;
                    for(int i=rest.size()-1;i>=0;i--){
                        if(!isdigit(rest[i])){
                            flag=0;
                        }
                    }
                    if(flag==0){
                        cout<<"Not a valid argumnent"<<endl;
                        continue;
                    }
                    x=stoi(rest);
                    if(x==0){
                        f->rollback(0);
                        continue;
                    }
                    if(x){
                        f->rollback(x);
                    }else{
                        cout<<"Rollback version_id must be an integer"<<endl;
                    }
                }
            }else{
                cout<<"File with filename "<<filename<<" doesn't exist"<<endl;
            }
        }else if(cmd=="HISTORY"){
            if(filename.size()==0){
                cout<<"Give the name of file"<<endl;
                continue;
            }
            file_struct *f1=root_map.get(filename);
            if(f1){
                f1->history();
            }else{
                cout<<"File with filename "<<filename<<" doesn't exist"<<endl;
            }
        }else if(cmd=="BIGGEST_TREES"){
            int x;
            bool flag=1;
            for(int i=filename.size()-1;i>=0;i--){
                if(!isdigit(filename[i])){
                    flag=0;
                }
            }
            if(system_wide.heap1.empty()){
                cout<<"Heap is empty"<<endl;
                continue;
            }
            if(filename.size()==0){
                cout<<"num_files argument is not given"<<endl;
                continue;
            }
            if(flag==0){
                cout<<"Second argument should be an integer"<<endl;
                continue;
            }
            x=stoi(filename);
            if(x>total_files){
                cout<<"Not that much file exist"<<endl;
            }else{
                vector<file_struct*> temp;
                for(int i=0;i<x;i++){
                    file_struct *f=system_wide.pop1();
                    cout<<f->filename<<" ";
                    temp.push_back(f);
                }
                for(int i=0;i<x;i++){
                    system_wide.push1(temp[i]);
                }
                cout<<endl;
            }
        }else if(cmd=="RECENT_FILES"){
            int x;
            bool flag=1;
            for(int i=filename.size()-1;i>=0;i--){
                if(!isdigit(filename[i])){
                    flag=0;
                }
            }
            if(system_wide.heap1.empty()){
                cout<<"Heap is empty"<<endl;
                continue;
            }
            if(filename.size()==0){
                cout<<"num_files argument is not given"<<endl;
                continue;
            }
            if(flag==0){
                cout<<"Second argument should be an integer"<<endl;
                continue;
            }
            x=stoi(filename);
            if(x>total_files){
                cout<<"Not that much file exist"<<endl;
            }else{
                vector<file_struct*> temp;
                for(int i=0;i<x;i++){
                    file_struct *f=system_wide.pop2();
                    cout<<f->filename<<" ";
                    temp.push_back(f);
                }
                for(int i=0;i<x;i++){
                    system_wide.push2(temp[i]);
                }
                cout<<endl;
            }
        }else if(cmd=="EXIT"){
            if(filename.size()==0 && rest.size()==0){
                cout<<"Program Terminated";
                break;
            }else{
                cout<<"Enter just EXIT to terminate"<<endl;
            }
        }else{
            cout<<"NOT A VALID COMMAND"<<endl;
        }
    }
    return 0;
}
