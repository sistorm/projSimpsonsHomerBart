/**
* GTI770 - Systemes intelligents et apprentissage machine
* Alessandro L. Koerich
* alessandro.koerich@etsmtl.ca
* 2015
*
* EXEMPLE 1: Feature extraction from RGB images
*                       Simpsons Family
**/

// INCLUDES
#include <cv.h> 			//OpenCV lib
#include <cvaux.h>			//OpenCV lib
#include <highgui.h>		//OpenCV lib
#include <stdio.h>	
#include <math.h>


// DEFINES
#define NUM_SAMPLES 100
#define NUM_FEATURES 9

// FONCTIONS
int countOrange(int red, int green, int blue);
int countWhite(int red, int green, int blue);
int countSkin(int red, int green, int blue);
int countShortBart(int red, int green, int blue);
int countLisaDress(int red, int green, int blue);
int countHomerBeard(int red, int green, int blue);
int countHomerPants(int red, int green, int blue);
int countHomerShoes(int red, int green, int blue);

// Bart Train: 80 items: bart1.bmp - bart80.bmp
// Homer Train 62 items: homer1.bmp - homer62.bmp
// Lisa Train 33 items: lisa1.bmp - lisa33.bmp
// Bart Valid: 54 items: bart116.bmp - bart169.bmp
// Homer Valid: 37 items: homer88.bmp - homer124.bmp
// Lisa Valid: 12 items: lisa34.bmp - lisa46.bmp

