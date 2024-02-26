#include "c_img.h"
#include "seamcarving.h"
#include <stdio.h>
#include <math.h>
 
/*void create_img(struct rgb_img **im, size_t height, size_t width){
    *im = (struct rgb_img *)malloc(sizeof(struct rgb_img));
    (*im)->height = height;
    (*im)->width = width;
    (*im)->raster = (uint8_t *)malloc(3 * height * width);
}
 
 
int read_2bytes(FILE *fp){
    uint8_t bytes[2];
    fread(bytes, sizeof(uint8_t), 1, fp);
    fread(bytes+1, sizeof(uint8_t), 1, fp);
    return (  ((int)bytes[0]) << 8)  + (int)bytes[1];
}
 
void write_2bytes(FILE *fp, int num){
    uint8_t bytes[2];
    bytes[0] = (uint8_t)((num & 0XFFFF) >> 8);
    bytes[1] = (uint8_t)(num & 0XFF);
    fwrite(bytes, 1, 1, fp);
    fwrite(bytes+1, 1, 1, fp);
}
 
void read_in_img(struct rgb_img **im, char *filename){
    FILE *fp = fopen(filename, "rb");
    size_t height = read_2bytes(fp);
    size_t width = read_2bytes(fp);
    create_img(im, height, width);
    fread((*im)->raster, 1, 3*width*height, fp);
    fclose(fp);
}
 
void write_img(struct rgb_img *im, char *filename){
    FILE *fp = fopen(filename, "wb");
    write_2bytes(fp, im->height);
    write_2bytes(fp, im->width);
    fwrite(im->raster, 1, im->height * im->width * 3, fp);
    fclose(fp);
}
 
uint8_t get_pixel(struct rgb_img *im, int y, int x, int col){
    return im->raster[3 * (y*(im->width) + x) + col];
}
 
void set_pixel(struct rgb_img *im, int y, int x, int r, int g, int b){
    im->raster[3 * (y*(im->width) + x) + 0] = r;
    im->raster[3 * (y*(im->width) + x) + 1] = g;
    im->raster[3 * (y*(im->width) + x) + 2] = b;
}
 
void destroy_image(struct rgb_img *im)
{
    free(im->raster);
    free(im);
}
 
 
void print_grad(struct rgb_img *grad){
    int height = grad->height;
    int width = grad->width;
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            printf("%d\t", get_pixel(grad, i, j, 0));
        }
    printf("\n");    
    }
}*/
 
void calc_energy(struct rgb_img *im, struct rgb_img **grad)
{
    int height = im->height;
    int width = im->width;
    create_img(grad, height, width);
    for(int i = 0;i<height; i++) 
    {
        for(int j=0;j<width;j++) 
        {
            int rx,gx,bx;
            int ry,gy,by;
            rx = get_pixel(im, i, (j+1)%(width), 0) - get_pixel(im, i, ((j-1)<0)?width+j-1: j-1, 0);
            gx = get_pixel(im, i, (j+1)%(width), 1) - get_pixel(im, i, ((j-1)<0)?width+j-1: j-1, 1);
            bx = get_pixel(im, i, (j+1)%(width), 2) - get_pixel(im, i, ((j-1)<0)?width+j-1: j-1, 2);
            ry = get_pixel(im, (i+1)%(height), j, 0) - get_pixel(im, ((i-1)<0)?height+i-1: i-1, j,0);
            gy = get_pixel(im, (i+1)%(height), j, 1) - get_pixel(im, ((i-1)<0)?height+i-1: i-1, j,1);
            by = get_pixel(im, (i+1)%(height), j, 2) - get_pixel(im, ((i-1)<0)?height+i-1: i-1, j,2);
            double delt = sqrt(rx*rx+gx*gx+bx*bx+ry*ry+gy*gy+by*by)/10;
            (*grad)->raster[3*(i*width+j)] = (uint8_t)delt;
        }
    }
}
#define min(a,b) ((a>=b)? b: a)
void dynamic_seam(struct rgb_img *grad, double **best_arr)
{
   int height = grad->height;
   int width = grad->width;
   (*best_arr) = (double*)malloc(height*width*sizeof(double));
   for(int i = 0;i< height;i++) {
       for(int j=0;j<width;j++) {
           if(i==0) { 
               (*best_arr)[i*width+j] = get_pixel(grad,i,j,0);  
           }
           else if(j==0) {
               (*best_arr)[i*width+j] = (get_pixel(grad,i,j,0)) + min((*best_arr)[((i-1)*width+j)],(*best_arr)[((i-1)*width+j+1)]);
           }
           else {
               (*best_arr)[i*width+j] = (get_pixel(grad,i,j,0)) + min((*best_arr)[(i-1)*width+j-1],min((*best_arr)[(i-1)*width+j], (*best_arr)[(i-1)*width+j+1]));
           }
       }
   }
}
void recover_path(double *best, int height, int width, int **path)
{
    (*path)= (int*)malloc(height*sizeof(int));
    for(int i=0;i<height;i++) {
        uint8_t min = 0xff;
        int min_idx = 0;
        for(int j=0;j<width;j++){
            if(best[i*width+j] < min) {
                min = best[i*width+j];
                min_idx = j;
            }
        }
        (*path)[i] = min_idx;
    }
}
void remove_seam(struct rgb_img *src, struct rgb_img **dest, int *path)
{
    int height = src->height;
    int width = src->width;
    create_img(dest, height, width-1);
    for(int i = 0;i<height;i++){
        int col = 0;
        for(int j=0;j<width; j++) {
            if(j != path[i])
                set_pixel((*dest),i,col++, get_pixel(src, i,j,0), get_pixel(src, i, j, 1), get_pixel(src, i,j,2));
        }
    }
}
/*
int main()
{
    struct rgb_img *in;
    read_in_img(&in, "6x5.bin");
    //for(int i = 0;i<50;i++) {
        struct rgb_img *grad;
        calc_energy(in, &grad);
        print_grad(grad);
        double *best_arr;
        dynamic_seam(grad, &best_arr);
 
        for(int i = 0; i < grad->height; i++){
            for(int j = 0; j < grad->width; j++){
                printf("%.1lf,", best_arr[i*grad->width+j]);
            }
        }
        printf("\n");
        
        int *path;
        recover_path(best_arr, in->height, in->width, &path);
        printf("path\n");
        for(int i = 0; i < in->height; i++){
            printf("%d,", path[i]);
        }
        printf("\n");
        printf("end\n");
        struct rgb_img *out;
        remove_seam(in, &out, path);
        for(int i = 0; i < in->height; i++){
            for(int j = 0; j < in->width; j++){
                for(int col = 0; col < 3; col++){
                    printf("%d,", get_pixel(in, i, j, col));
                }
            }
        }
        printf("all end\n");
        
        write_img(out, "out.bin");
        in = out;
    //}
}
*/
 
