/*
   This code can be compiled and run ok.
     利用智慧選取法完海戰船遊戲

   usage (how to run):
     hw42

   input file:
     data_4_6_1.txt

   output file:
     none

   compile (how to compile):
     g++ -o hw42 hw42.cpp

   pseudocode:
     發射砲彈到隨機選取的點，若擊中船則該點上下左右的點都變成better target(加入Target3)，斜對角的點都確定
     是海(從Target0刪除)，直到所有better target都被射過後，才再隨機選取1點射擊;若沒擊中則直接把該點刪除

   coded by 江冠駒, ID: H24031354, email: kevin040208@gmail.com
   date: 2018.05.14
*/


#include<iostream>
#include<fstream>
#include<cstdlib>
#include<ctime>
#include<vector>

using namespace std;

struct Zone{    //此為各網格之struct
        int row;    //儲存其縱軸座標，即由上而下第row列,row=0,...,M-1
        int col;    //儲存其橫軸座標，即由左而右第col行,col=0,...,N-1
        int id;     //儲存其編號，id=0,...,MN-1
        int status; //儲存其狀態，status=-1,0,2,3 (或可再自行定義)
        bool  isB;  //儲存其答案，若為船身則isB=1，否則為海水isB=0
    };

int main(){
    int M,N,            //海域之縱、橫軸總格數
        n_BT,           //戰艦類型(BattleshipType)總數，譬如n_BT=2代表2種船型
        *BTL,           //各船型之長度，譬如BTL[1]=2代表第2類船型之長度為2
        *BTN,           //各船型之船數，譬如BTN[3]=1代表第4類船共有1艘
        n_targets=0,      //全部之船格總數，譬如圖1(b)之n_targets=(2+3)*2=10
        n_shots=0,        //記錄當下已擊發之炮彈總數
        n_hits=0,         //記錄當下已擊中之炮彈總數，當n_hits==n_targets則終止測試
        **BMap,         //記錄作戰圖，BMap[i][j]=-1,0,2,3
        i,j,            //for loop用
        idx,            //隨機選取的index
        pushbackidx=-1,    //要加到Target3(上下左右)的網格在Target0的index
        eraseidx=-1,       //要從Target0移除(斜對角)的index
        wantidx,        //被擊中的網格index
        wantid;         //被擊中的網格id
    bool **SMap,        //記錄解答圖，若座標(i,j)為船身，則SMap[i][j]=1，否則為0
         isDebug;       //詳細模式(isDebug=1,PART1&2用)，列出每步驟過程
                        //扼要模式(isDebug=0,PART3用)，僅列出最後統計資料
    double  hr,         //命中率hitrate百分比=100.*n_hits/n_shots以%為單位
            hrlb,       //命中率百分比下限=100.*n_targets/(M*N)以%為單位
            hrimprate;  //命中改善率=100.*(hr-hrlb)/(100.-hrlb)以%為單位
                        //代表在可改善的空間(100.-hrlb)中，hr的改善(hr-hrlb)比率
    Zone temp;  //暫存某網格之狀態當作將一個struct放進vector的媒介
    vector<Zone> Target0;   //儲存狀態為0的網格，初始時全部網格皆存於此
    vector<Zone> Target3;   //儲存狀態為3的網格
    string  dataname;   //記錄輸入之測試檔名，譬如data_4_6_1.txt

    //----開始----輸入資料--------
    fstream inputfile;
    char filename[50];
    cout << "Enter filename: ";
    cin >> filename;
    inputfile.open(filename,ios::in);

    inputfile>>dataname>>M>>N>>n_BT;
    BTL = new int[n_BT];
    BTN = new int[n_BT];
    for(i=0;i<n_BT;i++){
        inputfile>>BTL[i];
        inputfile>>BTN[i];
        n_targets = n_targets + (BTL[i]*BTN[i]);
    }
    SMap = new bool*[M];
    for(i=0;i<M;i++){
        SMap[i] = new bool[N];
        for(j=0;j<N;j++){
            inputfile >> SMap[i][j];
        }
    }
    //----結束---輸入資料--------

    //----開始---初始化Target0、BMap--------
    for(i=0;i<M;i++){
        for(j=0;j<N;j++){
            temp.row=i; //因為Target0.isB.pushback(0)是行不通的，所以要設一個temp當媒介
            temp.col=j;
            temp.id = i*N+j;
            temp.status = 0;
            temp.isB = 0;
            Target0.push_back(temp);
        }
    }


    BMap = new int*[M];
    for(i=0;i<M;i++){
        BMap[i] = new int[N];
        for(j=0;j<N;j++){
            BMap[i][j]=0;
        }
    }
    //----結束---初始化Target0、BMap--------

    cout << "Debug mode(1:yes;0:no)? ";//是否要debug mode
    cin >> isDebug;

    srand(time(NULL));
    if(isDebug==1){
        //----開始---印出第一行--------
        cout<<"A "<<M<<"x"<<N<<" map, "<<n_BT<<" types, length={";
        for(i=0;i<n_BT;i++){
            if((i+1)==n_BT){cout << BTL[i];}
            else{cout << BTL[i] <<",";}
        }
        cout<<"}, number={";
        for(i=0;i<n_BT;i++){
            if((i+1)==n_BT){cout << BTN[i];}
            else{cout << BTN[i] <<",";}
        }
        cout<<"}, n_targets="<<n_targets<<endl;
        //----結束---印出第一行--------

        //----開始---印出每一步--------
        while(n_hits<n_targets){
            n_shots++;
            if(Target3.size()==0){
                idx = rand()%Target0.size();
                wantid = Target0[idx].id;
                if(SMap[Target0[idx].row][Target0[idx].col]==1){
                    n_hits++;
                    BMap[Target0[idx].row][Target0[idx].col]=2;
                    Target0[idx].status = 2;
                    Target0[idx].isB = 1;
                    cout<<"Trial "<<n_shots<<": zone "<<Target0[idx].id;
                    cout<<"=("<<Target0[idx].row<<","<<Target0[idx].col<<") ";
                    if(n_hits==n_targets){
                        cout<<"hit "<<n_hits<<", "<<(n_targets-n_hits)<<" to go!! Done!!"<<endl;
                    }
                    else{
                        cout<<"hit "<<n_hits<<", "<<(n_targets-n_hits)<<" to go!!"<<endl;
                    }
                    //----開始--處理上下左右(Target3)-----
                    if((Target0[idx].col+1)<N){ //右
                        for(i=0;i<Target0.size();i++){
                            if((Target0[i].row==Target0[idx].row) && (Target0[i].col==(Target0[idx].col+1))){
                                pushbackidx = i;
                                Target0[pushbackidx].status = 3;
                                Target3.push_back(Target0[pushbackidx]);
                                Target0.erase(Target0.begin()+pushbackidx);
                            }
                        }
                    }
                    if((Target0[idx].col-1)>=0){ //左
                        for(i=0;i<Target0.size();i++){
                            if((Target0[i].row==Target0[idx].row) && (Target0[i].col==(Target0[idx].col-1))){
                                pushbackidx = i;
                                Target0[pushbackidx].status = 3;
                                Target3.push_back(Target0[pushbackidx]);
                                Target0.erase(Target0.begin()+pushbackidx);
                            }
                        }
                    }
                    if((Target0[idx].row+1)<M){ //下
                        for(i=0;i<Target0.size();i++){
                            if((Target0[i].row==(Target0[idx].row+1)) && (Target0[i].col==Target0[idx].col)){
                                pushbackidx = i;
                                Target0[pushbackidx].status = 3;
                                Target3.push_back(Target0[pushbackidx]);
                                Target0.erase(Target0.begin()+pushbackidx);
                            }
                        }
                    }
                    if((Target0[idx].row-1)>=0){ //上
                        for(i=0;i<Target0.size();i++){
                            if((Target0[i].row==(Target0[idx].row-1)) && (Target0[i].col==Target0[idx].col)){
                                pushbackidx = i;
                                Target0[pushbackidx].status = 3;
                                Target3.push_back(Target0[pushbackidx]);
                                Target0.erase(Target0.begin()+pushbackidx);
                            }
                        }
                    }

                    //----結束--處理上下左右(Target3)-----

                    //----開始--處理斜對角-----
                    for(i=0;i<Target0.size();i++){ //左上
                        if(Target0[i].id == wantid){
                            wantidx = i;
                        }
                    }
                    if(((Target0[wantidx].row-1)>=0) && ((Target0[wantidx].col-1)>=0)){
                        for(i=0;i<Target0.size();i++){
                            if((Target0[i].row==(Target0[wantidx].row-1)) && (Target0[i].col==(Target0[wantidx].col-1))){
                                eraseidx = i;
                                Target0.erase(Target0.begin()+eraseidx);
                            }
                        }
                    }
                    for(i=0;i<Target0.size();i++){ //右上
                        if(Target0[i].id == wantid){
                            wantidx = i;
                        }
                    }
                    if(((Target0[wantidx].row-1)>=0) && ((Target0[wantidx].col+1)<N)){
                        for(i=0;i<Target0.size();i++){
                            if((Target0[i].row==(Target0[wantidx].row-1)) && (Target0[i].col==(Target0[wantidx].col+1))){
                                eraseidx = i;
                                Target0.erase(Target0.begin()+eraseidx);
                            }
                        }
                    }
                    for(i=0;i<Target0.size();i++){ //左下
                        if(Target0[i].id == wantid){
                            wantidx = i;
                        }
                    }
                    if(((Target0[wantidx].row+1)<M) && ((Target0[wantidx].col-1)>=0)){
                        for(i=0;i<Target0.size();i++){
                            if((Target0[i].row==(Target0[wantidx].row+1)) && (Target0[i].col==(Target0[wantidx].col-1))){
                                eraseidx = i;
                                Target0.erase(Target0.begin()+eraseidx);
                            }
                        }
                    }
                    for(i=0;i<Target0.size();i++){ //右下
                        if(Target0[i].id == wantid){
                            wantidx = i;
                        }
                    }
                    if(((Target0[wantidx].row+1)<M) && ((Target0[wantidx].col+1)<N)){
                        for(i=0;i<Target0.size();i++){
                            if((Target0[i].row==(Target0[wantidx].row+1)) && (Target0[i].col==(Target0[wantidx].col+1))){
                                eraseidx = i;
                                Target0.erase(Target0.begin()+eraseidx);
                            }
                        }
                    }
                    //----結束--處理斜對角-----
                }
                else{
                    BMap[Target0[idx].row][Target0[idx].col]=-1;
                    Target0[idx].status = -1;
                    Target0[idx].isB = 0;
                    cout<<"Trial "<<n_shots<<": zone "<<Target0[idx].id;
                    cout<<"=("<<Target0[idx].row<<","<<Target0[idx].col<<") ";
                    cout<<"missed"<<endl;
                }
                for(i=0;i<Target0.size();i++){
                    if(Target0[i].id == wantid){
                        wantidx = i;
                    }
                }
                Target0.erase(Target0.begin()+wantidx);//將選過的點從Target0中去除，以免重複選到
            }
            else{
                idx = rand()%Target3.size();
                wantid = Target3[idx].id;
                if(SMap[Target3[idx].row][Target3[idx].col]==1){
                    n_hits++;
                    BMap[Target3[idx].row][Target3[idx].col]=2;
                    Target3[idx].status = 2;
                    Target3[idx].isB = 1;
                    cout<<"Trial "<<n_shots<<": zone "<<Target3[idx].id;
                    cout<<"=("<<Target3[idx].row<<","<<Target3[idx].col<<") ";
                    if(n_hits==n_targets){
                        cout<<"hit "<<n_hits<<", "<<(n_targets-n_hits)<<" to go!! Done!!"<<endl;
                    }
                    else{
                        cout<<"hit "<<n_hits<<", "<<(n_targets-n_hits)<<" to go!!"<<endl;
                    }
                    //----開始--處理上下左右(Target3)-----
                    if((Target3[idx].col+1)<N){ //右
                        for(i=0;i<Target0.size();i++){
                            if((Target0[i].row==Target3[idx].row) && (Target0[i].col==(Target3[idx].col+1))){
                                pushbackidx = i;
                                Target0[pushbackidx].status = 3;
                                Target3.push_back(Target0[pushbackidx]);
                                Target0.erase(Target0.begin()+pushbackidx);
                            }
                        }
                    }
                    if((Target3[idx].col-1)>=0){ //左
                        for(i=0;i<Target0.size();i++){
                            if((Target0[i].row==Target3[idx].row) && (Target0[i].col==(Target3[idx].col-1))){
                                pushbackidx = i;
                                Target0[pushbackidx].status = 3;
                                Target3.push_back(Target0[pushbackidx]);
                                Target0.erase(Target0.begin()+pushbackidx);
                            }
                        }
                    }
                    if((Target3[idx].row+1)<M){ //下
                        for(i=0;i<Target0.size();i++){
                            if((Target0[i].row==(Target3[idx].row+1)) && (Target0[i].col==Target3[idx].col)){
                                pushbackidx = i;
                                Target0[pushbackidx].status = 3;
                                Target3.push_back(Target0[pushbackidx]);
                                Target0.erase(Target0.begin()+pushbackidx);
                            }
                        }
                    }
                    if((Target3[idx].row-1)>=0){ //上
                        for(i=0;i<Target0.size();i++){
                            if((Target0[i].row==(Target3[idx].row-1)) && (Target0[i].col==Target3[idx].col)){
                                pushbackidx = i;
                                Target0[pushbackidx].status = 3;
                                Target3.push_back(Target0[pushbackidx]);
                                Target0.erase(Target0.begin()+pushbackidx);
                            }
                        }
                    }

                    //----結束--處理上下左右(Target3)-----

                    //----開始--處理斜對角-----
                    for(i=0;i<Target3.size();i++){ //左上
                        if(Target3[i].id == wantid){
                            wantidx = i;
                        }
                    }
                    if(((Target3[wantidx].row-1)>=0) && ((Target3[wantidx].col-1)>=0)){
                        for(i=0;i<Target0.size();i++){
                            if((Target0[i].row==(Target3[wantidx].row-1)) && (Target0[i].col==(Target3[wantidx].col-1))){
                                eraseidx = i;
                                Target0.erase(Target0.begin()+eraseidx);
                            }
                        }
                    }
                    for(i=0;i<Target3.size();i++){ //右上
                        if(Target3[i].id == wantid){
                            wantidx = i;
                        }
                    }
                    if(((Target3[wantidx].row-1)>=0) && ((Target3[wantidx].col+1)<N)){
                        for(i=0;i<Target0.size();i++){
                            if((Target0[i].row==(Target3[wantidx].row-1)) && (Target0[i].col==(Target3[wantidx].col+1))){
                                eraseidx = i;
                                Target0.erase(Target0.begin()+eraseidx);
                            }
                        }
                    }
                    for(i=0;i<Target3.size();i++){ //左下
                        if(Target3[i].id == wantid){
                            wantidx = i;
                        }
                    }
                    if(((Target3[wantidx].row+1)<M) && ((Target3[wantidx].col-1)>=0)){
                        for(i=0;i<Target0.size();i++){
                            if((Target0[i].row==(Target3[wantidx].row+1)) && (Target0[i].col==(Target3[wantidx].col-1))){
                                eraseidx = i;
                                Target0.erase(Target0.begin()+eraseidx);
                            }
                        }
                    }
                    for(i=0;i<Target3.size();i++){ //右下
                        if(Target0[i].id == wantid){
                            wantidx = i;
                        }
                    }
                    if(((Target3[wantidx].row+1)<M) && ((Target3[wantidx].col+1)<N)){
                        for(i=0;i<Target0.size();i++){
                            if((Target0[i].row==(Target3[wantidx].row+1)) && (Target0[i].col==(Target3[wantidx].col+1))){
                                eraseidx = i;
                                Target0.erase(Target0.begin()+eraseidx);
                            }
                        }
                    }
                    //----結束--處理斜對角-----
                }
                else{
                    BMap[Target3[idx].row][Target3[idx].col]=-1;
                    Target3[idx].status = -1;
                    Target3[idx].isB = 0;
                    cout<<"Trial "<<n_shots<<": zone "<<Target3[idx].id;
                    cout<<"=("<<Target3[idx].row<<","<<Target3[idx].col<<") ";
                    cout<<"missed"<<endl;
                }
                for(i=0;i<Target3.size();i++){
                    if(Target3[i].id == wantid){
                        wantidx = i;
                    }
                }
                Target3.erase(Target3.begin()+wantidx);//將選過的點從Target3中去除，以免重複選到
            }

        }
        //----結束---印出每一步--------

        hr = 100.*n_hits/n_shots;
        hrlb = 100.*n_targets/(M*N);
        hrimprate = 100.*(hr-hrlb)/(100.-hrlb);
        printf("Hitrate = %d/%d = %.2f%%;\n",n_targets,n_shots,hr);
        printf("Hitrate lower bound = %d/%d = %.2f%%\n",n_targets,M*N,hrlb);
        printf("Hitrate Improvement Rate = 100.*(%.2f-%.2f)/(100.-%.2f) = %.2f%%",hr,hrlb,hrlb,hrimprate);
    }
    else{
        while(n_hits<n_targets){
            n_shots++;
            if(Target3.size()==0){
                idx = rand()%Target0.size();
                wantid = Target0[idx].id;
                if(SMap[Target0[idx].row][Target0[idx].col]==1){
                    n_hits++;
                    BMap[Target0[idx].row][Target0[idx].col]=2;
                    Target0[idx].status = 2;
                    Target0[idx].isB = 1;
                    //----開始--處理上下左右(Target3)-----
                    if((Target0[idx].col+1)<N){ //右
                        for(i=0;i<Target0.size();i++){
                            if((Target0[i].row==Target0[idx].row) && (Target0[i].col==(Target0[idx].col+1))){
                                pushbackidx = i;
                                Target0[pushbackidx].status = 3;
                                Target3.push_back(Target0[pushbackidx]);
                                Target0.erase(Target0.begin()+pushbackidx);
                            }
                        }
                    }
                    if((Target0[idx].col-1)>=0){ //左
                        for(i=0;i<Target0.size();i++){
                            if((Target0[i].row==Target0[idx].row) && (Target0[i].col==(Target0[idx].col-1))){
                                pushbackidx = i;
                                Target0[pushbackidx].status = 3;
                                Target3.push_back(Target0[pushbackidx]);
                                Target0.erase(Target0.begin()+pushbackidx);
                            }
                        }
                    }
                    if((Target0[idx].row+1)<M){ //下
                        for(i=0;i<Target0.size();i++){
                            if((Target0[i].row==(Target0[idx].row+1)) && (Target0[i].col==Target0[idx].col)){
                                pushbackidx = i;
                                Target0[pushbackidx].status = 3;
                                Target3.push_back(Target0[pushbackidx]);
                                Target0.erase(Target0.begin()+pushbackidx);
                            }
                        }
                    }
                    if((Target0[idx].row-1)>=0){ //上
                        for(i=0;i<Target0.size();i++){
                            if((Target0[i].row==(Target0[idx].row-1)) && (Target0[i].col==Target0[idx].col)){
                                pushbackidx = i;
                                Target0[pushbackidx].status = 3;
                                Target3.push_back(Target0[pushbackidx]);
                                Target0.erase(Target0.begin()+pushbackidx);
                            }
                        }
                    }

                    //----結束--處理上下左右(Target3)-----

                    //----開始--處理斜對角-----
                    for(i=0;i<Target0.size();i++){ //左上
                        if(Target0[i].id == wantid){
                            wantidx = i;
                        }
                    }
                    if(((Target0[wantidx].row-1)>=0) && ((Target0[wantidx].col-1)>=0)){
                        for(i=0;i<Target0.size();i++){
                            if((Target0[i].row==(Target0[wantidx].row-1)) && (Target0[i].col==(Target0[wantidx].col-1))){
                                eraseidx = i;
                                Target0.erase(Target0.begin()+eraseidx);
                            }
                        }
                    }
                    for(i=0;i<Target0.size();i++){ //右上
                        if(Target0[i].id == wantid){
                            wantidx = i;
                        }
                    }
                    if(((Target0[wantidx].row-1)>=0) && ((Target0[wantidx].col+1)<N)){
                        for(i=0;i<Target0.size();i++){
                            if((Target0[i].row==(Target0[wantidx].row-1)) && (Target0[i].col==(Target0[wantidx].col+1))){
                                eraseidx = i;
                                Target0.erase(Target0.begin()+eraseidx);
                            }
                        }
                    }
                    for(i=0;i<Target0.size();i++){ //左下
                        if(Target0[i].id == wantid){
                            wantidx = i;
                        }
                    }
                    if(((Target0[wantidx].row+1)<M) && ((Target0[wantidx].col-1)>=0)){
                        for(i=0;i<Target0.size();i++){
                            if((Target0[i].row==(Target0[wantidx].row+1)) && (Target0[i].col==(Target0[wantidx].col-1))){
                                eraseidx = i;
                                Target0.erase(Target0.begin()+eraseidx);
                            }
                        }
                    }
                    for(i=0;i<Target0.size();i++){ //右下
                        if(Target0[i].id == wantid){
                            wantidx = i;
                        }
                    }
                    if(((Target0[wantidx].row+1)<M) && ((Target0[wantidx].col+1)<N)){
                        for(i=0;i<Target0.size();i++){
                            if((Target0[i].row==(Target0[wantidx].row+1)) && (Target0[i].col==(Target0[wantidx].col+1))){
                                eraseidx = i;
                                Target0.erase(Target0.begin()+eraseidx);
                            }
                        }
                    }
                    //----結束--處理斜對角-----
                }
                else{
                    BMap[Target0[idx].row][Target0[idx].col]=-1;
                    Target0[idx].status = -1;
                    Target0[idx].isB = 0;
                }
                for(i=0;i<Target0.size();i++){
                    if(Target0[i].id == wantid){
                        wantidx = i;
                    }
                }
                Target0.erase(Target0.begin()+wantidx);//將選過的點從Target0中去除，以免重複選到
            }
            else{
                idx = rand()%Target3.size();
                wantid = Target3[idx].id;
                if(SMap[Target3[idx].row][Target3[idx].col]==1){
                    n_hits++;
                    BMap[Target3[idx].row][Target3[idx].col]=2;
                    Target3[idx].status = 2;
                    Target3[idx].isB = 1;
                    //----開始--處理上下左右(Target3)-----
                    if((Target3[idx].col+1)<N){ //右
                        for(i=0;i<Target0.size();i++){
                            if((Target0[i].row==Target3[idx].row) && (Target0[i].col==(Target3[idx].col+1))){
                                pushbackidx = i;
                                Target0[pushbackidx].status = 3;
                                Target3.push_back(Target0[pushbackidx]);
                                Target0.erase(Target0.begin()+pushbackidx);
                            }
                        }
                    }
                    if((Target3[idx].col-1)>=0){ //左
                        for(i=0;i<Target0.size();i++){
                            if((Target0[i].row==Target3[idx].row) && (Target0[i].col==(Target3[idx].col-1))){
                                pushbackidx = i;
                                Target0[pushbackidx].status = 3;
                                Target3.push_back(Target0[pushbackidx]);
                                Target0.erase(Target0.begin()+pushbackidx);
                            }
                        }
                    }
                    if((Target3[idx].row+1)<M){ //下
                        for(i=0;i<Target0.size();i++){
                            if((Target0[i].row==(Target3[idx].row+1)) && (Target0[i].col==Target3[idx].col)){
                                pushbackidx = i;
                                Target0[pushbackidx].status = 3;
                                Target3.push_back(Target0[pushbackidx]);
                                Target0.erase(Target0.begin()+pushbackidx);
                            }
                        }
                    }
                    if((Target3[idx].row-1)>=0){ //上
                        for(i=0;i<Target0.size();i++){
                            if((Target0[i].row==(Target3[idx].row-1)) && (Target0[i].col==Target3[idx].col)){
                                pushbackidx = i;
                                Target0[pushbackidx].status = 3;
                                Target3.push_back(Target0[pushbackidx]);
                                Target0.erase(Target0.begin()+pushbackidx);
                            }
                        }
                    }

                    //----結束--處理上下左右(Target3)-----

                    //----開始--處理斜對角-----
                    for(i=0;i<Target3.size();i++){ //左上
                        if(Target3[i].id == wantid){
                            wantidx = i;
                        }
                    }
                    if(((Target3[wantidx].row-1)>=0) && ((Target3[wantidx].col-1)>=0)){
                        for(i=0;i<Target0.size();i++){
                            if((Target0[i].row==(Target3[wantidx].row-1)) && (Target0[i].col==(Target3[wantidx].col-1))){
                                eraseidx = i;
                                Target0.erase(Target0.begin()+eraseidx);
                            }
                        }
                    }
                    for(i=0;i<Target3.size();i++){ //右上
                        if(Target3[i].id == wantid){
                            wantidx = i;
                        }
                    }
                    if(((Target3[wantidx].row-1)>=0) && ((Target3[wantidx].col+1)<N)){
                        for(i=0;i<Target0.size();i++){
                            if((Target0[i].row==(Target3[wantidx].row-1)) && (Target0[i].col==(Target3[wantidx].col+1))){
                                eraseidx = i;
                                Target0.erase(Target0.begin()+eraseidx);
                            }
                        }
                    }
                    for(i=0;i<Target3.size();i++){ //左下
                        if(Target3[i].id == wantid){
                            wantidx = i;
                        }
                    }
                    if(((Target3[wantidx].row+1)<M) && ((Target3[wantidx].col-1)>=0)){
                        for(i=0;i<Target0.size();i++){
                            if((Target0[i].row==(Target3[wantidx].row+1)) && (Target0[i].col==(Target3[wantidx].col-1))){
                                eraseidx = i;
                                Target0.erase(Target0.begin()+eraseidx);
                            }
                        }
                    }
                    for(i=0;i<Target3.size();i++){ //右下
                        if(Target0[i].id == wantid){
                            wantidx = i;
                        }
                    }
                    if(((Target3[wantidx].row+1)<M) && ((Target3[wantidx].col+1)<N)){
                        for(i=0;i<Target0.size();i++){
                            if((Target0[i].row==(Target3[wantidx].row+1)) && (Target0[i].col==(Target3[wantidx].col+1))){
                                eraseidx = i;
                                Target0.erase(Target0.begin()+eraseidx);
                            }
                        }
                    }
                    //----結束--處理斜對角-----
                }
                else{
                    BMap[Target3[idx].row][Target3[idx].col]=-1;
                    Target3[idx].status = -1;
                    Target3[idx].isB = 0;
                }
                for(i=0;i<Target3.size();i++){
                    if(Target3[i].id == wantid){
                        wantidx = i;
                    }
                }
                Target3.erase(Target3.begin()+wantidx);//將選過的點從Target3中去除，以免重複選到
            }

        }
        hr = 100.*n_hits/n_shots;
        hrlb = 100.*n_targets/(M*N);
        hrimprate = 100.*(hr-hrlb)/(100.-hrlb);

        cout<<"H24031354 "<<dataname<<" IS "<<M*N<<" "<<n_targets<<" "<<n_shots<<" ";
        printf("%.2f %.2f %.2f",hr,hrlb,hrimprate);
        cout<<endl;
    }
    return 0;
}




