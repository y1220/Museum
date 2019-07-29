#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define N 101
typedef struct museum_m{
    char name_m[N];
    int n_time;
    float *time_slot_s;
    float *time_slot_f;
    int *max_people;
}museum;
typedef struct museum_m_res{
    char *name_m_res;
    int day_res;
    int *max_people_res;
    struct museum_m_res *next_mus;
    struct museum_m_res *next_day;
}museum_res;
museum_res *head_m=NULL;
typedef struct reserve_r{
    char *name;
    char *museum;
    int day;
    float time_slot_s;
    float time_slot_f;
    int n_people;
    struct reserve_r *next_nord;
    struct reserve_r *next_same_p;
}reserve;
reserve *head=NULL;
///prototype///
museum *read_museum(int ,char *);
museum_res *new_nord_m(museum *,museum_res *,int );
museum_res *copy_val(museum *,museum_res *,int );
int transfer_number_time_slot(museum *, int , float , float );
int check_reservation(char *, int , float , float , int , museum *,int , museum_res *, int *);
museum_res *reservation_by_museum(museum_res *,char *, int , int , museum *, int ,int );
reserve *reservation_by_person(reserve *, char *, int , float , float , int , char *);
reserve *connect_name_list(reserve *, char *);
int check_visitor(reserve *, char *);
void print_result();
int main(int argc, char *argv[]){
    FILE *fp;
    char tmp[N], tmp_1[N], tmp_2[N];
    int cnt_t, cnt_m=0, i;
    char filename[N];
    if(argc==1){
        printf("Command line error!\n");
        exit(EXIT_FAILURE);
    }
    strcpy(filename,argv[1]);
    fp=fopen(filename,"r");
    if(fp==NULL){
       printf("ERROR!No file!\n");
       exit(EXIT_FAILURE);
    }///1.count the number of museum
    while(fscanf(fp,"%s %d",tmp,&cnt_t)!=EOF){
        for(i=0;i<cnt_t;i++){
                fscanf(fp,"%s %s",tmp_1,tmp_2);
        }
        cnt_m++;
    }
    fclose(fp);
    ///2.memory allocation
    museum *muse;
    muse=(museum*)malloc(sizeof(museum));
    muse=read_museum(cnt_m,filename);
    head_m=copy_val(muse,head_m,cnt_m);
    ///3.read the reservation(from screen)
    char name[N], m_name[N];
    float s_t, f_t;      //position of time slot, museum
    int day, n_peo, req, n_ti, n_mu, flag=1, num=1;
    printf("1. ADD NEW RESERVATION\t\t2. FINISH ADD AND GO TO NEXT STEP");
    printf("\n\nInsert the request.(1 or 2)\n  request: ");
    scanf("%d",&req);
    while(flag){
    if(req==1){
                printf("\n\nNO.%d: Museum name\tDay\tTime_slot\tNumber of people\tName\n",num);
                scanf("%s %d %f %f %d %s",m_name, &day, &s_t, &f_t, &n_peo, name);
                ///4.check the possibility of reservation
                n_ti=check_reservation(m_name, day, s_t, f_t, cnt_m, muse, n_peo, head_m, &n_mu);
        if(n_ti>=0){///5.make a reservation
            num++;
            head_m=reservation_by_museum(head_m, m_name, day, n_peo, muse, n_ti, n_mu);
            head=reservation_by_person(head, m_name, day, s_t, f_t, n_peo, name);
            printf("\n\nInsert the request.(1 or 2)\n  request: ");
            scanf("%d",&req);
        }else{
            printf("\nERROR! Your reservation is not acceptable\n");
            exit(EXIT_FAILURE);
        }
    }else if(req==2){
        printf("Finish adding.\n");
        flag=0;
    }else{
        printf("ERROR! Request number has to be 1 or 2!\n");
        exit(EXIT_FAILURE);
    }
    }///6.read the visitor's name and print the result
    char req_visitor[N];
    printf("\n\nInsert the visitor's name you are looking for\n\n");
    printf("Visitor's name: ");
    scanf("%s",req_visitor);
    if(check_visitor(head,req_visitor)==0){
        print_result(head,req_visitor);
    }else{
        printf("\nERROR!No reservation of the visitor's name you inserted!\n");
    }///7.free
    for(i=0;i<cnt_m;i++){
         free(muse[i].max_people);
         free(muse[i].time_slot_s);
         free(muse[i].time_slot_f);
    }
    free(muse);
    free(head->name);
    free(head->museum);
    free(head);
    free(head_m->max_people_res);
    free(head_m->name_m_res);
    free(head_m);
    return 0;
}
museum *read_museum(int cnt_m, char *filename){
    museum *tmp;
    int i, j, n_time;
    FILE *fp;
    fp=fopen(filename,"r");
    tmp=(museum*)malloc(cnt_m*(sizeof(museum)));
    if(tmp==NULL){
        printf("Memory allocation ERROR!\n");
        exit(EXIT_FAILURE);
    }
    for(i=0;i<cnt_m;i++){
        fscanf(fp,"%s %d",tmp[i].name_m,&n_time);
        tmp[i].n_time=n_time;
        tmp[i].time_slot_s=malloc(n_time*sizeof(float));
        tmp[i].time_slot_f=malloc(n_time*sizeof(float));
        tmp[i].max_people=malloc(n_time*sizeof(int));
        if((tmp[i].time_slot_s==NULL)||(tmp[i].time_slot_f==NULL)||(tmp[i].max_people==NULL)){
            printf("Memory allocation ERROR!\n");
            exit(EXIT_FAILURE);
        }
        for(j=0;j<n_time;j++){
            fscanf(fp,"%f-%f %d\n", &tmp[i].time_slot_s[j], &tmp[i].time_slot_f[j], &tmp[i].max_people[j]);
        }
    }
    fclose(fp);
    return tmp;
}
museum_res *new_nord_m(museum *muse, museum_res *tmp, int i){
    int j;
    tmp=malloc(sizeof(museum_res));
    tmp->name_m_res=strdup(muse[i].name_m);
    if((tmp==NULL)||(tmp->name_m_res==NULL)){
        printf("Memory allocation ERROR!\n");
        exit(EXIT_FAILURE);
    }
    tmp->day_res=-1;
    tmp->max_people_res=malloc(muse[i].n_time*sizeof(int));
    if(tmp->max_people_res==NULL){
        printf("Memory allocation ERROR!\n");
        exit(EXIT_FAILURE);
    }
    for(j=0;j<muse[i].n_time;j++){
        tmp->max_people_res[j]=muse[i].max_people[j];
    }
    return tmp;
}
museum_res *copy_val(museum *muse,museum_res *head_m,int cnt_m){
    int i;
    museum_res *tmp, *tmp2;
    for(i=cnt_m-1;i>-1;i--){
                if(i!=cnt_m-1){
                    tmp2=new_nord_m(muse,tmp2,i);
                    tmp2->next_day=NULL;
                    tmp2->next_mus=head_m;
                    head_m=tmp2;
                }if(i==cnt_m-1){
                    tmp=new_nord_m(muse,tmp,i);
                    tmp->next_mus=NULL;
                    tmp->next_day=NULL;
                    head_m=tmp;
                }
    }
    return head_m;
}
int transfer_number_time_slot(museum *muse, int cnt, float s_t, float f_t){
    int i;
    for(i=0;i<muse[cnt].n_time;i++){
        if((muse[cnt].time_slot_s[i]==s_t)&&(muse[cnt].time_slot_f[i]==f_t)){
            return i;
        }
    }///error! time slot DOES NOT exist
    return -1;
}
int check_reservation(char *m_name, int day, float s_t, float f_t, int cnt_m, museum *muse, int n_peo, museum_res *head_m, int *n_mu){
    int cnt=0,i;
    if((day<0)||(day>364)){
        printf("ERROR!(DAY must be 0 to 364)\n");
        return -1;
    }
    while((strcmp(muse[cnt].name_m,m_name)!=0)&&(cnt!=cnt_m-1)){
            cnt++;
    }if((strcmp(muse[cnt].name_m,m_name)!=0)&&(cnt==cnt_m-1)){
        printf("ERROR!(MUSEUM_NAME)\n");
        printf("MUSEUM_NAME:%s\n",m_name);
        return -1;
    }
    *n_mu=cnt;
    i=transfer_number_time_slot(muse, cnt, s_t, f_t);
    if(i>=0){
        if(muse[cnt].max_people[i]<n_peo){
            printf("ERROR!(TOO MUCH PEOPLE)\n");
            return -1;
        }
    }else{
        printf("ERROR!(TIME_SLOT)\n");
        return -1;
    }
    return i;
}
museum_res *reservation_by_museum(museum_res *head_m,char *m_name, int day, int n_peo, museum *muse, int n_ti, int n_mu){
    museum_res *tmp=head_m;
    head_m=tmp;
    int i=0, x;
    while(strcmp(tmp->name_m_res,m_name)!=0){
        i++;
        tmp=tmp->next_mus;
    }if(strcmp(tmp->name_m_res,m_name)==0){
        if(tmp->day_res==-1){///INITIAL
            x=tmp->max_people_res[n_ti]-n_peo;
            if(x<0){
                printf("ERROR!(TOO MUCH PEOPLE)\n");
                exit(EXIT_FAILURE);
            }
            tmp->day_res=day;
            tmp->max_people_res[n_ti]=x;
            return head_m;
        }///UPDATE
        if(tmp->day_res==day){
            x=tmp->max_people_res[n_ti]-n_peo;
            if(x<0){
                printf("ERROR!(TOO MUCH PEOPLE)\n");
                exit(EXIT_FAILURE);
            }
            tmp->max_people_res[n_ti]=x;
            return head_m;
        }
        while(tmp->next_day!=NULL){
                tmp=tmp->next_day;
            if(tmp->day_res==day){///UPDATE THE LIMIT
                x=tmp->max_people_res[n_ti]-n_peo;
                if(x<0){
                printf("ERROR!(TOO MUCH PEOPLE)\n");
                exit(EXIT_FAILURE);
                }
                tmp->max_people_res[n_ti]=x;
                return head_m;
            }
        }///NEW NODE(NEW DAY)
        museum_res *tmp2=NULL;
        tmp2=new_nord_m(muse,tmp2,i);
        tmp2->day_res=day;
        tmp2->next_day=NULL;
        tmp2->next_mus=NULL;
        tmp->next_day=tmp2;
        tmp2->max_people_res[n_ti]=tmp2->max_people_res[n_ti]-n_peo;
        return head_m;
    }
    return head_m;
}
reserve *reservation_by_person(reserve *head, char *m_name, int day, float s_t, float f_t, int n_peo, char *name){
    reserve *tmp;///NEW NORD on the bottom
    tmp=malloc(sizeof(reserve));
    if(tmp==NULL)
    {
        printf("Memory allocation ERROR!\n");
        exit(EXIT_FAILURE);
    }
    tmp->name=strdup(name);
    tmp->museum=strdup(m_name);
    if((tmp->name==NULL)||(tmp->museum==NULL))
    {
        printf("Memory allocation ERROR!\n");
        exit(EXIT_FAILURE);
    }
    tmp->day=day;
    tmp->time_slot_s=s_t;
    tmp->time_slot_f=f_t;
    tmp->n_people=n_peo;
    tmp->next_nord=NULL;
    tmp->next_same_p=NULL;
    if(head==NULL){///INITIAL
       return tmp;
    }
    reserve *tmp2=head;///ADD
    tmp->next_nord=tmp2;
    head=tmp;
    head=connect_name_list(head,name);
    return head;
}
reserve *connect_name_list(reserve *head,char *name){
    reserve *tmp=head;
    reserve *tmp2=head;
    while(tmp2->next_nord!=NULL){
        tmp2=tmp2->next_nord;
        if(strcmp(tmp2->name,name)==0){
                tmp->next_same_p=tmp2;
                head=tmp;
                return head;
        }
    }///LAST ELEMENT
    if(strcmp(tmp2->name,name)==0){
                tmp->next_same_p=tmp2;
                head=tmp;
                return head;
    }///NO SAME NAME
    return head;
}
int check_visitor(reserve *head, char *name){
    reserve *tmp=head;
    while(tmp->next_nord!=NULL){
        if(strcmp(tmp->name,name)==0){///FOUND
            return 0;
        }
        tmp=tmp->next_nord;
    }if(strcmp(tmp->name,name)==0){///FOUND(Last element)
            return 0;
    }///NOT FOUND
    return 1;
}
void print_result(reserve *head, char *name){
    reserve *tmp=head;
    while(tmp->name!=NULL){
        if(strcmp(tmp->name,name)==0){
                printf("%s, %d, %.2f-%.2f, %d\n",tmp->museum, tmp->day, tmp->time_slot_s, tmp->time_slot_f, tmp->n_people);
            while(tmp->next_same_p!=NULL){///Search next data by using NAME LIST
                tmp=tmp->next_same_p;
                printf("%s, %d, %.2f-%.2f, %d\n",tmp->museum, tmp->day, tmp->time_slot_s, tmp->time_slot_f, tmp->n_people);
            }
        }
        tmp=tmp->next_nord;
    }
}