// MAIN
int main( int argc, char** argv )   
{

	// Variable store pressed key
	int tecla;

	// General variables (loop)
	int h;
	int w;
	int ii;
	int jj;
	int iNum;

	// Variables to store the RGB values of a pixel
	unsigned char red;
	unsigned char blue;
	unsigned char green;

	// Feature vector [rows] [columns]
	// In fact it is a "matrix of features"
	float fVector[ NUM_SAMPLES ][ NUM_FEATURES ];

	// Feature variables
	float fOrange;
	float fWhite;
	float fSkin;
	float fShortBart;
	float fLisaDress;
	float fHomerBeard;
	float fHomerPants;
	float fHomerShoes;


	// Variable filename
	static char cFileName[ 50 ] = {'\0'};
	FILE *fp;
	
	// Open a text file to store the feature vectors
	fp = fopen ("apprentissage-homer-bart.txt","w");
	// fp = fopen ("validation-homer-bart.txt","w");

	if(fp == NULL) {
		perror("failed to open apprentissage-homer-bart.txt");
		// perror("failed to open validation-homer-bart.txt");
		return EXIT_FAILURE;
	}

	// OpenCV variables related to the image structure.
	// IplImage structure contains several information of the image (See OpenCV manual).	
	IplImage *img 			= NULL;
	IplImage *processed 	= NULL;
	IplImage *threshold 	= NULL;
	
	// OpenCV variable that stores the image width and height
	CvSize tam;

	// OpenCV variable that stores a pixel value
	CvScalar element;
	
	// Fill fVector with zeros
	for ( ii = 0 ; ii < NUM_SAMPLES ; ii++ )
	{
		for ( jj = 0; jj < NUM_FEATURES; jj++ )
		{
			fVector[ii][jj] = 0.0;
		}
	}

	// Fill cFileName with zeros
	for ( ii = 0 ; ii < 50 ; ii++ )
	{
		cFileName[ ii ] = '\0';
	}


	// *****************************************************************************************************************************************
	// TRAINING SAMPLES 
	// HOMER
	// Homer Train 62 items: homer1.bmp - homer62.bmp
	// *****************************************************************************************************************************************

	// Take all the image files at the range
	for ( iNum = 1; iNum <= 62; iNum++ )
	{
		// Build the image filename and path to read from disk
		sprintf ( cFileName, "Train/homer%d.bmp", (int)(iNum) );
		//sprintf ( cFileName, "Valid/homer%d.bmp", (int)(iNum) ); 
		printf ( " %s\n", cFileName);

		// Load the image from disk to the structure img.
		// 1  - Load a 3-channel image (color)
		// 0  - Load a 1-channel image (gray level)
		// -1 - Load the image as it is  (depends on the file)
		
		img = cvLoadImage( cFileName, -1 );

		// Gets the image size (width, height) 'img' 
		tam = cvGetSize( img );

		// Creates a header and allocates memory (tam) to store a copy of the original image.
		// 1 - gray level image
		// 3 - color image	
		// processed = cvCreateImage( tam, IPL_DEPTH_8U, 3);

		// Make a image clone and store it at processed and threshold
		processed 	= cvCloneImage( img );
		threshold  		= cvCloneImage( img );

		// Initialize variables with zero 
		fOrange 	= 0.0;
		fWhite 	= 0.0;
		fSkin = 0.0;
		fShortBart = 0.0;
		fLisaDress = 0.0;
		fHomerBeard = 0.0;
		fHomerPants = 0.0;
		fHomerShoes = 0.0;

		// Loop that reads each image pixel
		for( h = 0; h < img->height; h++ ) // rows
		{
			for( w = 0; w < img->width; w++ ) // columns
			{
				// Read each channel and writes it into the blue, green and red variables. Notice that OpenCV considers BGR
				blue  	= ( (uchar *)(img->imageData + h*img->widthStep) )[ w*img->nChannels + 0 ];
				green 	= ( (uchar *)(img->imageData + h*img->widthStep) )[ w*img->nChannels + 1 ];
				red     = ( (uchar *)(img->imageData + h*img->widthStep) )[ w*img->nChannels + 2 ];

				// Shows the pixel value at the screenl
				//printf( "pixel[%d][%d]= %d %d %d \n", h, w, (int)blue, (int)green, (int)red );
/*
				// Here starts the feature extraction....
				
				// Detect and count the number of orange pixels
				// Verify if the pixels have a given value ( Orange, defined as R[240-255], G[85-105], B[11-22] ). If so, count it...
				if ( blue>=11 && blue<=22 && green>=85 && green<=105 &&  red>=240 && red<=255 )
				{
					fOrange++;
				
					// Just to be sure we are doing the right thing, we change the color of the orange pixels to green [R=0, G=255, B=0] and show them into a cloned image (processed)
					/*
					( (uchar *)(processed->imageData + h*processed->widthStep) )[ w*processed->nChannels + 0 ] = 0; 
					( (uchar *)(processed->imageData + h*processed->widthStep) )[ w*processed->nChannels + 1 ] = 255; 
					( (uchar *)(processed->imageData + h*processed->widthStep) )[ w*processed->nChannels + 2 ] = 0; 
					*/
/*				}
				
				// Detect and count the number of white pixels (just a dummy feature...)
				// Verify if the pixels have a given value ( White, defined as R[253-255], G[253-255], B[253-255] ). If so, count it...
				if ( blue >= 253 && green >= 253 && red >= 253 )
				{
					fWhite++;
				}
				
				// Here you can add your own features....... Good luck
				
				// Detect and count the number of skin pixels
				if (blue >= 0 && blue <= 33 && green >= 185 && green <= 215 && red >= 235 && red <= 255)
				{
					fSkin++;

					// Just to be sure we are doing the right thing, we change the color of the orange pixels to green [R=0, G=255, B=0] and show them into a cloned image (processed)
					/*
					((uchar *)(processed->imageData + h*processed->widthStep))[w*processed->nChannels + 0] = 0;
					((uchar *)(processed->imageData + h*processed->widthStep))[w*processed->nChannels + 1] = 255;
					((uchar *)(processed->imageData + h*processed->widthStep))[w*processed->nChannels + 2] = 0;
					*/
/*				}
				// Here you can add your own features....... Good luck
				// Detect and count the number of skin pixels
				if (blue >= 130 && blue <= 135 && green >= 0 && green <= 15 && red >= 0 && red <= 0)
				{
					fShortBart++;

					// Just to be sure we are doing the right thing, we change the color of the orange pixels to green [R=0, G=255, B=0] and show them into a cloned image (processed)

					((uchar *)(processed->imageData + h*processed->widthStep))[w*processed->nChannels + 0] = 0;
					((uchar *)(processed->imageData + h*processed->widthStep))[w*processed->nChannels + 1] = 255;
					((uchar *)(processed->imageData + h*processed->widthStep))[w*processed->nChannels + 2] = 0;
				}
*/
				// Detect and count the number of orange pixels
				fOrange = countOrange(red, green, blue);

				// Detect and count the number of white pixels (just a dummy feature...)
				fWhite = countWhite(red, green, blue);

				// Here you can add your own features....... Good luck
				// Detect and count the number of skin pixels
				fSkin = countSkin(red, green, blue);

				// Detect and count the number of skin pixels
				fShortBart = countShortBart(red, green, blue);

				// Detect and count the number of pixels coresponding to the DRESS OF LISA
				fLisaDress = countLisaDress(red, green, blue);

				// Detect and count the number of pixels coresponding to the BEAR OF HOMER
				fHomerBeard = countHomerBeard(red, green, blue);

				// Detect and count the number of pixels coresponding to the PANTS OF HOMER
				fHomerPants = countHomerPants(red, green, blue);

				// Detect and count the number of pixels coresponding to the SHOES OF HOMER
				fHomerShoes = countHomerShoes(red, green, blue);


			}
		}

		// Lets make our counting somewhat independent on the image size...
		// Compute the percentage of pixels of a given colour.
		// Normalize the feature by the image size
		fOrange 	= fOrange / ( (int)img->height * (int)img->width );
		fWhite  	= fWhite  / ( (int)img->height * (int)img->width );
		fSkin = fSkin / ((int)img->height * (int)img->width);
		fShortBart = fShortBart / ((int)img->height * (int)img->width);
		fLisaDress = fLisaDress / ((int)img->height * (int)img->width);
		fHomerBeard = fHomerBeard / ((int)img->height * (int)img->width);
		fHomerPants = fHomerPants / ((int)img->height * (int)img->width);
		fHomerShoes = fHomerShoes / ((int)img->height * (int)img->width);

		// Store the feature value in the columns of the feature (matrix) vector
		fVector[iNum][1] = fOrange;
		fVector[iNum][2] = fWhite;
		fVector[iNum][3] = fSkin;
		fVector[iNum][4] = fShortBart;
		fVector[iNum][5] = fLisaDress;
		fVector[iNum][6] = fHomerBeard;
		fVector[iNum][7] = fHomerPants;
		fVector[iNum][8] = fHomerShoes;

		// Here you can add more features to your feature vector by filling the other columns: fVector[iNum][3] = ???; fVector[iNum][4] = ???;
		
		// Shows the feature vector at the screen
		printf("\n%d orange: %f | blanc:  %f | skin: %f | shortBart: %f | lisaDress: %f | homerBeard: %f | homerPants: %f | homerShoes: %f", iNum, fVector[iNum][1], fVector[iNum][2], fVector[iNum][3], fVector[iNum][4], fVector[iNum][5], fVector[iNum][6], fVector[iNum][7], fVector[iNum][8]);
		//printf( "\n%d %f %f %f %f %f", iNum, fVector[iNum][1], fVector[iNum][2], fVector[iNum][3], fVector[iNum][4], fVector[iNum][5] );

		// And finally, store your features in a file
		fprintf( fp, "%f,", fVector[iNum][1]);
		fprintf( fp, "%f,", fVector[iNum][2]);
		fprintf( fp, "%f,", fVector[iNum][3]);
		fprintf( fp, "%f,", fVector[iNum][4]);
		fprintf( fp, "%f,", fVector[iNum][5]);
		fprintf( fp, "%f,", fVector[iNum][6]);
		fprintf( fp, "%f,", fVector[iNum][7]);
		fprintf( fp, "%f,", fVector[iNum][8]);
		
		// IMPORTANT
		// Do not forget the label.... 	
		fprintf( fp, "%s\n", "Homer");


		// Finally, give a look at the original image and the image with the pixels of interest in green
		// OpenCV create an output window
		cvShowImage( "Original", img );
		cvShowImage( "Processed", processed );
		
		// Wait until a key is pressed to continue... 	
		tecla = cvWaitKey(0);
	}
	// *****************************************************************************************************************************************
	// *****************************************************************************************************************************************
	// *****************************************************************************************************************************************
	// TRAINING SAMPLES 
	// BART
	// Bart Train: 80 items: bart1.bmp - bart80.bmp
	// The code below is exactly the same for HOMER, except that we have changed the values of iNum and Homer -> Bart
	// along the file
	// *****************************************************************************************************************************************

	// Take all the image files at the range
	for ( iNum = 1; iNum <= 80; iNum++ )
	{
		// Build the image filename and path to read from disk
		sprintf ( cFileName, "Train/bart%d.bmp", (int)(iNum) ); 
		//sprintf ( cFileName, "Valid/bart%d.bmp", (int)(iNum) ); 
		printf ( " %s\n", cFileName);

		// Load the image from disk to the structure img.
		// 1  - Load a 3-channel image (color)
		// 0  - Load a 1-channel image (gray level)
		// -1 - Load the image as it is  (depends on the file)
		
		img = cvLoadImage( cFileName, -1 );

		// Gets the image size (width, height) 'img' 
		tam = cvGetSize( img );

		// Creates a header and allocates memory (tam) to store a copy of the original image.
		// 1 - gray level image
		// 3 - color image	
		// processed = cvCreateImage( tam, IPL_DEPTH_8U, 3);

		// Make a image clone and store it at processed and threshold
		processed 	= cvCloneImage( img );
		threshold  	= cvCloneImage( img );

		// Initialize variables with zero 
		fOrange 	= 0.0;
		fWhite 	= 0.0;
		fSkin = 0.0;
		fShortBart = 0.0;
		fLisaDress = 0.0;
		fHomerBeard = 0.0;
		fHomerPants = 0.0;
		fHomerShoes = 0.0;

		// Loop that reads each image pixel
		for( h = 0; h < img->height; h++ ) // rows
		{
			for( w = 0; w < img->width; w++ ) // columns
			{
				// Read each channel and writes it into the blue, green and red variables. Notice that OpenCV considers BGR
				blue  	= ( (uchar *)(img->imageData + h*img->widthStep) )[ w*img->nChannels + 0 ];
				green 	= ( (uchar *)(img->imageData + h*img->widthStep) )[ w*img->nChannels + 1 ];
				red   		= ( (uchar *)(img->imageData + h*img->widthStep) )[ w*img->nChannels + 2 ];

				// Shows the pixel value at the screenl
				//printf( "pixel[%d][%d]= %d %d %d \n", h, w, (int)blue, (int)green, (int)red );

				// Here starts the feature extraction....
/*				
				// Detect and count the number of orange pixels
				// Verify if the pixels have a given value ( Orange, defined as R[240-255], G[85-105], B[11-22] ). If so, count it...
				if ( blue>=11 && blue<=22 && green>=85 && green<=105 &&  red>=240 && red<=255 )
				{
					fOrange++;
				
					// Just to be sure we are doing the right thing, we change the color of the orange pixels to green [R=0, G=255, B=0] and show them into a cloned image (processed)
					/*
					( (uchar *)(processed->imageData + h*processed->widthStep) )[ w*processed->nChannels + 0 ] = 0; 
					( (uchar *)(processed->imageData + h*processed->widthStep) )[ w*processed->nChannels + 1 ] = 255; 
					( (uchar *)(processed->imageData + h*processed->widthStep) )[ w*processed->nChannels + 2 ] = 0; 
					*/
/*				}
				
				// Detect and count the number of white pixels (just a dummy feature...)
				// Verify if the pixels have a given value ( White, defined as R[253-255], G[253-255], B[253-255] ). If so, count it...
				if ( blue >= 253 && green >= 253 && red >= 253 )
				{
					fWhite++;
				}
				
				// Here you can add your own features....... Good luck
				// Detect and count the number of skin pixels
				if (blue >= 0 && blue <= 33 && green >= 185 && green <= 215 && red >= 235 && red <= 255)
				{
					fSkin++;

					// Just to be sure we are doing the right thing, we change the color of the orange pixels to green [R=0, G=255, B=0] and show them into a cloned image (processed)
					/*
					((uchar *)(processed->imageData + h*processed->widthStep))[w*processed->nChannels + 0] = 0;
					((uchar *)(processed->imageData + h*processed->widthStep))[w*processed->nChannels + 1] = 255;
					((uchar *)(processed->imageData + h*processed->widthStep))[w*processed->nChannels + 2] = 0;
					*/
/*				}

				// Here you can add your own features....... Good luck
				// Detect and count the number of skin pixels
				if (blue >= 130 && blue <= 135 && green >= 0 && green <= 15 && red >= 0 && red <= 0)
				{
					fShortBart++;

					// Just to be sure we are doing the right thing, we change the color of the orange pixels to green [R=0, G=255, B=0] and show them into a cloned image (processed)

					((uchar *)(processed->imageData + h*processed->widthStep))[w*processed->nChannels + 0] = 0;
					((uchar *)(processed->imageData + h*processed->widthStep))[w*processed->nChannels + 1] = 255;
					((uchar *)(processed->imageData + h*processed->widthStep))[w*processed->nChannels + 2] = 0;
				}
*/

				// Detect and count the number of orange pixels
				fOrange = countOrange(red, green, blue);

				// Detect and count the number of white pixels (just a dummy feature...)
				fWhite = countWhite(red, green, blue);

				// Here you can add your own features....... Good luck
				// Detect and count the number of skin pixels
				fSkin = countSkin(red, green, blue);

				// Detect and count the number of skin pixels
				fShortBart = countShortBart(red, green, blue);

				// Detect and count the number of pixels coresponding to the DRESS OF LISA
				fLisaDress = countLisaDress(red, green, blue);

				// Detect and count the number of pixels coresponding to the BEAR OF HOMER
				fHomerBeard = countHomerBeard(red, green, blue);

				// Detect and count the number of pixels coresponding to the PANTS OF HOMER
				fHomerPants = countHomerPants(red, green, blue);

				// Detect and count the number of pixels coresponding to the SHOES OF HOMER
				fHomerShoes = countHomerShoes(red, green, blue);

			}
		}

		// Lets make our counting somewhat independent on the image size...
		// Compute the percentage of pixels of a given colour.
		// Normalize the feature by the image size
		fOrange 	= fOrange / ( (int)img->height * (int)img->width );
		fWhite  	= fWhite  / ( (int)img->height * (int)img->width );
		fSkin = fSkin / ((int)img->height * (int)img->width);
		fShortBart = fShortBart / ((int)img->height * (int)img->width);
		fLisaDress = fLisaDress / ((int)img->height * (int)img->width);
		fHomerBeard = fHomerBeard / ((int)img->height * (int)img->width);
		fHomerPants = fHomerPants / ((int)img->height * (int)img->width);
		fHomerShoes = fHomerShoes / ((int)img->height * (int)img->width);

		// Store the feature value in the columns of the feature (matrix) vector
		fVector[iNum][1] = fOrange;
		fVector[iNum][2] = fWhite;
		fVector[iNum][3] = fSkin;
		fVector[iNum][4] = fShortBart;
		fVector[iNum][5] = fLisaDress;
		fVector[iNum][6] = fHomerBeard;
		fVector[iNum][7] = fHomerPants;
		fVector[iNum][8] = fHomerShoes;
		// Here you can add more features to your feature vector by filling the other columns: fVector[iNum][3] = ???; fVector[iNum][4] = ???;

		// Shows the feature vector at the screen
		printf("\n%d orange: %f | blanc:  %f | skin: %f | shortBart: %f | lisaDress: %f | homerBeard: %f | homerPants: %f | homerShoes: %f", iNum, fVector[iNum][1], fVector[iNum][2], fVector[iNum][3], fVector[iNum][4], fVector[iNum][5], fVector[iNum][6], fVector[iNum][7], fVector[iNum][8]);
		//printf( "\n%d %f %f %f %f %f", iNum, fVector[iNum][1], fVector[iNum][2], fVector[iNum][3], fVector[iNum][4], fVector[iNum][5] );

		// And finally, store your features in a file
		fprintf(fp, "%f,", fVector[iNum][1]);
		fprintf(fp, "%f,", fVector[iNum][2]);
		fprintf(fp, "%f,", fVector[iNum][3]);
		fprintf(fp, "%f,", fVector[iNum][4]);
		fprintf(fp, "%f,", fVector[iNum][5]);
		fprintf(fp, "%f,", fVector[iNum][6]);
		fprintf(fp, "%f,", fVector[iNum][7]);
		fprintf(fp, "%f,", fVector[iNum][8]);
		
		// IMPORTANT
		// Do not forget the label.... 	
		fprintf( fp, "%s\n", "Bart");


		// Finally, give a look at the original image and the image with the pixels of interest in green
		cvShowImage( "Original", img );
		cvShowImage( "Processed", processed );
		
		// Wait until a key is pressed to continue... 	
		tecla = cvWaitKey(0);
	}


	// *****************************************************************************************************************************************
	// *****************************************************************************************************************************************
	// *****************************************************************************************************************************************
	// TRAINING SAMPLES 
	// LISA
	// Lisa Train: 33 items: lisa1.bmp - lisa33.bmp
	// The code below is exactly the same for HOMER, except that we have changed the values of iNum and Homer -> Lisa
	// along the file
	// *****************************************************************************************************************************************

	// Take all the image files at the range
	for ( iNum = 1; iNum <= 33; iNum++ )
	{
		// Build the image filename and path to read from disk
		sprintf ( cFileName, "Train/lisa%d.bmp", (int)(iNum) ); 
		//sprintf ( cFileName, "Valid/lisa%d.bmp", (int)(iNum) ); 
		printf ( " %s\n", cFileName);

		// Load the image from disk to the structure img.
		// 1  - Load a 3-channel image (color)
		// 0  - Load a 1-channel image (gray level)
		// -1 - Load the image as it is  (depends on the file)
		
		img = cvLoadImage( cFileName, -1 );

		// Gets the image size (width, height) 'img' 
		tam = cvGetSize( img );

		// Creates a header and allocates memory (tam) to store a copy of the original image.
		// 1 - gray level image
		// 3 - color image	
		// processed = cvCreateImage( tam, IPL_DEPTH_8U, 3);

		// Make a image clone and store it at processed and threshold
		processed 	= cvCloneImage( img );
		threshold  	= cvCloneImage( img );

		// Initialize variables with zero 
		fOrange 	= 0.0;
		fWhite 	= 0.0;
		fSkin = 0.0;
		fShortBart = 0.0;
		fLisaDress = 0.0;
		fHomerBeard = 0.0;
		fHomerPants = 0.0;
		fHomerShoes = 0.0;

		// Loop that reads each image pixel
		for( h = 0; h < img->height; h++ ) // rows
		{
			for( w = 0; w < img->width; w++ ) // columns
			{
				// Read each channel and writes it into the blue, green and red variables. Notice that OpenCV considers BGR
				blue  	= ( (uchar *)(img->imageData + h*img->widthStep) )[ w*img->nChannels + 0 ];
				green 	= ( (uchar *)(img->imageData + h*img->widthStep) )[ w*img->nChannels + 1 ];
				red   	= ( (uchar *)(img->imageData + h*img->widthStep) )[ w*img->nChannels + 2 ];

				// Shows the pixel value at the screenl
				//printf( "pixel[%d][%d]= %d %d %d \n", h, w, (int)blue, (int)green, (int)red );

				// Here starts the feature extraction....
				
				// Detect and count the number of orange pixels
				fOrange = countOrange(red,green,blue);
				
				// Detect and count the number of white pixels (just a dummy feature...)
				fWhite = countWhite(red,green,blue);
				
				// Here you can add your own features....... Good luck
				// Detect and count the number of skin pixels
				fSkin = countSkin(red,green,blue);

				// Detect and count the number of skin pixels
				fShortBart = countShortBart(red,green,blue);

				// Detect and count the number of pixels coresponding to the DRESS OF LISA
				fLisaDress = countLisaDress(red,green,blue);

				// Detect and count the number of pixels coresponding to the BEAR OF HOMER
				fHomerBeard = countHomerBeard(red,green,blue);

				// Detect and count the number of pixels coresponding to the PANTS OF HOMER
				fHomerPants = countHomerPants(red, green, blue);

				// Detect and count the number of pixels coresponding to the SHOES OF HOMER
				fHomerShoes = countHomerShoes(red, green, blue);
			}
		}

		// Lets make our counting somewhat independent on the image size...
		// Compute the percentage of pixels of a given colour.
		// Normalize the feature by the image size
		fOrange 	= fOrange / ( (int)img->height * (int)img->width );
		fWhite  	= fWhite  / ( (int)img->height * (int)img->width );
		fSkin = fSkin / ((int)img->height * (int)img->width);
		fShortBart = fShortBart / ((int)img->height * (int)img->width);
		fLisaDress = fLisaDress / ((int)img->height * (int)img->width);
		fHomerBeard = fHomerBeard / ((int)img->height * (int)img->width);
		fHomerPants = fHomerPants / ((int)img->height * (int)img->width);
		fHomerShoes = fHomerShoes / ((int)img->height * (int)img->width);

		// Store the feature value in the columns of the feature (matrix) vector
		fVector[iNum][1] = fOrange;
		fVector[iNum][2] = fWhite;
		fVector[iNum][3] = fSkin;
		fVector[iNum][4] = fShortBart;
		fVector[iNum][5] = fLisaDress;
		fVector[iNum][6] = fHomerBeard;
		fVector[iNum][7] = fHomerPants;
		fVector[iNum][8] = fHomerShoes;

		// Here you can add more features to your feature vector by filling the other columns: fVector[iNum][3] = ???; fVector[iNum][4] = ???;

		// Shows the feature vector at the screen
		printf("\n%d orange: %f | blanc:  %f | skin: %f | shortBart: %f | lisaDress: %f | homerBeard: %f | homerPants: %f | homerShoes: %f", iNum, fVector[iNum][1], fVector[iNum][2], fVector[iNum][3], fVector[iNum][4], fVector[iNum][5], fVector[iNum][6], fVector[iNum][7], fVector[iNum][8]);
		//printf( "\n%d %f %f %f %f %f", iNum, fVector[iNum][1], fVector[iNum][2], fVector[iNum][3], fVector[iNum][4], fVector[iNum][5] );

		// And finally, store your features in a file
		fprintf(fp, "%f,", fVector[iNum][1]);
		fprintf(fp, "%f,", fVector[iNum][2]);
		fprintf(fp, "%f,", fVector[iNum][3]);
		fprintf(fp, "%f,", fVector[iNum][4]);
		fprintf(fp, "%f,", fVector[iNum][5]);
		fprintf(fp, "%f,", fVector[iNum][6]);
		fprintf(fp, "%f,", fVector[iNum][7]);
		fprintf(fp, "%f,", fVector[iNum][8]);

		
		// IMPORTANT
		// Do not forget the label.... 	
		fprintf( fp, "%s\n", "Lisa");


		// Finally, give a look at the original image and the image with the pixels of interest in green
		cvShowImage( "Original", img );
		cvShowImage( "Processed", processed );
		
		// Wait until a key is pressed to continue... 	
		tecla = cvWaitKey(0);
	}

	cvReleaseImage(&img);
	cvDestroyWindow("Original");

	cvReleaseImage(&processed);
	cvDestroyWindow("Processed");

	fclose(fp);

	return 0;
} 




