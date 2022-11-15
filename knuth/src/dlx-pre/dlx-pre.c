#define o mems++
#define oo mems+= 2
#define ooo mems+= 3
#define O "%"
#define mod %					\

#define max_item_name 50
#define max_item_name_str "50"

#define max_level 500
#define max_itms 100000
#define max_nodes 10000000
#define bufsize (9*max_itms+3)			\

#define show_basics 1
#define show_choices 2
#define show_details 4
#define show_orig_nos 8
#define show_tots 512
#define show_warnings 1024			\

#define len itm
#define aux color				\

#define root 0					\

#define sanity_checking 0			\

#define panic(m) {fprintf(stderr,""O"s!\n"O"d: "O".99s\n",m,p,buf) ;exit(-666) ;} \

/*4:*/
#line 160 "dlx-pre.w"

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <ctype.h> 
#include "gb_flip.h"
typedef unsigned int uint;
typedef unsigned long long ullng;
/*7:*/
#line 301 "dlx-pre.w"

typedef struct node_struct{
  int up,down;
  int itm;
  int color;
}node;

/*:7*//*8:*/
#line 315 "dlx-pre.w"

typedef struct itm_struct{
  char name[max_item_name];
  int prev,next;
}item;

/*:8*/
#line 168 "dlx-pre.w"
;
/*5:*/
#line 220 "dlx-pre.w"

int vbose= show_basics;
char buf[bufsize];
ullng options;
ullng imems,mems;
ullng thresh= 1000000000;
ullng delta= 10000000000;
ullng timeout= 0x1fffffffffffffff;
int rounds= max_nodes;
int options_out,itms_out;

/*:5*//*9:*/
#line 321 "dlx-pre.w"

node*nd;
int last_node;
item cl[max_itms+2];
int second= max_itms;
int last_itm;

/*:9*//*30:*/
#line 684 "dlx-pre.w"

int rnd;
int stack;
int change;

/*:30*/
#line 169 "dlx-pre.w"
;
/*11:*/
#line 342 "dlx-pre.w"

void print_option(int p,FILE*stream){
  register int k,q;
  if(p<last_itm||p>=last_node||nd[p].itm<=0){
    fprintf(stderr,"Illegal option "O"d!\n",p);
    return;
  }
  for(q= p;;){
    fprintf(stream," "O"."max_item_name_str"s",cl[nd[q].itm].name);
    if(nd[q].color)
      fprintf(stream,":"O"c",nd[q].color> 0?nd[q].color:nd[nd[q].itm].color);
    q++;
    while(nd[q].itm<=0)q= nd[q].up;
    if(q==p)break;
  }
  for(q= nd[nd[p].itm].down,k= 1;q!=p;k++){
    if(q==nd[p].itm){
      fprintf(stream," (?)\n");return;
    }else q= nd[q].down;
  }
  fprintf(stream," ("O"d of "O"d)\n",k,nd[nd[p].itm].len);
}

void prow(int p){
  print_option(p,stderr);
}

/*:11*//*12:*/
#line 375 "dlx-pre.w"

int dpoption(int p,FILE*stream){
  register int q,c;
  for(p--;nd[p].itm> 0||nd[p].down<p;p--);
  for(q= p+1;;q++){
    c= nd[q].itm;
    if(c<0)return-c;
    if(c> 0){
      fprintf(stream," "O"."max_item_name_str"s",cl[c].name);
      if(nd[q].color)
	fprintf(stream,":"O"c",nd[q].color);
    }
  }
}

/*:12*//*13:*/
#line 392 "dlx-pre.w"

void print_itm(int c){
  register int p;
  if(c<root||c>=last_itm){
    fprintf(stderr,"Illegal item "O"d!\n",c);
    return;
  }
  if(c<second)
    fprintf(stderr,"Item "O"."max_item_name_str"s, length "O"d, neighbors "O"."max_item_name_str"s and "O"."max_item_name_str"s:\n",
	    cl[c].name,nd[c].len,cl[cl[c].prev].name,cl[cl[c].next].name);
  else fprintf(stderr,"Item "O"."max_item_name_str"s, length "O"d:\n",cl[c].name,nd[c].len);
  for(p= nd[c].down;p>=last_itm;p= nd[p].down)prow(p);
}

