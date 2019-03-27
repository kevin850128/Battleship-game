/*
   This code can be compiled and run ok.
     利用隨機選取法完海戰船遊戲

   usage (how to run):
     hw41

   input file:
     data_4_6_1.txt

   output file:
     none

   compile (how to compile):
     g++ -o hw41 hw41.cpp

   pseudocode:
     發射砲彈到隨機選取的點，直到目標點數變成0
     關鍵是利用vector將選過的點從vector中拿掉，這樣就不會選取到重複的點

   coded by 江冠駒, ID: H24031354, email: kevin040208@gmail.com
   date: 2018.05.04
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
        int status; //儲存其狀態，status=-1,0,1,2,3,4  (或可再自行定義)
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
        **BMap,         //記錄作戰圖，BMap[i][j]=-1,0,1,2,3  or  4
        i,j,            //for loop用
        idx;            //隨機選取的index
    bool **SMap,        //記錄解答圖，若座標(i,j)為船身，則SMap[i][j]=1，否則為0
         isDebug;       //詳細模式(isDebug=1,PART1&2用)，列出每步驟過程
                        //扼要模式(isDebug=0,PART3用)，僅列出最後統計資料
    double  hr,         //命中率hitrate百分比=100.*n_hits/n_shots以%為單位
            hrlb,       //命中率百分比下限=100.*n_targets/(M*N)以%為單位
            hrimprate;  //命中改善率=100.*(hr-hrlb)/(100.-hrlb)以%為單位
                        //代表在可改善的空間(100.-hrlb)中，hr的改善(hr-hrlb)比率
    Zone temp;  //暫存某網格之狀態當作將一個struct放進vector的媒介
    vector<Zone> Target0;   //儲存狀態為0的網格，初始時全部網格皆存於此
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
            idx = rand()%Target0.size();
            n_shots++;
            if(SMap[Target0[idx].row][Target0[idx].col]==1){
                n_hits++;
                BMap[Target0[idx].row][Target0[idx].col]=2;
                cout<<"Trial "<<n_shots<<": zone "<<Target0[idx].id;
                cout<<"=("<<Target0[idx].row<<","<<Target0[idx].col<<") ";
                if(n_hits==n_targets){
                    cout<<"hit "<<n_hits<<", "<<(n_targets-n_hits)<<" to go!! Done!!"<<endl;
                }
                else{
                    cout<<"hit "<<n_hits<<", "<<(n_targets-n_hits)<<" to go!!"<<endl;
                }
            }
            else{
                BMap[Target0[idx].row][Target0[idx].col]=-1;
                cout<<"Trial "<<n_shots<<": zone "<<Target0[idx].id;
                cout<<"=("<<Target0[idx].row<<","<<Target0[idx].col<<") ";
                cout<<"missed"<<endl;
            }
            Target0.erase(Target0.begin()+idx);//將選過的點從Target0中去除，以免重複選到
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
            idx = rand()%Target0.size();
            n_shots++;
            if(SMap[Target0[idx].row][Target0[idx].col]==1){
                n_hits++;
                BMap[Target0[idx].row][Target0[idx].col]=2;
            }
            else{
                BMap[Target0[idx].row][Target0[idx].col]=-1;
            }
            Target0.erase(Target0.begin()+idx);
        }
        hr = 100.*n_hits/n_shots;
        hrlb = 100.*n_targets/(M*N);
        hrimprate = 100.*(hr-hrlb)/(100.-hrlb);

        cout<<"H24031354 "<<dataname<<" RS "<<M*N<<" "<<n_targets<<" "<<n_shots<<" ";
        printf("%.2f %.2f %.2f",hr,hrlb,hrimprate);
        cout<<endl;
    }
    return 0;
}