int countHomerBeard(int red, int green, int blue)
{
	// Detect and count the number of pixels coresponding pixels to the DRESS OF LISA
	int fHomerBeard = 0;

	//Homer Beard --> Red=189, Green=173, Blue=107
	if (red >= 187 && red <= 191 && green >= 171 && green <= 109 &&  blue >= 105 && blue <= 109)
	{
		fHomerBeard++;

	}
	return fHomerBeard;
}

int countHomerPants(int red, int green, int blue)
{
	// Detect and count the number of pixels coresponding pixels to the PANTS OF HOMER
	int fHomerPants = 0;

	//Homer Pants --> Red=74, Green=115, Blue=173
	if (red >= 72 && red <= 76 && green >= 113 && green <= 117 && blue >= 171 && blue <= 175)
	{
		fHomerPants++;

	}
	return fHomerPants;
}

int countHomerShoes(int red, int green, int blue)
{
	// Detect and count the number of pixels coresponding pixels to the SHOES OF HOMER
	int fHomerShoes = 0;

	//Homer Shoes --> Red=66, Green=66, Blue=66
	if (red >= 64 && red <= 68 && green >= 64 && green <= 68 && blue >= 64 && blue <= 68)
	{
		fHomerShoes++;

	}
	return fHomerShoes;
}