/*:13*//*14:*/
#line 411 "dlx-pre.w"

void sanity(void){
  register int k,p,q,pp,qq,t;
  for(q= root,p= cl[q].next;;q= p,p= cl[p].next){
    if(cl[p].prev!=q)fprintf(stderr,"Bad prev field at itm "O"."max_item_name_str"s!\n",
			     cl[p].name);
    if(p==root)break;
    /*15:*/
#line 422 "dlx-pre.w"

    for(qq= p,pp= nd[qq].down,k= 0;;qq= pp,pp= nd[pp].down,k++){
      if(nd[pp].up!=qq)fprintf(stderr,"Bad up field at node "O"d!\n",pp);
      if(pp==p)break;
      if(nd[pp].itm!=p)fprintf(stderr,"Bad itm field at node "O"d!\n",pp);
    }
    if(nd[p].len!=k)fprintf(stderr,"Bad len field in item "O"."max_item_name_str"s!\n",
			    cl[p].name);

    /*:15*/
#line 418 "dlx-pre.w"
    ;
  }
}

/*:14*//*27:*/
#line 632 "dlx-pre.w"

void hide(int c){
  register int cc,l,r,rr,nn,uu,dd,t,k= 0;
  for(o,rr= nd[c].down;rr>=last_itm;o,rr= nd[rr].down)if(o,!nd[rr].color){
      for(nn= rr+1;nn!=rr;){
	o,uu= nd[nn].up,dd= nd[nn].down;
	o,cc= nd[nn].itm;
	if(cc<=0){
	  nn= uu;
	  continue;
	}
	oo,nd[uu].down= dd,nd[dd].up= uu;
	o,t= nd[cc].len-1;
	o,nd[cc].len= t;
	if(t==0&&cc<second)stack= cc;
	nn++;
      }
    }
}

/*:27*//*28:*/
#line 652 "dlx-pre.w"

void unhide(int c){
  register int cc,l,r,rr,nn,uu,dd,t;
  for(o,rr= nd[c].down;rr>=last_itm;o,rr= nd[rr].down)if(o,!nd[rr].color){
      for(nn= rr+1;nn!=rr;){
	o,uu= nd[nn].up,dd= nd[nn].down;
	o,cc= nd[nn].itm;
	if(cc<=0){
	  nn= uu;
	  continue;
	}
	o,t= nd[cc].len;
	oo,nd[uu].down= nd[dd].up= nn;
	o,nd[cc].len= t+1;
	nn++;
      }
    }
}

