#include "types.h"
#include "stat.h"
#include "user.h"

char buf[512];

void
uniq(int fd, int count, int dup, int lower)
{
  int i,j,n;
  int ct = 0;
  int c = 0;
  int pos,linelen = 0;
  int first = 1;
  int beginline = 1;
  char *line = malloc(sizeof(char)*sizeof(buf));
  char *prevline = malloc(sizeof(char)*sizeof(buf));

  while((n=read(fd,buf,sizeof(buf)))>0){
    if (n!=sizeof(buf)){
      buf[n]='\n';
      n++;
    }
    // for(i=n;i<sizeof(buf);i++){
    //   buf[i]='\n';
    // }
    for(i=0;i<n;i++){
      if(beginline){
        // printf(1,"allocate\n");
        line = malloc(sizeof(char)*sizeof(buf));
        for(j=0;j<sizeof(buf);j++){
          line[j]='\0';
        }
        // printf(1,"line");
        // printf(1,"%s\n",line);
        // printf(1,"end\n");
        // printf(1,"line");
        // printf(1,"%s\n",line);
        beginline=0;
      }
      if(buf[i]=='\n'){
        line[i-pos+(512*c)]='\n';
        for(j=i-pos+(512*c);j<linelen;j++){
          line[j]='\0';
        }
        c=0;
        pos=pos+linelen+1;
        // printf(1,"%d\n",pos);
        linelen=0;
        beginline=1;
        if(first){
          first = 0;
          free(prevline);
          prevline = malloc(sizeof(char)*sizeof(line));
          strcpy(prevline,line);
          free(line);
          if (count == 0 && dup == 0){
            printf(1,"%s",prevline);
          }
          else{
            ct++;
          }
        }
        else{
          if(strcmp(prevline,line)!=0){
            if(count==0 && dup==0 && lower==0){
              printf(1,"%s",line);
            }
            else{
              //make both lower case and compare
              if(lower==1){
                char tempprev[512];
                char templine[512];
                for(j=0; j<512; j++){
                  if (prevline[j]=='\n'){
                    break;
                  }
                  if ((prevline[j] >= 'A') && (prevline[j] <= 'Z')){
                    tempprev[j] = prevline[j] + 32;
                  }
                }
                for(j=0; j<100; j++){
                  if (line[j]=='\n'){
                    break;
                  }
                  if ((line[j] >= 'A') && (line[j] <= 'Z')){
                    templine[j] = line[j] + 32;
                  }
                }
                if(strcmp(tempprev,templine)==0){
                  printf(1,"%s",line);
                }
                else{
                  printf(1,"%s",prevline);
                }
              }
              //only print if there are duplicate lines
              else if(dup==1){
                if(ct>1){
                  printf(1,"%s",prevline);
                }
              }
              //for the last line
              else if(i == n-1 && n<sizeof(buf)){
                printf(1,"%d %s",ct,prevline);
                printf(1,"%d %s",ct,line);
              }
              else {
                printf(1,"%d %s",ct,prevline);
              }
              ct = 1;
            }
            free(prevline);
            prevline = malloc(sizeof(char)*sizeof(line));
            for(j=0;j<sizeof(line);j++){
              prevline[j]='\0';
            }
            strcpy(prevline,line);
            free(line);
          }
          else{
            // printf(1,"else");
            // printf(1,"%s",line);
            if (count==1 || dup==1){
              ct++;
            }
            free(line);
          }
        }
      }
      else if(i==n-1){
        pos = 0;
        linelen++;
        c++;
      }
      else{
        line[i-pos+(512*c)]=buf[i];
        // printf(1,"%d\n",i);
        // printf(1,"i,pos");
        // printf(1,"%d\n",pos);
        // printf(1,"%d\n",(i-pos+(512*c)));
        linelen++;
      }
    }
    // printf(1,"%d %s",ct,prevline);
  }

  if(n < 0){
    printf(1, "uniq: read error\n");
    exit();
  }

  free(prevline);
}

int
main(int argc, char* argv[])
{
  int fd, i;
  int count,dup,lower = 0;

  if(argc <= 1){
    uniq(0,0,0,0);
    exit();
  }
  if(argc > 1){
    for(i=1;i<argc;i++){
      if(argv[i][0]=='-'){
        if(argv[i][1]=='c')
          count=1;
        if(argv[i][1]=='d')
          dup=1;
        if(argv[i][1]=='i')
          lower=1;
      }
    }
  }

  for(i=1;i<argc;i++){
    if((fd = open(argv[1], 0)) >= 0){
      uniq(fd,count,dup,lower);
      close(fd);
    }
    else if((fd = open(argv[argc-1], 0)) >= 0){
      uniq(fd,count,dup,lower);
      close(fd);
    }
    else{
      printf(1, "uniq: cannot open %s\n", argv[i]);
      exit();
    }
  }

  exit();
}