int countShortBart(int red, int green, int blue)
{
	// Detect and count the number of pixels coresponding pixels to the SHORT OF BART
	int fShortBart = 0;

	if (blue >= 130 && blue <= 135 && green >= 0 && green <= 15 && red >= 0 && red <= 0)
	{
		fShortBart++;

		// Just to be sure we are doing the right thing, we change the color of the orange pixels to green [R=0, G=255, B=0] and show them into a cloned image (processed)

		((uchar *)(processed->imageData + h*processed->widthStep))[w*processed->nChannels + 0] = 0;
		((uchar *)(processed->imageData + h*processed->widthStep))[w*processed->nChannels + 1] = 255;
		((uchar *)(processed->imageData + h*processed->widthStep))[w*processed->nChannels + 2] = 0;
	}
	return fShortBart;
}


int countLisaDress(int red, int green, int blue)
{
	// Detect and count the number of pixels coresponding to the dress of Lisa
	int fLisaDress = 0;

	//Lisa Dress --> Red=255, Green=0, Blue=0
	if (red >= 253 && red <= 255 && green >= 0 && green <= 3 &&  blue >= 0 && blue <= 3)
	{
		fLisaDress++;

	}
	return fLisaDress;
}


int countSkin(int red, int green, int blue)
{
	// Detect and count the number of skin pixels
	int fSkin = 0;
	
	if (blue >= 0 && blue <= 33 && green >= 185 && green <= 215 && red >= 235 && red <= 255)
	{
		fSkin++;

		// Just to be sure we are doing the right thing, we change the color of the orange pixels to green [R=0, G=255, B=0] and show them into a cloned image (processed)
		/*
		((uchar *)(processed->imageData + h*processed->widthStep))[w*processed->nChannels + 0] = 0;
		((uchar *)(processed->imageData + h*processed->widthStep))[w*processed->nChannels + 1] = 255;
		((uchar *)(processed->imageData + h*processed->widthStep))[w*processed->nChannels + 2] = 0;
		*/
	}

	return fSkin;
}