/*:28*/
#line 170 "dlx-pre.w"
;
main(int argc,char*argv[]){
  register int c,cc,dd,i,j,k,p,pp,q,qq,r,rr,rrr,t,uu,x,cur_node,best_itm;
  /*6:*/
#line 234 "dlx-pre.w"

  for(j= argc-1,k= 0;j;j--)switch(argv[j][0]){
    case'v':k|= (sscanf(argv[j]+1,""O"d",&vbose)-1);break;
    case'd':k|= (sscanf(argv[j]+1,""O"lld",&delta)-1),thresh= delta;break;
    case't':k|= (sscanf(argv[j]+1,""O"d",&rounds)-1);break;
    case'T':k|= (sscanf(argv[j]+1,""O"lld",&timeout)-1);break;
    default:k= 1;
    }
  if(k){
    fprintf(stderr,
	    "Usage: "O"s [v<n>] [d<n>] [t<n>] [T<n>] < foo.dlx > bar.dlx\n",argv[0]);
    exit(-1);
  }

  /*:6*/
#line 173 "dlx-pre.w"
  ;
/*16:*/
#line 436 "dlx-pre.w"

 nd= (node*)calloc(max_nodes,sizeof(node));
 if(!nd){
   fprintf(stderr,"I couldn't allocate space for "O"d nodes!\n",max_nodes);
   exit(-666);
 }
 if(max_nodes<=2*max_itms){
   fprintf(stderr,"Recompile me: max_nodes must exceed twice max_itms!\n");
   exit(-999);
 }
 while(1){
   if(!fgets(buf,bufsize,stdin))break;
   if(o,buf[p= strlen(buf)-1]!='\n')panic("Input line way too long");
   for(p= 0;o,isspace(buf[p]);p++);
   if(buf[p]=='|'||!buf[p])continue;
   last_itm= 1;
   break;
 }
 if(!last_itm)panic("No items");
 for(;o,buf[p];){
   for(j= 0;j<max_item_name&&(o,!isspace(buf[p+j]));j++){
     if(buf[p+j]==':'||buf[p+j]=='|')
       panic("Illegal character in item name");
     o,cl[last_itm].name[j]= buf[p+j];
   }
   if(j==max_item_name&&!isspace(buf[p+j]))panic("Item name too long");
   /*17:*/
#line 476 "dlx-pre.w"

   for(k= 1;o,strncmp(cl[k].name,cl[last_itm].name,max_item_name);k++);
   if(k<last_itm)panic("Duplicate item name");

   /*:17*/
#line 462 "dlx-pre.w"
   ;
   /*18:*/
#line 480 "dlx-pre.w"

   if(last_itm> max_itms)panic("Too many items");
   if(second==max_itms)
     oo,cl[last_itm-1].next= last_itm,cl[last_itm].prev= last_itm-1;
   else o,cl[last_itm].next= cl[last_itm].prev= last_itm;

   o,nd[last_itm].up= nd[last_itm].down= last_itm;
   last_itm++;

   /*:18*/
#line 463 "dlx-pre.w"
   ;
   for(p+= j+1;o,isspace(buf[p]);p++);
   if(buf[p]=='|'){
     if(second!=max_itms)panic("Item name line contains | twice");
     second= last_itm;
     for(p++;o,isspace(buf[p]);p++);
   }
 }
 if(second==max_itms)second= last_itm;
 o,cl[root].prev= second-1;
 last_node= last_itm;
 o,nd[last_node].itm= 0;

 /*:16*/
#line 174 "dlx-pre.w"
 ;
 /*19:*/
#line 495 "dlx-pre.w"

 while(1){
   if(!fgets(buf,bufsize,stdin))break;
   if(o,buf[p= strlen(buf)-1]!='\n')panic("Option line too long");
   for(p= 0;o,isspace(buf[p]);p++);
   if(buf[p]=='|'||!buf[p])continue;
   i= last_node;
   for(pp= 0;buf[p];){
     for(j= 0;j<max_item_name&&(o,!isspace(buf[p+j]))&&buf[p+j]!=':';j++)
       o,cl[last_itm].name[j]= buf[p+j];
     if(!j)panic("Empty item name");
     if(j==max_item_name&&!isspace(buf[p+j])&&buf[p+j]!=':')
       panic("Item name too long");
     if(j<max_item_name)o,cl[last_itm].name[j]= '\0';
     /*20:*/
#line 536 "dlx-pre.w"

     for(k= 0;o,strncmp(cl[k].name,cl[last_itm].name,max_item_name);k++);
     if(k==last_itm)panic("Unknown item name");
     if(o,nd[k].aux>=i)panic("Duplicate item name in this option");
     last_node++;
     if(last_node==max_nodes)panic("Too many nodes");
     o,nd[last_node].itm= k;
     if(k<second)pp= 1;
     o,t= nd[k].len+1;
     /*21:*/
#line 551 "dlx-pre.w"

     o,nd[k].len= t;
     nd[k].aux= last_node;
     o,r= nd[k].up;
     ooo,nd[r].down= nd[k].up= last_node,nd[last_node].up= r,nd[last_node].down= k;

     /*:21*/
#line 545 "dlx-pre.w"
     ;

     /*:20*/
#line 509 "dlx-pre.w"
     ;
     if(buf[p+j]!=':')o,nd[last_node].color= 0;
     else if(k>=second){
       if((o,isspace(buf[p+j+1]))||(o,!isspace(buf[p+j+2])))
	 panic("Color must be a single character");
       o,nd[last_node].color= buf[p+j+1];
       p+= 2;
     }else panic("Primary item must be uncolored");
     for(p+= j+1;o,isspace(buf[p]);p++);
   }
   if(!pp){
     if(vbose&show_warnings)
       fprintf(stderr,"Option ignored (no primary items): "O"s",buf);
     while(last_node> i){
       /*22:*/
#line 557 "dlx-pre.w"

       o,k= nd[last_node].itm;
       oo,nd[k].len--,nd[k].aux= i-1;
       o,q= nd[last_node].up,r= nd[last_node].down;
       oo,nd[q].down= r,nd[r].up= q;

       /*:22*/
#line 523 "dlx-pre.w"
       ;
       last_node--;
     }
   }else{
     o,nd[i].down= last_node;
     last_node++;
     if(last_node==max_nodes)panic("Too many nodes");
     options++;
     o,nd[last_node].up= i+1;
     o,nd[last_node].itm= -options;
   }
 }

 /*:19*/
#line 175 "dlx-pre.w"
 ;
 if(vbose&show_basics)
   /*23:*/
#line 563 "dlx-pre.w"

   fprintf(stderr,
	   "("O"lld options, "O"d+"O"d items, "O"d entries successfully read)\n",
	   options,second-1,last_itm-second,last_node-last_itm);

 /*:23*/
#line 177 "dlx-pre.w"
 ;
 if(vbose&show_tots)
   /*24:*/
#line 573 "dlx-pre.w"

   {
     fprintf(stderr,"Item totals:");
     for(k= 1;k<last_itm;k++){
       if(k==second)fprintf(stderr," |");
       fprintf(stderr," "O"d",nd[k].len);
     }
     fprintf(stderr,"\n");
   }

 /*:24*/
#line 179 "dlx-pre.w"
 ;
 imems= mems,mems= 0;
 /*29:*/
#line 673 "dlx-pre.w"

 for(cc= 1;cc<last_itm;cc++)if(o,nd[cc].len==0)
			      /*41:*/
#line 916 "dlx-pre.w"

			      {
				itms_out++;
				if(cc>=second){
				  if(vbose&show_details)
				    fprintf(stderr," "O"."max_item_name_str"s is in no options\n",cl[cc].name);
				}else/*42:*/
#line 929 "dlx-pre.w"

				  {
				    if(vbose&show_details)
				      fprintf(stderr,"Primary item "O"."max_item_name_str"s is in no options!\n",cl[cc].name);
				    options_out= options;
				    itms_out= last_itm-1;
				    printf(""O"."max_item_name_str"s\n",cl[cc].name);
				    goto all_done;
				  }

				/*:42*/
#line 922 "dlx-pre.w"
				;
			      }

 /*:41*/
#line 675 "dlx-pre.w"
 ;
 for(rnd= 1;rnd<rounds;rnd++){
   if(vbose&show_choices)
     fprintf(stderr,"Beginning round "O"d:\n",rnd);
   for(change= 0,c= 1;c<last_itm;c++)if(o,nd[c].len)
				       /*31:*/
#line 692 "dlx-pre.w"

				       {
					 if(sanity_checking)sanity();
					 if(delta&&(mems>=thresh)){
					   thresh+= delta;
					   fprintf(stderr,
						   " after "O"lld mems: "O"d."O"d, "O"d items out, "O"d options out\n",
						   mems,rnd,c,itms_out,options_out);
					 }
					 if(mems>=timeout)goto finish;
					 stack= 0,hide(c);
					 if(stack)/*32:*/
#line 719 "dlx-pre.w"

					   {
					     unhide(c);
					     if(vbose&show_details)
					       fprintf(stderr,"Deleting item "O"."max_item_name_str"s, forced by "O"."max_item_name_str"s\n",
						       cl[c].name,cl[stack].name);
					     for(o,r= nd[c].down;r>=last_itm;r= rrr){
					       o,rrr= nd[r].down;
					       /*33:*/
#line 739 "dlx-pre.w"

					       {
						 for(q= r+1;q!=r;){
						   o,cc= nd[q].itm;
						   if(cc<=0){
						     o,q= nd[q].up;
						     continue;
						   }
						   if(cc==stack)break;
						   q++;
						 }
						 if(q!=r)/*34:*/
#line 754 "dlx-pre.w"

						   {
						     if(vbose&show_details){
						       fprintf(stderr," shortening");
						       t= dpoption(r,stderr),
							 fprintf(stderr," (option "O"d)\n",t);
						     }
						     o,nd[r].up= r+1,nd[r].down= r-1;
						     o,nd[r].itm= 0;
						   }

						 /*:34*/
#line 750 "dlx-pre.w"

						 else/*35:*/
#line 765 "dlx-pre.w"

						   {
						     if(vbose&show_details){
						       fprintf(stderr," deleting");
						       t= dpoption(r,stderr),
							 fprintf(stderr," (option "O"d)\n",t);
						     }
						     options_out++;
						     for(o,q= r+1;q!=r;){
						       o,cc= nd[q].itm;
						       if(cc<=0){
							 o,q= nd[q].up;
							 continue;
						       }
						       o,t= nd[cc].len-1;
						       if(t==0)/*41:*/
#line 916 "dlx-pre.w"

							 {
							   itms_out++;
							   if(cc>=second){
							     if(vbose&show_details)
							       fprintf(stderr," "O"."max_item_name_str"s is in no options\n",cl[cc].name);
							   }else/*42:*/
#line 929 "dlx-pre.w"

							     {
							       if(vbose&show_details)
								 fprintf(stderr,"Primary item "O"."max_item_name_str"s is in no options!\n",cl[cc].name);
							       options_out= options;
							       itms_out= last_itm-1;
							       printf(""O"."max_item_name_str"s\n",cl[cc].name);
							       goto all_done;
							     }

							   /*:42*/
#line 922 "dlx-pre.w"
							   ;
							 }

						       /*:41*/
#line 780 "dlx-pre.w"
						       ;
 o,nd[cc].len= t;
 o,uu= nd[q].up,dd= nd[q].down;
 oo,nd[uu].down= dd,nd[dd].up= uu;
 q++;
						     }
						   }

						 /*:35*/
#line 751 "dlx-pre.w"
						 ;
					       }

					       /*:33*/
#line 727 "dlx-pre.w"
					       ;
					     }
					     o,nd[c].up= nd[c].down= c;
					     o,nd[c].len= 0,itms_out++;
					     change= 1;
					   }

					 /*:32*/
#line 703 "dlx-pre.w"

					 else{
					   for(o,r= nd[c].down;r>=last_itm;o,r= nd[r].down){
					     for(q= r-1;o,nd[q].down==q-1;q--);
					     if(o,nd[q].itm<=0)
					       /*36:*/
#line 802 "dlx-pre.w"

					       {
						 for(q= r+1;;){
						   o,cc= nd[q].itm;
						   if(cc<=0){
						     o,q= nd[q].up;
						     if(q> r)continue;
						     break;
						   }
						   o,nd[cc].aux= r,q++;
						 }
						 for(pp= 0,q= r+1;;){
						   o,cc= nd[q].itm;
						   if(cc<=0){
						     o,q= nd[q].up;
						     if(q> r)continue;
						     break;
						   }
						   for(x= nd[q].color,o,p= nd[cc].down;p>=last_itm;o,p= nd[p].down){
						     if(x> 0&&(o,nd[p].color==x))continue;
						     /*37:*/
#line 846 "dlx-pre.w"

						     for(qq= p+1;qq!=p;){
						       o,cc= nd[qq].itm;
						       if(cc<=0){
							 o,qq= nd[qq].up;
							 continue;
						       }
						       o,t= nd[cc].len-1;
						       if(!t&&cc<second&&nd[cc].aux!=r){
							 pp= cc;
							 goto midst;
						       }
						       o,nd[cc].len= t;
						       o,uu= nd[qq].up,dd= nd[qq].down;
						       oo,nd[uu].down= dd,nd[dd].up= uu;
						       qq++;
						     }

						     /*:37*/
#line 822 "dlx-pre.w"
						     ;
						   }
						   q++;
						 }
					       backup:for(q= r-1;q!=r;){
						   o,cc= nd[q].itm;
						   if(cc<=0){
						     o,q= nd[q].down;
						     continue;
						   }
						   for(x= nd[q].color,o,p= nd[cc].up;p>=last_itm;o,p= nd[p].up){
						     if(x> 0&&(o,nd[p].color==x))continue;
						     /*38:*/
#line 864 "dlx-pre.w"

						     for(qq= p-1;qq!=p;){
						       o,cc= nd[qq].itm;
						       if(cc<=0){
							 o,qq= nd[qq].down;
							 continue;
						       }
						       oo,nd[cc].len++;
						       o,uu= nd[qq].up,dd= nd[qq].down;
						       oo,nd[uu].down= nd[dd].up= qq;
						     midst:qq--;
						     }

						     /*:38*/
#line 834 "dlx-pre.w"
						     ;
						   }
						   q--;
						 }
						 if(pp)/*39:*/
#line 890 "dlx-pre.w"

						   {
						     if(vbose&show_details){
						       fprintf(stderr," "O"."max_item_name_str"s blocked by",cl[pp].name);
						       t= dpoption(r,stderr),
							 fprintf(stderr," (option "O"d)\n",t);
						     }
						     options_out++,change= 1;
						     o,nd[r].itm= stack,stack= r;
						   }

						 /*:39*/
#line 838 "dlx-pre.w"
						 ;
					       }

					     /*:36*/
#line 709 "dlx-pre.w"
					     ;
					   }
					   unhide(c);
					   for(r= stack;r;r= rr){
					     oo,rr= nd[r].itm,nd[r].itm= c;
					     /*40:*/
#line 901 "dlx-pre.w"

					     for(p= r+1;;){
					       o,cc= nd[p].itm;
					       if(cc<=0){
						 o,p= nd[p].up;
						 continue;
					       }
					       o,uu= nd[p].up,dd= nd[p].down;
					       oo,nd[uu].down= dd,nd[dd].up= uu;
					       oo,nd[cc].len--;
					       if(nd[cc].len==0)/*41:*/
#line 916 "dlx-pre.w"

						 {
						   itms_out++;
						   if(cc>=second){
						     if(vbose&show_details)
						       fprintf(stderr," "O"."max_item_name_str"s is in no options\n",cl[cc].name);
						   }else/*42:*/
#line 929 "dlx-pre.w"

						     {
						       if(vbose&show_details)
							 fprintf(stderr,"Primary item "O"."max_item_name_str"s is in no options!\n",cl[cc].name);
						       options_out= options;
						       itms_out= last_itm-1;
						       printf(""O"."max_item_name_str"s\n",cl[cc].name);
						       goto all_done;
						     }

						   /*:42*/
#line 922 "dlx-pre.w"
						   ;
						 }

					       /*:41*/
#line 911 "dlx-pre.w"
					       ;
 if(p==r)break;
 p++;
					     }

					     /*:40*/
#line 714 "dlx-pre.w"
					     ;
					   }
					 }
				       }

   /*:31*/
#line 680 "dlx-pre.w"
   ;
 if(!change)break;
 }

 /*:29*/
#line 181 "dlx-pre.w"
 ;
 finish:/*43:*/
#line 941 "dlx-pre.w"

/*44:*/
#line 945 "dlx-pre.w"

 for(c= 1;c<last_itm;c++){
   if(c==second)printf(" |");
   if(o,nd[c].len)printf(" "O"."max_item_name_str"s",cl[c].name);
 }
 printf("\n");

 /*:44*/
#line 942 "dlx-pre.w"
 ;
 /*45:*/
#line 952 "dlx-pre.w"

 for(c= 1;c<last_itm;c++)if(o,nd[c].len){
     for(o,r= nd[c].down;r>=last_itm;o,r= nd[r].down){
       for(q= r-1;o,nd[q].down==q-1;q--);
       if(o,nd[q].itm<=0){
	 t= dpoption(r,stdout);
	 printf("\n");
	 if(vbose&show_orig_nos)
	   printf("| (from "O"d)\n",t);
       }
     }
   }

 /*:45*/
#line 943 "dlx-pre.w"
 ;

/*:43*/
#line 182 "dlx-pre.w"
 ;
 done:if(vbose&show_tots)
   /*24:*/
#line 573 "dlx-pre.w"

   {
     fprintf(stderr,"Item totals:");
     for(k= 1;k<last_itm;k++){
       if(k==second)fprintf(stderr," |");
       fprintf(stderr," "O"d",nd[k].len);
     }
     fprintf(stderr,"\n");
   }

 /*:24*/
#line 184 "dlx-pre.w"
 ;
 all_done:if(vbose&show_basics){
  fprintf(stderr,
	  "Removed "O"d option"O"s and "O"d item"O"s, after "O"llu+"O"llu mems,",
	  options_out,options_out==1?"":"s",
	  itms_out,itms_out==1?"":"s",imems,mems);
  fprintf(stderr," "O"d round"O"s.\n",
	  rnd,rnd==1?"":"s");
 }
}

/*:4*/
