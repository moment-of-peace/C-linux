/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright by The HDF Group.                                               *
 * Copyright by the Board of Trustees of the University of Illinois.         *
 * All rights reserved.                                                      *
 *                                                                           *
 * This file is part of HDF5.  The full HDF5 copyright notice, including     *
 * terms governing use, modification, and redistribution, is contained in    *
 * the files COPYING and Copyright.html.  COPYING can be found at the root   *
 * of the source code distribution tree; Copyright.html can be found at the  *
 * root level of an installed copy of the electronic HDF5 document set and   *
 * is linked from the top-level documents page.  It can also be found at     *
 * http://hdfgroup.org/HDF5/doc/Copyright.html.  If you do not have          *
 * access to either file, you may request a copy from help@hdfgroup.org.     *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 *  This example writes data to the HDF5 file.
 *  Data conversion is performed during write operation.
 */

#include "hdf5.h"

#define H5FILE_NAME        "SDS.h5"
#define DATASETNAME "IntArray"
#define NX     40                      /* dataset dimensions */
#define NY     10
#define RANK   2

void converter(double data[][10], char line[]) {
	int m = 0;
	int j = 0;
	int k = 0;
	int index = 0;
	int negtive = 1;
	int dot = 0;
	double decimal = 0.1;

	while (line[m] != '\0') {
		if (line[m] == 44) {
			
//printf(",");
			data[j][k] = data[j][k] * negtive;

			negtive = 1;
			index = 0;
			dot = 0;
			decimal = 0.1;
			k++;
			m++;
			continue;
		}

		if (line[m] == 13 || line[m] == 10) {
			
//printf("\n");
			data[j][k] = data[j][k] * negtive;

			negtive = 1;
			index = 0;
			dot = 0;
			decimal = 0.1;
			k = 0;
			j++;
			m++;
			continue;
		}

		if (line[m] == 45) {
			negtive = -1;
			m++;
			continue;
		}

		if (line[m] == 46) {
			dot = 1;
			m++;
			continue;
		}

		if (index == 0) {
			data[j][k] = line[m] - 48;
			index++;
		} else if (dot == 0) {
			data[j][k] = data[j][k] * 10 + line[m] - 48;
		} else if (dot == 1) {
			data[j][k] = data[j][k] + (line[m] - 48) * decimal;
			decimal = decimal/10;
		}

		m++;
	}
}

int main (int argc, char *argv[])
{
	double tables[4][10][10] = {0};

	char re = '\r';  //r 13 n 10 . 46 - 45 ,44
	char re2 = '-';
printf("%d %d %s\n", re, re2, argv[1]);

	char line[600] = {'\0'};
	FILE   *fp; 
    char   cChar; 
    int   i; 
	int file_i;

	for (file_i = 1; file_i < argc; file_i++) {
		fp=fopen(argv[file_i],"r"); 
		i=0; 
		cChar=fgetc(fp); 
		while(cChar!='\0' && !feof(fp)) 
		{ 
			if (cChar != 13 && cChar != 10) {
				//printf("%c", cChar);
				line[i]=cChar; 
				i++; 
			} else {
				line[i]=cChar; 
				i++;
				//printf("\n");
			}
		    
		    cChar=fgetc(fp); 
		} 
		line[i]='\0'; 
	
		//printf("\n%d\n", i);

		converter(tables[file_i-1],line);
//printf("convert end\n");
		int i1, i2;
		for (i1 = 0; i1 < 10; i1++) {
			for (i2 = 0; i2 < 10; i2++) {
				printf("%.3f ", tables[file_i-1][i1][i2]);
			}
			printf("\n");
		}
	}


    hid_t       file, dataset;         /* file and dataset handles */
    hid_t       datatype, dataspace;   /* handles */
    hsize_t     dimsf[2];              /* dataset dimensions */
    herr_t      status;
    double         data[NX][NY];          /* data to write */
    int          j;
	i = 0;

    /*
     * Data  and output buffer initialization.
     */
    for(j = 0; j < 10; j++)
	for(i = 0; i < NY; i++)
	    data[j][i] = tables[0][j][i];

	for(j = 10; j < 20; j++)
	for(i = 0; i < NY; i++)
	    data[j][i] = tables[1][j-10][i];

	for(j = 20; j < 30; j++)
	for(i = 0; i < NY; i++)
	    data[j][i] = tables[2][j-20][i];

	for(j = 30; j < 40; j++)
	for(i = 0; i < NY; i++)
	    data[j][i] = tables[3][j-30][i];
    /*
     * 0 1 2 3 4 5
     * 1 2 3 4 5 6
     * 2 3 4 5 6 7
     * 3 4 5 6 7 8
     * 4 5 6 7 8 9
     */

    /*
     * Create a new file using H5F_ACC_TRUNC access,
     * default file creation properties, and default file
     * access properties.
     */
    file = H5Fcreate(H5FILE_NAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    /*
     * Describe the size of the array and create the data space for fixed
     * size dataset.
     */
    dimsf[0] = NX;
    dimsf[1] = NY;
    dataspace = H5Screate_simple(RANK, dimsf, NULL);

    /*
     * Define datatype for the data in the file.
     * We will store little endian INT numbers.
     */
    datatype = H5Tcopy(H5T_NATIVE_DOUBLE);
    status = H5Tset_order(datatype, H5T_ORDER_LE);

    /*
     * Create a new dataset within the file using defined dataspace and
     * datatype and default dataset creation properties.
     */
    dataset = H5Dcreate2(file, DATASETNAME, datatype, dataspace,
			H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    /*
     * Write the data to the dataset using default transfer properties.
     */
    status = H5Dwrite(dataset, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);

    /*
     * Close/release resources.
     */
    H5Sclose(dataspace);
    H5Tclose(datatype);
    H5Dclose(dataset);
    H5Fclose(file);

    return 0;
}