int countWhite(int red, int green, int blue)
{
	// Detect and count the number of white pixels
	int fWhite = 0;
	
	// Verify if the pixels have a given value ( White, defined as R[253-255], G[253-255], B[253-255] ). If so, count it...
	if ( blue >= 253 && green >= 253 && red >= 253 )
	{
		fWhite++;
	}

	return fWhite;
}


int countOrange(int red, int green, int blue)
{
	// Detect and count the number of orange pixels
	int fOrange = 0;
	
	// Verify if the pixels have a given value ( Orange, defined as R[240-255], G[85-105], B[11-22] ). If so, count it...
	if ( blue>=11 && blue<=22 && green>=85 && green<=105 &&  red>=240 && red<=255 )
	{
		fOrange++;
	
		// Just to be sure we are doing the right thing, we change the color of the orange pixels to green [R=0, G=255, B=0] and show them into a cloned image (processed)
		/*
		( (uchar *)(processed->imageData + h*processed->widthStep) )[ w*processed->nChannels + 0 ] = 0; 
		( (uchar *)(processed->imageData + h*processed->widthStep) )[ w*processed->nChannels + 1 ] = 255; 
		( (uchar *)(processed->imageData + h*processed->widthStep) )[ w*processed->nChannels + 2 ] = 0; 
		*/
	}

	return fOrange;
}