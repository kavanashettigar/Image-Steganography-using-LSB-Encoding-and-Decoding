#include <stdio.h>
#include "decode.h"
#include "types.h"
#include "common.h"
#include <string.h>

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if(strstr(argv[2],".bmp")==NULL) //checking stego image file has .bmp extension
    return e_failure;
    decInfo->stego_image_fname=argv[2]; //storing in structure variable
    if(argv[3]==NULL) //checking output file passed or not
    {
        printf("INFO: Output File not mentioned.Creating default file name as output\n");
        decInfo->secret_fname="output"; //If not passed storing default name in structure
    }
    else
    {
        //if outputfile name passed
        if(strchr(argv[3],'.')!=NULL) //removing extension if with extention file name passed
        decInfo->secret_fname=strtok(argv[3],"."); //storing output file name in structure
        else
        decInfo->secret_fname=argv[3]; //storing output file name in structure
    }
    return e_success;

}

Status open_file(DecodeInfo *decInfo)
{
    // stegno Image file
    printf("INFO: Opening required files\n");
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

        return e_failure;
    }
    printf("INFO: Opened %s\n",decInfo->stego_image_fname);

    return e_success;
}

Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    
    char imageBuffer[8];
    printf("INFO: Decoding Magic String Signature\n");
    /// Decode each character of the magic string
    for(int i=0;i<2;i++) 
    {
        char data=0;// Variable to store decoded character

        fread(imageBuffer,sizeof(char),8,decInfo->fptr_stego_image);//reading 8 bytes from stego image to image buffer
        decode_byte_to_lsb(&data,imageBuffer);// Decode one character from LSBs
        if(data!=magic_string[i]) // Compare decoded character with magic string
        return e_failure; 
    }
    return e_success;
}

Status decode_secret_file_extn_size(DecodeInfo *decInfo)
{
    char  imageBuffer[32];
    printf("INFO: Decoding %s File Extension size\n",decInfo->secret_fname);
    fread(imageBuffer,sizeof(char),32,decInfo->fptr_stego_image);// Read 32 bytes from stego image
    decInfo->size_extn_secret_file=0;// Initialize extension size
    decode_size_to_lsb(imageBuffer,&decInfo->size_extn_secret_file);// Decode extension size from LSBs
    printf("INFO: Done\n");
    return  e_success;
}

Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    char imageBuffer[8];
    printf("INFO: Decoding %s File Extension\n",decInfo->secret_fname);
    for(int i=0;i<decInfo->size_extn_secret_file;i++)// Decode each character of file extension
    {
        char data=0;
        fread(imageBuffer,sizeof(char),8,decInfo->fptr_stego_image);// Read 8 bytes from stego image
        decode_byte_to_lsb(&data,imageBuffer);// Decode extension character
        decInfo->extn_secret_file[i]=data;// Store decoded character
    }
    decInfo->extn_secret_file[decInfo->size_extn_secret_file] = '\0';

    printf("INFO: Done\n");
    return  e_success;
    
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    char  imageBuffer[32];
    printf("INFO: Decoding %s File size\n",decInfo->secret_fname);
    fread(imageBuffer,sizeof(char),32,decInfo->fptr_stego_image);// Read 32 bytes from stego image
    decInfo->size_secret_file=0;// Initialize secret file size
    decode_size_to_lsb(imageBuffer,&decInfo->size_secret_file);// Decode secret file size from LSBs
    printf("INFO: Done\n");
    return  e_success;
    
}

Status decode_secret_file_data(DecodeInfo *decInfo)
{
    char  imageBuffer[8];
    printf("INFO: Decoding %s File Data\n",decInfo->secret_fname);
    for(int i=0;i<decInfo->size_secret_file;i++) // Decode each byte of secret file
    {
        char data=0;
        fread(imageBuffer,sizeof(char),8,decInfo->fptr_stego_image);// Read 8 bytes from stego image
        decode_byte_to_lsb(&data,imageBuffer);// Decode one byte of secret data
        fprintf(decInfo->fptr_secret,"%c",data);// Write decoded data to output file
    }
    if(ftell(decInfo->fptr_secret)==decInfo->size_secret_file)// Verify whether full data is written
    {
        printf("INFO: Done\n");
        return  e_success;
    }
    else
    return e_failure;

}


Status decode_byte_to_lsb(char* data, char *image_buffer)
{
    for(int i = 0; i < 8; i++)// decode one byte from LSBs of 8 image bytes
    {
        *data |= (image_buffer[i] & 1) << i;
    }
    return e_success;
}

Status decode_size_to_lsb(char *imageBuffer,int* data)
{
    for(int i = 0; i < 32; i++)// Decode 32-bit integer from LSBs of 32 image bytes
    {
        *data |= (imageBuffer[i] & 1) << i;
    }
    return e_success;
}
    





Status do_decoding(DecodeInfo *decInfo)
{
    if(open_file(decInfo)==e_failure) // Open required files
    return e_failure;
    printf("INFO: ## Decoding Procedure Started ##\n");
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);// Skip BMP header (54 bytes)
    if(decode_magic_string(MAGIC_STRING,decInfo)==e_failure)// Decode and verify magic string
    return e_failure;

    if(decode_secret_file_extn_size(decInfo)==e_failure) // Decode secret file extension size
    return  e_failure;

    if(decode_secret_file_extn(decInfo)==e_failure)// Decode secret file extension
    return e_failure;
    // Create output file name with extension
    sprintf(decInfo->temp,"%s%s",decInfo->secret_fname,decInfo->extn_secret_file);
    decInfo->secret_fname=decInfo->temp;
    decInfo->fptr_secret=fopen(decInfo->secret_fname,"w");// Open output file
    if(decInfo->fptr_secret==NULL)
    printf("ERROR:unable to open %s file\n",decInfo->secret_fname);

    if(decode_secret_file_size(decInfo)==e_failure)// Decode secret file size
    return e_failure;

    if(decode_secret_file_data(decInfo) == e_failure)// Decode secret file data
    return e_failure;

// Close files
    fclose(decInfo->fptr_secret);
    fclose(decInfo->fptr_stego_image);
    printf("INFO: ##  Decoding Done Successfully ##\n");

    return e_success;
}