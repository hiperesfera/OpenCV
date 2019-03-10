#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main() {


CvCapture* capture = cvCaptureFromCAM(-1);//captura las imagenes de una cámara genérica, si están varias instaladas en el ordenador salta un menú para cambiar la cámara.
//Cambiar las propiedades de la captura de la cámara
cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_WIDTH,480.);anchura
cvSetCaptureProperty(capture,CV_CAP_PROP_FRAME_HEIGHT,352.);altura
// cvSetCaptureProperty(capture,CV_CAP_PROP_FPS,15.);número de frames
//cvSetCaptureProperty(capture,CV_CAP_PROP_BRIGHTNESS,0.);brillo
//cvSetCaptureProperty(capture,CV_CAP_PROP_CONTRAST,0.); contraste
//cvSetCaptureProperty(capture,CV_CAP_PROP_SATURATION,0.);saturación



int i,j,r,z;
int q=0;  


IplImage* imagen2 =NULL;
sleep(1500);//esta función es para que espere segundo y medio, si lo ejecutas en windows es tal cual si la ejecutas en linux es 1.5 el tiempo.
IplImage* imagen = cvQueryFrame( capture );//lo que capturamos con la cámara lo pasamos a imagen para tratarlo.

int h = imagen->height;//altura de la imagen
int w = imagen->width; //anchura de la imagen

cvNamedWindow( "real",CV_WINDOW_AUTOSIZE);//crea la ventana en donde vamos a poner las imagenes capturadas por la cámara directamente
cvNamedWindow( "background",CV_WINDOW_AUTOSIZE);//crea la ventana en donde vamos a poner la imagen de lo que considera el programa como fondo
cvNamedWindow( "analizar",CV_WINDOW_AUTOSIZE);//vamos a poner las imágenes en la que se resaltará sólo los objetos considerados en movimiento


cvSaveImage("fondo.png",imagen);//gravamos la primera imagen en el disco duro, la que consideramos como fondo
imagen2=cvLoadImage("fondo.png",0);//la cargamos como imagen en blanco y negro
cvShowImage ("background",imagen2);//la sacamos por pantalla en la ventana background


CvScalar s,o;    

int calculo[h][w],fondo[h][w],error[h][w],medio[h][w];

int min,max,x;

//Paso los valores de los píxeles de la imagen considerada fondo a una matriz para hacer sobre la matriz los cálculos pertinentes.

 for(i=0;i<h;i++)
 {
  for(j=0;j<w;j++)
  {         
       
  r = ((uchar *)(imagen2->imageData + i*imagen2-> widthStep))[j*imagen2->nChannels + 0];

  fondo[i][j]=r;
  } 
 }
 
//Cálculo del valor medio de cada píxel y error
for(i=0;i<h;i++)
       {
        for(j=0;j<w;j++)
               {                
                             medio[i][j]=(fondo[i-1][j-1]+fondo[i-1][j]+fondo[i-1][j+1]+fondo[i][j-1]+fondo[i][j]+fondo[i][j+1]+fondo[i+1][j-1]+fondo[i+1][j]+fondo[i+1][j+1])/9;//valor medio
               error[i][j]=(sqrt(((fondo[i][j]-medio[i][j])*(fondo[i][j]-medio[i][j]))+((fondo[i-1][j-1]-medio[i][j])*(fondo[i-1][j-1]-medio[i][j]))+((fondo[i-1][j+1]-medio[i][j])*(fondo[i-1][j+1]-medio[i][j]))+((fondo[i-1][j]-medio[i][j])*(fondo[i-1][j]-medio[i][j]))+((fondo[i][j-1]-medio[i][j])*(fondo[i][j-1]-medio[i][j]))+((fondo[i][j+1]-medio[i][j])*(fondo[i][j+1]-medio[i][j]))+((fondo[i+1][j-1]-medio[i][j])*(fondo[i+1][j-1]-medio[i][j]))+((fondo[i+1][j]-medio[i][j])*(fondo[i+1][j]-medio[i][j]))+((fondo[i+1][j+1]-medio[i][j])*(fondo[i+1][j+1]-medio[i][j]))))/9;//el error

                           }
          }
          
IplImage* color = cvCreateImage(cvSize(w,h),IPL_DEPTH_8U,3);//creo el puntero de la imagen en color en la que voi a poner mi imagen procesada.

//Vamos ha hacer un bucle infinito para que el resto de las imágenes que siga capturando la cámara, las vaya pasando por el algoritmo
while( 1 ) 

  {
     
      IplImage* imagen3 = cvQueryFrame( capture );//la siguiente imagen procesada la paso a un nuevo puntero imagen 3.

      cvShowImage( "real", imagen3 );//la imagen que acabo de capturar me la saca por pantalla en la ventana real
      cvSaveImage("analizar.png",imagen3);//grabo esta misma imagen

      IplImage* imagen4=cvLoadImage("analizar.png",0);//la misma imagen grabada la cargo pero ahora en blanco y negro para compararla con el fondo

//hace el calculo en cada pixel para ver si hay movimiento o no
    for(i=0;i<h;i++)
       {
         for(j=0;j<w;j++)
           {                
        
               r = ((uchar *)(imagen4->imageData + i*imagen4-> widthStep))[j*imagen4->nChannels + 0];
           calculo[i][j]=r;
//los valores entre los que puede estar la imagen
           min=fondo[i][j]-error[i][j];
           max=fondo[i][j]+error[i][j];

               if(calculo[i][j]>(max+60) || calculo[i][j]<(min-60)) //El valor 60 es el tamaño de la barra de error que hemos puesto
                      {
                      q++;//para saber el número de pixeles que tienen movimiento
                         ((uchar *)(color->imageData+i*color->widthStep))[j*color->nChannels+ 1]=calculo[i][j];//a los píxeles en los que nos detecta movimiento le da el valor de este píxel a la imagen que vamos a pasar por pantalla, pero ahora en color verde
                                }
             else //a los pixeles en los que no nos detecta movimiento, los pone a cero todos los canales
              {
                     ((uchar *)(color->imageData+i*color->widthStep))[j*color->nChannels+ 0]=0;
                     ((uchar *)(color->imageData+i*color->widthStep))[j*color->nChannels+ 1]=0;
                     ((uchar *)(color->imageData+i*color->widthStep))[j*color->nChannels+ 2]=0;
                            }
               } 
        }

if(q>300)//cuando el número de píxeles es menor que 300 pone la imagen en negro es como si no detectase movimiento
{

cvShowImage ("analizar",color);//sacamos en pantalla la imagen a la que le extraemos el movimiento.

 }        
   else
   {

      q=0;
      ((uchar *)(color->imageData+i*color->widthStep))[j*color->nChannels+ 1]=0;
      ((uchar *)(color->imageData+i*color->widthStep))[j*color->nChannels+ 1]=0;
      ((uchar *)(color->imageData+i*color->widthStep))[j*color->nChannels+ 2]=0;
 }
if( (cvWaitKey(10) & 255) == 27 ) break;//Dejar el bucle infinito
  }
cvWaitKey(0);
cvDestroyAllWindows();
return 0;
}
