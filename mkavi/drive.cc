#include <cstdio>
#include <cstdlib>
#include "mkavi.h"

int main(int argc, char **argv)
{
  unsigned char *image;
  int nframes=45,fps=16;
  int h=248,w=272;
  FILE *fp;

  int i;
  char buf[256];
  char fname[256];
  if (argc != 5){
    fprintf(stderr, "usage: mkavi nframes fps wid hgt\n");
    return 1;
  }
  nframes=atoi(argv[1]);
  fps=atoi(argv[2]);
  w=atoi(argv[3]);
  h=atoi(argv[4]);
  image = (unsigned char *)malloc(h*w*3);
  if(mkavi(nframes,fps,w,h,0,1,image)==0){
    fprintf(stderr, "sorry - windows must be multiples of 4\n");
    free(image);
    return 1;
  }

  for(i=0;i<nframes;i++){
    sprintf(fname,"frame_%d.ppm",i);
    fp=fopen(fname,"rb");
    fgets(buf,256,fp);
    fgets(buf,256,fp);
    fgets(buf,256,fp);
    fread(image,1,h*w*3,fp);
    fclose(fp);

    mkavi(nframes,fps,w,h,i,2,image);
  }

  mkavi(nframes,fps,w,h,0,3,image);
  free(image);

  return 0;
}
